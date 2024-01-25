#include "app.hpp"

DigitalOut led_blue(LED_GPIO_Port, LED_Pin);

float a = 0.0f;
void main_app() {
    Flash_EEPROM flash;
    flash.writeFlashTest();
    while (1) {
        a += 0.1f;
        printf("Hello from main_app()! %f\n", a);
        led_blue = !led_blue;
        HAL_Delay(500);
    }
}