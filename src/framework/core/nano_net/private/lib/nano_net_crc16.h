#pragma once

#ifdef __cplusplus
 extern "C" {
#endif 
#include <stdint.h>

uint16_t nano_net_crc16(const uint8_t *buf, int len);

#ifdef __cplusplus
}
#endif
