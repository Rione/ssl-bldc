#include "app.hpp"

DigitalOut led_alive(LED_ALIVE_GPIO_Port, LED_ALIVE_Pin);

DigitalOut led_red(LED_R_GPIO_Port, LED_R_Pin);
DigitalOut led_green(LED_G_GPIO_Port, LED_G_Pin);
DigitalOut led_blue(LED_B_GPIO_Port, LED_B_Pin);

AS5048A encoder(&hspi1, SPI1_NSS_GPIO_Port, SPI1_NSS_Pin);
Flash_EEPROM flash;
CAN can(&hfdcan1, 0x555);
Timer timer;
CAN::CANData canRecvData;
CAN::CANData canSendData;

typedef enum {
    RED,
    GREEN,
    BLUE,
    YELLOW,
    CYAN,
    MAGENTA,
    WHITE,
    OFF
} rgb_t;

void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs) {
    if (can.getHcan() == &hfdcan1) {
        can.recv(canRecvData);
        canRecvData.stdId = 0x555;
        can.send(canRecvData);
        led_alive = !led_alive;
    }
}

void rgb(rgb_t color) {
    switch (color) {
    case rgb_t::RED:
        led_red = 0;
        led_green = 1;
        led_blue = 1;
        break;
    case rgb_t::GREEN:
        led_red = 1;
        led_green = 0;
        led_blue = 1;
        break;
    case rgb_t::BLUE:
        led_red = 1;
        led_green = 1;
        led_blue = 0;
        break;
    case rgb_t::YELLOW:
        led_red = 0;
        led_green = 0;
        led_blue = 1;
        break;
    case rgb_t::CYAN:
        led_red = 1;
        led_green = 0;
        led_blue = 0;
        break;
    case rgb_t::MAGENTA:
        led_red = 0;
        led_green = 1;
        led_blue = 0;
        break;
    case rgb_t::WHITE:
        led_red = 0;
        led_green = 0;
        led_blue = 0;
        break;
    case rgb_t::OFF:
    default:
        led_red = 1;
        led_green = 1;
        led_blue = 1;
    }
}

void ledTest() {
    rgb(rgb_t::RED);
    HAL_Delay(100);
    rgb(rgb_t::GREEN);
    HAL_Delay(100);
    rgb(rgb_t::BLUE);
    HAL_Delay(100);
    rgb(rgb_t::YELLOW);
    HAL_Delay(100);
    rgb(rgb_t::CYAN);
    HAL_Delay(100);
    rgb(rgb_t::MAGENTA);
    HAL_Delay(100);
    rgb(rgb_t::WHITE);
    HAL_Delay(100);
    rgb(rgb_t::OFF);
    HAL_Delay(100);
}

void main_app() {
    // flash.writeFlashTest();
    can.init();
    printf("Hello World!\n");
    while (1) {
        float deg = encoder.getAngleDeg();
        led_alive = !led_alive;

        printf("Angle: %f\n", deg);
        printf("canRecvData:%d %d %d %d %d %d %d %d\n", canRecvData.data[0], canRecvData.data[1], canRecvData.data[2], canRecvData.data[3], canRecvData.data[4], canRecvData.data[5], canRecvData.data[6], canRecvData.data[7]);
        HAL_Delay(100);
        ledTest();
    }
}