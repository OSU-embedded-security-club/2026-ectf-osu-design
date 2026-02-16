#ifndef ti_dl_m0p_dl_sysctl__include
#define ti_dl_m0p_dl_sysctl__include

#include <stdint.h>
#include <ti/devices/msp/msp.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DL_SYSCTL_BOR_THRESHOLD_LEVEL_0 (0)
#define DL_SYSCTL_SYSOSC_FREQ_BASE (0)
#define DL_SYSCTL_MCLK_DIVIDER_DISABLE (0)

#include <ti/driverlib/dl_log.h>

__STATIC_INLINE void DL_SYSCTL_setBORThreshold(uint32_t level) {
  DL_LOG_DEBUG("DL_SYSCTL_setBORThreshold: level: %u", level);
}

__STATIC_INLINE void DL_SYSCTL_setSYSOSCFreq(uint32_t freq) {
  DL_LOG_DEBUG("DL_SYSCTL_setSYSOSCFreq: freq: %u", freq);
}

__STATIC_INLINE void DL_SYSCTL_setMCLKDivider(uint32_t divider) {
  DL_LOG_DEBUG("DL_SYSCTL_setMCLKDivider: divider: %u", divider);
}

#ifdef __cplusplus
}
#endif

#endif /* ti_dl_m0p_dl_sysctl__include */
