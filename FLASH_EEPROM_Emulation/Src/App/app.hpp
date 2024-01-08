#ifndef __APP__
#define __APP__

#include <main.h>
#include <stdio.h>
#include "FLASH_EEPROM.hpp"
#include "spi.h"
#include "PWM.hpp"

#ifdef __cplusplus

extern "C" {

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
