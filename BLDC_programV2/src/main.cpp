#include <mbed.h>
#include "BLDC.h"
// LPF
Serial pc(USBTX, USBRX, 230400); // tx, rx
BLDCMotor BLDC(PC_7, PB_4, PB_10, 8, 5e-3, &pc);
DigitalOut led(PC_14);

char buffer[64];
volatile bool data_received = false;

void recvRx() {
    if (pc.readable()) {
        pc.gets(buffer, sizeof(buffer));
        int v = atoi(buffer);
        pc.printf("%d\n", v);
        BLDC.setVelocity(v);
    }
}

void setup() {
    BLDC.init();
    BLDC.setPIDGain(0.02, 0.7, 0);
    BLDC.setSupplyVoltage(16, 7);
    BLDC.setVelocityLimit(350);
    BLDC.setVelocity(50);
    pc.attach(recvRx);
}

int main() {
    setup();
    while (1) {
        BLDC.drive();
        led = abs(BLDC.getTargetVelocity() - BLDC.getAngularVelocity()) < 1;
    }
}