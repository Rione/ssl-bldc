#include <mbed.h>
#include "BLDC.h"

Serial pc(USBTX, USBRX, 230400); // tx, rx
BLDCMotor BLDC(PB_10, PB_4, PC_7, 8, 5e-3, &pc);
DigitalOut led(PC_14);

Ticker ticker;

int C = 0;
int8_t direction = 1;
int8_t save = 0;
// CAN can(PB_8, PB_9);

char buffer[64];

void recvRx() {
    if (pc.readable()) {
        pc.gets(buffer, sizeof(buffer));
        int v = atoi(buffer);
        pc.printf("%d\n", v);
        BLDC.setVelocity(v);
    }
}

void setup() {
    led = 1;
    wait_ms(200);
    led = 0;
    wait_ms(200);
    led = 1;
    wait_ms(200);
    led = 0;
    wait_ms(200);
    BLDC.init();
    BLDC.setPIDGain(0.02, 0.7, 0);
    BLDC.setSupplyVoltage(16, 7);
    BLDC.setVelocityLimit(350);
    BLDC.setVelocity(30);
    pc.attach(recvRx);
}

int main() {
    setup();
    while (1) {
        BLDC.drive();
        led = abs(BLDC.getTargetVelocity() - BLDC.getAngularVelocity()) < 5;
    }
}