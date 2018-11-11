#include "qpn_port.h"
#include "qhsmElevator.h"
#include <stdio.h>

#define DOOR_DELAY 10
#define FLOOR_DELAY 5

#define max(a_, b_) ((a_) > (b_) ? (a_) : (b_))
#define min(a_, b_) ((a_) < (b_) ? (a_) : (b_))
#define maxEquals(a_, b_) ((a_) = max(a_, b_))
#define minEquals(a_, b_) ((a_) = min(a_, b_))

static void updatePressed();

// internal states
static QState ESM_initial(ESM *me);
static QState ESM_idle(ESM *me);
static QState ESM_move(ESM *me);
static QState ESM_door(ESM *me);

// global state machine reference
ESM elevator;
int ticTime = 0;

void pressButton(int floor) {
	elevator.pressed[floor-1] = 1;
    elevator.pressTime[floor-1] = ticTime;
}

static void updatePressed() {
    for (int i = 0; i < N_FLOOR; i++) {
        elevator.internalPressed[i] = elevator.pressed[i];
    }
}

static int upperFloorPressed(ESM* me) {
    for (int i = me->floor+1; i < N_FLOOR; i++) {
        if (me->internalPressed[i]) {
            return 1;
        }
    }

    return 0;
}

static int lowerFloorPressed(ESM* me) {
    for (int i = me->floor-1; i >= 0; i--) {
        if (me->internalPressed[i]) {
            return 1;
        }
    }

    return 0;
}

void dispatch(Signal sig) {
    // printf("Dispatching TIC:\n   ");
    Q_SIG((QHsm *)&elevator) = sig;
    QHsm_dispatch((QHsm *)&elevator);
    ticTime++;
    // printf("\nTime: %d, ", ticTime);
    // printState();
    // printf("\n");
}

static void printPressed(int * array) {
    printf("[%d", array[0]);
    for (int i = 1; i < N_FLOOR; i++) {
        printf(", %d", array[i]);
    }
    printf("]");
}

void printState() {
    printf("floor: %d --> %d, count: %d, dir: %d",
        elevator.floor,
        elevator.destFloor,
        elevator.count,
        elevator.direction);

    printf(", pressed: ");
    printPressed(elevator.pressed);
    printf(", internal: ");
    printPressed(elevator.internalPressed);
    printf("\n");
}

void ESM_ctor() {
	// initialize superclass
	QHsm_ctor(&elevator.super, (QStateHandler)&ESM_initial);

	// intialize class
	elevator.floor = 0;
	for (int i = 0; i < N_FLOOR; i++) {
		elevator.pressed[i] = 0;
        elevator.internalPressed[i] = 0;
        elevator.timeSum[i] = 0;
        elevator.timeN[i] = 0;
    }
	elevator.direction = UP;

	// timer count
	elevator.count = 0;
}

static QState ESM_initial(ESM *me) {
	// printf("top-INIT;");
	return Q_TRAN(&ESM_idle);
}

static QState ESM_idle(ESM* me) {
    switch(Q_SIG(me)) {
        case Q_INIT_SIG: {
            // printf("idle-INIT;");

            if (me->internalPressed[me->floor]) {
                return Q_TRAN(ESM_door);
            }

            if (me->direction == UP) {
                if (upperFloorPressed(me)) {
                    return Q_TRAN(ESM_move);
                } else if (lowerFloorPressed(me)) {
                    me->direction = DOWN;
                    return Q_TRAN(ESM_move);
                }
            } else {
                if (lowerFloorPressed(me)) {
                    return Q_TRAN(ESM_move);
                } else if (upperFloorPressed(me)) {
                    me->direction = UP;
                    return Q_TRAN(ESM_move);
                }
            }

            // printf("Idle; ");

            return Q_HANDLED();
        }
        case TIC_SIG: {
            // printf("idle-TIC;");

            updatePressed();

            return Q_TRAN(ESM_idle);
        }
    }

    return Q_SUPER(QHsm_top);
}

