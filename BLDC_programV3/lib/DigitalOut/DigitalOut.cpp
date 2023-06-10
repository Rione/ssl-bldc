#include "DigitalOut.hpp"

DigitalOut::DigitalOut(GPIO_TypeDef *_GPIOx, uint16_t _GPIO_Pin) {
    GPIOx = _GPIOx;
    GPIO_Pin = _GPIO_Pin;
}

void DigitalOut::write(GPIO_PinState value) {
    HAL_GPIO_WritePin(GPIOx, GPIO_Pin, value);
}

DigitalOut &DigitalOut::operator=(GPIO_PinState value) {
    write(value);
    return *this;
}
