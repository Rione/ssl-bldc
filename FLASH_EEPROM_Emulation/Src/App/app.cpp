#include "app.hpp"

Serial uart2(&huart2, 64);
PwmOut pwm[2] = {
    PwmOut(&htim1, TIM_CHANNEL_1, TIM_CHANNEL_2, TIM_CHANNEL_3),
    PwmOut(&htim3, TIM_CHANNEL_1, TIM_CHANNEL_2, TIM_CHANNEL_3)};
AS5048A encoder[2] = {
    AS5048A(&hspi1, CS0_GPIO_Port, CS0_Pin),
    AS5048A(&hspi1, CS1_GPIO_Port, CS1_Pin)};
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

void setup() {
    printf("Hello World!\n");
    HAL_Delay(100);

    Flash_EEPROM flash;
    flash.writeFlashTest();

    HAL_Delay(1000);
}

void main_app() {
    setup();

    uart2.init();

    while (1) {
        while (uart2.available()) {
            recvRx();
        }
    }
}