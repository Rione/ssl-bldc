#include "app.hpp"

Serial uart2(&huart2, 64);
PwmOut pwm1(&pin_PWM1_A, &pin_PWM1_B, &pin_PWM1_C);
AS5048A encoder1(&hspi1, CS1_GPIO_Port, CS1_Pin);

DigitalOut led_blue(led_blue_GPIO_Port, led_blue_Pin);
DigitalOut led_red(led_red_GPIO_Port, led_red_Pin);
DigitalOut led_green(led_green_GPIO_Port, led_green_Pin);
DigitalOut led_alive(led_alive_GPIO_Port, led_alive_Pin);

// CAN
CAN_RxHeaderTypeDef RxHeader;
CAN_TxHeaderTypeDef TxHeader;

uint8_t TxData[8];
uint8_t RxData[8];
uint32_t TxMailbox;
uint8_t cnt;

typedef union {
    int16_t data;
    uint8_t split[2];
} int16Splitter;

int16Splitter vel[4];
uint8_t velIndex = 0;                 // 4つが最大
uint8_t velInt8Index = 0;             // 4 * 2 = 8が最大
uint8_t motorId = Constrain(2, 1, 4); // 1 ~ 4

uint32_t id;
uint32_t dlc;
uint8_t data[8];

void recvRx() {
    /*
    0. ヘッダを受信
    1. vel[0]の 1byte目を受信
    2. vel[0]の 2byte目を受信
    3. vel[1]の 1byte目を受信
    4. vel[1]の 2byte目を受信
    5. vel[2]の 1byte目を受信
    6. vel[2]の 2byte目を受信
    7. vel[3]の 1byte目を受信
    8. vel[3]の 2byte目を受信
    9. チェックサムを受信
     */
    const uint8_t HEADER = 0xAA; // 仮のヘッダバイト
    static bool headerReceived = false;
    static uint8_t sum = 0;

    if (uart2.available()) {
        uint8_t receivedByte = uart2.getc();

        printf("headerReceived:%d, velInt8Index:%d receivedByte:%d\n", headerReceived, velInt8Index, receivedByte);
        if (!headerReceived) {
            velIndex = 0;
            velInt8Index = 0;
            if (receivedByte == HEADER) {
                // ヘッダを受信したら速度データの受信を開始
                headerReceived = true;
                printf("header received %d\n ", receivedByte);
            } else {
                headerReceived = false;
                printf("error: Header is not received %d\n", receivedByte);
            }
        } else { // ヘッダを受信した後の処理
            if (velInt8Index < 8) {
                // 速度データ受信
                vel[velIndex].split[velInt8Index % 2] = receivedByte;
                velInt8Index++;
                velIndex = velInt8Index / 2;

                // checksum
                sum += receivedByte;
            } else { // 速度データ受信完了
                // チェックサムの処理
                uint8_t checksum = receivedByte;
                if (sum == checksum) {
                    // 受信成功
                    printf("success: Check sum match %d == %d\n", sum, checksum);
                    printf("motorId:%d vel[0]:%d\tvel[1]:%d\tvel[2]:%d\tvel[3]:%d\n", motorId, vel[0].data, vel[1].data, vel[2].data, vel[3].data);

                    int16_t v = vel[motorId - 1].data;
                    printf("%d\n", v);
                    // BLDC.setVelocity(v);
                } else {
                    // 受信失敗
                    printf("error: Check sum doesn't match %d != %d\n", sum, checksum);
                }

                headerReceived = false; // 次のヘッダを待つ準備
            }
        }
    }
}

void setup() {
    printf("Hello World!\n");
    HAL_Delay(100);

    Flash_EEPROM flash;
    flash.writeFlashTest();

    HAL_Delay(1000);
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
    CAN_RxHeaderTypeDef RxHeader;
    uint8_t RxData[8];
    if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, RxData) == HAL_OK) {
        id = (RxHeader.IDE == CAN_ID_STD) ? RxHeader.StdId : RxHeader.ExtId; // ID
        dlc = RxHeader.DLC;                                                  // DLC
        data[0] = RxData[0];                                                 // Data
        data[1] = RxData[1];
        data[2] = RxData[2];
        data[3] = RxData[3];
        data[4] = RxData[4];
        data[5] = RxData[5];
        data[6] = RxData[6];
        data[7] = RxData[7];
    }
}
void main_app() {
    setup();
    HAL_CAN_Start(&hcan1);

    CAN_FilterTypeDef filter;
    filter.FilterIdHigh = 0;                        // フィルターID(上位16ビット)
    filter.FilterIdLow = 0;                         // フィルターID(下位16ビット)
    filter.FilterMaskIdHigh = 0;                    // フィルターマスク(上位16ビット)
    filter.FilterMaskIdLow = 0;                     // フィルターマスク(下位16ビット)
    filter.FilterScale = CAN_FILTERSCALE_32BIT;     // フィルタースケール
    filter.FilterFIFOAssignment = CAN_FILTER_FIFO0; // フィルターに割り当てるFIFO
    filter.FilterBank = 0;                          // フィルターバンクNo
    filter.FilterMode = CAN_FILTERMODE_IDMASK;      // フィルターモード
    filter.SlaveStartFilterBank = 14;               // スレーブCANの開始フィルターバンクNo
    filter.FilterActivation = ENABLE;               // フィルター無効／有効
    HAL_CAN_ConfigFilter(&hcan1, &filter);
    if (HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK) Error_Handler();

    while (1) {
        // printf(" id=%ld %3d %3d %3d %3d %3d %3d %3d %3d \n", RxHeader.StdId, RxData[0], RxData[1], RxData[2], RxData[3], RxData[4], RxData[5], RxData[6], RxData[7]);
        // HAL_Delay(100);
        uint8_t TxData[8];
        if (0 < HAL_CAN_GetTxMailboxesFreeLevel(&hcan1)) {
            TxHeader.StdId = 0x555;                // CAN ID
            TxHeader.RTR = CAN_RTR_DATA;           // フレームタイプはデータフレーム
            TxHeader.IDE = CAN_ID_STD;             // 標準ID(11ﾋﾞｯﾄ)
            TxHeader.DLC = 8;                      // データ長は8バイトに
            TxHeader.TransmitGlobalTime = DISABLE; // ???
            TxData[0] = 0x11;
            TxData[1] = 0x22;
            TxData[2] = 0x33;
            TxData[3] = 0x44;
            TxData[4] = 0x55;
            TxData[5] = 0x66;
            TxData[6] = 0x77;
            TxData[7] = 0x88;
            HAL_CAN_AddTxMessage(&hcan1, &TxHeader, TxData, &TxMailbox);
        }
        HAL_Delay(100);
        printf("Send\n");
    }
}