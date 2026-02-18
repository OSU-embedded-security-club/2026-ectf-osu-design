#include <ti_msp_dl_config.h>

#include "utils.h"

// #include "secrets.h"
#include "message/header.h"
#include "message/list.h"
#include "message/read.h"
#include "message/write.h"
#include "message/listen.h"
#include "message/interrogate.h"
#include "message/receive.h"
#include "pin_utils.h"


//! Program Entrypoint
int main(void) {
    
    // Configure System
    SYSCFG_DL_init();
    // Get the insecure stage1 hash rehashed as quickly as possible
    init_pin();

    // Setup LED pin
    DL_GPIO_clearPins(GPIOB, DL_GPIO_PIN_14);
    DL_GPIO_enableOutput(GPIOB, DL_GPIO_PIN_14);

    NVIC_EnableIRQ(HOST_INST_INT_IRQN);
    DL_UART_enableInterrupt(HOST_INST, DL_UART_INTERRUPT_RX);

    char msg[] = "Initalized\n";
    for(int i = 0; i < sizeof(msg); i++) {
        DL_UART_transmitDataBlocking(HOST_INST, msg[i]);
    }

}

// This doesn't work yet
void HOST_INST_IRQHandler(void) {
    
    if(DL_UART_receiveData(HOST_INST) != '%') {
        DL_UART_clearInterruptStatus(HOST_INST, DL_UART_INTERRUPT_RX);
        return;
    }

    // Disable UART Interrupt until done processing current Interrupt
    // NVIC_DisableIRQ(HOST_INST_INT_IRQN);

    // TODO: Set watchdog to ensure reset if header gets hungup

    message_header_t header = message_header_request(HOST_INST);

    switch(header.operation) {
        case MESSAGE_LIST:
            message_list_response(header);
            break;
        case MESSAGE_READ:
            message_read_response(header);
            break;
        case MESSAGE_WRITE:
            message_write_response(header);
            break;
        case MESSAGE_RECEIVE:
            message_recieve(header);
            break;
        case MESSAGE_INTERROGATE:
            message_interrogate(header);
            break;
        case MESSAGE_LISTEN:
            message_listen(header);
            break;
        default:
            char err_msg[] = "Operation not Supported";
            message_header_send_error(HOST_INST, err_msg, sizeof(err_msg));
            break;
    };

    // Don't Clear Interrupt until done processing
    DL_UART_clearInterruptStatus(HOST_INST, DL_UART_INTERRUPT_RX);

}
