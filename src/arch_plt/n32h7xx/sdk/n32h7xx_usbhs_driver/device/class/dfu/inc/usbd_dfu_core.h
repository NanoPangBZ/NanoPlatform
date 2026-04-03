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

#ifndef __USB_DFU_CORE_H_
#define __USB_DFU_CORE_H_

#include  "usbd_ioreq.h"
#include  "usbd_dfu_mal.h"


#define USB_DFU_CONFIG_DESC_SIZ       (18 + (9 * USBD_ITF_MAX_NUM))
#define USB_DFU_DESC_SIZ              9

#define DFU_DESCRIPTOR_TYPE           0x21


/**************************************************/
/* DFU Requests  DFU states                       */
/**************************************************/


#define STATE_appIDLE                 0
#define STATE_appDETACH               1
#define STATE_dfuIDLE                 2
#define STATE_dfuDNLOAD_SYNC          3
#define STATE_dfuDNBUSY               4
#define STATE_dfuDNLOAD_IDLE          5
#define STATE_dfuMANIFEST_SYNC        6
#define STATE_dfuMANIFEST             7
#define STATE_dfuMANIFEST_WAIT_RESET  8
#define STATE_dfuUPLOAD_IDLE          9
#define STATE_dfuERROR                10

/**************************************************/
/* DFU Requests  DFU status                       */
/**************************************************/

#define STATUS_OK                   0x00
#define STATUS_ERRTARGET            0x01
#define STATUS_ERRFILE              0x02
#define STATUS_ERRWRITE             0x03
#define STATUS_ERRERASE             0x04
#define STATUS_ERRCHECK_ERASED      0x05
#define STATUS_ERRPROG              0x06
#define STATUS_ERRVERIFY            0x07
#define STATUS_ERRADDRESS           0x08
#define STATUS_ERRNOTDONE           0x09
#define STATUS_ERRFIRMWARE          0x0A
#define STATUS_ERRVENDOR            0x0B
#define STATUS_ERRUSBR              0x0C
#define STATUS_ERRPOR               0x0D
#define STATUS_ERRUNKNOWN           0x0E
#define STATUS_ERRSTALLEDPKT        0x0F

/**************************************************/
/* DFU Requests  DFU states Manifestation State   */
/**************************************************/

#define Manifest_complete           0x00
#define Manifest_In_Progress        0x01


/**************************************************/
/* Special Commands  with Download Request        */
/**************************************************/

#define CMD_GETCOMMANDS              0x00
#define CMD_SETADDRESSPOINTER        0x21
#define CMD_ERASE                    0x41

/**************************************************/
/* Other defines                                  */
/**************************************************/
/* Bit Detach capable = bit 3 in bmAttributes field */
#define DFU_DETACH_MASK              (uint8_t)(1 << 4) 
/**
  * @}
  */ 


/** @defgroup USBD_CORE_Exported_TypesDefinitions
  * @{
  */
/**************************************************/
/* DFU Requests                                   */
/**************************************************/

typedef enum _DFU_REQUESTS {
  DFU_DETACH = 0,
  DFU_DNLOAD = 1,
  DFU_UPLOAD,
  DFU_GETSTATUS,
  DFU_CLRSTATUS,
  DFU_GETSTATE,
  DFU_ABORT
} DFU_REQUESTS;

typedef  void (*pFunction)(void);
/**
  * @}
  */ 



/** @defgroup USBD_CORE_Exported_Macros
  * @{
  */ 
/**********  Descriptor of DFU interface 0 Alternate setting n ****************/  
#define USBD_DFU_IF_DESC(n)   0x09,   /* bLength: Interface Descriptor size */ \
                              USB_INTERFACE_DESCRIPTOR_TYPE,   /* bDescriptorType */ \
                              0x00,   /* bInterfaceNumber: Number of Interface */ \
                              (n),      /* bAlternateSetting: Alternate setting */ \
                              0x00,   /* bNumEndpoints*/ \
                              0xFE,   /* bInterfaceClass: Application Specific Class Code */ \
                              0x01,   /* bInterfaceSubClass : Device Firmware Upgrade Code */ \
                              0x02,   /* nInterfaceProtocol: DFU mode protocol */ \
                              USBD_IDX_INTERFACE_STR + (n) + 1 /* iInterface: Index of string descriptor */ \
                              /* 18 */
  
/**
  * @}
  */ 

/** @defgroup USBD_CORE_Exported_Variables
  * @{
  */ 

extern USBD_Class_cb_TypeDef  USBD_DFU_cb;
/**
  * @}
  */ 

/** @defgroup USB_CORE_Exported_Functions
  * @{
  */
/**
  * @}
  */ 

#endif /* __USB_DFU_CORE_H_ */
