#include "code.hpp"

#include "DigitalOut.hpp"

DigitalOut led(LED_GPIO_Port, LED_Pin);

int count = 0;
int maxValue = 180;
int amount = 1;
void setup() {
    printf("hello\n");
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3); // PWMの開始
}

void loop() {
    if (count == maxValue) {
        amount = -1;
    } else if (count == 0) {
        amount = 1;
    }
    count += amount;

    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, count); // PWMのデューティ比の設定

    printf("pwm%d\n", count);
    HAL_Delay(2);
    // led = GPIO_PIN_SET;
    // HAL_Delay(10);
    // led = GPIO_PIN_RESET;
    // HAL_Delay(100);
}
