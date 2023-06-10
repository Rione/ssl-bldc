#include <mbed.h>

Serial pc(USBTX, USBRX, 230400); // tx, rx
DigitalOut led(PC_14);

DigitalOut red(PB_14);
DigitalOut green(PB_15);
DigitalOut blue(PB_13);

char counter = 0;

void rgb(bool r, bool g, bool b) {
    red = !r;
    green = !g;
    blue = !b;
}

int main() {
    while (1) {
        // flash rgb led with all patterns
        rgb(1, 0, 0);
        wait_ms(100);
        rgb(0, 1, 0); // pink
        wait_ms(100);
        rgb(0, 0, 1);
        wait_ms(100);
        rgb(1, 1, 0); // r
        wait_ms(100);
        rgb(1, 0, 1);
        wait_ms(100);
        rgb(0, 1, 1); // b
        wait_ms(100);
        rgb(1, 1, 1);
        led = !led;
        wait_ms(100);
    }
}


                   // // Creates a CAN interface connected to specific pins.

                   // @param rd read from transmitter
                   //     @param td transmit to transmitter

                   // #include "mbed.h"

                   //         Ticker ticker;
                   // DigitalOut led1(LED1);
                   // DigitalOut led2(LED2);
                   // // The constructor takes in RX, and TX pin respectively.
                   // // These pins, for this example, are defined in mbed_app.json
                   // CAN can1(MBED_CONF_APP_CAN1_RD, MBED_CONF_APP_CAN1_TD);
                   // CAN can2(MBED_CONF_APP_CAN2_RD, MBED_CONF_APP_CAN2_TD);

                   // char counter = 0;

                   // void send() {
                   //     if (can1.write(CANMessage(1337, &counter, 1))) {
                   //         printf("Message sent: %d\n", counter);
                   //         counter++;
                   //     }
                   //     led1 = !led1;
                   // }

                   // int main() {
                   //     ticker.attach(&send, 1);
                   //     CANMessage msg;
                   //     while (1) {
                   //         if (can2.read(msg)) {
                   //             printf("Message received: %d\n\n", msg.data[0]);
                   //             led2 = !led2;
                   //         }
                   //         wait(0.2);
                   //     }
                   // }