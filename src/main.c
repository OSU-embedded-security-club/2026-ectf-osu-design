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

    DL_AESADV_disablePower(AESADV);

    NVIC_SetPriority(DMA_INT_IRQn, 1);
    NVIC_SetPriority(HOST_INST_INT_IRQN, 3);

    NVIC_EnableIRQ(HOST_INST_INT_IRQN);
    NVIC_EnableIRQ(DMA_INT_IRQn);

    DL_UART_setRXFIFOThreshold(HOST_INST, DL_UART_RX_FIFO_LEVEL_ONE_ENTRY);
    DL_UART_enableInterrupt(HOST_INST, DL_UART_INTERRUPT_RX);

}

// void Reset_Handler() {
//     while(1) {}
// }

void NMI_Handler()  {
    while(1) {}
}

void HardFault_Handler() {
    while(1) {}
}

void SVC_Handler() {
    while(1) {}
}

void PendSV_Handler() {
    while(1) {}
}



void HOST_INST_IRQHandler(void) {
    // TODO: Set watchdog to ensure reset if header gets hungup
    message_header_t header;
    int result = message_header_request(HOST_INST, &header);

    if(result != 0) {
        DL_UART_clearInterruptStatus(HOST_INST, DL_UART_INTERRUPT_RX);
        return;
    }

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
            break;
    };

    // Don't Clear Interrupt until done processing
    DL_UART_clearInterruptStatus(HOST_INST, DL_UART_INTERRUPT_RX);

}
