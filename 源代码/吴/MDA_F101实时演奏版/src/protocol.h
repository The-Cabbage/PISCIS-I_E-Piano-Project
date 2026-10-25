#pragma once
#include <stdint.h>
#include <string.h>
#define RT_FRAMES 441
#define RT_REQUEST 144
#define RT_PCM_BYTES (RT_FRAMES*4)
#define RT_RESPONSE (16+RT_PCM_BYTES+4)
static inline uint32_t rt_crc(const uint8_t *p,uint32_t n){uint32_t c=~0u;while(n--){c^=*p++;for(int b=0;b<8;++b)c=(c>>1)^(0xedb88320u&(0u-(c&1)));}return ~c;}
static inline uint32_t rt_u32(const uint8_t *p){return (uint32_t)p[0]|((uint32_t)p[1]<<8)|((uint32_t)p[2]<<16)|((uint32_t)p[3]<<24);}
static inline uint16_t rt_u16(const uint8_t *p){return (uint16_t)p[0]|((uint16_t)p[1]<<8);}
static inline void rt_put32(uint8_t *p,uint32_t v){for(int i=0;i<4;++i)p[i]=(uint8_t)(v>>(8*i));}
static inline void rt_put16(uint8_t *p,uint16_t v){p[0]=(uint8_t)v;p[1]=(uint8_t)(v>>8);}
static inline int rt_valid_request(const uint8_t *p){return !memcmp(p,"MDRQ",4)&&rt_u16(p+8)==RT_FRAMES&&p[10]<=1&&p[11]<=1&&rt_crc(p,RT_REQUEST-4)==rt_u32(p+RT_REQUEST-4);}
static inline int rt_valid_response(const uint8_t *p,uint32_t seq){return !memcmp(p,"MDRS",4)&&rt_u32(p+4)==seq&&rt_u16(p+8)==RT_FRAMES&&rt_crc(p,RT_RESPONSE-4)==rt_u32(p+RT_RESPONSE-4);}
