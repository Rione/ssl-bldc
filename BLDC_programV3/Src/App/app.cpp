#include "app.hpp"
#include "tim.h"
#include "i2c.h"
DigitalOut led_alive(LED_ALIVE_GPIO_Port, LED_ALIVE_Pin);

DigitalOut led_red(LED_R_GPIO_Port, LED_R_Pin);
DigitalOut led_green(LED_G_GPIO_Port, LED_G_Pin);
DigitalOut led_blue(LED_B_GPIO_Port, LED_B_Pin);
DigitalOut wake(WAKE_GPIO_Port, WAKE_Pin);
DigitalIn ready(READY_GPIO_Port, READY_Pin);
DigitalIn nFault(NFAULT_GPIO_Port, NFAULT_Pin);

AS5048A encoder(&hspi1, SPI1_NSS_GPIO_Port, SPI1_NSS_Pin);
Flash_EEPROM flash;
CAN can(&hfdcan1, 0x555);
Timer timer;
CAN::CANData canRecvData;
CAN::CANData canSendData;

#define STSPING4_CONTROLER_ADDR 0b1000111
#define I2C_DATA_SIZE 8

#define POWMNG_REG 0x01
#define LOGIC_REG 0x02
#define READY_REG 0x07
#define NFAULT_REG 0x08
#define CLEAR__REG 0x09
#define STBY_REG 0x0A
#define LOCK_REG 0x0B
#define RESET_REG 0x0C
#define STATUS_REG 0x80

unsigned char i2c_data[I2C_DATA_SIZE];
int read_end;

void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c) {
    if (hi2c->Instance == I2C1) {
        read_end = 1;
    }
}

bool I2C1_WriteData(uint8_t devAddr, uint8_t regAddr, uint8_t data) {
    uint8_t i2c_data[1] = {data};
    // HAL_I2C_Mem_Write(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress,uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout)
    if (HAL_I2C_Mem_Write(&hi2c3, devAddr << 1, regAddr, I2C_MEMADD_SIZE_8BIT, i2c_data, 1, HAL_MAX_DELAY) != HAL_OK) {
        // 通信エラーが発生した場合の処理
        return false;
    }
    return true;
}

void write_register(uint8_t register_value) {
    uint8_t data[1] = {register_value};
    HAL_I2C_Master_Transmit(&hi2c3, STSPING4_CONTROLER_ADDR, data, 1, 100); // data is the start pointer of our array
}

typedef enum {
    RED,
    GREEN,
    BLUE,
    YELLOW,
    CYAN,
    MAGENTA,
    WHITE,
    OFF
} rgb_t;

void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs) {
    if (can.getHcan() == &hfdcan1) {
        can.recv(canRecvData);
        canRecvData.stdId = 0x555;
        can.send(canRecvData);
        led_alive = !led_alive;
    }
}

void rgb(rgb_t color) {
    switch (color) {
    case rgb_t::RED:
        led_red = 0;
        led_green = 1;
        led_blue = 1;
        break;
    case rgb_t::GREEN:
        led_red = 1;
        led_green = 0;
        led_blue = 1;
        break;
    case rgb_t::BLUE:
        led_red = 1;
        led_green = 1;
        led_blue = 0;
        break;
    case rgb_t::YELLOW:
        led_red = 0;
        led_green = 0;
        led_blue = 1;
        break;
    case rgb_t::CYAN:
        led_red = 1;
        led_green = 0;
        led_blue = 0;
        break;
    case rgb_t::MAGENTA:
        led_red = 0;
        led_green = 1;
        led_blue = 0;
        break;
    case rgb_t::WHITE:
        led_red = 0;
        led_green = 0;
        led_blue = 0;
        break;
    case rgb_t::OFF:
    default:
        led_red = 1;
        led_green = 1;
        led_blue = 1;
    }
}

