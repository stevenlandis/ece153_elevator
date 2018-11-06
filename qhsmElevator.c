#include "qpn_port.h"
#include "qhsmElevator.h"
#include <stdio.h>

#define max(a_, b_) ((a_) > (b_) ? (a_) : (b_))
#define min(a_, b_) ((a_) < (b_) ? (a_) : (b_))
#define maxEquals(a_, b_) ((a_) = max(a_, b_))
#define minEquals(a_, b_) ((a_) = min(a_, b_))

static QState ESM_initial(ESM *me);
static QState ESM_s(ESM *me);
static QState ESM_open(ESM *me);
static QState ESM_floor(ESM *me);
static QState ESM_move(ESM *me);

// global state machine reference
ESM elevator;

void pressButton(int floor) {
	elevator.pressed[floor-1] = 1;
	maxEquals(elevator.tempHighFloor, floor);
	minEquals(elevator.tempLowFloor, floor);
}

void ESM_ctor() {
	// initialize superclass
	QHsm_ctor(&elevator.super, (QStateHandler)&ESM_initial);

	// intialize class
	elevator.floor = 0;
	for (int i = 0; i < N_FLOOR; i++)
		elevator.pressed[i] = 0;
	elevator.direction = UP;

	// actual destination floors
	elevator.lowFloor = N_FLOOR+1;
	elevator.highFloor = 0;

	// temp floors to be updated when the elevator passes a floor
	elevator.tempLowFloor = elevator.lowFloor;
	elevator.tempHighFloor = elevator.highFloor;

	// timer cound
	elevator.count = 0;
}

static QState ESM_initial(ESM *me) {
	printf("top-INIT;");
	return Q_TRAN(ESM_floor);
}

static QState ESM_floor(ESM *me) {
	switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            printf("floor-ENTRY;");

            if (me->pressed[me->floor-1]) {
            	Q_TRAN(ESM_open);
            }

            me->lowFloor = me->tempLowFloor;
            me->highFloor = me->tempHighFloor;
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            printf("s-EXIT;");
            return Q_HANDLED();
        }
        case Q_INIT_SIG: {
            printf("s-INIT;");
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&QHsm_top);
}

static QState ESM_open(ESM *me) {
	switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            printf("open-ENTRY;");

            me->count = 0;
            me->pressed[me->floor-1] = 0;

            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            printf("open-EXIT;");
            return Q_HANDLED();
        }
        case Q_INIT_SIG: {
            printf("open-INIT;");
            return Q_HANDLED();
        }
        case TIC_SIG: {
        	printf("open-SIG;");

        	if (me->count < 10) {
        		me->count++;
        		return Q_HANDLED();
        	} else {
        		return Q_TRAN(ESM_floor);
        	}
        }
    }
    return Q_SUPER(&QHsm_top);
}

static QState ESM_move(ESM *me) {
	switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            printf("move-ENTRY;");

            me->count = 0;

            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            printf("move-EXIT;");
            return Q_HANDLED();
        }
        case Q_INIT_SIG: {
            printf("move-INIT;");
            return Q_HANDLED();
        }
        case TIC_SIG: {
        	printf("move-SIG;");

        	if (me->count < 5) {
        		me->count++;
        		return Q_HANDLED();
        	} else {
        		me->floor = me->destFloor;
        		return Q_TRAN(ESM_floor);
        	}
        }
    }
    return Q_SUPER(&QHsm_top);
}