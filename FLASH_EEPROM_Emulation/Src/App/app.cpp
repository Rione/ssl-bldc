#include "app.hpp"

uint8_t spiCalcEvenParity(uint16_t value) {
    // https://github.com/eborghi10/AS5048A/blob/master/src/AS5048A.cpp
    uint8_t cnt = 0;

    for (uint8_t i = 0; i < 16; i++) {
        if (value & 0x1) {
            cnt++;
        }
        value >>= 1;
    }
    return cnt & 0x1;
}

uint16_t getAS5048A_DMA(uint16_t addr) {
    typedef union {
        uint16_t data;
        uint8_t bytes[2];
    } int16to2bytes;

    int16to2bytes txData;
    int16to2bytes rxData;
    int16to2bytes NOP;
    // txData.data = addr;
    rxData.data = 0;
    NOP.data = 0x0000;

    uint16_t command = 0x4000; // PAR=0 R/W=R
    command = command | addr;

    // Add a parity bit on the the MSB
    command |= static_cast<uint16_t>(spiCalcEvenParity(command) << 0xF);

    txData.data = command;

    printf("command:0x%x\n", command);

    HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_RESET);
    HAL_SPI_TransmitReceive_DMA(&hspi1, txData.bytes, rxData.bytes, 1);

    // 受信完了まで待機
    while (HAL_SPI_GetState(&hspi1) != HAL_SPI_STATE_READY) {
    }
    printf("data1:%d\n", rxData.data);
    HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET);

    HAL_Delay(1);

    HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_RESET);
    // NOPを送信
    HAL_SPI_TransmitReceive_DMA(&hspi1, NOP.bytes, rxData.bytes, 1);
    ;
    // 受信完了まで待機
    while (HAL_SPI_GetState(&hspi1) != HAL_SPI_STATE_READY) {
    }
    HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET);

    // Check if the error bit is set
    printf("data2:%d\n", rxData.data);

    if (rxData.data & 0x4000) {
        printf("Setting error bit ");
    } else {
    }
    return rxData.data & 0x3FFF;
}

void setup() {
    // angleSensor.init();
    printf("setup\n");
}

void main_app() {

    setup();
    printf("Hello World!\n");
    // while(1);
    HAL_Delay(100);
    // printf("data:%d\n", getAS5048A_DMA(0x3FFF) * 360 / 0x3FFF);

    Flash_EEPROM flash;

    flash.writeFlashTest();

    HAL_Delay(1000);

    PwmOut pwm1(&htim1, TIM_CHANNEL_1, TIM_CHANNEL_2, TIM_CHANNEL_3);
    PwmOut pwm2(&htim3, TIM_CHANNEL_1, TIM_CHANNEL_2, TIM_CHANNEL_3);
    pwm1.init();
    pwm2.init();
    int deg = 0;
    while (1) {
        deg++;
        float u = 0.5 * MyMath::sinDeg(deg) + 0.5;
        float v = 0.5 * MyMath::sinDeg(deg + 120) + 0.5;
        float w = 0.5 * MyMath::sinDeg(deg + 240) + 0.5;
        pwm1.write(u, v, w);
        pwm2.write(u, v, w);
        HAL_Delay(10);
        printf("%d %d %d\n", (int)(u * 1000), (int)(v * 1000), (int)(w * 1000));
    }
}

App::App() {
    init();
}

App::~App() {
    deinit();
}

void App::init() {
    // Initialize the main window
    printf("hello world\n");
}

void App::loop() {
    // Main loop
    printf("loop\n");
}

void App::deinit() {
    // Deinitialize the main window
    printf("deinit\n");
}