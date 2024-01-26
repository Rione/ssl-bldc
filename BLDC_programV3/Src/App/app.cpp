#include "app.hpp"

DigitalOut led_blue(LED_GPIO_Port, LED_Pin);
AS5048A encoder(&hspi1, SPI1_NSS_GPIO_Port, SPI1_NSS_Pin);
Flash_EEPROM flash;
CAN can(&hfdcan1, 0x555);
Timer timer;
CAN::CANData canRecvData;
CAN::CANData canSendData;

void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs) {
    if (can.getHcan() == &hfdcan1) {
        can.recv(canRecvData);
        canRecvData.stdId = 0x555;
        can.send(canRecvData);
        led_blue = !led_blue;
    }
}

void main_app() {
    // flash.writeFlashTest();
    can.init();
    printf("Hello World!\n");
    while (1) {
        float deg = encoder.getAngleDeg();
        printf("Angle: %f\n", deg);
        printf("canRecvData:%d %d %d %d %d %d %d %d\n", canRecvData.data[0], canRecvData.data[1], canRecvData.data[2], canRecvData.data[3], canRecvData.data[4], canRecvData.data[5], canRecvData.data[6], canRecvData.data[7]);
        HAL_Delay(500);
    }
}