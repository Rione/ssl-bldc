#ifndef DIGITALOUT_H
#define DIGITALOUT_H

#include "main.h"

class DigitalOut {
  public:
    DigitalOut(GPIO_TypeDef *_GPIOx, uint16_t _GPIO_Pin);
    void write(GPIO_PinState value);
    DigitalOut &operator=(GPIO_PinState value);

  private:
    GPIO_TypeDef *GPIOx;
    uint16_t GPIO_Pin;
};

#endif