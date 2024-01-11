#include "app.hpp"

uint8_t rxBuf[64];
uint16_t rxTop = 0, rxBtm = 0;

void setup() {
    printf("Hello World!\n");
    HAL_Delay(100);

    Flash_EEPROM flash;
    flash.writeFlashTest();

    HAL_Delay(1000);
}

void main_app() {
    setup();
    HAL_UART_Receive_DMA(&huart2, rxBuf, sizeof(rxBuf));
    while (1) {
        uint16_t rxTop = sizeof(rxBuf) - huart2.hdmarx->Instance->NDTR;
        while (rxTop != rxBtm) {
            // HAL_UART_Transmit(&huart2, rxBuf + rxBtm, (rxTop - rxBtm) & (sizeof(rxBuf) - 1), 100);
            for (uint16_t i = rxBtm; i < rxTop; i++) {
                printf("%c", rxBuf[i]);
            }
            rxBtm = rxTop;
        }
        printf(".");
        HAL_Delay(100);
    }
}