#include <mbed.h>
#include "BLDC.h"
// LPF
RawSerial pc(USBTX, USBRX, 2000000); // tx, rx
BLDCMotor BLDC(D9, D5, D6, 8, 5e-3, &pc);
int d = 0;

void setup() {
    wait(3);
    BLDC.init();
    BLDC.setPIDGain(0.01, 0.1, 0);
    BLDC.setSupplyVoltage(16, 7);
    BLDC.setVelocity(30);
    BLDC.setVelocityLimit(300);
}

int main() {
    setup();
    while (1) {
        BLDC.drive();
        BLDC.setVelocity(300 * sinDeg(d * 0.001));
        d++;
    }
}