#ifndef ti_dl_dl_trng__include
#define ti_dl_dl_trng__include

#include <stdbool.h>
#include <stdint.h>
#include <ti/devices/msp/msp.h>

#include <ti/driverlib/dl_log.h>

__STATIC_INLINE bool DL_TRNG_isCaptureReady(const TRNG_Regs *trng) {
  DL_LOG_DEBUG("DL_TRNG_isCaptureReady: %p", trng);
  return true;
}

__STATIC_INLINE uint32_t DL_TRNG_getCapture(const TRNG_Regs *trng) {
  DL_LOG_DEBUG("DL_TRNG_getCapture: %p", trng);
  return 0x12345678;
}

#ifdef __cplusplus
extern "C" {
#endif

#define DL_TRNG_CLOCK_DIVIDE_2 (0)
#define DL_TRNG_CMD_NORM_FUNC (0)
#define DL_TRNG_INTERRUPT_CMD_DONE_EVENT (0)
#define DL_TRNG_DECIMATION_RATE_8 (0)

__STATIC_INLINE void DL_TRNG_enablePower(TRNG_Regs *trng) {
  DL_LOG_DEBUG("DL_TRNG_enablePower: %p", trng);
}

__STATIC_INLINE void DL_TRNG_reset(TRNG_Regs *trng) {
  DL_LOG_DEBUG("DL_TRNG_reset: %p", trng);
}

__STATIC_INLINE void DL_TRNG_setClockDivider(TRNG_Regs *trng,
                                             uint32_t divider) {
  DL_LOG_DEBUG("DL_TRNG_setClockDivider: %p, divider: %u", trng, divider);
}

__STATIC_INLINE void DL_TRNG_sendCommand(TRNG_Regs *trng, uint32_t cmd) {
  DL_LOG_DEBUG("DL_TRNG_sendCommand: %p, cmd: %u", trng, cmd);
}

__STATIC_INLINE uint32_t DL_TRNG_isCommandDone(TRNG_Regs *trng) {
  DL_LOG_DEBUG("DL_TRNG_isCommandDone: %p", trng);
  return 1;
}

__STATIC_INLINE void DL_TRNG_clearInterruptStatus(TRNG_Regs *trng,
                                                  uint32_t status) {
  DL_LOG_DEBUG("DL_TRNG_clearInterruptStatus: %p, status: 0x%08x", trng,
               status);
}

__STATIC_INLINE void DL_TRNG_setDecimationRate(TRNG_Regs *trng, uint32_t rate) {
  DL_LOG_DEBUG("DL_TRNG_setDecimationRate: %p, rate: %u", trng, rate);
}

#ifdef __cplusplus
}
#endif

#endif /* ti_dl_dl_trng__include */
