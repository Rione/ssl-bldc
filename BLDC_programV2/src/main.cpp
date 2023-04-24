#include <mbed.h>
#include "BLDC.h"
// LPF
RawSerial pc(USBTX, USBRX, 2000000); // tx, rx
BLDCMotor BLDC(D9, D5, D6, 8, 5e-3, &pc);

void setup() {
    wait(3);
    BLDC.init();
    BLDC.setPIDGain(0.01, 0, 0);
    BLDC.setSupplyVoltage(16, 8);
    BLDC.setVelocity(10);
}

// V = 16のとき
// Uq = 5の時に2.0A流れます → PIDlimit 5

int deg = 0;
int main() {
    setup();
    while (1) {
        // BLDC.drive();
        // BLDC.setPhaseVoltage(1.0, Radians(deg));

        // wait_us(1);
        // BLDC.Diagnose();
        deg += 1;
        BLDC.openLoopControl(1, Radians(deg));

        // float velocity = BLDC.getAngularVelocity();
    }
}