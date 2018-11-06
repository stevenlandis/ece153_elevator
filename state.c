#include <stdio.h>
#include <stdlib.h>
#include "qpn_port.h"
#include "qhsmElevator.h"

int main() {
	printf("Starting\n");

	// initialize the state machine
	ESM_ctor();

	// start it up
	QHsm_init((QHsm *)&elevator);

	printf("\n");
}

// define the assertion failed function
void Q_onAssert(char const Q_ROM * const Q_ROM_VAR file, int line) {
    printf("Assertion failed in %s, line %d", file, line);
    exit(-1);
}