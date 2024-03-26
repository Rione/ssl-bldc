#include "app.hpp"
#include "STSPIN32G4.hpp"
#include "RGBLED.hpp"
DigitalOut led_alive(LED_ALIVE_GPIO_Port, LED_ALIVE_Pin);
RGBLED rgb;
AS5048A encoder(&hspi1, SPI1_NSS_GPIO_Port, SPI1_NSS_Pin);
STSPIN32G4 stspin(&hi2c3);
Flash_EEPROM flash;
CAN can(&hfdcan1, 0x555);
Timer timer;
CAN::CANData canRecvData;
CAN::CANData canSendData;

void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs) {
    if (can.getHcan() == &hfdcan1) {
        can.recv(canRecvData);
        led_alive = !led_alive;
    }
}

void stspinInit() {
    stspin.reset();
    HAL_Delay(500);
    stspin.setBuckConverterVoltage(12);
    stspin.setNfaultStatus();
    HAL_Delay(500);
    stspin.clearRegister();
}

void main_app() {
    rgb.led(rgb_t::RED);
    PwmOut pwm(&htim1, TIM_CHANNEL_1, TIM_CHANNEL_2, TIM_CHANNEL_3);
    BLDCMotor motor(&pwm, &encoder, 8, 0.001);
    stspinInit();
    can.init();
    pwm.init();
    motor.init();
    motor.setPIDGain(0.1, 0.5, 0);
    // flash.writeFlashTest();
    for (size_t i = 0; i < 3; i++) {
        rgb.led(rgb_t::GREEN);
        HAL_Delay(50);
        rgb.led(rgb_t::OFF);
        HAL_Delay(50);
    }

    printf("Start\n");
    while (1) {
        led_alive = !led_alive;
        motor.setVelocity(30);
        motor.drive();
    }
}