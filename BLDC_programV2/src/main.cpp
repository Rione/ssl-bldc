#include <mbed.h>
#include "BLDC.h"

Serial pc(USBTX, USBRX, 230400); // tx, rx
// BLDCMotor BLDC(PB_10, PB_4, PC_7, 8, 5e-3, &pc);
BLDCMotor BLDC(PC_7, PB_4, PB_10, 8, 5e-3, &pc);
DigitalOut led(PC_14);

DigitalOut red(PB_14);
DigitalOut green(PB_15);
DigitalOut blue(PB_13);

DigitalIn swA(PB_7, PullUp);
DigitalIn swB(PB_5, PullUp);

char counter = 0;
Ticker ticker;
void rgb(bool r, bool g, bool b) {
    red = !r;
    green = !g;
    blue = !b;
}

// CAN can(PB_8, PB_9);

char buffer[64];

typedef union {
    int16_t data;
    uint8_t split[2];
} int16Splitter;

int16Splitter vel[4];
uint8_t velIndex = 0;                 // 4つが最大
uint8_t velInt8Index = 0;             // 4 * 2 = 8が最大
uint8_t motorId = Constrain(2, 1, 4); // 1 ~ 4

// void recvRx() {
//     if (pc.readable()) {
//         pc.gets(buffer, sizeof(buffer));
//         int v = atoi(buffer);
//         pc.printf("%d\n", v);
//         BLDC.setVelocity(v);
//     }
// }

void recvRx() {
    /*
    0. ヘッダを受信
    1. vel[0]の 1byte目を受信
    2. vel[0]の 2byte目を受信
    3. vel[1]の 1byte目を受信
    4. vel[1]の 2byte目を受信
    5. vel[2]の 1byte目を受信
    6. vel[2]の 2byte目を受信
    7. vel[3]の 1byte目を受信
    8. vel[3]の 2byte目を受信
    9. チェックサムを受信
     */
    const uint8_t HEADER = 0xAA; // 仮のヘッダバイト
    static bool headerReceived = false;
    static uint8_t sum = 0;

    if (pc.readable()) {
        uint8_t receivedByte = pc.getc();

        pc.printf("headerReceived:%d, velInt8Index:%d receivedByte:%d\n", headerReceived, velInt8Index, receivedByte);
        if (!headerReceived) {
            velIndex = 0;
            velInt8Index = 0;
            if (receivedByte == HEADER) {
                // ヘッダを受信したら速度データの受信を開始
                headerReceived = true;
                pc.printf("header received %d\n ", receivedByte);
            } else {
                headerReceived = false;
                pc.printf("error: Header is not received %d\n", receivedByte);
            }
        } else { // ヘッダを受信した後の処理
            if (velInt8Index < 8) {
                // 速度データ受信
                vel[velIndex].split[velInt8Index % 2] = receivedByte;
                velInt8Index++;
                velIndex = velInt8Index / 2;

                // checksum
                sum += receivedByte;
            } else { // 速度データ受信完了
                // チェックサムの処理
                uint8_t checksum = receivedByte;
                if (sum == checksum) {
                    // 受信成功
                    pc.printf("success: Check sum match %d == %d\n", sum, checksum);
                    pc.printf("motorId:%d vel[0]:%d\tvel[1]:%d\tvel[2]:%d\tvel[3]:%d\n", motorId, vel[0].data, vel[1].data, vel[2].data, vel[3].data);

                    int16_t v = vel[motorId - 1].data;
                    pc.printf("%d\n", v);
                    BLDC.setVelocity(v);
                } else {
                    // 受信失敗
                    pc.printf("error: Check sum doesn't match %d != %d\n", sum, checksum);
                }

                headerReceived = false; // 次のヘッダを待つ準備
            }
        }
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
    wait_us(200000);
    led = 0;
    wait_us(200000);
    led = 1;
    wait_us(200000);
    led = 0;
    wait_us(200000);
    BLDC.init();
    BLDC.setPIDGain(0.02, 0.7, 0);
    BLDC.setSupplyVoltage(16, 7);
    BLDC.setVelocityLimit(500);
    BLDC.setVelocity(-30);
    pc.attach(recvRx);
    // bool a = swA.read();
    // bool b = swB.read();
    // if (a == 0 && b == 0) {
    //     motorId = 1;
    // } else if (a == 0 && b == 1) {
    //     motorId = 2;
    // } else if (a == 1 && b == 0) {
    //     motorId = 3;
    // } else if (a == 1 && b == 1) {
    //     motorId = 4;
    // }
    bool a = !swA.read();
    bool b = !swB.read();
    motorId = ((a << 1) | b) + 1;
}

int main() {
    setup();
    while (1) {
        BLDC.drive();
        Led();
        pc.printf("targetVel:%f, vel:%f\n", BLDC.getTargetVelocity(), BLDC.getAngularVelocity());
    }
}