void ledTest() {
    rgb(rgb_t::RED);
    HAL_Delay(100);
    rgb(rgb_t::GREEN);
    HAL_Delay(100);
    rgb(rgb_t::BLUE);
    HAL_Delay(100);
    rgb(rgb_t::YELLOW);
    HAL_Delay(100);
    rgb(rgb_t::CYAN);
    HAL_Delay(100);
    rgb(rgb_t::MAGENTA);
    HAL_Delay(100);
    rgb(rgb_t::WHITE);
    HAL_Delay(100);
    rgb(rgb_t::OFF);
    HAL_Delay(100);
}

void setBuckConverterVoltage(uint8_t voltage) {
    uint8_t value = 0b00000000;
    switch (voltage) {
    case 8:
        value = 0b00000000;
        break;
    case 10:
        value = 0b00000001;
        break;
    case 12:
        value = 0b00000010;
        break;
    case 15:
        value = 0b00000011;
        break;
    default:
        value = 0b00000000;
        printf("Invalid voltage value. Set to 8V\n");
        break;
    }
    bool i2c_status = false;
    i2c_status = I2C1_WriteData(STSPING4_CONTROLER_ADDR, LOCK_REG, 0b11110000);
    printf("unlock!! READY:%d NFAULT:%d I2C_success:%d\n", ready.read(), nFault.read(), i2c_status);
    HAL_Delay(100);
    i2c_status = I2C1_WriteData(STSPING4_CONTROLER_ADDR, POWMNG_REG, value);
    printf("Writereg READY:%d NFAULT:%d I2C_success:%d\n", ready.read(), nFault.read(), i2c_status);
    HAL_Delay(100);
    i2c_status = I2C1_WriteData(STSPING4_CONTROLER_ADDR, LOCK_REG, 0b00000000);
    printf("lock!!   READY:%d NFAULT:%d I2C_success:%d\n", ready.read(), nFault.read(), i2c_status);
    HAL_Delay(100);
}

void setNFault_status() {
    /*
    NFAULT  Address 0x08  [[Protected]]
    bit7                0:
    bit6                1:
    bit5                1:
    bit4                1:
    bit3                1:
    bit2 [VDS_P_FLT]    1 :Signaling of the VDS protection triggering: Enabled by default, 0 to disable
    bit1 [THSD _FLT]    1 :Signaling of the thermal shutdown status: Enabled by default, 0 to disable
    bit0 [VCC_UVLO_FLT] 1 :Signaling of the VCC UVLO status: Enabled by default, 0 to disable
    */

    uint8_t value = 0b01111000;
    // value &= ~(1 << 2); // VDS_P_FLT 0: disable
    // value &= ~(1 << 1); // THSD_FLT 0: disable
    // value &= ~(1 << 0); // VCC_UVLO_FLT 0: disable

    bool i2c_status = false;
    // unlock
    i2c_status = I2C1_WriteData(STSPING4_CONTROLER_ADDR, LOCK_REG, 0b11110000);
    printf("unlock!! READY:%d NFAULT:%d I2C_success:%d\n", ready.read(), nFault.read(), i2c_status);
    HAL_Delay(100);
    // write
    i2c_status = I2C1_WriteData(STSPING4_CONTROLER_ADDR, NFAULT_REG, value);
    printf("Writereg READY:%d NFAULT:%d I2C_success:%d\n", ready.read(), nFault.read(), i2c_status);
    HAL_Delay(100);
    // lock
    i2c_status = I2C1_WriteData(STSPING4_CONTROLER_ADDR, LOCK_REG, 0b00000000);
    printf("lock!!   READY:%d NFAULT:%d I2C_success:%d\n", ready.read(), nFault.read(), i2c_status);
    HAL_Delay(100);
}

bool clear() {
    bool i2c_status = false;
    i2c_status = I2C1_WriteData(STSPING4_CONTROLER_ADDR, CLEAR__REG, 0b11111111);
    return i2c_status;
}

