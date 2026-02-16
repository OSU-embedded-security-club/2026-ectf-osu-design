#ifndef ti_devices_msp_msp__include
#define ti_devices_msp_msp__include

#include <stdint.h>

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#define __STATIC_INLINE static inline

typedef volatile struct {
  uint32_t dummy;
} GPIO_Regs;

typedef volatile struct {
  uint32_t dummy;
} UART_Regs;

typedef volatile struct {
  uint32_t dummy;
} TRNG_Regs;

typedef volatile struct {
  uint32_t SRAMFLASH;
  uint32_t res1[7];
  uint32_t DEVICEID;
  uint32_t USERID;
  uint32_t res2[2];
  uint32_t BSLPIN_UART;
  uint32_t BSLPIN_I2C;
  uint32_t BSLPIN_INVOKE;
  uint32_t res3[13];
  uint32_t TRACEID;
  uint32_t res4[5];
  uint32_t TEMP_SENSE0;
  uint32_t res5[11];
  uint32_t BOOTCRC;
} FACTORYREGION_OPEN_Regs;

#define GPIOA ((GPIO_Regs *)0x40001000)
#define GPIOB ((GPIO_Regs *)0x40002000)
#define GPIOC ((GPIO_Regs *)0x40003000)
#define UART0 ((UART_Regs *)0x40101000)
#define UART1 ((UART_Regs *)0x40102000)
#define TRNG ((TRNG_Regs *)0x40201000)
#define FACTORYREGION ((FACTORYREGION_OPEN_Regs *)0x41C40000)

#define IOMUX_PINCM19 (0)
#define IOMUX_PINCM20 (0)
#define IOMUX_PINCM25 (0)
#define IOMUX_PINCM26 (0)

#define IOMUX_PINCM19_PF_UART1_TX (0)
#define IOMUX_PINCM20_PF_UART1_RX (0)
#define IOMUX_PINCM25_PF_UART0_TX (0)
#define IOMUX_PINCM26_PF_UART0_RX (0)

#define UART0_INT_IRQn (15)

__STATIC_INLINE void NVIC_EnableIRQ(int32_t irqn) { (void)irqn; }

#ifdef __cplusplus
}
#endif

#endif /* ti_devices_msp_msp__include */
