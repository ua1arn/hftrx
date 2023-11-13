/*
 * rza1xx_hal.h
 *
 *  Created on: May 5, 2021
 *      Author: gena
 */

#ifndef SRC_HAL_RZA1XX_HAL_H_
#define SRC_HAL_RZA1XX_HAL_H_

#include "hardware.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Exported types ------------------------------------------------------------*/

#if (USE_RTOS == 1U)
  /* Reserved for future use */
  #error "USE_RTOS should be 0 in the current HAL release"
#else
  #define __HAL_LOCK(__HANDLE__)                                           \
                                do{                                        \
                                    if((__HANDLE__)->Lock == HAL_LOCKED)   \
                                    {                                      \
                                       return HAL_BUSY;                    \
                                    }                                      \
                                    else                                   \
                                    {                                      \
                                       (__HANDLE__)->Lock = HAL_LOCKED;    \
                                    }                                      \
                                  }while (0U)

  #define __HAL_UNLOCK(__HANDLE__)                                          \
                                  do{                                       \
                                      (__HANDLE__)->Lock = HAL_UNLOCKED;    \
                                    }while (0U)
#endif /* USE_RTOS */

#if  defined ( __GNUC__ )
  #ifndef __weak
    #define __weak   __attribute__((weak))
  #endif /* __weak */
  #ifndef __packed
    #define __packed __attribute__((__packed__))
  #endif /* __packed */
#endif /* __GNUC__ */


/* Macro to get variable aligned on 4-bytes, for __ICCARM__ the directive "#pragma data_alignment=4" must be used instead */
#if defined   (__GNUC__)        /* GNU Compiler */
  #ifndef __ALIGN_END
	#define __ALIGN_END    __attribute__ ((aligned (64U)))
  #endif /* __ALIGN_END */
  #ifndef __ALIGN_BEGIN  
	#define __ALIGN_BEGIN
  #endif /* __ALIGN_BEGIN */
#else
  #ifndef __ALIGN_END
	#define __ALIGN_END
  #endif /* __ALIGN_END */
  #ifndef __ALIGN_BEGIN      
    #if defined   (__CC_ARM)      /* ARM Compiler */
		#define __ALIGN_BEGIN    __align(64U)
    #elif defined (__ICCARM__)    /* IAR Compiler */
      #define __ALIGN_BEGIN 
    #endif /* __CC_ARM */
  #endif /* __ALIGN_BEGIN */
#endif /* __GNUC__ */

/** 
  * @brief  __RAM_FUNC definition
  */
#if defined ( __CC_ARM   )
/* ARM Compiler
   ------------
   RAM functions are defined using the toolchain options.
   Functions that are executed in RAM should reside in a separate source module.
   Using the 'Options for File' dialog you can simply change the 'Code / Const'
   area of a module to a memory space in physical RAM.
   Available memory areas are declared in the 'Target' tab of the 'Options for Target'
   dialog.
*/
#define __RAM_FUNC

#elif defined ( __ICCARM__ )
/* ICCARM Compiler
   ---------------
   RAM functions are defined using a specific toolchain keyword "__ramfunc".
*/
#define __RAM_FUNC __ramfunc

#elif defined   (  __GNUC__  )
/* GNU Compiler
   ------------
  RAM functions are defined using a specific toolchain attribute
   "__attribute__((section(".RamFunc")))".
*/
#define __RAM_FUNC __attribute__((section(".RamFunc")))

#endif

/**
  * @brief  __NOINLINE definition
  */ 
#if defined ( __CC_ARM   ) || defined   (  __GNUC__  )
/* ARM & GNUCompiler 
   ---------------- 
*/
#define __NOINLINE __attribute__ ( (noinline) )

#elif defined ( __ICCARM__ )
/* ICCARM Compiler
   ---------------
*/
#define __NOINLINE _Pragma("optimize = no_inline")

#endif


/** 
  * @brief  HAL Status structures definition  
  */  
typedef enum 
{
  HAL_OK       = 0x00U,
  HAL_ERROR    = 0x01U,
  HAL_BUSY     = 0x02U,
  HAL_TIMEOUT  = 0x03U
} HAL_StatusTypeDef;

/** 
  * @brief  HAL Lock structures definition  
  */
typedef enum 
{
  HAL_UNLOCKED = 0x00U,
  HAL_LOCKED   = 0x01U  
} HAL_LockTypeDef;


#define UNUSED(x) ((void)(sizeof(x)))	/* To avoid gcc/g++ warnings */

#define HAL_MAX_DELAY      0xFFFFFFFFU

HAL_StatusTypeDef HAL_Init(void);
HAL_StatusTypeDef HAL_DeInit(void);
extern uint32_t SystemCoreClock;          /*!< System Clock Frequency (Core Clock) */
void HAL_Delay(uint32_t Delay);
void HAL_IncTick(void);
//HAL_StatusTypeDef HAL_InitTick (uint32_t TickPriority);
void HAL_MspInit(void);
void HAL_MspDeInit(void);



/** @defgroup USB_LL_EP_Type USB Low Layer EP Type
  * @{
  */
#define EP_TYPE_CTRL                           0U
#define EP_TYPE_ISOC                           1U
#define EP_TYPE_BULK                           2U
#define EP_TYPE_INTR                           3U
#define EP_TYPE_MSK                            3U
/**
  * @}
  */

#ifdef __cplusplus
}
#endif


#endif /* SRC_HAL_RZA1XX_HAL_H_ */
