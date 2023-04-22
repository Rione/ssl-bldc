
#include "mbed.h"
#include <setup.h>

RawSerial pc(USBTX, USBRX, 2000000); // tx, rx
BLDCMotor motor(PC_7, PB_4, PB_10, 8, 5e-3, &pc);

void setup() {
    motor.init();
    motor.setPIDGain(0.1, 0.1, 0.1);
    motor.setVelocity(0);
}

int main() {
    setup();
    while (1) {
    }
}