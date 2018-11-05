#include <stdio.h>

#define currentFloorIsPressed pressed & (1<<floorN)
#define lowerFloorIsPressed pressed && ((1<<floorN) - 1)
#define higherFloorIsPressed pressed && ((~0) << (floorN+1))
#define pressButton(sig_) pressed |= 1<<sig_;
#define unpressCurrentFloor() pressed &= ~(1<<floorN)
#define currentState() printf("Floor %d, count %d, rf %d, dir, %d\n",floorN, count, returnFloor, direction);

enum DirTag {UP, DOWN}; typedef enum DirTag Dir;
enum SigTag {B1 = 0, B2, B3, B4, B5, TIC}; typedef enum SigTag Sig;

int floorN = 0;
Dir direction = UP;
int pressed = 0;
int count = 0;
int returnFloor = 0;

void dispatch(Sig sig);
void dispatchN(Sig sig, int n);
void arriveFloor();
void cycleDoors();
void moveUp();
void moveDown();

int main() {
    printf("Starting\n");

    currentState();

    dispatch(B1);
    dispatchN(TIC, 15);
    dispatch(B2);
    dispatchN(TIC, 20);
    dispatch(B3);
    dispatch(B4);
    dispatch(B5);
    dispatch(B1);
    dispatchN(TIC, 100);
}

void dispatch(Sig sig) {
    printf("sig %d --> ", sig);
    currentState();

    if (sig == TIC && count > 0) {
        count--;
    } else if (sig != TIC) {
        // floor button pressed
        pressButton(sig);
    }

    if (count == 0) {
        arriveFloor();
    }
}

void dispatchN(Sig sig, int n) {
    for (int i = 0; i < n; i++) {
        dispatch(sig);
    }
}

void cycleDoors() {
    unpressCurrentFloor();
    printf("   Opening Doors\n");
    count = 10;
    returnFloor = floorN;
}

void moveUp() {
    count = 5;
    returnFloor = floorN + 1;
}

void moveDown() {
    count = 5;
    returnFloor = floorN - 1;
}

void arriveFloor() {
    floorN = returnFloor;
    printf("   Arriving at floor %d\n", floorN);
    if (currentFloorIsPressed) {
        cycleDoors();
    } else {
        if (floorN == 0) {
            direction = UP;
        } else if (floorN == 4) {
            direction = DOWN;
        }

        if (direction == UP && higherFloorIsPressed) {
            moveUp();
        } else if (direction == DOWN && lowerFloorIsPressed) {
            moveDown();
        }
    }
}