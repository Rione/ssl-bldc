#include <mbed.h>
#include "BLDC.h"
// LPF
RawSerial pc(USBTX, USBRX, 2000000); // tx, rx
BLDCMotor BLDC(D9, D5, D6, 8, 5e-3, &pc);

void setup() {
    wait(3);
    BLDC.init();
    BLDC.setPIDGain(0.3, 1, 0.0001);
    BLDC.setSupplyVoltage(12, 6);
    BLDC.setVelocity(0);
}

int deg = 0;
int main() {
    setup();
    while (1) {
        // BLDC.drive();
        // BLDC.updateEncoder();
        // float velocity = BLDC.getAngularVelocity();
    }
}