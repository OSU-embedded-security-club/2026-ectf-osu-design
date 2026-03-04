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
    
    // Reset UART
    DL_UART_reset(HOST_INST);
    DL_UART_reset(HSM_INST);
    
    // Configure System
    SYSCFG_DL_init();

    // Get the insecure stage1 hash rehashed as quickly as possible
    init_pin();

    // DL_AESADV_enablePower(AESADV);

    NVIC_SetPriority(DMA_INT_IRQn, 1);
    NVIC_SetPriority(HOST_INST_INT_IRQN, 3);

    NVIC_EnableIRQ(HOST_INST_INT_IRQN);
    NVIC_EnableIRQ(DMA_INT_IRQn);

    while(1) {
        // 4 Second Watchdog
        DL_WWDT_restart(WWDT0);

        // Poll UART
        if(!DL_UART_isRXFIFOEmpty(HOST_INST)) {
            message_header_t header;
            int result = message_header_request(HOST_INST, &header);

            if(result != 0) {
                continue;
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

        }
    }

}

void NMI_Handler()  {
    delay_cycles(PIN_DELAY);
    char msg[] = "NMI";
    message_header_send_error(HOST_INST, msg, sizeof(msg));
    DL_SYSCTL_resetDevice(DL_SYSCTL_RESET_POR);
}

void HardFault_Handler() {
    delay_cycles(PIN_DELAY);
    char msg[] = "HardFault";
    message_header_send_error(HOST_INST, msg, sizeof(msg));
 
    DL_SYSCTL_resetDevice(DL_SYSCTL_RESET_POR);
}

void HOST_INST_IRQHandler(void) {
    DL_UART_IIDX interrupt = DL_UART_getPendingInterrupt(HOST_INST);

    switch(interrupt) {
        case DL_UART_IIDX_BREAK_ERROR:
            delay_cycles(PIN_DELAY);
            const char break_msg[] = "UART Break Error";
            message_header_send_error(HOST_INST, break_msg, sizeof(break_msg));
            DL_SYSCTL_resetDevice(DL_SYSCTL_RESET_POR);
        break;
        case DL_UART_IIDX_FRAMING_ERROR:
            delay_cycles(PIN_DELAY);
            const char frame_msg[] = "UART Framing Error";
            message_header_send_error(HOST_INST, break_msg, sizeof(break_msg));
            DL_SYSCTL_resetDevice(DL_SYSCTL_RESET_POR);
        break;
        case DL_UART_IIDX_OVERRUN_ERROR:
            delay_cycles(PIN_DELAY);
            const char overrun_msg[] = "UART Overrun Error";
            message_header_send_error(HOST_INST, overrun_msg, sizeof(overrun_msg));
            DL_SYSCTL_resetDevice(DL_SYSCTL_RESET_POR);
        break;
        default:
            delay_cycles(PIN_DELAY);
            char msg[] = "UART Error";
            message_header_send_error(HOST_INST, msg, sizeof(msg));
            DL_SYSCTL_resetDevice(DL_SYSCTL_RESET_POR);
    }
}
