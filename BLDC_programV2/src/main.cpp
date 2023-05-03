#include <mbed.h>
#include "BLDC.h"

Serial pc(USBTX, USBRX, 230400); // tx, rx
BLDCMotor BLDC(D9, D5, D6, 8, 5e-3, &pc);
DigitalOut led(PC_14);

Ticker ticker;

int melody[8] = {1046, 1174, 1318, 1396, 1567, 1760, 1975, 2093};
int C = 0;
int8_t direction = 1;
int8_t save = 0;
// CAN can(PB_8, PB_9);

char buffer[64];

void changeMelody() {
    BLDC.setPWMFrequency(melody[C] * 19.6);
    if (save == 0) C += direction;
    if (C == 0 || C == 7) {
        save++;
        if (save == 3) {
            direction = -direction;
            save = 0;
        }
    }
}

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
    BLDC.setPWMFrequency(20501);
    BLDC.setVelocity(30);
    pc.attach(recvRx);
    ticker.attach(changeMelody, 0.45);
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
    }
}