/**
*     Copyright (c) 2023, Nations Technologies Inc.
* 
*     All rights reserved.
*
*     This software is the exclusive property of Nations Technologies Inc. (Hereinafter 
* referred to as NATIONS). This software, and the product of NATIONS described herein 
* (Hereinafter referred to as the Product) are owned by NATIONS under the laws and treaties
* of the People's Republic of China and other applicable jurisdictions worldwide.
*
*     NATIONS does not grant any license under its patents, copyrights, trademarks, or other 
* intellectual property rights. Names and brands of third party may be mentioned or referred 
* thereto (if any) for identification purposes only.
*
*     NATIONS reserves the right to make changes, corrections, enhancements, modifications, and 
* improvements to this software at any time without notice. Please contact NATIONS and obtain 
* the latest version of this software before placing orders.

*     Although NATIONS has attempted to provide accurate and reliable information, NATIONS assumes 
* no responsibility for the accuracy and reliability of this software.
* 
*     It is the responsibility of the user of this software to properly design, program, and test 
* the functionality and safety of any application made of this information and any resulting product. 
* In no event shall NATIONS be liable for any direct, indirect, incidental, special,exemplary, or 
* consequential damages arising in any way out of the use of this software or the Product.
*
*     NATIONS Products are neither intended nor warranted for usage in systems or equipment, any
* malfunction or failure of which may cause loss of human life, bodily injury or severe property 
* damage. Such applications are deemed, "Insecure Usage".
*
*     All Insecure Usage shall be made at user's risk. User shall indemnify NATIONS and hold NATIONS 
* harmless from and against all claims, costs, damages, and other liabilities, arising from or related 
* to any customer's Insecure Usage.

*     Any express or implied warranty with regard to this software or the Product, including,but not 
* limited to, the warranties of merchantability, fitness for a particular purpose and non-infringement
* are disclaimed to the fullest extent permitted by law.

*     Unless otherwise explicitly permitted by NATIONS, anyone may not duplicate, modify, transcribe
* or otherwise distribute this software for any purposes, in whole or in part.
*
*     NATIONS products and technologies shall not be used for or incorporated into any products or systems
* whose manufacture, use, or sale is prohibited under any applicable domestic or foreign laws or regulations. 
* User shall comply with any applicable export control laws and regulations promulgated and administered by 
* the governments of any countries asserting jurisdiction over the parties or transactions.
**/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DFU_MAL_H
#define __DFU_MAL_H

/* Includes ------------------------------------------------------------------*/
#include "usbhs_conf.h"

#include "usbd_conf.h"
#include "usbd_dfu_core.h"

/* Exported types ------------------------------------------------------------*/
typedef struct _DFU_MAL_PROP
{
  const uint8_t* pStrDesc;
  uint16_t (*pMAL_Init)     (void);   
  uint16_t (*pMAL_DeInit)   (void);   
  uint16_t (*pMAL_Erase)    (uint32_t Add);
  uint16_t (*pMAL_Write)    (uint32_t Add, uint32_t Len);
  uint8_t  *(*pMAL_Read)    (uint32_t Add, uint32_t Len);
  uint16_t (*pMAL_CheckAdd) (uint32_t Add);
  const uint32_t EraseTiming;
  const uint32_t WriteTiming;
}
DFU_MAL_Prop_TypeDef;


/* Exported constants --------------------------------------------------------*/
#define MAL_OK                          0
#define MAL_FAIL                        1

#define FLASH_2KB                     ((uint32_t)0x800)

/* useful macro ---------------------------------------------------------------*/
#define _1st_BYTE(x)  (uint8_t)((x)&0xFF)             /* 1st addressing cycle */
#define _2nd_BYTE(x)  (uint8_t)(((x)&0xFF00)>>8)      /* 2nd addressing cycle */
#define _3rd_BYTE(x)  (uint8_t)(((x)&0xFF0000)>>16)   /* 3rd addressing cycle */
#define _4th_BYTE(x)  (uint8_t)(((x)&0xFF000000)>>24) /* 4th addressing cycle */

/* Exported macro ------------------------------------------------------------*/
#define SET_POLLING_TIMING(x)   buffer[1] = _1st_BYTE(x);\
                                buffer[2] = _2nd_BYTE(x);\
                                buffer[3] = _3rd_BYTE(x);  

typedef enum 
{
    CMD_GET_INF             = 0x10,
    CMD_FLASH_ERASE         = 0x30, 
    CMD_FLASH_DWNLD         = 0x31, 
    CMD_DATA_CRC_CHECK      = 0x32, 
    CMD_SYS_RESET           = 0x50, 
    CMD_APP_GO              = 0x51
} CMD_H_NUM;

typedef enum 
{
    CMD_PAR_SUCCESS      = 0x00,
    CMD_PAR_FAILED       = 0x01,
    CMD_PAR_CRCERR       = 0x10,
    CMD_PAR_FLASHLIMET   = 0x0C,
    CMD_PAR_ADDERR       = 0x0D,
    CMD_PAR_LENERR       = 0x0E,
    CMD_PAR_PROGRAMERR   = 0x0F,
} CMD_RETURN_PAR;

typedef struct
{
    uint8_t CMD_H;
    uint8_t CMD_L;
    uint8_t LEN[2];
    uint8_t Par[4];
    uint8_t Data[256];
    uint8_t Flag;
} BOOTLOADER_CMD_Type;

typedef struct
{
    uint8_t CMD_H;
    uint8_t CMD_L;
    uint8_t LEN[2];
    uint8_t Data[256];
    uint8_t CR1;
    uint8_t CR2;
} BOOTLOADER_IC_CMD_Type;

/* Exported functions ------------------------------------------------------- */
void Bootloader_CMD(void);
void Bootloader_Return_CMD(void);

uint16_t MAL_Init (void);
uint16_t MAL_DeInit (void);
uint16_t MAL_Erase (uint32_t SectorAddress);
uint16_t MAL_Write (uint32_t SectorAddress, uint32_t DataLength);
uint8_t *MAL_Read  (uint32_t SectorAddress, uint32_t DataLength);
uint16_t MAL_GetStatus(uint32_t SectorAddress ,uint8_t Cmd, uint8_t *buffer);

extern uint8_t  MAL_Buffer[XFERSIZE]; /* RAM Buffer for Downloaded Data */
#endif /* __DFU_MAL_H */

