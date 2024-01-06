#ifndef __APP__
#define __APP__

#include <main.h>
#include <stdio.h>
#include "FLASH_EEPROM.hpp"
#include "spi.h"
#include "as5048a.h"

#ifdef __cplusplus

extern "C" {
AS5048A angleSensor(&hspi1, GPIOA, GPIO_PIN_4);

void main_app();
class App {
  public:
    App();
    ~App();
    void run();

  private:
    void init();
    void loop();
    void deinit();
};
}
#endif
#endif
