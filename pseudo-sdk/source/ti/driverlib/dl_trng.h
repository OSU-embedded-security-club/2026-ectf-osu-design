#ifndef ti_dl_dl_trng__include
#define ti_dl_dl_trng__include

#include <stdbool.h>
#include <stdint.h>
#include <ti/devices/msp/msp.h>

__STATIC_INLINE bool DL_TRNG_isCaptureReady(const TRNG_Regs *trng) {
  (void)trng;
  return true;
}

__STATIC_INLINE uint32_t DL_TRNG_getCapture(const TRNG_Regs *trng) {
  (void)trng;
  return 0;
}

#ifdef __cplusplus
extern "C" {
#endif

#define DL_TRNG_CLOCK_DIVIDE_2 (0)
#define DL_TRNG_CMD_NORM_FUNC (0)
#define DL_TRNG_INTERRUPT_CMD_DONE_EVENT (0)
#define DL_TRNG_DECIMATION_RATE_8 (0)

__STATIC_INLINE void DL_TRNG_enablePower(TRNG_Regs *trng) { (void)trng; }

__STATIC_INLINE void DL_TRNG_reset(TRNG_Regs *trng) { (void)trng; }

__STATIC_INLINE void DL_TRNG_setClockDivider(TRNG_Regs *trng,
                                             uint32_t divider) {
  (void)trng;
  (void)divider;
}

__STATIC_INLINE void DL_TRNG_sendCommand(TRNG_Regs *trng, uint32_t cmd) {
  (void)trng;
  (void)cmd;
}

__STATIC_INLINE uint32_t DL_TRNG_isCommandDone(TRNG_Regs *trng) {
  (void)trng;
  return 1;
}

__STATIC_INLINE void DL_TRNG_clearInterruptStatus(TRNG_Regs *trng,
                                                  uint32_t status) {
  (void)trng;
  (void)status;
}

__STATIC_INLINE void DL_TRNG_setDecimationRate(TRNG_Regs *trng, uint32_t rate) {
  (void)trng;
  (void)rate;
}

#ifdef __cplusplus
}
#endif

#endif /* ti_dl_dl_trng__include */
