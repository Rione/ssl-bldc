#include "app.hpp"

Serial uart2(&huart2, 64);
PwmOut pwm1(&pin_PWM1_A, &pin_PWM1_B, &pin_PWM1_C);
AS5048A encoder1(&hspi1, CS1_GPIO_Port, CS1_Pin);
CAN can(&hcan1, 0x555);

DigitalOut led_blue(led_blue_GPIO_Port, led_blue_Pin);
DigitalOut led_red(led_red_GPIO_Port, led_red_Pin);
DigitalOut led_green(led_green_GPIO_Port, led_green_Pin);
DigitalOut led_alive(led_alive_GPIO_Port, led_alive_Pin);

BLDCMotor BLDC(&pwm1, &encoder1, 8, 0.005);

Timer timer;
CAN::CANData canRecvData;
typedef union {
    int16_t data;
    uint8_t split[2];
} int16Splitter;

int16Splitter vel[4];
uint8_t velIndex = 0;                 // 4つが最大
uint8_t velInt8Index = 0;             // 4 * 2 = 8が最大
uint8_t motorId = Constrain(2, 1, 4); // 1 ~ 4

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

    if (uart2.available()) {
        uint8_t receivedByte = uart2.getc();

        printf("headerReceived:%d, velInt8Index:%d receivedByte:%d\n", headerReceived, velInt8Index, receivedByte);
        if (!headerReceived) {
            velIndex = 0;
            velInt8Index = 0;
            if (receivedByte == HEADER) {
                // ヘッダを受信したら速度データの受信を開始
                headerReceived = true;
                printf("header received %d\n ", receivedByte);
            } else {
                headerReceived = false;
                printf("error: Header is not received %d\n", receivedByte);
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
                    printf("success: Check sum match %d == %d\n", sum, checksum);
                    printf("motorId:%d vel[0]:%d\tvel[1]:%d\tvel[2]:%d\tvel[3]:%d\n", motorId, vel[0].data, vel[1].data, vel[2].data, vel[3].data);

                    int16_t v = vel[motorId - 1].data;
                    printf("%d\n", v);
                    // BLDC.setVelocity(v);
                } else {
                    // 受信失敗
                    printf("error: Check sum doesn't match %d != %d\n", sum, checksum);
                }

                headerReceived = false; // 次のヘッダを待つ準備
            }
        }
    }
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
    if (can.getHcan() == &hcan1) {
        can.recv(canRecvData);
        canRecvData.stdId = 0x555;
        can.send(canRecvData);
        led_alive = !led_alive;
    }
}

void setup() {
    printf("Hello World!\n");
    HAL_Delay(100);

    uart2.init();
    can.init();
    pwm1.init();

    Flash_EEPROM flash;
    flash.writeFlashTest();

    HAL_Delay(1000);
}

void main_app() {
    setup();
    uint16_t deg = 0;
    while (1) {
        timer.reset();
        deg++;
        float u = 0.5 * MyMath::sinDeg(deg) + 0.5;
        float v = 0.5 * MyMath::sinDeg(deg + 120) + 0.5;
        float w = 0.5 * MyMath::sinDeg(deg + 240) + 0.5;

        pwm1.write(u, v, w);
        // HAL_Delay(10);
        // printf("%d %d %d\t", (int)(u * 1000), (int)(v * 1000), (int)(w * 1000));
        uint16_t deg = encoder1.getAngleDeg();
        led_blue = (u > 0.5);
        led_red = (v > 0.5);
        led_green = (w > 0.5);
        while (uart2.available()) {
            recvRx();
            led_alive = !led_alive;
        }

        uint32_t time = timer.read_us();
        printf("time: %ld deg: %3d CANrecv: %d data:%x %x %x %x %x %x %x %x\n", time, deg, canRecvData.stdId, canRecvData.data[0], canRecvData.data[1], canRecvData.data[2], canRecvData.data[3], canRecvData.data[4], canRecvData.data[5], canRecvData.data[6], canRecvData.data[7]);
    }
}