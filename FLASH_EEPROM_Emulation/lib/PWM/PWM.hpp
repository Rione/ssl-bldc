#ifndef MYPWM_HPP
#define MYPWM_HPP

#include "tim.h"
#include "MyMath.hpp"

#ifdef __cplusplus
extern "C" {
class PwmOut {
  public:
    PwmOut(TIM_HandleTypeDef *_htim, uint32_t channel1, uint32_t channel2, uint32_t channel3) {
        htim = _htim;
        channel[0] = channel1;
        channel[1] = channel2;
        channel[2] = channel3;
    }

    void init() {
        maxValue = htim->Init.Period;
        HAL_TIM_PWM_Start(htim, channel[0]);
        HAL_TIM_PWM_Start(htim, channel[1]);
        HAL_TIM_PWM_Start(htim, channel[2]);
    }

    void write(float duty1, float duty2, float duty3) {
        duty1 = (int)(Constrain(duty1, 0.0, 1.0) * maxValue);
        duty2 = (int)(Constrain(duty2, 0.0, 1.0) * maxValue);
        duty3 = (int)(Constrain(duty3, 0.0, 1.0) * maxValue);

        __HAL_TIM_SET_COMPARE(htim, channel[0], duty1);
        __HAL_TIM_SET_COMPARE(htim, channel[1], duty2);
        __HAL_TIM_SET_COMPARE(htim, channel[2], duty3);
    }

  private:
    TIM_HandleTypeDef *htim;
    uint32_t channel[3];
    uint32_t maxValue;
};
};

#endif
#endif