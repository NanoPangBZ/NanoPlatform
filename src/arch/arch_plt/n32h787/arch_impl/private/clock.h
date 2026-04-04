#pragma once

#include "n32h7xx.h"

#ifdef __cplusplus
extern "C" {
#endif

ErrorStatus SetSysClockToHSI(void);
ErrorStatus SetSysClockToHSE(void);
ErrorStatus SetSysClockToMSI(void);
ErrorStatus SetSysClockToPLL1(uint32_t PLL_source, uint64_t sysclk_freq);

#ifdef __cplusplus
}
#endif

