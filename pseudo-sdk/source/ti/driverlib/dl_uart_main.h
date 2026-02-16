#ifndef ti_dl_dl_uart_main__include
#define ti_dl_dl_uart_main__include

#include <ti/driverlib/dl_uart.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef DL_UART_ClockConfig DL_UART_Main_ClockConfig;
typedef DL_UART_Config DL_UART_Main_Config;

#define DL_UART_Main_enablePower DL_UART_enablePower
#define DL_UART_Main_reset DL_UART_reset
#define DL_UART_Main_setClockConfig DL_UART_setClockConfig
#define DL_UART_Main_init DL_UART_init
#define DL_UART_Main_setOversampling DL_UART_setOversampling
#define DL_UART_Main_setBaudRateDivisor DL_UART_setBaudRateDivisor
#define DL_UART_Main_enableInterrupt DL_UART_enableInterrupt
#define DL_UART_Main_enableFIFOs DL_UART_enableFIFOs
#define DL_UART_Main_setRXFIFOThreshold DL_UART_setRXFIFOThreshold
#define DL_UART_Main_setTXFIFOThreshold DL_UART_setTXFIFOThreshold
#define DL_UART_Main_enable DL_UART_enable
#define DL_UART_Main_receiveData DL_UART_receiveData
#define DL_UART_Main_receiveDataBlocking DL_UART_receiveDataBlocking

#ifdef __cplusplus
}
#endif

#endif /* ti_dl_dl_uart_main__include */
