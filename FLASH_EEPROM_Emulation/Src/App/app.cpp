#include "app.hpp"

void setup() {
    printf("Hello World!\n");
    HAL_Delay(100);

    Flash_EEPROM flash;
    flash.writeFlashTest();

    HAL_Delay(1000);
}

void main_app() {
    setup();

    PwmOut pwm[2] = {
        PwmOut(&htim1, TIM_CHANNEL_1, TIM_CHANNEL_2, TIM_CHANNEL_3),
        PwmOut(&htim3, TIM_CHANNEL_1, TIM_CHANNEL_2, TIM_CHANNEL_3)};

    pwm[0].init();
    pwm[1].init();

    AS5048A encoder[2] = {AS5048A(&hspi1, CS0_GPIO_Port, CS0_Pin),
                          AS5048A(&hspi1, CS1_GPIO_Port, CS1_Pin)};

    while (1) {
        static int deg = 0;
        deg++;
        float u = 0.5 * MyMath::sinDeg(deg) + 0.5;
        float v = 0.5 * MyMath::sinDeg(deg + 120) + 0.5;
        float w = 0.5 * MyMath::sinDeg(deg + 240) + 0.5;
        pwm[0].write(u, v, w);
        pwm[1].write(u, v, w);

        printf("PWM:%4d %4d %4d ", (int)(u * 1000), (int)(v * 1000), (int)(w * 1000));
        printf("ENC:%4d %4d\n", (int)encoder[0].getAngleDeg(), (int)encoder[1].getAngleDeg());
        HAL_Delay(10);
    }
}

// App::App() {
//     init();
// }

// App::~App() {
//     deinit();
// }

// void App::init() {
//     // Initialize the main window
//     printf("hello world\n");
// }

// void App::loop() {
//     // Main loop
//     printf("loop\n");
// }

// void App::deinit() {
//     // Deinitialize the main window
//     printf("deinit\n");
// }