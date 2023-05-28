#include "code.hpp"
#include <stdio.h>

void pwm();
int count = 0;
int maxValue = 1800;

void setup() {
    printf("hello\n");
}

void loop() {

    pwm();
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
}

void pwm() {
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
    while (1) {
        HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
        HAL_Delay(1000);
        HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
        HAL_Delay(50);
        if (count > maxValue) {
            count = 0;
        } else {
            count++;
        }

        printf("pwm%d\n", count);
    }
}