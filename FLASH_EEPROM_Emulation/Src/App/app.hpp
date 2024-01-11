#ifndef __APP__
#define __APP__

#include <main.h>
#include <stdio.h>
#include "MyMath.hpp"
#include "PWM.hpp"
#include "FLASH_EEPROM.hpp"
#include "AS5048A.hpp"
#include "usart.h"

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