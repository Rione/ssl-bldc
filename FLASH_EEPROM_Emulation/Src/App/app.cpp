#include "app.hpp"

void setup() {
    printf("Hello World!\n");
    HAL_Delay(100);

    Flash_EEPROM flash;
    flash.writeFlashTest();

    HAL_Delay(1000);
}

void main_app() {
    setup();

    Serial uart2(&huart2, 64);
    uart2.init();

    while (1) {
        while (uart2.available()) {
            printf("%c", uart2.read());
        }
        HAL_Delay(1000);
    }
}