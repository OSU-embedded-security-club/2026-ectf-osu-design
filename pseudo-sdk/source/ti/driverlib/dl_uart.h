#ifndef ti_dl_dl_uart__include
#define ti_dl_dl_uart__include

#include <stdint.h>
#include <ti/devices/msp/msp.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  uint32_t clockSel;
  uint32_t divideRatio;
} DL_UART_ClockConfig;

typedef struct {
  uint32_t mode;
  uint32_t direction;
  uint32_t flowControl;
  uint32_t parity;
  uint32_t wordLength;
  uint32_t stopBits;
} DL_UART_Config;

#define DL_UART_MAIN_CLOCK_BUSCLK (0)
#define DL_UART_MAIN_CLOCK_DIVIDE_RATIO_1 (0)
#define DL_UART_MAIN_MODE_NORMAL (0)
#define DL_UART_MAIN_DIRECTION_TX_RX (0)
#define DL_UART_MAIN_FLOW_CONTROL_NONE (0)
#define DL_UART_MAIN_PARITY_NONE (0)
#define DL_UART_MAIN_WORD_LENGTH_8_BITS (0)
#define DL_UART_MAIN_STOP_BITS_ONE (0)
#define DL_UART_OVERSAMPLING_RATE_16X (0)
#define DL_UART_MAIN_INTERRUPT_RX (0)
#define DL_UART_RX_FIFO_LEVEL_FULL (0)
#define DL_UART_TX_FIFO_LEVEL_1_2_EMPTY (0)
#define DL_UART_TX_FIFO_LEVEL_EMPTY (0)

#define DL_UART_INTERRUPT_RX (1)

__STATIC_INLINE void DL_UART_enablePower(UART_Regs *uart) { (void)uart; }

__STATIC_INLINE void DL_UART_reset(UART_Regs *uart) { (void)uart; }

__STATIC_INLINE void DL_UART_transmitDataBlocking(UART_Regs *uart,
                                                  uint8_t data) {
  (void)uart;
  (void)data;
}

__STATIC_INLINE uint8_t DL_UART_receiveData(const UART_Regs *uart) {
  (void)uart;
  return 0;
}

__STATIC_INLINE uint8_t DL_UART_receiveDataBlocking(const UART_Regs *uart) {
  (void)uart;
  return 0;
}

__STATIC_INLINE void DL_UART_clearInterruptStatus(UART_Regs *uart,
                                                  uint32_t status) {
  (void)uart;
  (void)status;
}

__STATIC_INLINE void DL_UART_setClockConfig(UART_Regs *uart,
                                            DL_UART_ClockConfig *config) {
  (void)uart;
  (void)config;
}

__STATIC_INLINE void DL_UART_init(UART_Regs *uart, DL_UART_Config *config) {
  (void)uart;
  (void)config;
}

__STATIC_INLINE void DL_UART_setOversampling(UART_Regs *uart, uint32_t rate) {
  (void)uart;
  (void)rate;
}

__STATIC_INLINE void DL_UART_setBaudRateDivisor(UART_Regs *uart, uint32_t ibrd,
                                                uint32_t fbrd) {
  (void)uart;
  (void)ibrd;
  (void)fbrd;
}

__STATIC_INLINE void DL_UART_enableInterrupt(UART_Regs *uart,
                                             uint32_t interrupt) {
  (void)uart;
  (void)interrupt;
}

__STATIC_INLINE void DL_UART_enableFIFOs(UART_Regs *uart) { (void)uart; }

__STATIC_INLINE void DL_UART_setRXFIFOThreshold(UART_Regs *uart,
                                                uint32_t threshold) {
  (void)uart;
  (void)threshold;
}

__STATIC_INLINE void DL_UART_setTXFIFOThreshold(UART_Regs *uart,
                                                uint32_t threshold) {
  (void)uart;
  (void)threshold;
}

__STATIC_INLINE void DL_UART_enable(UART_Regs *uart) { (void)uart; }

#ifdef __cplusplus
}
#endif

#endif /* ti_dl_dl_uart__include */
