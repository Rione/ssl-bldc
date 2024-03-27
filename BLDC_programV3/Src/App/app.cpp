#include "app.hpp"
#include "STSPIN32G4.hpp"
#include "RGBLED.hpp"

DigitalOut led_alive(LED_ALIVE_GPIO_Port, LED_ALIVE_Pin);
DigitalIn sw_a(SW_A_GPIO_Port, SW_A_Pin);
DigitalIn sw_b(SW_B_GPIO_Port, SW_B_Pin);
DigitalIn sw(SW_GPIO_Port, SW_Pin);
RGBLED rgb;
AS5048A encoder(&hspi1, SPI1_NSS_GPIO_Port, SPI1_NSS_Pin);
STSPIN32G4 stspin(&hi2c3);
Flash_EEPROM flash;
PwmOut pwm(&htim1, TIM_CHANNEL_1, TIM_CHANNEL_2, TIM_CHANNEL_3);
BLDCMotor motor(&pwm, &encoder, 8, 0.001);
CAN can(&hfdcan1, 0x555);
Timer canLastRecv;

CAN::CANData canSendData;

float targetVelocity = 0; // radian per sec
bool isEmergency = false;
uint8_t boardId = 0;

typedef union {
    int16_t data;
    uint8_t byte[2];
} twoByteSplitter;

typedef struct {
    union {
        uint64_t i64; // 64bit
        float f32[2]; // 32bit x 2 = 64bit
    };
} flashData_t;

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
        canLastRecv.reset();
        isEmergency = false;
    default:
        break;
    }
}

bool checkFlashIsEmpty() {
    flashData_t data;
    bool isEmpty = false;
    printf("checkFlashIsEmpty\n");
    flash.loadFlash(FLASH_START_ADDRESS, (uint64_t *)&data, sizeof(flashData_t));
    printf("load : f32[0]:%3.1f f32[1]:%3.1f\n", data.f32[0], data.f32[1]);
    if (data.i64 == 0xFFFFFFFFFFFFFFFF) {
        printf("Flash is empty.\n");
        isEmpty = true;
    } else {
        printf("Flash is not empty.\n");
    }
    return isEmpty;
}

void writeZeroPosToFlash(float zeroPos) {
    flashData_t data;
    data.f32[0] = zeroPos;
    data.f32[1] = 0.0;
    flash.writeFlash(FLASH_START_ADDRESS, (uint64_t *)&data, sizeof(flashData_t));
    printf("write: f32[0]:%3.1f f32[1]:%3.1f\n", data.f32[0], data.f32[1]);
    checkFlashIsEmpty();
}

void setZeroPosFromFlash() {
    flashData_t data;
    flash.loadFlash(FLASH_START_ADDRESS, (uint64_t *)&data, sizeof(flashData_t));
    motor.setAbsoluteZero(data.f32[0]);
    printf("setZero :%3.1f\n", data.f32[0]);
}

void main_app() {
    rgb.led(rgb_t::RED);
    // init処理
    stspin.init();
    can.init();
    pwm.init();
    motor.init(false);
    canLastRecv.reset();

    // FlashからZeroPosを読み込む
    if (checkFlashIsEmpty()) {
        motor.setAbsoluteZero();
        writeZeroPosToFlash(motor.getZeroPos());
    } else {
        setZeroPosFromFlash();
    }

    // モータのPIDゲイン設定
    motor.setPIDGain(0.1, 0.5, 0);

    // イニシャライズ完了
    for (size_t i = 0; i < 3; i++) {
        rgb.led(rgb_t::GREEN);
        HAL_Delay(50);
        rgb.led(rgb_t::OFF);
        HAL_Delay(50);
    }

    printf("Start\n");

    while (1) {
        if (isEmergency || canLastRecv.read_ms() > 1000) {
            motor.writePwm(0, 0, 0);
            rgb.led(rgb_t::MAGENTA);
            HAL_Delay(100);
            rgb.led(rgb_t::CYAN);
            HAL_Delay(100);
            printf("Stop\n");
            targetVelocity = 0;
        } else {
            if (sw.read() == 0) {
                // ボタンが押されたらZeroPosの設定をする
                rgb.led(rgb_t::RED);
                motor.writePwm(0, 0, 0);
                wait_ms(1000);
                motor.setAbsoluteZero();
                float zeroPos = motor.getZeroPos();
                writeZeroPosToFlash(zeroPos);
                for (size_t i = 0; i < 3; i++) {
                    rgb.led(rgb_t::GREEN);
                    HAL_Delay(50);
                    rgb.led(rgb_t::OFF);
                    HAL_Delay(50);
                }

            } else {
                motor.setVelocity(targetVelocity);
                motor.drive();
                abs(motor.getTargetVelocity() - motor.getAngularVelocity()) < 3 ? rgb.led(rgb_t::GREEN) : rgb.led(rgb_t::MAGENTA);
                // printf("targetVelocity: %.2f boardId:%d\n", targetVelocity, boardId);
            }
        }
    }
}