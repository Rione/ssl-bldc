#include <mbed.h>
#include "BLDC.h"

Serial pc(USBTX, USBRX, 230400); // tx, rx
BLDCMotor BLDC(PB_10, PB_4, PC_7, 8, 5e-3, &pc);
DigitalOut led(PC_14);

DigitalOut red(PB_14);
DigitalOut green(PB_15);
DigitalOut blue(PB_13);

char counter = 0;
Ticker ticker;
void rgb(bool r, bool g, bool b) {
    red = !r;
    green = !g;
    blue = !b;
}

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

void Led() {
    float vel = BLDC.getAngularVelocity();
    if (abs(BLDC.getTargetVelocity() - vel) < 5) {
        led = 1;
        if (vel > 0) {
            rgb(1, 0, 1);
        } else {
            rgb(0, 1, 1);
        }
    } else {
        led = 0;
        if (abs(vel) < 2) {
            rgb(1, 1, 1);
        } else {
            if (vel > 0) {
                rgb(1, 0, 0);
            } else {
                rgb(0, 0, 1);
            }
        }
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
    BLDC.setVelocity(-30);
    pc.attach(recvRx);
}

int main() {
    setup();
    while (1) {
        BLDC.drive();
        Led();
    }
}