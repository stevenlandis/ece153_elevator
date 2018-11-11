#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "qpn_port.h"
#include "qhsmElevator.h"

#define N_BUTTON_TIMES 5
const int buttonTimes[] = {10, 25, 50, 100, 200};

const char* fileName = "out.txt";
FILE* fileRef;

void nt(int n);
void getAverages(int n);
void handleAverage();
void seedRand();

int main() {
	printf("Starting\n");
	seedRand();
	fileRef = fopen(fileName, "w");

	for (int i = 0; i < 1000; i++) {
		getAverages(1000000);
	}

	fclose(fileRef);
}

// define the assertion failed function
void Q_onAssert(char const Q_ROM * const Q_ROM_VAR file, int line) {
    printf("Assertion failed in %s, line %d", file, line);
    exit(-1);
}

void nt(int n) {
	for (int i = 0; i < n; i++) {
		dispatch(TIC_SIG);
	}
}

void getAverages(int n) {
	// create and initialize the state machine
	ESM_ctor();
	QHsm_init((QHsm *)&elevator);

	// press a bunch of buttons
	for (int i = 0; i < n; i++) {
		int delay = buttonTimes[rand() % N_BUTTON_TIMES];
		int floor = 1 + rand() % N_FLOOR;

		nt(delay);
		pressButton(floor);
	}

	handleAverage();
}

void handleAverage() {
    for (int i = 0; i < N_FLOOR; i++) {
    	fprintf(fileRef, "%f ",
    		(double)elevator.timeSum[i] / elevator.timeN[i]);
        // fprintf(fileRef, "Floor %d: %d x %ds --> %f\n",
        //     i+1,
        //     elevator.timeN[i],
        //     elevator.timeSum[i],
        //     (double)elevator.timeSum[i] / elevator.timeN[i]);
    }
    fprintf(fileRef, "\n");
}

void seedRand() {
	time_t t = time(0);
	printf("Seeding rand with %ld\n", t);
	srand(t);
}