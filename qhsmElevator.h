#ifndef qhsmElevator
#define qhsmElevator

typedef enum SignalTag {
	TIC_SIG = Q_USER_SIG
} Signal;
#define N_FLOOR 5

typedef enum DirTag {UP, DOWN} Dir;

typedef struct ESMTag {
	QHsm super;

	// new variables
	int floor;
	int destFloor;
	int pressed[N_FLOOR];
	Dir direction;
	int lowFloor;
	int highFloor;
	int tempLowFloor;
	int tempHighFloor;
	int count;
} ESM;

extern ESM elevator;

// function for choosing which floor to go to
// This is not a signal to the state machine to make it easier
// to change in the future.
void pressButton(int floor);

void ESM_ctor();

#endif