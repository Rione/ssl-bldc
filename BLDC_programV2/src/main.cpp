#include <mbed.h>
#include "BLDC.h"
// LPF
RawSerial pc(USBTX, USBRX, 2000000); // tx, rx
BLDCMotor BLDC(D9, D5, D6, 8, 5e-3, &pc);
DigitalOut led(PC_14);
int d = 0;

void setup() {
    BLDC.init();
    BLDC.setPIDGain(0.02, 0.7, 0);
    BLDC.setSupplyVoltage(16, 7);
    BLDC.setVelocityLimit(300);
    BLDC.setVelocity(50);
}

int main() {
    setup();
    while (1) {
        BLDC.drive();
        if (abs(BLDC.getTargetVelocity() - BLDC.getAngularVelocity()) < 1) {
            led = 1;
        } else {
            led = 0;
        }
        // BLDC.setVelocity(300 * sinDeg(d * 0.001));
        d++;
    }
}