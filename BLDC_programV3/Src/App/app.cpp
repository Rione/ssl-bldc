#include "app.hpp"

DigitalOut led_blue(LED_GPIO_Port, LED_Pin);
AS5048A encoder(&hspi1, SPI1_NSS_GPIO_Port, SPI1_NSS_Pin);
Flash_EEPROM flash;

Timer timer;
void main_app() {
    // flash.writeFlashTest();
    printf("Hello World!\n");
    while (1) {
        float deg = encoder.getAngleDeg();
        led_blue = !led_blue;
        printf("Angle: %f\n", deg);
        HAL_Delay(500);
    }
}