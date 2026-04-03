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

/* Includes ------------------------------------------------------------------*/
#include "usbd_dfu_mal.h"

#include "usbd_flash_if.h"
#include "n32h7xx_crc.h"

#ifdef DFU_MAL_SUPPORT_OTP
 #include "usbd_otp_if.h"
#endif

#ifdef DFU_MAL_SUPPORT_MEM
 #include "usbd_mem_if_template.h"
#endif

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Global Memories callback and string descriptors reference tables.
   To add a new memory, modify the value of MAX_USED_MEDIA in usbd_dfu_mal.h
   and add the pointer to the callback structure in this table.
   Then add the pointer to the memory string descriptor in usbd_dfu_StringDesc table.
   No other operation is required. */
DFU_MAL_Prop_TypeDef* tMALTab[MAX_USED_MEDIA] = 
{
    &DFU_Flash_cb
#ifdef DFU_MAL_SUPPORT_OTP
  , &DFU_Otp_cb
#endif
#ifdef DFU_MAL_SUPPORT_MEM
  , &DFU_Mem_cb
#endif
};

#ifdef USB_INTERNAL_DMA_ENABLED
  #if defined ( __ICCARM__ ) /*!< IAR Compiler */
    #pragma data_alignment=4   
  #endif
#endif /* USB_INTERNAL_DMA_ENABLED */

__ALIGN_BEGIN const uint8_t* usbd_dfu_StringDesc[MAX_USED_MEDIA] __ALIGN_END  = {
    FLASH_IF_STRING
#ifdef DFU_MAL_SUPPORT_OTP
  , OTP_IF_STRING
#endif
#ifdef DFU_MAL_SUPPORT_MEM
  , MEM_IF_STRING
#endif
};

#ifdef USB_INTERNAL_DMA_ENABLED
  #if defined ( __ICCARM__ ) /*!< IAR Compiler */
    #pragma data_alignment=4   
  #endif
#endif /* USB_INTERNAL_DMA_ENABLED */
/* RAM Buffer for Downloaded Data */
__ALIGN_BEGIN uint8_t  MAL_Buffer[XFERSIZE] __ALIGN_END ; 

/* Private function prototypes -----------------------------------------------*/
static uint8_t  MAL_CheckAdd  (uint32_t Add);
/* Private functions ---------------------------------------------------------*/


BOOTLOADER_CMD_Type bl_cmd;
BOOTLOADER_IC_CMD_Type ic_cmd;

#define P_UCID                    ((uint8_t *) 0x15000000)//((uint8_t *) 0x1FFFF7C0)
#define P_CHIP_ID                 ((uint8_t *) 0x15000010)//((uint8_t *) 0x1FFFF7F0)
#define P_IDCODE                  ((uint8_t *) 0x15000020)//((uint8_t *) 0x1FFFF7FC)
#define P_CHIP_SERIES             ((uint8_t *) 0x15000030)//((uint8_t *) 0x1FFFF7E0)

uint8_t cmd_return_par;
uint8_t Return_Par[6][2] = 
{
    {0xA0,0xB0},{0xE0,0x10},{0xE0,0x11},{0xE0,0x12},{0xE0,0x13},{0xBB,0xCC}
};

//uint8_t Return_Info_CR[25][2] __attribute__((at(0x1FFFF780)));

/**
  * @brief  MAL_Init
  *         Initializes the Media on the N32
  * @param  None
  * @retval Result of the operation (MAL_OK in all cases)
  */
uint16_t MAL_Init(void)
{
    uint32_t memIdx = 0;

    /* Init all supported memories */
    for(memIdx = 0; memIdx < MAX_USED_MEDIA; memIdx++)
    {
        /* If the check addres is positive, exit with the memory index */
        if (tMALTab[memIdx]->pMAL_Init != NULL)
        {
            tMALTab[memIdx]->pMAL_Init();
        }
    }

    return MAL_OK;
}

/**
  * @brief  MAL_DeInit
  *         DeInitializes the Media on the N32
  * @param  None
  * @retval Result of the operation (MAL_OK in all cases)
  */
uint16_t MAL_DeInit(void)
{
    uint32_t memIdx = 0;

    /* Init all supported memories */
    for(memIdx = 0; memIdx < MAX_USED_MEDIA; memIdx++)
    {
        /* Check if the command is supported */
        if (tMALTab[memIdx]->pMAL_DeInit != NULL)
        {
            tMALTab[memIdx]->pMAL_DeInit();
        }
    }

    return MAL_OK;
}

