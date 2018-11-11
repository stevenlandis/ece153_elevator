#ifndef qhsmElevator
#define qhsmElevator

#define N_FLOOR 5

typedef enum SignalTag {
	TIC_SIG = Q_USER_SIG
} Signal;

typedef enum DirTag {UP, DOWN} Dir;

typedef struct ESMTag {
	QHsm super;

	// internal variables
	int floor;
	int destFloor;
	int pressed[N_FLOOR];
	int internalPressed[N_FLOOR];
	int pressTime[N_FLOOR];
	int timeSum[N_FLOOR];
	int timeN[N_FLOOR];
	Dir direction;
	int count;
} ESM;

extern ESM elevator;
extern int ticTime;

// function for choosing which floor to go to
// This is not a signal to the state machine to make it easier
// to change in the future.
void pressButton(int floor);

void dispatch(Signal sig);

void printState();

void ESM_ctor();

#endif