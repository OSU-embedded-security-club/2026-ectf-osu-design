#ifndef ti_dl_dl_gpio__include
#define ti_dl_dl_gpio__include

#include <stdint.h>
#include <ti/devices/msp/msp.h>

#ifdef __cplusplus
extern "C" {
#endif

__STATIC_INLINE void DL_GPIO_reset(GPIO_Regs *gpio) { (void)gpio; }

__STATIC_INLINE void DL_GPIO_enablePower(GPIO_Regs *gpio) { (void)gpio; }

__STATIC_INLINE void DL_GPIO_clearPins(GPIO_Regs *gpio, uint32_t pins) {
  (void)gpio;
  (void)pins;
}

__STATIC_INLINE void DL_GPIO_enableOutput(GPIO_Regs *gpio, uint32_t pins) {
  (void)gpio;
  (void)pins;
}

/* Common GPIO defines/functions used in ti_msp_dl_config.c */
#define DL_GPIO_PIN_0 (0x00000001)
#define DL_GPIO_PIN_1 (0x00000002)
#define DL_GPIO_PIN_2 (0x00000004)
#define DL_GPIO_PIN_3 (0x00000008)
#define DL_GPIO_PIN_4 (0x00000010)
#define DL_GPIO_PIN_5 (0x00000020)
#define DL_GPIO_PIN_6 (0x00000040)
#define DL_GPIO_PIN_7 (0x00000080)
#define DL_GPIO_PIN_8 (0x00000100)
#define DL_GPIO_PIN_9 (0x00000200)
#define DL_GPIO_PIN_10 (0x00000400)
#define DL_GPIO_PIN_11 (0x00000800)
#define DL_GPIO_PIN_12 (0x00001000)
#define DL_GPIO_PIN_13 (0x00002000)
#define DL_GPIO_PIN_14 (0x00004000)
#define DL_GPIO_PIN_15 (0x00008000)

__STATIC_INLINE void DL_GPIO_initPeripheralOutputFunction(uint32_t iomux,
                                                          uint32_t func) {
  (void)iomux;
  (void)func;
}

__STATIC_INLINE void DL_GPIO_initPeripheralInputFunction(uint32_t iomux,
                                                         uint32_t func) {
  (void)iomux;
  (void)func;
}

#ifdef __cplusplus
}
#endif

#endif /* ti_dl_dl_gpio__include */
