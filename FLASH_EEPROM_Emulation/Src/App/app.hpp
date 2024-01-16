#ifndef __APP__
#define __APP__

#include <stdio.h>

#include "main.h"
#include "MyMath.hpp"
#include "PWM.hpp"
#include "FLASH_EEPROM.hpp"
#include "AS5048A.hpp"
#include "Serial.hpp"
#include "DigitalInOut.hpp"
#include "Timer.hpp"
#include "CAN.hpp"

#include "BLDC.hpp"

#ifdef __cplusplus

extern "C" {

Pin_t pin_PWM1_A = {
    .pin = PWM1_A_Pin,
    .port = PWM1_A_GPIO_Port,
    .htim = &htim3,
    .timerChannel = TIM_CHANNEL_2};

Pin_t pin_PWM1_B = {
    .pin = PWM1_B_Pin,
    .port = PWM1_B_GPIO_Port,
    .htim = &htim3,
    .timerChannel = TIM_CHANNEL_1};

Pin_t pin_PWM1_C = {
    .pin = PWM1_C_Pin,
    .port = PWM1_C_GPIO_Port,
    .htim = &htim2,
    .timerChannel = TIM_CHANNEL_3};

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