/**
  * @brief  MAL_Erase
  *         Erase a sector of memory.
  * @param  Add: Sector address/code
  * @retval Result of the operation: MAL_OK if all operations are OK else MAL_FAIL
  */
uint16_t MAL_Erase(uint32_t Add)
{
    uint32_t memIdx = MAL_CheckAdd(Add);

    /* Check if the area is protected */
    if (DFU_MAL_IS_PROTECTED_AREA(Add))
    {
        return MAL_FAIL;
    }    

    if (memIdx < MAX_USED_MEDIA)
    {
        /* Check if the command is supported */
        if (tMALTab[memIdx]->pMAL_Erase != NULL)
        {
            return tMALTab[memIdx]->pMAL_Erase(Add);
        }
        else
        {
            return MAL_FAIL;
        }
    }
    else
    {
        return MAL_FAIL;
    }
}

/**
  * @brief  MAL_Write
  *         Write sectors of memory.
  * @param  Add: Sector address/code
  * @param  Len: Number of data to be written (in bytes)
  * @retval Result of the operation: MAL_OK if all operations are OK else MAL_FAIL
  */
uint16_t MAL_Write (uint32_t Add, uint32_t Len)
{
    uint32_t memIdx = MAL_CheckAdd(Add);

    /* Check if the area is protected */
    if (DFU_MAL_IS_PROTECTED_AREA(Add))
    {
        return MAL_FAIL;
    }
    if (memIdx < MAX_USED_MEDIA)
    {
        /* Check if the command is supported */
        if (tMALTab[memIdx]->pMAL_Write != NULL)
        {
            return tMALTab[memIdx]->pMAL_Write(Add, Len);
        }
        else
        {
            return MAL_FAIL;
        }    
    }
    else
    {
        return MAL_FAIL;
    }
}

/**
  * @brief  MAL_Read
  *         Read sectors of memory.
  * @param  Add: Sector address/code
  * @param  Len: Number of data to be written (in bytes)
  * @retval Buffer pointer
  */
uint8_t *MAL_Read (uint32_t Add, uint32_t Len)
{
    uint32_t memIdx = MAL_CheckAdd(Add);

    if (memIdx < MAX_USED_MEDIA)
    {
        /* Check if the command is supported */
        if (tMALTab[memIdx]->pMAL_Read != NULL)
        {
            return tMALTab[memIdx]->pMAL_Read(Add, Len);
        }
        else
        {
            return MAL_Buffer;
        }     
    }
    else
    {
        return MAL_Buffer;
    }
}

/**
  * @brief  MAL_GetStatus
  *         Get the status of a given memory.
  * @param  Add: Sector address/code (allow to determine which memory will be addressed)
  * @param  Cmd: 0 for erase and 1 for write
  * @param  buffer: pointer to the buffer where the status data will be stored.
  * @retval Buffer pointer
  */
uint16_t MAL_GetStatus(uint32_t Add , uint8_t Cmd, uint8_t *buffer)
{
    uint32_t memIdx = MAL_CheckAdd(Add);

    if (memIdx < MAX_USED_MEDIA)
    {
        if (Cmd & 0x01)
        {
            SET_POLLING_TIMING(tMALTab[memIdx]->EraseTiming);
        }
        else
        {
            SET_POLLING_TIMING(tMALTab[memIdx]->WriteTiming);
        }

        return MAL_OK;
    }
    else
    {
        return MAL_FAIL;
    }
}

/**
  * @brief  MAL_CheckAdd
  *         Determine which memory should be managed.
  * @param  Add: Sector address/code (allow to determine which memory will be addressed)
  * @retval Index of the addressed memory.
  */
static uint8_t  MAL_CheckAdd(uint32_t Add)
{
    uint32_t memIdx = 0;

    /* Check with all supported memories */
    for(memIdx = 0; memIdx < MAX_USED_MEDIA; memIdx++)
    {
        /* If the check addres is positive, exit with the memory index */
        if (tMALTab[memIdx]->pMAL_CheckAdd(Add) == MAL_OK)
        {
            return memIdx;
        }
    }
    /* If no memory found, return MAX_USED_MEDIA */
    return (MAX_USED_MEDIA);
}
