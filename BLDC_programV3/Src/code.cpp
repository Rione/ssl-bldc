#include "code.hpp"
#include <stdio.h>
/*
# PWMの周波数に関するCubeMXの設定について
- HCLK(システムクロック)
- PSC(プリスケーラ)
- CP(カウンターピリオド)

Freq = HCLK / (PSC + 1) * (CP + 1)

# Freqを100Khzにしたい．
- HCLK = 180MHz

$$ 180MHz / 100KHz = 1800 $$
$$ 1800 = (PSC + 1) * (CP + 1) $$

- PSC = 0
- CP = 1799

# デューティ比に関するCubeMXの設定について
- PWM Mode
- Channel
- Pulse

Duty = Pulse / (CP + 1)

# Dutyを50%にしたい．
- Pulse = 900

$$ 900 = 900 / (1799 + 1) $$

# その他
- この設定だと，PWMの周波数は100KHz，デューティ比は50%になる．
- この設定だと，TIM2_CH3(PA2)ピンにPWM信号が出力される．

# 参考
- https://qiita.com/ShunHattori/items/68f099f1d77702d2535d
- https://garberas.com/archives/2138
- https://moons.link/post-632/
- https://rt-net.jp/mobility/archives/19815

*/

int count = 0;
int maxValue = 1800;

void setup() {
    printf("hello\n");
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3); // PWMの開始
}

void loop() {
    if (count > maxValue) {
        count = 0;
    } else {
        count++;
    }
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, count); // PWMのデューティ比の設定

    printf("pwm%d\n", count);

    HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
    HAL_Delay(50);
    HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
    HAL_Delay(50);
}
