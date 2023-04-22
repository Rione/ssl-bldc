#include "mbed.h"
#include <setup.h>

RawSerial pc(USBTX, USBRX, 2000000); // tx, rx
BLDCMotor BLDC(PC_7, PB_4, PB_10, 8, 5e-3, &pc);

void setup() {
    BLDC.init();
    BLDC.setPIDGain(0.3, 1, 0.0001);
    BLDC.setVelocity(0);
}

int main() {
    setup();
    while (1) {
        BLDC.drive();
    }
}