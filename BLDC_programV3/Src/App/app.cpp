#include "app.hpp"
#include "STSPIN32G4.hpp"
#include "RGBLED.hpp"
DigitalOut led_alive(LED_ALIVE_GPIO_Port, LED_ALIVE_Pin);
DigitalIn sw_a(SW_A_GPIO_Port, SW_A_Pin);
DigitalIn sw_b(SW_B_GPIO_Port, SW_B_Pin);
RGBLED rgb;
AS5048A encoder(&hspi1, SPI1_NSS_GPIO_Port, SPI1_NSS_Pin);
STSPIN32G4 stspin(&hi2c3);
Flash_EEPROM flash;
CAN can(&hfdcan1, 0x555);
Timer timer;

CAN::CANData canSendData;

float targetVelocity = 0; // radian per sec
bool isEmergency = false;
uint8_t boardId = 0;

typedef union {
    int16_t data;
    uint8_t byte[2];
} twoByteSplitter;

void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs) {
    if (can.getHcan() != &hfdcan1) return;
    CAN::CANData canRecvData;
    can.recv(canRecvData);

    switch (canRecvData.stdId) {
    case 0x00:
        isEmergency = true;
        break;
    case 0x1AA:
        twoByteSplitter vel;
        // A = 0 B = 0 → boardId = 0
        // A = 0 B = 1 → boardId = 1
        // A = 1 B = 0 → boardId = 2
        // A = 1 B = 1 → boardId = 3
        boardId = (!sw_a.read() << 1) | !sw_b.read();
        vel.byte[0] = canRecvData.data[boardId];
        vel.byte[1] = canRecvData.data[boardId + 1];
        targetVelocity = vel.data;
    default:
        break;
    }
}

void main_app() {
    PwmOut pwm(&htim1, TIM_CHANNEL_1, TIM_CHANNEL_2, TIM_CHANNEL_3);
    BLDCMotor motor(&pwm, &encoder, 8, 0.001);

    rgb.led(rgb_t::RED);

    // init処理
    stspin.init();
    can.init();
    pwm.init();
    motor.init();

    // モータのPIDゲイン設定
    motor.setPIDGain(0.1, 0.5, 0);
    // flash.writeFlashTest();
    // チカチカ
    for (size_t i = 0; i < 3; i++) {
        rgb.led(rgb_t::GREEN);
        HAL_Delay(50);
        rgb.led(rgb_t::OFF);
        HAL_Delay(50);
    }

    printf("Start\n");
    while (1) {
        // printf("targetVelocity: %.2f boardId:%d\n", targetVelocity, boardId);
        motor.setVelocity(targetVelocity);
        motor.drive();

        abs(motor.getTargetVelocity() - motor.getAngularVelocity()) < 3 ? rgb.led(rgb_t::GREEN) : rgb.led(rgb_t::MAGENTA);
    }
}