void reset() {
    bool i2c_status = false;
    i2c_status = I2C1_WriteData(STSPING4_CONTROLER_ADDR, LOCK_REG, 0b11110000);
    printf("unlock!! READY:%d NFAULT:%d I2C_success:%d\n", ready.read(), nFault.read(), i2c_status);
    HAL_Delay(100);
    // write
    i2c_status = I2C1_WriteData(STSPING4_CONTROLER_ADDR, RESET_REG, 0xFF);
    printf("Writereg READY:%d NFAULT:%d I2C_success:%d\n", ready.read(), nFault.read(), i2c_status);
    HAL_Delay(100);
    // lock
    i2c_status = I2C1_WriteData(STSPING4_CONTROLER_ADDR, LOCK_REG, 0b00000000);
    printf("lock!!   READY:%d NFAULT:%d I2C_success:%d\n", ready.read(), nFault.read(), i2c_status);
    HAL_Delay(100);
}

void read_status() {
    uint8_t data[1];
    uint8_t addr = STATUS_REG;
    HAL_I2C_Master_Transmit(&hi2c3, STSPING4_CONTROLER_ADDR << 1, &addr, 1, 100);
    HAL_I2C_Master_Receive(&hi2c3, STSPING4_CONTROLER_ADDR << 1, data, 1, 100);
    printf("LOCK:%d RESET:%d VDS_P:%d THSD:%d VCC_UVLO:%d\n", (data[0] >> 7) & 0x01, (data[0] >> 3) & 0x01, (data[0] >> 2) & 0x01, (data[0] >> 1) & 0x01, (data[0] >> 0) & 0x01);
}

void main_app() {
    static int deg = 0;
    rgb(rgb_t::OFF);
    // flash.writeFlashTest();
    can.init();

    PwmOut pwm(&htim1, TIM_CHANNEL_1, TIM_CHANNEL_2, TIM_CHANNEL_3);
    pwm.init();
    wake = true;
    printf("Hello World!\n");
    reset();
    HAL_Delay(1000);
    setBuckConverterVoltage(12);
    setNFault_status();
    HAL_Delay(1000);
    clear();
    while (1) {
        led_alive = !led_alive;
        deg += 60;
        // printf("Hello World!\n");
        float duty1 = MyMath::sinDeg(deg) * 0.1 + 0.5;
        float duty2 = MyMath::sinDeg(deg + 120) * 0.1 + 0.5;
        float duty3 = MyMath::sinDeg(deg + 240) * 0.1 + 0.5;
        pwm.write(duty1, duty2, duty3);
        wait_us(10);
        read_status();
        // pwm.write(0.6, 0.1, 0.1);
        // HAL_Delay(2);

        // pwm.write(0.6, 0.6, 0.1);
        // HAL_Delay(2);

        // pwm.write(0.1, 0.6, 0.1);
        // HAL_Delay(2);

        // pwm.write(0.1, 0.6, 0.6);
        // HAL_Delay(2);

        // pwm.write(0.1, 0.1, 0.6);
        // HAL_Delay(2);

        // pwm.write(0.6, 0.1, 0.6);
        // HAL_Delay(2);
        // printf("nfault: %d\n", nFault.read());
        // read_status();
        // HAL_Delay(50);
        // pwm.write(0.9, 0.9, 0.1);
        // printf("nfault: %d\n", nFault.read());
        // HAL_Delay(50);
        // pwm.write(0.1, 0.9, 0.1);
        // printf("nfault: %d\n", nFault.read());
        // HAL_Delay(50);
        // pwm.write(0.1, 0.9, 0.9);
        // printf("nfault: %d\n", nFault.read());
        // HAL_Delay(50);
        // pwm.write(0.1, 0.1, 0.9);
        // printf("nfault: %d\n", nFault.read());
        // HAL_Delay(50);
        // pwm.write(0.9, 0.1, 0.9);
        // printf("nfault: %d\n", nFault.read());
        // HAL_Delay(50);
        // printf("nfault: %d\n", nFault.read());
        // HAL_Delay(50);
        // clear();
        // setBuckConverterVoltage(12);
        // setNFault_status();
        // read_status();
        // HAL_Delay(10);
    }
}