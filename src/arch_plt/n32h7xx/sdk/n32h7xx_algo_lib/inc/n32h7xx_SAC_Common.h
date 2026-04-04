/* Copyright 2022, Nations Technologies Inc. All rights reserved. */


#ifndef __SAC_Common_H
#define __SAC_Common_H
#include <stdint.h>
//#include "ns3610.h"

/** 
 * @file SAC_Common.h 
 * @brief Declaring registers related to SAC module.
 * @author Nations
 * @version V0.1
 * @date 7th Dec, 2022
 */

#define SDPU_BASE               (0x400F2000)
#define SDMA_BASE               (SDPU_BASE)//(AHB2PERIPH_BASE + 0x2000):0x400F2000
#define SAC_BASE                (SDPU_BASE + 0x1000)
#define RNG_BASE                (SDPU_BASE + 0x1080)
typedef struct
{
	 volatile uint32_t SACCR ;//0
	 volatile uint32_t SACSR;//4
	 volatile uint32_t RESERVED0;//8
	 volatile uint32_t SACSFR1;//C
}SAC_TypeDef;

typedef struct
{
	volatile  uint32_t SDMACR;//0
	 volatile uint32_t SDMACH0SRCADDR;//4
	volatile  uint32_t SDMACH1DSTADDR;//8
	volatile  uint32_t RESERVED0;//C
	 volatile uint32_t RESERVED1;//10
	 volatile uint32_t SDMACHCTRL;//14
	 volatile uint32_t SDMAINTSTATUS;//18
	 volatile uint32_t SDMAINTEN;//1C
	 volatile uint32_t SDMACRCVALUE0;//20
   volatile uint32_t SDMACRCVALUE1;//24	
}SDMA_TypeDef;

typedef struct
{
	 volatile uint32_t RNGCCTRL;//0
	 volatile uint32_t RNGSTATE;//4
	 volatile uint32_t RNGDATA1;//8
	 volatile uint32_t RNGDRNGSEED;//c	
	 volatile uint32_t RNGDRNGSEED1;//10
	 volatile uint32_t RNGINTOUTEN;//14
	
}RNG_TypeDef;

#define SAC                     ((SAC_TypeDef * ) SAC_BASE)   
#define SDMA                    ((SDMA_TypeDef * ) SDMA_BASE)
#define RNG                     ((RNG_TypeDef * ) RNG_BASE)
#define REG32( addr )			(*(volatile uint32_t *)(addr))
#define SACP1ADR                REG32(SDPU_BASE + 0x1040)
#define SACP2ADR                REG32(SDPU_BASE + 0x1044)
#define SACP3ADR                REG32(SDPU_BASE + 0x1048)
#define SACKADR                 REG32(SDPU_BASE + 0x104C)
#define SACIADR                 REG32(SDPU_BASE + 0x1050)
#define SACOADR                 REG32(SDPU_BASE + 0x1054)
#define SACCNTADR               REG32(SDPU_BASE + 0x1058)
#define RNGINTEN                REG32((SDPU_BASE + 0x1094))
#define SDPU_M7_EN              REG32 (0x580300B0)
#define RNG_En        REG32 (0x58030170)
#define AFEC_RNG      REG32 (0x58000434)


#endif