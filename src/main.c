#include "sysconfig/ti_msp_dl_config.h"


//! Program Entrypoint
int main(void) {
    
    // Configure System
    SYSCFG_DL_init();

    // Setup LED pin
    DL_GPIO_clearPins(GPIOB, DL_GPIO_PIN_14);
    DL_GPIO_enableOutput(GPIOB, DL_GPIO_PIN_14);

    char msg[] = "Initalized\n";
    for(int i = 0; i < sizeof(msg); i++) {
        DL_UART_transmitDataBlocking(HOST_INST, msg[i]);
    }

    // Blink LED
    while(true) {
        DL_GPIO_togglePins(GPIOB, DL_GPIO_PIN_14);

        delay_cycles(10000 * 32000);
    }

}

// This doesn't work yet
void HOST_INST_IRQHandler(void) {
    uint8_t buf[4];
    DL_UART_drainRXFIFO(HOST_INST, buf, sizeof(buf));

    for(int i = 0; i < sizeof(buf); i++) {
        DL_UART_transmitDataBlocking(HOST_INST, buf[i]);
    }
}