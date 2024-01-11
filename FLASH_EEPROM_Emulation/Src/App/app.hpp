#ifndef __APP__
#define __APP__

#include <stdio.h>

#include "main.h"
#include "MyMath.hpp"
#include "PWM.hpp"
#include "FLASH_EEPROM.hpp"
#include "AS5048A.hpp"
#include "Serial.hpp"

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
