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

__STATIC_INLINE void DL_SYSCTL_setBORThreshold(uint32_t level) { (void)level; }

__STATIC_INLINE void DL_SYSCTL_setSYSOSCFreq(uint32_t freq) { (void)freq; }

__STATIC_INLINE void DL_SYSCTL_setMCLKDivider(uint32_t divider) {
  (void)divider;
}

#ifdef __cplusplus
}
#endif

#endif /* ti_dl_m0p_dl_sysctl__include */
