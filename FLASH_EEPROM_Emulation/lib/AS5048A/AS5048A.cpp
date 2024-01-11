#include "AS5048A.hpp"

AS5048A::AS5048A(SPI_HandleTypeDef *hspi, GPIO_TypeDef *csPort, uint16_t csPin) : hspi(hspi), csPort(csPort), csPin(csPin) {
    HAL_GPIO_WritePin(csPort, csPin, GPIO_PIN_SET);
}

uint16_t AS5048A::get(uint16_t addr) {
    typedef union {
        uint16_t data;
        uint8_t bytes[2];
    } int16to2bytes;

    int16to2bytes txData;
    int16to2bytes rxData;
    int16to2bytes NOP;
    // txData.data = addr;
    rxData.data = 0;
    NOP.data = 0;

    uint16_t command = 0x4000; // PAR=0 R/W=R
    command = command | addr;

    // Add a parity bit on the the MSB
    command |= static_cast<uint16_t>(spiCalcEvenParity(command) << 0xF);

    txData.data = command;

    // printf("command:0x%x\n", command);

    HAL_GPIO_WritePin(csPort, csPin, GPIO_PIN_RESET);
    HAL_SPI_TransmitReceive_DMA(&hspi1, txData.bytes, rxData.bytes, 1);

    // 受信完了まで待機
    while (HAL_SPI_GetState(&hspi1) != HAL_SPI_STATE_READY) {
    }
    // printf("data1:%d\n", rxData.data);
    HAL_GPIO_WritePin(csPort, csPin, GPIO_PIN_SET);

    HAL_Delay(1);

    HAL_GPIO_WritePin(csPort, csPin, GPIO_PIN_RESET);
    // NOPを送信
    HAL_SPI_TransmitReceive_DMA(&hspi1, NOP.bytes, rxData.bytes, 1);
    ;
    // 受信完了まで待機
    while (HAL_SPI_GetState(&hspi1) != HAL_SPI_STATE_READY) {
    }
    HAL_GPIO_WritePin(csPort, csPin, GPIO_PIN_SET);

    // Check if the error bit is set
    // printf("data2:%d\n", rxData.data);

    if (rxData.data & 0x4000) {
        // printf("Setting error bit ");
        return 65535;
    } else {
        return rxData.data & 0x3FFF;
    }
}

uint8_t AS5048A::spiCalcEvenParity(uint16_t value) {
    // https://github.com/eborghi10/AS5048A/blob/master/src/AS5048A.cpp
    uint8_t cnt = 0;

    for (uint8_t i = 0; i < 16; i++) {
        if (value & 0x1) {
            cnt++;
        }
        value >>= 1;
    }
    return cnt & 0x1;
}

float AS5048A::getAngleRad() {
    return static_cast<float>(get(0x3FFF)) * 2 * PI / 0x3FFF;
}

float AS5048A::getAngleDeg() {
    return static_cast<float>(get(0x3FFF)) * 360 / 0x3FFF;
}