#ifndef __PinDefs__
#define __PinDefs__

#include "main.h"
#include "tim.h"

#ifdef __cplusplus
extern "C" {

typedef struct {
    uint16_t pin;
    GPIO_TypeDef *port;
    TIM_HandleTypeDef *htim;
    uint32_t timerChannel;
} Pin_t;

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
}

#endif
#endif