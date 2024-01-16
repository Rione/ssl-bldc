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
}
#endif
#endif