static QState ESM_move(ESM *me) {
    switch(Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            // printf("move-ENTRY;");

            updatePressed();

            // set the next floor
            if (me->direction == UP) {
                me->destFloor = me->floor+1;
            } else {
                me->destFloor = me->floor-1;
                // printf("Moving down; ");
            }

            // printf("Moved; ");

            me->count = 0;

            return Q_HANDLED();
        }
        case TIC_SIG: {
            // printf("move-TIC;");

            me->count++;
            if (me->count >= FLOOR_DELAY) {
                me->floor = me->destFloor;
                // printf("Moved to %d; ", me->floor);
                return Q_TRAN(ESM_idle);
            }

            // printf("Moving; ");

            return Q_HANDLED();
        }
    }

    return Q_SUPER(ESM_idle);
}

static QState ESM_door(ESM *me) {
    switch(Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            // printf("door-ENTRY;");
            // printf("Opened Door; ");

            me->count = 0;
            me->internalPressed[me->floor] = 0;
            me->pressed[me->floor] = 0;
            // printf("Took %d to reach floor %d; ",
            //     ticTime - me->pressTime[me->floor],
            //     me->floor);
            me->timeSum[me->floor] += ticTime - me->pressTime[me->floor];
            me->timeN[me->floor]++;

            return Q_HANDLED();
        }
        case TIC_SIG: {
            // printf("door-TIC;");

            me->count++;
            if (me->count >= DOOR_DELAY) {
                // printf("Closed Door; ");
                updatePressed();
                return Q_TRAN(ESM_idle);
            }

            // printf("Door open; ");
            
            return Q_HANDLED();
        }
    }

    return Q_SUPER(ESM_idle);
}

// static QState ESM_(ESM *me) {
// 	switch (Q_SIG(me)) {
//         case Q_ENTRY_SIG: {
//             printf("floor-ENTRY;");

//             // if (me->direction == UP) {
//             //     // up
//             //     if (me->highFloor != me->floor) {
//             //         me->destFloor = me->floor+1;
//             //         return Q_TRAN(ESM_move);
//             //     }
//             // } else {
//             //     // down
//             //     if (me->lowFloor != me->floor) {
//             //         me->destFloor = me->floor-1;
//             //         return Q_TRAN(ESM_move);
//             //     }
//             // }

//             return Q_HANDLED();
//         }
//         case Q_EXIT_SIG: {
//             printf("floor-EXIT;");

//             updatePressed();

//             return Q_HANDLED();
//         }
//         case Q_INIT_SIG: {
//             printf("floor-INIT;");

//             printf(" - %d - ", me->internalPressed[me->floor]);
//             if (me->internalPressed[me->floor]) {
//                 printf(" - - - - - ");
//                 return Q_TRAN(&ESM_open);
//             }

//             return Q_HANDLED();
//         }
//         case TIC_SIG: {
//             printf("floor-TIC;");

//             return Q_TRAN(ESM_floor);
//         }
//     }
//     return Q_SUPER(&QHsm_top);
// }

// static QState ESM_open(ESM *me) {
// 	switch (Q_SIG(me)) {
//         case Q_ENTRY_SIG: {
//             printf("open-ENTRY;");

//             me->count = 0;
//             me->pressed[me->floor] = 0;
//             me->internalPressed[me->floor] = 0;

//             return Q_HANDLED();
//         }
//         case Q_EXIT_SIG: {
//             printf("open-EXIT;");
//             return Q_HANDLED();
//         }
//         case Q_INIT_SIG: {
//             printf("open-INIT;");
//             return Q_HANDLED();
//         }
//         case TIC_SIG: {
//         	printf("open-SIG;");

//         	if (me->count < 10) {
//         		me->count++;
//         		return Q_HANDLED();
//         	} else {
//         		return Q_TRAN(ESM_floor);
//         	}
//         }
//     }
//     return Q_SUPER(&QHsm_floor);
// }

// static QState ESM_move(ESM *me) {
// 	switch (Q_SIG(me)) {
//         case Q_ENTRY_SIG: {
//             printf("move-ENTRY;");

//             me->count = 0;

//             return Q_HANDLED();
//         }
//         case Q_EXIT_SIG: {
//             printf("move-EXIT;");
//             return Q_HANDLED();
//         }
//         case Q_INIT_SIG: {
//             printf("move-INIT;");
//             return Q_HANDLED();
//         }
//         case TIC_SIG: {
//         	printf("move-SIG;");

//         	if (me->count < 5) {
//         		me->count++;
//         		return Q_HANDLED();
//         	} else {
//         		me->floor = me->destFloor;
//         		return Q_TRAN(ESM_floor);
//         	}
//         }
//     }
//     return Q_SUPER(&QHsm_top);
// }