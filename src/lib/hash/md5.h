#ifndef MD5_H
#define MD5_H

#ifdef __cplusplus
 extern "C" {
#endif 

#include <stdint.h>

typedef struct {
    uint32_t count[2];
    uint32_t state[4];
    uint8_t buffer[64];
}md5_ctx_t;

#define F(x, y, z)  ((x & y) | (~x & z))
#define G(x, y, z)  ((x & z) | (y & ~z))
#define H(x, y, z)  (x ^ y ^ z)
#define I(x, y, z)  (y ^ (x | ~z))
#define ROTATE_LEFT(x, n)   ((x << n) | (x >> (32 - n)))
#define FF(a, b, c, d, x, s, ac) \
        { \
        a += F(b, c, d) + x + ac; \
        a = ROTATE_LEFT(a, s); \
        a += b; \
        }
#define GG(a, b, c, d, x, s, ac) \
        { \
        a += G(b, c, d) + x + ac; \
        a = ROTATE_LEFT(a, s); \
        a += b; \
        }
#define HH(a, b, c, d, x, s, ac) \
        { \
        a += H(b, c, d) + x + ac; \
        a = ROTATE_LEFT(a, s); \
        a += b; \
        }
#define II(a, b, c, d, x, s, ac) \
        { \
        a += I(b, c, d) + x + ac; \
        a = ROTATE_LEFT(a, s); \
        a += b; \
        }

void md5_init(md5_ctx_t *context);
void md5_update(md5_ctx_t *context, uint8_t *input, uint32_t inputlen);
void md5_final(md5_ctx_t *context, uint8_t digest[16]);
void md5_transform(uint32_t state[4], uint8_t block[64]);
void md5_encode(uint8_t *output, uint32_t *input, uint32_t len);
void md5_decode(uint32_t *output, uint8_t *input, uint32_t len);
void md5_calc(uint8_t *input, uint32_t inputlen, uint8_t digest[16]);

#ifdef __cplusplus
}
#endif
#endif
