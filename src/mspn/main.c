#include "sysconfig/ti_msp_dl_config.h"
#include "impl.h"


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


}

