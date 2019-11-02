/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
// STM32F42XX LCD-TFT Controller (LTDC)
// ILI9341 display driver chip
#include "hardware.h"

#include <stdint.h>
#include <string.h>

#include "formats.h"	// for debug prints

#if WITHSDRAMHW

#if CPUSTYLE_STM32F

#define assert_param(expr) do { } while (0)
/**
  * @brief  FMC SDRAM Bank address
  */   
  
/**
  * @brief  FMC SDRAM Memory Width
  */  
/* #define SDRAM_MEMORY_WIDTH   FMC_SDMemory_Width_8b  */
#define SDRAM_MEMORY_WIDTH    FMC_SDMemory_Width_16b 

/**
  * @brief  FMC SDRAM CAS Latency
  */  
/* #define SDRAM_CAS_LATENCY   FMC_CAS_Latency_2  */
#define SDRAM_CAS_LATENCY    FMC_CAS_Latency_3

/**
  * @brief  FMC SDRAM Memory clock period
  */  
#define SDCLOCK_PERIOD    FMC_SDClock_Period_2        /* Default configuration used with LCD */
/* #define SDCLOCK_PERIOD    FMC_SDClock_Period_3 */

/**
  * @brief  FMC SDRAM Memory Read Burst feature
  */  
#define SDRAM_READBURST    FMC_Read_Burst_Disable    /* Default configuration used with LCD */
/* #define SDRAM_READBURST    FMC_Read_Burst_Enable  */

/**
  * @brief  FMC SDRAM Bank Remap
  */    
/* #define SDRAM_BANK_REMAP */   



/**
 * @brief Uncomment the line below if you want to use user defined Delay function
 *        (for precise timing), otherwise default _delay_ function defined within
 *         this driver is used (less precise timing).  
 */
 
/* #define USE_Delay */

#ifdef USE_Delay
  #define __Delay     Delay      /*  User can provide more timing precise __Delay function
                                    (with 10ms time base), using SysTick for example */
#else
  #define __Delay     local_delay_ms      /*  Default __Delay function with less precise timing */
#endif

/**
  * @brief  FMC SDRAM Mode definition register defines
  */
#define SDRAM_MODEREG_BURST_LENGTH_1             ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_LENGTH_2             ((uint16_t)0x0001)
#define SDRAM_MODEREG_BURST_LENGTH_4             ((uint16_t)0x0002)
#define SDRAM_MODEREG_BURST_LENGTH_8             ((uint16_t)0x0004)
#define SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL      ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_TYPE_INTERLEAVED     ((uint16_t)0x0008)
#define SDRAM_MODEREG_CAS_LATENCY_2              ((uint16_t)0x0020)
#define SDRAM_MODEREG_CAS_LATENCY_3              ((uint16_t)0x0030)
#define SDRAM_MODEREG_OPERATING_MODE_STANDARD    ((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_PROGRAMMED ((uint16_t)0x0000) 
#define SDRAM_MODEREG_WRITEBURST_MODE_SINGLE     ((uint16_t)0x0200)      

/**
  * @}
  */  

/** @defgroup STM32429I_DISCO_SDRAM_Exported_Functions
  * @{
  */ 
void  SDRAM_Init(void);
void  SDRAM_GPIOConfig(void);
void  SDRAM_InitSequence(void);
void  SDRAM_WriteBuffer(uint32_t* pBuffer, uint32_t uwWriteAddress, uint32_t uwBufferSize);
void  SDRAM_ReadBuffer(uint32_t* pBuffer, uint32_t uwReadAddress, uint32_t uwBufferSize);

/* Exported types ------------------------------------------------------------*/  

/** 
  * @brief  Timing parameters For NOR/SRAM Banks  
  */
typedef struct
{
  uint32_t FMC_AddressSetupTime;       /*!< Defines the number of HCLK cycles to configure
                                             the duration of the address setup time. 
                                             This parameter can be a value between 0 and 15.
                                             @note This parameter is not used with synchronous NOR Flash memories. */

  uint32_t FMC_AddressHoldTime;        /*!< Defines the number of HCLK cycles to configure
                                             the duration of the address hold time.
                                             This parameter can be a value between 1 and 15. 
                                             @note This parameter is not used with synchronous NOR Flash memories.*/

  uint32_t FMC_DataSetupTime;          /*!< Defines the number of HCLK cycles to configure
                                             the duration of the data setup time.
                                             This parameter can be a value between 1 and 255.
                                             @note This parameter is used for SRAMs, ROMs and asynchronous multiplexed NOR Flash memories. */

  uint32_t FMC_BusTurnAroundDuration;  /*!< Defines the number of HCLK cycles to configure
                                             the duration of the bus turnaround.
                                             This parameter can be a value between 0 and 15.
                                             @note This parameter is only used for multiplexed NOR Flash memories. */

  uint32_t FMC_CLKDivision;            /*!< Defines the period of CLK clock output signal, expressed in number of HCLK cycles.
                                             This parameter can be a value between 1 and 15.
                                             @note This parameter is not used for asynchronous NOR Flash, SRAM or ROM accesses. */

  uint32_t FMC_DataLatency;            /*!< Defines the number of memory clock cycles to issue
                                             to the memory before getting the first data.
                                             The parameter value depends on the memory type as shown below:
                                              - It must be set to 0 in case of a CRAM
                                              - It is don't care in asynchronous NOR, SRAM or ROM accesses
                                              - It may assume a value between 0 and 15 in NOR Flash memories
                                                with synchronous burst mode enable */

  uint32_t FMC_AccessMode;             /*!< Specifies the asynchronous access mode. 
                                             This parameter can be a value of @ref FMC_Access_Mode */
}FMC_NORSRAMTimingInitTypeDef;

/** 
  * @brief  FMC NOR/SRAM Init structure definition
  */
typedef struct
{
  uint32_t FMC_Bank;                /*!< Specifies the NOR/SRAM memory bank that will be used.
                                          This parameter can be a value of @ref FMC_NORSRAM_Bank */

  uint32_t FMC_DataAddressMux;      /*!< Specifies whether the address and data values are
                                          multiplexed on the databus or not. 
                                          This parameter can be a value of @ref FMC_Data_Address_Bus_Multiplexing */

  uint32_t FMC_MemoryType;          /*!< Specifies the type of external memory attached to
                                          the corresponding memory bank.
                                          This parameter can be a value of @ref FMC_Memory_Type */

  uint32_t FMC_MemoryDataWidth;     /*!< Specifies the external memory device width.
                                          This parameter can be a value of @ref FMC_NORSRAM_Data_Width */

  uint32_t FMC_BurstAccessMode;     /*!< Enables or disables the burst access mode for Flash memory,
                                          valid only with synchronous burst Flash memories.
                                          This parameter can be a value of @ref FMC_Burst_Access_Mode */                                        

  uint32_t FMC_WaitSignalPolarity;  /*!< Specifies the wait signal polarity, valid only when accessing
                                          the Flash memory in burst mode.
                                          This parameter can be a value of @ref FMC_Wait_Signal_Polarity */

  uint32_t FMC_WrapMode;            /*!< Enables or disables the Wrapped burst access mode for Flash
                                          memory, valid only when accessing Flash memories in burst mode.
                                          This parameter can be a value of @ref FMC_Wrap_Mode */

  uint32_t FMC_WaitSignalActive;    /*!< Specifies if the wait signal is asserted by the memory one
                                          clock cycle before the wait state or during the wait state,
                                          valid only when accessing memories in burst mode. 
                                          This parameter can be a value of @ref FMC_Wait_Timing */

  uint32_t FMC_WriteOperation;      /*!< Enables or disables the write operation in the selected bank by the FMC. 
                                          This parameter can be a value of @ref FMC_Write_Operation */

  uint32_t FMC_WaitSignal;          /*!< Enables or disables the wait state insertion via wait
                                          signal, valid for Flash memory access in burst mode. 
                                          This parameter can be a value of @ref FMC_Wait_Signal */

  uint32_t FMC_ExtendedMode;        /*!< Enables or disables the extended mode.
                                          This parameter can be a value of @ref FMC_Extended_Mode */
  
  uint32_t FMC_AsynchronousWait;     /*!< Enables or disables wait signal during asynchronous transfers,
                                          valid only with asynchronous Flash memories.
                                          This parameter can be a value of @ref FMC_AsynchronousWait */  

  uint32_t FMC_WriteBurst;          /*!< Enables or disables the write burst operation.
                                          This parameter can be a value of @ref FMC_Write_Burst */ 

  uint32_t FMC_ContinousClock;       /*!< Enables or disables the FMC clock output to external memory devices.
                                          This parameter is only enabled through the FMC_BCR1 register, and don't care 
                                          through FMC_BCR2..4 registers.
                                          This parameter can be a value of @ref FMC_Continous_Clock */ 

  
  FMC_NORSRAMTimingInitTypeDef* FMC_ReadWriteTimingStruct; /*!< Timing Parameters for write and read access if the  Extended Mode is not used*/  

  FMC_NORSRAMTimingInitTypeDef* FMC_WriteTimingStruct;     /*!< Timing Parameters for write access if the  Extended Mode is used*/      
}FMC_NORSRAMInitTypeDef;

/** 
  * @brief  Timing parameters For FMC NAND and PCCARD Banks
  */
typedef struct
{
  uint32_t FMC_SetupTime;      /*!< Defines the number of HCLK cycles to setup address before
                                     the command assertion for NAND-Flash read or write access
                                     to common/Attribute or I/O memory space (depending on
                                     the memory space timing to be configured).
                                     This parameter can be a value between 0 and 255.*/

  uint32_t FMC_WaitSetupTime;  /*!< Defines the minimum number of HCLK cycles to assert the
                                     command for NAND-Flash read or write access to
                                     common/Attribute or I/O memory space (depending on the
                                     memory space timing to be configured). 
                                     This parameter can be a number between 0 and 255 */

  uint32_t FMC_HoldSetupTime;  /*!< Defines the number of HCLK clock cycles to hold address
                                     (and data for write access) after the command de-assertion
                                     for NAND-Flash read or write access to common/Attribute
                                     or I/O memory space (depending on the memory space timing
                                     to be configured).
                                     This parameter can be a number between 0 and 255 */

  uint32_t FMC_HiZSetupTime;   /*!< Defines the number of HCLK clock cycles during which the
                                     databus is kept in HiZ after the start of a NAND-Flash
                                     write access to common/Attribute or I/O memory space (depending
                                     on the memory space timing to be configured).
                                     This parameter can be a number between 0 and 255 */
}FMC_NAND_PCCARDTimingInitTypeDef;

/** 
  * @brief  FMC NAND Init structure definition
  */
typedef struct
{
  uint32_t FMC_Bank;              /*!< Specifies the NAND memory bank that will be used.
                                      This parameter can be a value of @ref FMC_NAND_Bank */

  uint32_t FMC_Waitfeature;      /*!< Enables or disables the Wait feature for the NAND Memory Bank.
                                       This parameter can be any value of @ref FMC_Wait_feature */

  uint32_t FMC_MemoryDataWidth;  /*!< Specifies the external memory device width.
                                       This parameter can be any value of @ref FMC_NAND_Data_Width */

  uint32_t FMC_ECC;              /*!< Enables or disables the ECC computation.
                                       This parameter can be any value of @ref FMC_ECC */

  uint32_t FMC_ECCPageSize;      /*!< Defines the page size for the extended ECC.
                                       This parameter can be any value of @ref FMC_ECC_Page_Size */

  uint32_t FMC_TCLRSetupTime;    /*!< Defines the number of HCLK cycles to configure the
                                       delay between CLE low and RE low.
                                       This parameter can be a value between 0 and 255. */

  uint32_t FMC_TARSetupTime;     /*!< Defines the number of HCLK cycles to configure the
                                       delay between ALE low and RE low.
                                       This parameter can be a number between 0 and 255 */ 

  FMC_NAND_PCCARDTimingInitTypeDef*  FMC_CommonSpaceTimingStruct;   /*!< FMC Common Space Timing */ 

  FMC_NAND_PCCARDTimingInitTypeDef*  FMC_AttributeSpaceTimingStruct; /*!< FMC Attribute Space Timing */
}FMC_NANDInitTypeDef;

/** 
  * @brief  FMC PCCARD Init structure definition
  */

typedef struct
{
  uint32_t FMC_Waitfeature;    /*!< Enables or disables the Wait feature for the Memory Bank.
                                    This parameter can be any value of @ref FMC_Wait_feature */

  uint32_t FMC_TCLRSetupTime;  /*!< Defines the number of HCLK cycles to configure the
                                     delay between CLE low and RE low.
                                     This parameter can be a value between 0 and 255. */

  uint32_t FMC_TARSetupTime;   /*!< Defines the number of HCLK cycles to configure the
                                     delay between ALE low and RE low.
                                     This parameter can be a number between 0 and 255 */ 

  
  FMC_NAND_PCCARDTimingInitTypeDef*  FMC_CommonSpaceTimingStruct; /*!< FMC Common Space Timing */

  FMC_NAND_PCCARDTimingInitTypeDef*  FMC_AttributeSpaceTimingStruct;  /*!< FMC Attribute Space Timing */ 
  
  FMC_NAND_PCCARDTimingInitTypeDef*  FMC_IOSpaceTimingStruct; /*!< FMC IO Space Timing */  
}FMC_PCCARDInitTypeDef;

/** 
  * @brief  Timing parameters for FMC SDRAM Banks
  */
  
typedef struct
{
  uint32_t FMC_LoadToActiveDelay;      /*!< Defines the delay between a Load Mode Register command and 
                                            an active or Refresh command in number of memory clock cycles.
                                            This parameter can be a value between 1 and 16. */
  
  uint32_t FMC_ExitSelfRefreshDelay;   /*!< Defines the delay from releasing the self refresh command to 
                                            issuing the Activate command in number of memory clock cycles.
                                            This parameter can be a value between 1 and 16. */
   
  uint32_t FMC_SelfRefreshTime;        /*!< Defines the minimum Self Refresh period in number of memory clock 
                                            cycles.
                                            This parameter can be a value between 1 and 16. */
                                            
  uint32_t FMC_RowCycleDelay;          /*!< Defines the delay between the Refresh command and the Activate command
                                            and the delay between two consecutive Refresh commands in number of 
                                            memory clock cycles.
                                            This parameter can be a value between 1 and 16. */
                                            
  uint32_t FMC_WriteRecoveryTime;      /*!< Defines the Write recovery Time in number of memory clock cycles.
                                            This parameter can be a value between 1 and 16. */
                                            
  uint32_t FMC_RPDelay;                /*!< Defines the delay between a Precharge Command and an other command 
                                            in number of memory clock cycles.
                                            This parameter can be a value between 1 and 16. */
                                            
  uint32_t FMC_RCDDelay;               /*!< Defines the delay between the Activate Command and a Read/Write command
                                            in number of memory clock cycles.
                                            This parameter can be a value between 1 and 16. */
                                            
}FMC_SDRAMTimingInitTypeDef;

/** 
  * @brief  Command parameters for FMC SDRAM Banks
  */


typedef struct
{
  uint32_t FMC_CommandMode;            /*!< Defines the command issued to the SDRAM device.
                                            This parameter can be a value of @ref FMC_Command_Mode. */
                                            
  uint32_t FMC_CommandTarget;          /*!< Defines which bank (1 or 2) the command will be issued to.
                                            This parameter can be a value of @ref FMC_Command_Target. */
                                            
  uint32_t FMC_AutoRefreshNumber;      /*!< Defines the number of consecutive auto refresh command issued
                                            in auto refresh mode.
                                            This parameter can be a value between 1 and 16. */                                           
                                                                                                             
  uint32_t FMC_ModeRegisterDefinition; /*!< Defines the SDRAM Mode register content */
  
}FMC_SDRAMCommandTypeDef;

/** 
  * @brief  FMC SDRAM Init structure definition
  */

typedef struct
{
  uint32_t FMC_Bank;                   /*!< Specifies the SDRAM memory bank that will be used.
                                          This parameter can be a value of @ref FMC_SDRAM_Bank */

  uint32_t FMC_ColumnBitsNumber;       /*!< Defines the number of bits of column address.
                                            This parameter can be a value of @ref FMC_ColumnBits_Number. */
                                            
  uint32_t FMC_RowBitsNumber;          /*!< Defines the number of bits of column address..
                                            This parameter can be a value of @ref FMC_RowBits_Number. */
                                            
  uint32_t FMC_SDMemoryDataWidth;        /*!< Defines the memory device width.
                                            This parameter can be a value of @ref FMC_SDMemory_Data_Width. */
                                            
  uint32_t FMC_InternalBankNumber;     /*!< Defines the number of bits of column address.
                                            This parameter can be of @ref FMC_InternalBank_Number. */
                                            
  uint32_t FMC_CASLatency;             /*!< Defines the SDRAM CAS latency in number of memory clock cycles.
                                            This parameter can be a value of @ref FMC_CAS_Latency. */
                                            
  uint32_t FMC_WriteProtection;        /*!< Enables the SDRAM bank to be accessed in write mode.
                                            This parameter can be a value of @ref FMC_Write_Protection. */
                                            
  uint32_t FMC_SDClockPeriod;          /*!< Define the SDRAM Clock Period for both SDRAM Banks and they allow to disable
                                            the clock before changing frequency.
                                            This parameter can be a value of @ref FMC_SDClock_Period. */
                                            
  uint32_t FMC_ReadBurst;              /*!< This bit enable the SDRAM controller to anticipate the next read commands 
                                            during the CAS latency and stores data in the Read FIFO.
                                            This parameter can be a value of @ref FMC_Read_Burst. */
                                            
  uint32_t FMC_ReadPipeDelay;          /*!< Define the delay in system clock cycles on read data path.
                                            This parameter can be a value of @ref FMC_ReadPipe_Delay. */
                                            
  FMC_SDRAMTimingInitTypeDef* FMC_SDRAMTimingStruct;   /*!< Timing Parameters for write and read access*/                                            
  
}FMC_SDRAMInitTypeDef;


/* Exported constants --------------------------------------------------------*/

/** @defgroup FMC_Exported_Constants
  * @{
  */ 

/** @defgroup FMC_NORSRAM_Bank 
  * @{
  */
#define FMC_Bank1_NORSRAM1                      ((uint32_t)0x00000000)
#define FMC_Bank1_NORSRAM2                      ((uint32_t)0x00000002)
#define FMC_Bank1_NORSRAM3                      ((uint32_t)0x00000004)
#define FMC_Bank1_NORSRAM4                      ((uint32_t)0x00000006)

#define IS_FMC_NORSRAM_BANK(BANK) (((BANK) == FMC_Bank1_NORSRAM1) || \
                                   ((BANK) == FMC_Bank1_NORSRAM2) || \
                                   ((BANK) == FMC_Bank1_NORSRAM3) || \
                                   ((BANK) == FMC_Bank1_NORSRAM4))
/**
  * @}
  */

/** @defgroup FMC_NAND_Bank 
  * @{
  */  
#define FMC_Bank2_NAND                          ((uint32_t)0x00000010)
#define FMC_Bank3_NAND                          ((uint32_t)0x00000100)

#define IS_FMC_NAND_BANK(BANK) (((BANK) == FMC_Bank2_NAND) || \
                                ((BANK) == FMC_Bank3_NAND))
/**
  * @}
  */

/** @defgroup FMC_PCCARD_Bank 
  * @{
  */    
#define FMC_Bank4_PCCARD                        ((uint32_t)0x00001000)
/**
  * @}                                                         
  */

/** @defgroup FMC_SDRAM_Bank
  * @{
  */
#define FMC_Bank1_SDRAM                    ((uint32_t)0x00000000)
#define FMC_Bank2_SDRAM                    ((uint32_t)0x00000001)

#define IS_FMC_SDRAM_BANK(BANK) (((BANK) == FMC_Bank1_SDRAM) || \
                                 ((BANK) == FMC_Bank2_SDRAM)) 

/**
  * @}
  */                               

                              
/** @defgroup FMC_NOR_SRAM_Controller 
  * @{
  */

/** @defgroup FMC_Data_Address_Bus_Multiplexing 
  * @{
  */

#define FMC_DataAddressMux_Disable                ((uint32_t)0x00000000)
#define FMC_DataAddressMux_Enable                 ((uint32_t)0x00000002)

#define IS_FMC_MUX(MUX) (((MUX) == FMC_DataAddressMux_Disable) || \
                         ((MUX) == FMC_DataAddressMux_Enable))
/**
  * @}
  */

/** @defgroup FMC_Memory_Type 
  * @{
  */

#define FMC_MemoryType_SRAM                     ((uint32_t)0x00000000)
#define FMC_MemoryType_PSRAM                    ((uint32_t)0x00000004)
#define FMC_MemoryType_NOR                      ((uint32_t)0x00000008)

#define IS_FMC_MEMORY(MEMORY) (((MEMORY) == FMC_MemoryType_SRAM) || \
                               ((MEMORY) == FMC_MemoryType_PSRAM)|| \
                               ((MEMORY) == FMC_MemoryType_NOR))
/**
  * @}
  */

/** @defgroup FMC_NORSRAM_Data_Width 
  * @{
  */

#define FMC_NORSRAM_MemoryDataWidth_8b                  ((uint32_t)0x00000000)
#define FMC_NORSRAM_MemoryDataWidth_16b                 ((uint32_t)0x00000010)
#define FMC_NORSRAM_MemoryDataWidth_32b                 ((uint32_t)0x00000020)

#define IS_FMC_NORSRAM_MEMORY_WIDTH(WIDTH) (((WIDTH) == FMC_NORSRAM_MemoryDataWidth_8b)  || \
                                            ((WIDTH) == FMC_NORSRAM_MemoryDataWidth_16b) || \
                                            ((WIDTH) == FMC_NORSRAM_MemoryDataWidth_32b))
/**
  * @}
  */

/** @defgroup FMC_Burst_Access_Mode 
  * @{
  */

#define FMC_BurstAccessMode_Disable             ((uint32_t)0x00000000) 
#define FMC_BurstAccessMode_Enable              ((uint32_t)0x00000100)

#define IS_FMC_BURSTMODE(STATE) (((STATE) == FMC_BurstAccessMode_Disable) || \
                                  ((STATE) == FMC_BurstAccessMode_Enable))
/**
  * @}
  */
    
/** @defgroup FMC_AsynchronousWait 
  * @{
  */
#define FMC_AsynchronousWait_Disable            ((uint32_t)0x00000000)
#define FMC_AsynchronousWait_Enable             ((uint32_t)0x00008000)

#define IS_FMC_ASYNWAIT(STATE) (((STATE) == FMC_AsynchronousWait_Disable) || \
                                 ((STATE) == FMC_AsynchronousWait_Enable))
/**
  * @}
  */

/** @defgroup FMC_Wait_Signal_Polarity 
  * @{
  */
#define FMC_WaitSignalPolarity_Low              ((uint32_t)0x00000000)
#define FMC_WaitSignalPolarity_High             ((uint32_t)0x00000200)

#define IS_FMC_WAIT_POLARITY(POLARITY) (((POLARITY) == FMC_WaitSignalPolarity_Low) || \
                                         ((POLARITY) == FMC_WaitSignalPolarity_High))
/**
  * @}
  */

/** @defgroup FMC_Wrap_Mode 
  * @{
  */
#define FMC_WrapMode_Disable                    ((uint32_t)0x00000000)
#define FMC_WrapMode_Enable                     ((uint32_t)0x00000400) 

#define IS_FMC_WRAP_MODE(MODE) (((MODE) == FMC_WrapMode_Disable) || \
                                 ((MODE) == FMC_WrapMode_Enable))
/**
  * @}
  */

/** @defgroup FMC_Wait_Timing 
  * @{
  */
#define FMC_WaitSignalActive_BeforeWaitState    ((uint32_t)0x00000000)
#define FMC_WaitSignalActive_DuringWaitState    ((uint32_t)0x00000800) 

#define IS_FMC_WAIT_SIGNAL_ACTIVE(ACTIVE) (((ACTIVE) == FMC_WaitSignalActive_BeforeWaitState) || \
                                            ((ACTIVE) == FMC_WaitSignalActive_DuringWaitState))
/**
  * @}
  */

/** @defgroup FMC_Write_Operation 
  * @{
  */
#define FMC_WriteOperation_Disable                     ((uint32_t)0x00000000)
#define FMC_WriteOperation_Enable                      ((uint32_t)0x00001000)

#define IS_FMC_WRITE_OPERATION(OPERATION) (((OPERATION) == FMC_WriteOperation_Disable) || \
                                            ((OPERATION) == FMC_WriteOperation_Enable))                         
/**
  * @}
  */

/** @defgroup FMC_Wait_Signal 
  * @{
  */
#define FMC_WaitSignal_Disable                  ((uint32_t)0x00000000)
#define FMC_WaitSignal_Enable                   ((uint32_t)0x00002000) 

#define IS_FMC_WAITE_SIGNAL(SIGNAL) (((SIGNAL) == FMC_WaitSignal_Disable) || \
                                      ((SIGNAL) == FMC_WaitSignal_Enable))
/**
  * @}
  */

/** @defgroup FMC_Extended_Mode 
  * @{
  */
#define FMC_ExtendedMode_Disable                ((uint32_t)0x00000000)
#define FMC_ExtendedMode_Enable                 ((uint32_t)0x00004000)

#define IS_FMC_EXTENDED_MODE(MODE) (((MODE) == FMC_ExtendedMode_Disable) || \
                                     ((MODE) == FMC_ExtendedMode_Enable)) 
/**
  * @}
  */

/** @defgroup FMC_Write_Burst 
  * @{
  */

#define FMC_WriteBurst_Disable                  ((uint32_t)0x00000000)
#define FMC_WriteBurst_Enable                   ((uint32_t)0x00080000) 

#define IS_FMC_WRITE_BURST(BURST) (((BURST) == FMC_WriteBurst_Disable) || \
                                    ((BURST) == FMC_WriteBurst_Enable))
/**
  * @}
  */
  
/** @defgroup FMC_Continous_Clock 
  * @{
  */

#define FMC_CClock_SyncOnly                     ((uint32_t)0x00000000)
#define FMC_CClock_SyncAsync                    ((uint32_t)0x00100000) 

#define IS_FMC_CONTINOUS_CLOCK(CCLOCK) (((CCLOCK) == FMC_CClock_SyncOnly) || \
                                        ((CCLOCK) == FMC_CClock_SyncAsync))
/**
  * @}
  */  

/** @defgroup FMC_Address_Setup_Time 
  * @{
  */
#define IS_FMC_ADDRESS_SETUP_TIME(TIME) ((TIME) <= 15)
/**
  * @}
  */

/** @defgroup FMC_Address_Hold_Time 
  * @{
  */
#define IS_FMC_ADDRESS_HOLD_TIME(TIME) (((TIME) > 0) && ((TIME) <= 15))
/**
  * @}
  */

/** @defgroup FMC_Data_Setup_Time 
  * @{
  */
#define IS_FMC_DATASETUP_TIME(TIME) (((TIME) > 0) && ((TIME) <= 255))
/**
  * @}
  */

/** @defgroup FMC_Bus_Turn_around_Duration 
  * @{
  */
#define IS_FMC_TURNAROUND_TIME(TIME) ((TIME) <= 15)
/**
  * @}
  */

/** @defgroup FMC_CLK_Division 
  * @{
  */
#define IS_FMC_CLK_DIV(DIV) (((DIV) > 0) && ((DIV) <= 15))
/**
  * @}
  */

/** @defgroup FMC_Data_Latency 
  * @{
  */
#define IS_FMC_DATA_LATENCY(LATENCY) ((LATENCY) <= 15)
/**
  * @}
  */

/** @defgroup FMC_Access_Mode 
  * @{
  */
#define FMC_AccessMode_A                        ((uint32_t)0x00000000)
#define FMC_AccessMode_B                        ((uint32_t)0x10000000) 
#define FMC_AccessMode_C                        ((uint32_t)0x20000000)
#define FMC_AccessMode_D                        ((uint32_t)0x30000000)

#define IS_FMC_ACCESS_MODE(MODE) (((MODE) == FMC_AccessMode_A)  || \
                                   ((MODE) == FMC_AccessMode_B) || \
                                   ((MODE) == FMC_AccessMode_C) || \
                                   ((MODE) == FMC_AccessMode_D))
/**
  * @}
  */

/**
  * @}
  */
  
/** @defgroup FMC_NAND_PCCARD_Controller 
  * @{
  */

/** @defgroup FMC_Wait_feature 
  * @{
  */
#define FMC_Waitfeature_Disable                 ((uint32_t)0x00000000)
#define FMC_Waitfeature_Enable                  ((uint32_t)0x00000002)

#define IS_FMC_WAIT_FEATURE(FEATURE) (((FEATURE) == FMC_Waitfeature_Disable) || \
                                       ((FEATURE) == FMC_Waitfeature_Enable))
/**
  * @}
  */

/** @defgroup FMC_NAND_Data_Width 
  * @{
  */
#define FMC_NAND_MemoryDataWidth_8b             ((uint32_t)0x00000000)
#define FMC_NAND_MemoryDataWidth_16b            ((uint32_t)0x00000010)

#define IS_FMC_NAND_MEMORY_WIDTH(WIDTH) (((WIDTH) == FMC_NAND_MemoryDataWidth_8b) || \
                                         ((WIDTH) == FMC_NAND_MemoryDataWidth_16b))
/**
  * @}
  */

/** @defgroup FMC_ECC 
  * @{
  */
#define FMC_ECC_Disable                         ((uint32_t)0x00000000)
#define FMC_ECC_Enable                          ((uint32_t)0x00000040)

#define IS_FMC_ECC_STATE(STATE) (((STATE) == FMC_ECC_Disable) || \
                                  ((STATE) == FMC_ECC_Enable))
/**
  * @}
  */

/** @defgroup FMC_ECC_Page_Size 
  * @{
  */
#define FMC_ECCPageSize_256Bytes                ((uint32_t)0x00000000)
#define FMC_ECCPageSize_512Bytes                ((uint32_t)0x00020000)
#define FMC_ECCPageSize_1024Bytes               ((uint32_t)0x00040000)
#define FMC_ECCPageSize_2048Bytes               ((uint32_t)0x00060000)
#define FMC_ECCPageSize_4096Bytes               ((uint32_t)0x00080000)
#define FMC_ECCPageSize_8192Bytes               ((uint32_t)0x000A0000)

#define IS_FMC_ECCPAGE_SIZE(SIZE) (((SIZE) == FMC_ECCPageSize_256Bytes)   || \
                                    ((SIZE) == FMC_ECCPageSize_512Bytes)  || \
                                    ((SIZE) == FMC_ECCPageSize_1024Bytes) || \
                                    ((SIZE) == FMC_ECCPageSize_2048Bytes) || \
                                    ((SIZE) == FMC_ECCPageSize_4096Bytes) || \
                                    ((SIZE) == FMC_ECCPageSize_8192Bytes))
/**
  * @}
  */

/** @defgroup FMC_TCLR_Setup_Time 
  * @{
  */
#define IS_FMC_TCLR_TIME(TIME) ((TIME) <= 255)
/**
  * @}
  */

/** @defgroup FMC_TAR_Setup_Time 
  * @{
  */
#define IS_FMC_TAR_TIME(TIME) ((TIME) <= 255)
/**
  * @}
  */

/** @defgroup FMC_Setup_Time 
  * @{
  */
#define IS_FMC_SETUP_TIME(TIME) ((TIME) <= 255)
/**
  * @}
  */

/** @defgroup FMC_Wait_Setup_Time 
  * @{
  */
#define IS_FMC_WAIT_TIME(TIME) ((TIME) <= 255)
/**
  * @}
  */

/** @defgroup FMC_Hold_Setup_Time 
  * @{
  */
#define IS_FMC_HOLD_TIME(TIME) ((TIME) <= 255)
/**
  * @}
  */

/** @defgroup FMC_HiZ_Setup_Time 
  * @{
  */
#define IS_FMC_HIZ_TIME(TIME) ((TIME) <= 255)
/**
  * @}
  */

/**
  * @}
  */  


/** @defgroup FMC_NOR_SRAM_Controller 
  * @{
  */
        
/** @defgroup FMC_ColumnBits_Number 
  * @{
  */
#define FMC_ColumnBits_Number_8b           ((uint32_t)0x00000000)
#define FMC_ColumnBits_Number_9b           ((uint32_t)0x00000001)
#define FMC_ColumnBits_Number_10b          ((uint32_t)0x00000002)
#define FMC_ColumnBits_Number_11b          ((uint32_t)0x00000003)

#define IS_FMC_COLUMNBITS_NUMBER(COLUMN) (((COLUMN) == FMC_ColumnBits_Number_8b)  || \
                                          ((COLUMN) == FMC_ColumnBits_Number_9b)  || \
                                          ((COLUMN) == FMC_ColumnBits_Number_10b) || \
                                          ((COLUMN) == FMC_ColumnBits_Number_11b))

/**
  * @}
  */
  
/** @defgroup FMC_RowBits_Number 
  * @{
  */
#define FMC_RowBits_Number_11b             ((uint32_t)0x00000000)
#define FMC_RowBits_Number_12b             ((uint32_t)0x00000004)
#define FMC_RowBits_Number_13b             ((uint32_t)0x00000008)

#define IS_FMC_ROWBITS_NUMBER(ROW) (((ROW) == FMC_RowBits_Number_11b) || \
                                    ((ROW) == FMC_RowBits_Number_12b) || \
                                    ((ROW) == FMC_RowBits_Number_13b))

/**
  * @}
  */  

/** @defgroup FMC_SDMemory_Data_Width 
  * @{
  */
#define FMC_SDMemory_Width_8b                ((uint32_t)0x00000000)
#define FMC_SDMemory_Width_16b               ((uint32_t)0x00000010)
#define FMC_SDMemory_Width_32b               ((uint32_t)0x00000020)

#define IS_FMC_SDMEMORY_WIDTH(WIDTH) (((WIDTH) == FMC_SDMemory_Width_8b)  || \
                                      ((WIDTH) == FMC_SDMemory_Width_16b) || \
                                      ((WIDTH) == FMC_SDMemory_Width_32b))

/**
  * @}
  */
  
/** @defgroup FMC_InternalBank_Number
  * @{
  */
#define FMC_InternalBank_Number_2          ((uint32_t)0x00000000)
#define FMC_InternalBank_Number_4          ((uint32_t)0x00000040)

#define IS_FMC_INTERNALBANK_NUMBER(NUMBER) (((NUMBER) == FMC_InternalBank_Number_2) || \
                                            ((NUMBER) == FMC_InternalBank_Number_4)) 

/**
  * @}
  */  
  
  
/** @defgroup FMC_CAS_Latency 
  * @{
  */
#define FMC_CAS_Latency_1                  ((uint32_t)0x00000080)
#define FMC_CAS_Latency_2                  ((uint32_t)0x00000100)
#define FMC_CAS_Latency_3                  ((uint32_t)0x00000180)

#define IS_FMC_CAS_LATENCY(LATENCY) (((LATENCY) == FMC_CAS_Latency_1) || \
                                     ((LATENCY) == FMC_CAS_Latency_2) || \
                                     ((LATENCY) == FMC_CAS_Latency_3))

/**
  * @}
  */  

/** @defgroup FMC_Write_Protection
  * @{
  */
#define FMC_Write_Protection_Disable       ((uint32_t)0x00000000)
#define FMC_Write_Protection_Enable        ((uint32_t)0x00000200)

#define IS_FMC_WRITE_PROTECTION(WRITE) (((WRITE) == FMC_Write_Protection_Disable) || \
                                        ((WRITE) == FMC_Write_Protection_Enable))

/**
  * @}
  */  
  

/** @defgroup FMC_SDClock_Period
  * @{
  */
#define FMC_SDClock_Disable                ((uint32_t)0x00000000)
#define FMC_SDClock_Period_2               ((uint32_t)0x00000800)
#define FMC_SDClock_Period_3               ((uint32_t)0x00000C00)

#define IS_FMC_SDCLOCK_PERIOD(PERIOD) (((PERIOD) == FMC_SDClock_Disable) || \
                                       ((PERIOD) == FMC_SDClock_Period_2) || \
                                       ((PERIOD) == FMC_SDClock_Period_3))

/**
  * @}
  */ 
  
/** @defgroup FMC_Read_Burst
  * @{
  */
#define FMC_Read_Burst_Disable             ((uint32_t)0x00000000)
#define FMC_Read_Burst_Enable              ((uint32_t)0x00001000)

#define IS_FMC_READ_BURST(RBURST) (((RBURST) == FMC_Read_Burst_Disable) || \
                                   ((RBURST) == FMC_Read_Burst_Enable))

/**
  * @}
  */

/** @defgroup FMC_ReadPipe_Delay
  * @{
  */
#define FMC_ReadPipe_Delay_0               ((uint32_t)0x00000000)
#define FMC_ReadPipe_Delay_1               ((uint32_t)0x00002000)
#define FMC_ReadPipe_Delay_2               ((uint32_t)0x00004000)

#define IS_FMC_READPIPE_DELAY(DELAY) (((DELAY) == FMC_ReadPipe_Delay_0) || \
                                      ((DELAY) == FMC_ReadPipe_Delay_1) || \
                                      ((DELAY) == FMC_ReadPipe_Delay_2))

/**
  * @}
  */
  
/** @defgroup FMC_LoadToActive_Delay
  * @{
  */
#define IS_FMC_LOADTOACTIVE_DELAY(DELAY) (((DELAY) > 0) && ((DELAY) <= 16))
/**
  * @}
  */
  
/** @defgroup FMC_ExitSelfRefresh_Delay
  * @{
  */
#define IS_FMC_EXITSELFREFRESH_DELAY(DELAY) (((DELAY) > 0) && ((DELAY) <= 16))
/**
  * @}
  */ 
     
/** @defgroup FMC_SelfRefresh_Time
  * @{
  */  
#define IS_FMC_SELFREFRESH_TIME(TIME) (((TIME) > 0) && ((TIME) <= 16))
/**
  * @}
  */
  
/** @defgroup FMC_RowCycle_Delay
  * @{
  */  
#define IS_FMC_ROWCYCLE_DELAY(DELAY) (((DELAY) > 0) && ((DELAY) <= 16))
/**
  * @}
  */  
  
/** @defgroup FMC_Write_Recovery_Time
  * @{
  */  
#define IS_FMC_WRITE_RECOVERY_TIME(TIME) (((TIME) > 0) && ((TIME) <= 16))
/**
  * @}
  */         
  
/** @defgroup FMC_RP_Delay
  * @{
  */  
#define IS_FMC_RP_DELAY(DELAY) (((DELAY) > 0) && ((DELAY) <= 16))
/**
  * @}
  */ 
  
/** @defgroup FMC_RCD_Delay 
  * @{
  */  
#define IS_FMC_RCD_DELAY(DELAY) (((DELAY) > 0) && ((DELAY) <= 16))

/**
  * @}
  */  
  
/** @defgroup FMC_Command_Mode
  * @{
  */
#define FMC_Command_Mode_normal            ((uint32_t)0x00000000)
#define FMC_Command_Mode_CLK_Enabled       ((uint32_t)0x00000001)
#define FMC_Command_Mode_PALL              ((uint32_t)0x00000002)
#define FMC_Command_Mode_AutoRefresh       ((uint32_t)0x00000003)
#define FMC_Command_Mode_LoadMode          ((uint32_t)0x00000004)
#define FMC_Command_Mode_Selfrefresh       ((uint32_t)0x00000005)
#define FMC_Command_Mode_PowerDown         ((uint32_t)0x00000006)

#define IS_FMC_COMMAND_MODE(COMMAND) (((COMMAND) == FMC_Command_Mode_normal)      || \
                                      ((COMMAND) == FMC_Command_Mode_CLK_Enabled) || \
                                      ((COMMAND) == FMC_Command_Mode_PALL)        || \
                                      ((COMMAND) == FMC_Command_Mode_AutoRefresh) || \
                                      ((COMMAND) == FMC_Command_Mode_LoadMode)    || \
                                      ((COMMAND) == FMC_Command_Mode_Selfrefresh) || \
                                      ((COMMAND) == FMC_Command_Mode_PowerDown))

/**
  * @}
  */

/** @defgroup FMC_Command_Target
  * @{
  */
#define FMC_Command_Target_bank2           ((uint32_t)0x00000008)
#define FMC_Command_Target_bank1           ((uint32_t)0x00000010)
#define FMC_Command_Target_bank1_2         ((uint32_t)0x00000018)

#define IS_FMC_COMMAND_TARGET(TARGET) (((TARGET) == FMC_Command_Target_bank1) || \
                                       ((TARGET) == FMC_Command_Target_bank2) || \
                                       ((TARGET) == FMC_Command_Target_bank1_2))

/**
  * @}
  */   
  
/** @defgroup FMC_AutoRefresh_Number
  * @{
  */  
#define IS_FMC_AUTOREFRESH_NUMBER(NUMBER) (((NUMBER) > 0) && ((NUMBER) <= 16))

/**
  * @}
  */

/** @defgroup FMC_ModeRegister_Definition
  * @{
  */
#define IS_FMC_MODE_REGISTER(CONTENT) ((CONTENT) <= 8191)

/**
  * @}
  */
  

/** @defgroup FMC_Mode_Status 
  * @{
  */
#define FMC_NormalMode_Status                     ((uint32_t)0x00000000)
#define FMC_SelfRefreshMode_Status                FMC_SDSR_MODES1_0
#define FMC_PowerDownMode_Status                  FMC_SDSR_MODES1_1

#define IS_FMC_MODE_STATUS(STATUS) (((STATUS) == FMC_NormalMode_Status)       || \
                                    ((STATUS) == FMC_SelfRefreshMode_Status)  || \
                                    ((STATUS) == FMC_PowerDownMode_Status))


/**
  * @}
  */      

/**
  * @}
  */  

/** @defgroup FMC_Interrupt_sources 
  * @{
  */
#define FMC_IT_RisingEdge                       ((uint32_t)0x00000008)
#define FMC_IT_Level                            ((uint32_t)0x00000010)
#define FMC_IT_FallingEdge                      ((uint32_t)0x00000020)
#define FMC_IT_Refresh                          ((uint32_t)0x00004000)

#define IS_FMC_IT(IT) ((((IT) & (uint32_t)0xFFFFBFC7) == 0x00000000) && ((IT) != 0x00000000))
#define IS_FMC_GET_IT(IT) (((IT) == FMC_IT_RisingEdge)  || \
                           ((IT) == FMC_IT_Level)       || \
                           ((IT) == FMC_IT_FallingEdge) || \
                           ((IT) == FMC_IT_Refresh)) 
                           
#define IS_FMC_IT_BANK(BANK) (((BANK) == FMC_Bank2_NAND)   || \
                              ((BANK) == FMC_Bank3_NAND)   || \
                              ((BANK) == FMC_Bank4_PCCARD) || \
                              ((BANK) == FMC_Bank1_SDRAM)  || \
                              ((BANK) == FMC_Bank2_SDRAM))                           
/**
  * @}
  */

/** @defgroup FMC_Flags 
  * @{
  */
#define FMC_FLAG_RisingEdge                     ((uint32_t)0x00000001)
#define FMC_FLAG_Level                          ((uint32_t)0x00000002)
#define FMC_FLAG_FallingEdge                    ((uint32_t)0x00000004)
#define FMC_FLAG_FEMPT                          ((uint32_t)0x00000040)
#define FMC_FLAG_Refresh                        FMC_SDSR_RE
#define FMC_FLAG_Busy                           FMC_SDSR_BUSY

#define IS_FMC_GET_FLAG(FLAG) (((FLAG) == FMC_FLAG_RisingEdge)       || \
                               ((FLAG) == FMC_FLAG_Level)            || \
                               ((FLAG) == FMC_FLAG_FallingEdge)      || \
                               ((FLAG) == FMC_FLAG_FEMPT)            || \
                               ((FLAG) == FMC_FLAG_Refresh)          || \
                               ((FLAG) == FMC_SDSR_BUSY))

#define IS_FMC_GETFLAG_BANK(BANK) (((BANK) == FMC_Bank2_NAND)    || \
                                   ((BANK) == FMC_Bank3_NAND)    || \
                                   ((BANK) == FMC_Bank4_PCCARD)  || \
                                   ((BANK) == FMC_Bank1_SDRAM)   || \
                                   ((BANK) == FMC_Bank2_SDRAM)   || \
                                   ((BANK) == (FMC_Bank1_SDRAM | FMC_Bank2_SDRAM)))
                                   
#define IS_FMC_CLEAR_FLAG(FLAG) ((((FLAG) & (uint32_t)0xFFFFFFF8) == 0x00000000) && ((FLAG) != 0x00000000))


/**
  * @}
  */

/** @defgroup FMC_Refresh_count
  * @{
  */
#define IS_FMC_REFRESH_COUNT(COUNT) ((COUNT) <= 8191)

/**
  * @}
  */

/**
  * @}
  */


/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/ 

/* NOR/SRAM Controller functions **********************************************/
void FMC_NORSRAMDeInit(uint32_t FMC_Bank);
void FMC_NORSRAMInit(FMC_NORSRAMInitTypeDef* FMC_NORSRAMInitStruct);
void FMC_NORSRAMStructInit(FMC_NORSRAMInitTypeDef* FMC_NORSRAMInitStruct);
void FMC_NORSRAMCmd(uint32_t FMC_Bank, FunctionalState NewState);

/* NAND Controller functions **************************************************/
void     FMC_NANDDeInit(uint32_t FMC_Bank);
void     FMC_NANDInit(FMC_NANDInitTypeDef* FMC_NANDInitStruct);
void     FMC_NANDStructInit(FMC_NANDInitTypeDef* FMC_NANDInitStruct);
void     FMC_NANDCmd(uint32_t FMC_Bank, FunctionalState NewState);
void     FMC_NANDECCCmd(uint32_t FMC_Bank, FunctionalState NewState);
uint32_t FMC_GetECC(uint32_t FMC_Bank);

/* PCCARD Controller functions ************************************************/
void FMC_PCCARDDeInit(void);
void FMC_PCCARDInit(FMC_PCCARDInitTypeDef* FMC_PCCARDInitStruct);
void FMC_PCCARDStructInit(FMC_PCCARDInitTypeDef* FMC_PCCARDInitStruct);
void FMC_PCCARDCmd(FunctionalState NewState);

/* SDRAM Controller functions ************************************************/
void     FMC_SDRAMDeInit(uint32_t FMC_Bank);
void     FMC_SDRAMInit(FMC_SDRAMInitTypeDef* FMC_SDRAMInitStruct);
void     FMC_SDRAMStructInit(FMC_SDRAMInitTypeDef* FMC_SDRAMInitStruct);
void     FMC_SDRAMCmdConfig(FMC_SDRAMCommandTypeDef* FMC_SDRAMCommandStruct);
uint32_t FMC_GetModeStatus(uint32_t SDRAM_Bank);
void     FMC_SetRefreshCount(uint32_t FMC_Count);
void     FMC_SetAutoRefresh_Number(uint32_t FMC_Number);
void     FMC_SDRAMWriteProtectionConfig(uint32_t SDRAM_Bank, FunctionalState NewState);

/* Interrupts and flags management functions **********************************/
void       FMC_ITConfig(uint32_t FMC_Bank, uint32_t FMC_IT, FunctionalState NewState);
FlagStatus FMC_GetFlagStatus(uint32_t FMC_Bank, uint32_t FMC_FLAG);
void       FMC_ClearFlag(uint32_t FMC_Bank, uint32_t FMC_FLAG);
ITStatus   FMC_GetITStatus(uint32_t FMC_Bank, uint32_t FMC_IT);
void       FMC_ClearITPendingBit(uint32_t FMC_Bank, uint32_t FMC_IT);

/** @addtogroup STM32F4xx_StdPeriph_Driver
  * @{
  */

/** @addtogroup RCC
  * @{
  */ 

/* Exported types ------------------------------------------------------------*/
typedef struct
{
  uint32_t SYSCLK_Frequency; /*!<  SYSCLK clock frequency expressed in Hz */
  uint32_t HCLK_Frequency;   /*!<  HCLK clock frequency expressed in Hz   */
  uint32_t PCLK1_Frequency;  /*!<  PCLK1 clock frequency expressed in Hz  */
  uint32_t PCLK2_Frequency;  /*!<  PCLK2 clock frequency expressed in Hz  */
}RCC_ClocksTypeDef;

/* Exported constants --------------------------------------------------------*/

/** @defgroup RCC_Exported_Constants
  * @{
  */
  
/** @defgroup RCC_HSE_configuration 
  * @{
  */
#define RCC_HSE_OFF                      ((uint8_t)0x00)
#define RCC_HSE_ON                       ((uint8_t)0x01)
#define RCC_HSE_Bypass                   ((uint8_t)0x05)
#define IS_RCC_HSE(HSE) (((HSE) == RCC_HSE_OFF) || ((HSE) == RCC_HSE_ON) || \
                         ((HSE) == RCC_HSE_Bypass))
/**
  * @}
  */ 

/** @defgroup RCC_LSE_Dual_Mode_Selection
  * @{
  */
#define RCC_LSE_LOWPOWER_MODE           ((uint8_t)0x00)
#define RCC_LSE_HIGHDRIVE_MODE          ((uint8_t)0x01)
#define IS_RCC_LSE_MODE(MODE)           (((MODE) == RCC_LSE_LOWPOWER_MODE) || \
                                         ((MODE) == RCC_LSE_HIGHDRIVE_MODE))
/**
  * @}
  */

/** @defgroup RCC_PLLSAIDivR_Factor
  * @{
  */
#define RCC_PLLSAIDivR_Div2                ((uint32_t)0x00000000)
#define RCC_PLLSAIDivR_Div4                ((uint32_t)0x00010000)
#define RCC_PLLSAIDivR_Div8                ((uint32_t)0x00020000)
#define RCC_PLLSAIDivR_Div16               ((uint32_t)0x00030000)
#define IS_RCC_PLLSAI_DIVR_VALUE(VALUE) (((VALUE) == RCC_PLLSAIDivR_Div2) ||\
                                        ((VALUE) == RCC_PLLSAIDivR_Div4)  ||\
                                        ((VALUE) == RCC_PLLSAIDivR_Div8)  ||\
                                        ((VALUE) == RCC_PLLSAIDivR_Div16))
/**
  * @}
  */

/** @defgroup RCC_PLL_Clock_Source 
  * @{
  */
#define RCC_PLLSource_HSI                ((uint32_t)0x00000000)
#define RCC_PLLSource_HSE                ((uint32_t)0x00400000)
#define IS_RCC_PLL_SOURCE(SOURCE) (((SOURCE) == RCC_PLLSource_HSI) || \
                                   ((SOURCE) == RCC_PLLSource_HSE))
#define IS_RCC_PLLM_VALUE(VALUE) ((VALUE) <= 63)
#define IS_RCC_PLLN_VALUE(VALUE) ((192 <= (VALUE)) && ((VALUE) <= 432))
#define IS_RCC_PLLP_VALUE(VALUE) (((VALUE) == 2) || ((VALUE) == 4) || ((VALUE) == 6) || ((VALUE) == 8))
#define IS_RCC_PLLQ_VALUE(VALUE) ((4 <= (VALUE)) && ((VALUE) <= 15))
#if defined(STM32F446xx)
#define IS_RCC_PLLR_VALUE(VALUE) ((2 <= (VALUE)) && ((VALUE) <= 7))
#endif /* STM32F446xx */

#define IS_RCC_PLLI2SN_VALUE(VALUE) ((192 <= (VALUE)) && ((VALUE) <= 432))
#define IS_RCC_PLLI2SR_VALUE(VALUE) ((2 <= (VALUE)) && ((VALUE) <= 7))
#define IS_RCC_PLLI2SM_VALUE(VALUE) ((VALUE) <= 63)
#define IS_RCC_PLLI2SQ_VALUE(VALUE) ((2 <= (VALUE)) && ((VALUE) <= 15))
#if defined(STM32F446xx)
#define IS_RCC_PLLI2SP_VALUE(VALUE) (((VALUE) == 2) || ((VALUE) == 4) || ((VALUE) == 6) || ((VALUE) == 8))
#define IS_RCC_PLLSAIM_VALUE(VALUE) ((VALUE) <= 63)
#endif /* STM32F446xx */
#define IS_RCC_PLLSAIN_VALUE(VALUE) ((49 <= (VALUE)) && ((VALUE) <= 432))
#if defined(STM32F446xx)
#define IS_RCC_PLLSAIP_VALUE(VALUE) (((VALUE) == 2) || ((VALUE) == 4) || ((VALUE) == 6) || ((VALUE) == 8))
#endif /* STM32F446xx */
#define IS_RCC_PLLSAIQ_VALUE(VALUE) ((2 <= (VALUE)) && ((VALUE) <= 15))
#define IS_RCC_PLLSAIR_VALUE(VALUE) ((2 <= (VALUE)) && ((VALUE) <= 7))  

#define IS_RCC_PLLSAI_DIVQ_VALUE(VALUE) ((1 <= (VALUE)) && ((VALUE) <= 32))
#define IS_RCC_PLLI2S_DIVQ_VALUE(VALUE) ((1 <= (VALUE)) && ((VALUE) <= 32))
/**
  * @}
  */ 
  
/** @defgroup RCC_System_Clock_Source 
  * @{
  */

#if defined(STM32F446xx)
#define RCC_SYSCLKSource_HSI             ((uint32_t)0x00000000)
#define RCC_SYSCLKSource_HSE             ((uint32_t)0x00000001)
#define RCC_SYSCLKSource_PLLPCLK         ((uint32_t)0x00000002)
#define RCC_SYSCLKSource_PLLRCLK         ((uint32_t)0x00000003)
#define IS_RCC_SYSCLK_SOURCE(SOURCE) (((SOURCE) == RCC_SYSCLKSource_HSI) || \
                                      ((SOURCE) == RCC_SYSCLKSource_HSE) || \
                                      ((SOURCE) == RCC_SYSCLKSource_PLLPCLK) || \
                                      ((SOURCE) == RCC_SYSCLKSource_PLLRCLK))
/* Add legacy definition */
#define  RCC_SYSCLKSource_PLLCLK    RCC_SYSCLKSource_PLLPCLK  
#endif /* STM32F446xx */

#if defined(STM32F40_41xxx) || defined(STM32F427_437xx) || defined(STM32F429xx) || defined(STM32F401xC) || defined(STM32F411xE)
#define RCC_SYSCLKSource_HSI             ((uint32_t)0x00000000)
#define RCC_SYSCLKSource_HSE             ((uint32_t)0x00000001)
#define RCC_SYSCLKSource_PLLCLK          ((uint32_t)0x00000002)
#define IS_RCC_SYSCLK_SOURCE(SOURCE) (((SOURCE) == RCC_SYSCLKSource_HSI) || \
                                      ((SOURCE) == RCC_SYSCLKSource_HSE) || \
                                      ((SOURCE) == RCC_SYSCLKSource_PLLCLK))
#endif /* STM32F40_41xxx || STM32F427_437xx || STM32F429xx || STM32F401xC || STM32F411xE */ 
/**
  * @}
  */ 
  
/** @defgroup RCC_AHB_Clock_Source
  * @{
  */
#define RCC_SYSCLK_Div1                  ((uint32_t)0x00000000)
#define RCC_SYSCLK_Div2                  ((uint32_t)0x00000080)
#define RCC_SYSCLK_Div4                  ((uint32_t)0x00000090)
#define RCC_SYSCLK_Div8                  ((uint32_t)0x000000A0)
#define RCC_SYSCLK_Div16                 ((uint32_t)0x000000B0)
#define RCC_SYSCLK_Div64                 ((uint32_t)0x000000C0)
#define RCC_SYSCLK_Div128                ((uint32_t)0x000000D0)
#define RCC_SYSCLK_Div256                ((uint32_t)0x000000E0)
#define RCC_SYSCLK_Div512                ((uint32_t)0x000000F0)
#define IS_RCC_HCLK(HCLK) (((HCLK) == RCC_SYSCLK_Div1) || ((HCLK) == RCC_SYSCLK_Div2) || \
                           ((HCLK) == RCC_SYSCLK_Div4) || ((HCLK) == RCC_SYSCLK_Div8) || \
                           ((HCLK) == RCC_SYSCLK_Div16) || ((HCLK) == RCC_SYSCLK_Div64) || \
                           ((HCLK) == RCC_SYSCLK_Div128) || ((HCLK) == RCC_SYSCLK_Div256) || \
                           ((HCLK) == RCC_SYSCLK_Div512))
/**
  * @}
  */ 
  
/** @defgroup RCC_APB1_APB2_Clock_Source
  * @{
  */
#define RCC_HCLK_Div1                    ((uint32_t)0x00000000)
#define RCC_HCLK_Div2                    ((uint32_t)0x00001000)
#define RCC_HCLK_Div4                    ((uint32_t)0x00001400)
#define RCC_HCLK_Div8                    ((uint32_t)0x00001800)
#define RCC_HCLK_Div16                   ((uint32_t)0x00001C00)
#define IS_RCC_PCLK(PCLK) (((PCLK) == RCC_HCLK_Div1) || ((PCLK) == RCC_HCLK_Div2) || \
                           ((PCLK) == RCC_HCLK_Div4) || ((PCLK) == RCC_HCLK_Div8) || \
                           ((PCLK) == RCC_HCLK_Div16))
/**
  * @}
  */ 
  
/** @defgroup RCC_Interrupt_Source 
  * @{
  */
#define RCC_IT_LSIRDY                    ((uint8_t)0x01)
#define RCC_IT_LSERDY                    ((uint8_t)0x02)
#define RCC_IT_HSIRDY                    ((uint8_t)0x04)
#define RCC_IT_HSERDY                    ((uint8_t)0x08)
#define RCC_IT_PLLRDY                    ((uint8_t)0x10)
#define RCC_IT_PLLI2SRDY                 ((uint8_t)0x20) 
#define RCC_IT_PLLSAIRDY                 ((uint8_t)0x40)
#define RCC_IT_CSS                       ((uint8_t)0x80)

#define IS_RCC_IT(IT) ((((IT) & (uint8_t)0x80) == 0x00) && ((IT) != 0x00))
#define IS_RCC_GET_IT(IT) (((IT) == RCC_IT_LSIRDY) || ((IT) == RCC_IT_LSERDY) || \
                           ((IT) == RCC_IT_HSIRDY) || ((IT) == RCC_IT_HSERDY) || \
                           ((IT) == RCC_IT_PLLRDY) || ((IT) == RCC_IT_CSS) || \
                           ((IT) == RCC_IT_PLLSAIRDY) || ((IT) == RCC_IT_PLLI2SRDY))
#define IS_RCC_CLEAR_IT(IT)((IT) != 0x00)

/**
  * @}
  */ 
  
/** @defgroup RCC_LSE_Configuration 
  * @{
  */
#define RCC_LSE_OFF                      ((uint8_t)0x00)
#define RCC_LSE_ON                       ((uint8_t)0x01)
#define RCC_LSE_Bypass                   ((uint8_t)0x04)
#define IS_RCC_LSE(LSE) (((LSE) == RCC_LSE_OFF) || ((LSE) == RCC_LSE_ON) || \
                         ((LSE) == RCC_LSE_Bypass))
/**
  * @}
  */ 
  
/** @defgroup RCC_RTC_Clock_Source
  * @{
  */
#define RCC_RTCCLKSource_LSE             ((uint32_t)0x00000100)
#define RCC_RTCCLKSource_LSI             ((uint32_t)0x00000200)
#define RCC_RTCCLKSource_HSE_Div2        ((uint32_t)0x00020300)
#define RCC_RTCCLKSource_HSE_Div3        ((uint32_t)0x00030300)
#define RCC_RTCCLKSource_HSE_Div4        ((uint32_t)0x00040300)
#define RCC_RTCCLKSource_HSE_Div5        ((uint32_t)0x00050300)
#define RCC_RTCCLKSource_HSE_Div6        ((uint32_t)0x00060300)
#define RCC_RTCCLKSource_HSE_Div7        ((uint32_t)0x00070300)
#define RCC_RTCCLKSource_HSE_Div8        ((uint32_t)0x00080300)
#define RCC_RTCCLKSource_HSE_Div9        ((uint32_t)0x00090300)
#define RCC_RTCCLKSource_HSE_Div10       ((uint32_t)0x000A0300)
#define RCC_RTCCLKSource_HSE_Div11       ((uint32_t)0x000B0300)
#define RCC_RTCCLKSource_HSE_Div12       ((uint32_t)0x000C0300)
#define RCC_RTCCLKSource_HSE_Div13       ((uint32_t)0x000D0300)
#define RCC_RTCCLKSource_HSE_Div14       ((uint32_t)0x000E0300)
#define RCC_RTCCLKSource_HSE_Div15       ((uint32_t)0x000F0300)
#define RCC_RTCCLKSource_HSE_Div16       ((uint32_t)0x00100300)
#define RCC_RTCCLKSource_HSE_Div17       ((uint32_t)0x00110300)
#define RCC_RTCCLKSource_HSE_Div18       ((uint32_t)0x00120300)
#define RCC_RTCCLKSource_HSE_Div19       ((uint32_t)0x00130300)
#define RCC_RTCCLKSource_HSE_Div20       ((uint32_t)0x00140300)
#define RCC_RTCCLKSource_HSE_Div21       ((uint32_t)0x00150300)
#define RCC_RTCCLKSource_HSE_Div22       ((uint32_t)0x00160300)
#define RCC_RTCCLKSource_HSE_Div23       ((uint32_t)0x00170300)
#define RCC_RTCCLKSource_HSE_Div24       ((uint32_t)0x00180300)
#define RCC_RTCCLKSource_HSE_Div25       ((uint32_t)0x00190300)
#define RCC_RTCCLKSource_HSE_Div26       ((uint32_t)0x001A0300)
#define RCC_RTCCLKSource_HSE_Div27       ((uint32_t)0x001B0300)
#define RCC_RTCCLKSource_HSE_Div28       ((uint32_t)0x001C0300)
#define RCC_RTCCLKSource_HSE_Div29       ((uint32_t)0x001D0300)
#define RCC_RTCCLKSource_HSE_Div30       ((uint32_t)0x001E0300)
#define RCC_RTCCLKSource_HSE_Div31       ((uint32_t)0x001F0300)
#define IS_RCC_RTCCLK_SOURCE(SOURCE) (((SOURCE) == RCC_RTCCLKSource_LSE) || \
                                      ((SOURCE) == RCC_RTCCLKSource_LSI) || \
                                      ((SOURCE) == RCC_RTCCLKSource_HSE_Div2) || \
                                      ((SOURCE) == RCC_RTCCLKSource_HSE_Div3) || \
                                      ((SOURCE) == RCC_RTCCLKSource_HSE_Div4) || \
                                      ((SOURCE) == RCC_RTCCLKSource_HSE_Div5) || \
                                      ((SOURCE) == RCC_RTCCLKSource_HSE_Div6) || \
                                      ((SOURCE) == RCC_RTCCLKSource_HSE_Div7) || \
                                      ((SOURCE) == RCC_RTCCLKSource_HSE_Div8) || \
                                      ((SOURCE) == RCC_RTCCLKSource_HSE_Div9) || \
                                      ((SOURCE) == RCC_RTCCLKSource_HSE_Div10) || \
                                      ((SOURCE) == RCC_RTCCLKSource_HSE_Div11) || \
                                      ((SOURCE) == RCC_RTCCLKSource_HSE_Div12) || \
                                      ((SOURCE) == RCC_RTCCLKSource_HSE_Div13) || \
                                      ((SOURCE) == RCC_RTCCLKSource_HSE_Div14) || \
                                      ((SOURCE) == RCC_RTCCLKSource_HSE_Div15) || \
                                      ((SOURCE) == RCC_RTCCLKSource_HSE_Div16) || \
                                      ((SOURCE) == RCC_RTCCLKSource_HSE_Div17) || \
                                      ((SOURCE) == RCC_RTCCLKSource_HSE_Div18) || \
                                      ((SOURCE) == RCC_RTCCLKSource_HSE_Div19) || \
                                      ((SOURCE) == RCC_RTCCLKSource_HSE_Div20) || \
                                      ((SOURCE) == RCC_RTCCLKSource_HSE_Div21) || \
                                      ((SOURCE) == RCC_RTCCLKSource_HSE_Div22) || \
                                      ((SOURCE) == RCC_RTCCLKSource_HSE_Div23) || \
                                      ((SOURCE) == RCC_RTCCLKSource_HSE_Div24) || \
                                      ((SOURCE) == RCC_RTCCLKSource_HSE_Div25) || \
                                      ((SOURCE) == RCC_RTCCLKSource_HSE_Div26) || \
                                      ((SOURCE) == RCC_RTCCLKSource_HSE_Div27) || \
                                      ((SOURCE) == RCC_RTCCLKSource_HSE_Div28) || \
                                      ((SOURCE) == RCC_RTCCLKSource_HSE_Div29) || \
                                      ((SOURCE) == RCC_RTCCLKSource_HSE_Div30) || \
                                      ((SOURCE) == RCC_RTCCLKSource_HSE_Div31))
/**
  * @}
  */ 

#if defined(STM32F446xx)
/** @defgroup RCC_I2S_Clock_Source
  * @{
  */
#define RCC_I2SCLKSource_PLLI2S             ((uint32_t)0x00)
#define RCC_I2SCLKSource_Ext                ((uint32_t)RCC_DCKCFGR_I2S1SRC_0)
#define RCC_I2SCLKSource_PLL                ((uint32_t)RCC_DCKCFGR_I2S1SRC_1)
#define RCC_I2SCLKSource_HSI_HSE            ((uint32_t)RCC_DCKCFGR_I2S1SRC_0 | RCC_DCKCFGR_I2S1SRC_1)

#define IS_RCC_I2SCLK_SOURCE(SOURCE) (((SOURCE) == RCC_I2SCLKSource_PLLI2S) || ((SOURCE) == RCC_I2SCLKSource_Ext) || \
                                      ((SOURCE) == RCC_I2SCLKSource_PLL) || ((SOURCE) == RCC_I2SCLKSource_HSI_HSE))                                
/**
  * @}
  */

/** @defgroup RCC_I2S_APBBus
  * @{
  */
#define RCC_I2SBus_APB1             ((uint8_t)0x00)
#define RCC_I2SBus_APB2             ((uint8_t)0x01)
#define IS_RCC_I2S_APBx(BUS) (((BUS) == RCC_I2SBus_APB1) || ((BUS) == RCC_I2SBus_APB2))                                
/**
  * @}
  */
    
/** @defgroup RCC_SAI_Clock_Source
  * @{
  */
#define RCC_SAICLKSource_PLLSAI             ((uint32_t)0x00)
#define RCC_SAICLKSource_PLLI2S             ((uint32_t)RCC_DCKCFGR_SAI1SRC_0)
#define RCC_SAICLKSource_PLL                ((uint32_t)RCC_DCKCFGR_SAI1SRC_1)
#define RCC_SAICLKSource_HSI_HSE            ((uint32_t)RCC_DCKCFGR_SAI1SRC_0 | RCC_DCKCFGR_SAI1SRC_1)

#define IS_RCC_SAICLK_SOURCE(SOURCE) (((SOURCE) == RCC_SAICLKSource_PLLSAI) || ((SOURCE) == RCC_SAICLKSource_PLLI2S) || \
                                      ((SOURCE) == RCC_SAICLKSource_PLL) || ((SOURCE) == RCC_SAICLKSource_HSI_HSE))                                
/**
  * @}
  */    
    
/** @defgroup RCC_SAI_Instance
  * @{
  */
#define RCC_SAIInstance_SAI1             ((uint8_t)0x00)
#define RCC_SAIInstance_SAI2             ((uint8_t)0x01)
#define IS_RCC_SAI_INSTANCE(BUS) (((BUS) == RCC_SAIInstance_SAI1) || ((BUS) == RCC_SAIInstance_SAI2))                                
/**
  * @}
  */
#endif /* STM32F446xx */

#if defined(STM32F40_41xxx) || defined(STM32F427_437xx) || defined(STM32F429xx) || defined(STM32F401xC) || defined(STM32F411xE)
/** @defgroup RCC_I2S_Clock_Source
  * @{
  */
#define RCC_I2S2CLKSource_PLLI2S             ((uint8_t)0x00)
#define RCC_I2S2CLKSource_Ext                ((uint8_t)0x01)

#define IS_RCC_I2SCLK_SOURCE(SOURCE) (((SOURCE) == RCC_I2S2CLKSource_PLLI2S) || ((SOURCE) == RCC_I2S2CLKSource_Ext))                                
/**
  * @}
  */ 

/** @defgroup RCC_SAI_BlockA_Clock_Source
  * @{
  */
#define RCC_SAIACLKSource_PLLSAI             ((uint32_t)0x00000000)
#define RCC_SAIACLKSource_PLLI2S             ((uint32_t)0x00100000)
#define RCC_SAIACLKSource_Ext                ((uint32_t)0x00200000)

#define IS_RCC_SAIACLK_SOURCE(SOURCE) (((SOURCE) == RCC_SAIACLKSource_PLLI2S) ||\
                                       ((SOURCE) == RCC_SAIACLKSource_PLLSAI) ||\
                                       ((SOURCE) == RCC_SAIACLKSource_Ext))
/**
  * @}
  */ 

/** @defgroup RCC_SAI_BlockB_Clock_Source
  * @{
  */
#define RCC_SAIBCLKSource_PLLSAI             ((uint32_t)0x00000000)
#define RCC_SAIBCLKSource_PLLI2S             ((uint32_t)0x00400000)
#define RCC_SAIBCLKSource_Ext                ((uint32_t)0x00800000)

#define IS_RCC_SAIBCLK_SOURCE(SOURCE) (((SOURCE) == RCC_SAIBCLKSource_PLLI2S) ||\
                                       ((SOURCE) == RCC_SAIBCLKSource_PLLSAI) ||\
                                       ((SOURCE) == RCC_SAIBCLKSource_Ext))
/**
  * @}
  */ 
#endif /* STM32F40_41xxx || STM32F427_437xx || STM32F429xx || STM32F401xC || STM32F411xE */

/** @defgroup RCC_TIM_PRescaler_Selection
  * @{
  */
#define RCC_TIMPrescDesactivated             ((uint8_t)0x00)
#define RCC_TIMPrescActivated                ((uint8_t)0x01)

#define IS_RCC_TIMCLK_PRESCALER(VALUE) (((VALUE) == RCC_TIMPrescDesactivated) || ((VALUE) == RCC_TIMPrescActivated))
/**
  * @}
  */

#if defined(STM32F446xx)
/** @defgroup RCC_SDIO_Clock_Source_Selection
  * @{
  */
#define RCC_SDIOCLKSource_48MHZ              ((uint8_t)0x00)
#define RCC_SDIOCLKSource_SYSCLK             ((uint8_t)0x01)
#define IS_RCC_SDIO_CLOCKSOURCE(CLKSOURCE)   (((CLKSOURCE) == RCC_SDIOCLKSource_48MHZ) || \
                                              ((CLKSOURCE) == RCC_SDIOCLKSource_SYSCLK))
/**
  * @}
  */


/** @defgroup RCC_48MHZ_Clock_Source_Selection
  * @{
  */
#define RCC_48MHZCLKSource_PLL                ((uint8_t)0x00)
#define RCC_48MHZCLKSource_PLLSAI             ((uint8_t)0x01)
#define IS_RCC_48MHZ_CLOCKSOURCE(CLKSOURCE)   (((CLKSOURCE) == RCC_48MHZCLKSource_PLL) || \
                                               ((CLKSOURCE) == RCC_48MHZCLKSource_PLLSAI))
/**
  * @}
  */
#endif /* STM32F446xx */

#if defined(STM32F446xx) 
/** @defgroup RCC_SPDIFRX_Clock_Source_Selection
  * @{
  */
#define RCC_SPDIFRXCLKSource_PLLR                 ((uint8_t)0x00)
#define RCC_SPDIFRXCLKSource_PLLI2SP              ((uint8_t)0x01)
#define IS_RCC_SPDIFRX_CLOCKSOURCE(CLKSOURCE)     (((CLKSOURCE) == RCC_SPDIFRXCLKSource_PLLR) || \
                                                   ((CLKSOURCE) == RCC_SPDIFRXCLKSource_PLLI2SP))
/**
  * @}
  */

/** @defgroup RCC_CEC_Clock_Source_Selection
  * @{
  */
#define RCC_CECCLKSource_HSIDiv488            ((uint8_t)0x00)
#define RCC_CECCLKSource_LSE                  ((uint8_t)0x01)
#define IS_RCC_CEC_CLOCKSOURCE(CLKSOURCE)     (((CLKSOURCE) == RCC_CECCLKSource_HSIDiv488) || \
                                               ((CLKSOURCE) == RCC_CECCLKSource_LSE))
/**
  * @}
  */

/** @defgroup RCC_FMPI2C1_Clock_Source
  * @{
  */
#define RCC_FMPI2C1CLKSource_APB1            ((uint32_t)0x00)
#define RCC_FMPI2C1CLKSource_SYSCLK          ((uint32_t)RCC_DCKCFGR2_FMPI2C1SEL_0)
#define RCC_FMPI2C1CLKSource_HSI             ((uint32_t)RCC_DCKCFGR2_FMPI2C1SEL_1)
    
#define IS_RCC_FMPI2C1_CLOCKSOURCE(SOURCE) (((SOURCE) == RCC_FMPI2C1CLKSource_APB1) || ((SOURCE) == RCC_FMPI2C1CLKSource_SYSCLK) || \
                                         ((SOURCE) == RCC_FMPI2C1CLKSource_HSI))                                
/**
  * @}
  */

/** @defgroup RCC_AHB1_ClockGating
  * @{
  */ 
#define RCC_AHB1ClockGating_APB1Bridge         ((uint32_t)0x00000001)
#define RCC_AHB1ClockGating_APB2Bridge         ((uint32_t)0x00000002)
#define RCC_AHB1ClockGating_CM4DBG             ((uint32_t)0x00000004)
#define RCC_AHB1ClockGating_SPARE              ((uint32_t)0x00000008)
#define RCC_AHB1ClockGating_SRAM               ((uint32_t)0x00000010)
#define RCC_AHB1ClockGating_FLITF              ((uint32_t)0x00000020)
#define RCC_AHB1ClockGating_RCC                ((uint32_t)0x00000040)

#define IS_RCC_AHB1_CLOCKGATING(PERIPH) ((((PERIPH) & 0xFFFFFF80) == 0x00) && ((PERIPH) != 0x00))

/**
  * @}
  */
#endif /* STM32F446xx */

/** @defgroup RCC_AHB1_Peripherals 
  * @{
  */ 
#define RCC_AHB1Periph_GPIOA             ((uint32_t)0x00000001)
#define RCC_AHB1Periph_GPIOB             ((uint32_t)0x00000002)
#define RCC_AHB1Periph_GPIOC             ((uint32_t)0x00000004)
#define RCC_AHB1Periph_GPIOD             ((uint32_t)0x00000008)
#define RCC_AHB1Periph_GPIOE             ((uint32_t)0x00000010)
#define RCC_AHB1Periph_GPIOF             ((uint32_t)0x00000020)
#define RCC_AHB1Periph_GPIOG             ((uint32_t)0x00000040)
#define RCC_AHB1Periph_GPIOH             ((uint32_t)0x00000080)
#define RCC_AHB1Periph_GPIOI             ((uint32_t)0x00000100) 
#define RCC_AHB1Periph_GPIOJ             ((uint32_t)0x00000200)
#define RCC_AHB1Periph_GPIOK             ((uint32_t)0x00000400)
#define RCC_AHB1Periph_CRC               ((uint32_t)0x00001000)
#define RCC_AHB1Periph_FLITF             ((uint32_t)0x00008000)
#define RCC_AHB1Periph_SRAM1             ((uint32_t)0x00010000)
#define RCC_AHB1Periph_SRAM2             ((uint32_t)0x00020000)
#define RCC_AHB1Periph_BKPSRAM           ((uint32_t)0x00040000)
#define RCC_AHB1Periph_SRAM3             ((uint32_t)0x00080000)
#define RCC_AHB1Periph_CCMDATARAMEN      ((uint32_t)0x00100000)
#define RCC_AHB1Periph_DMA1              ((uint32_t)0x00200000)
#define RCC_AHB1Periph_DMA2              ((uint32_t)0x00400000)
#define RCC_AHB1Periph_DMA2D             ((uint32_t)0x00800000)
#define RCC_AHB1Periph_ETH_MAC           ((uint32_t)0x02000000)
#define RCC_AHB1Periph_ETH_MAC_Tx        ((uint32_t)0x04000000)
#define RCC_AHB1Periph_ETH_MAC_Rx        ((uint32_t)0x08000000)
#define RCC_AHB1Periph_ETH_MAC_PTP       ((uint32_t)0x10000000)
#define RCC_AHB1Periph_OTG_HS            ((uint32_t)0x20000000)
#define RCC_AHB1Periph_OTG_HS_ULPI       ((uint32_t)0x40000000)

#define IS_RCC_AHB1_CLOCK_PERIPH(PERIPH) ((((PERIPH) & 0x810BE800) == 0x00) && ((PERIPH) != 0x00))
#define IS_RCC_AHB1_RESET_PERIPH(PERIPH) ((((PERIPH) & 0xDD1FE800) == 0x00) && ((PERIPH) != 0x00))
#define IS_RCC_AHB1_LPMODE_PERIPH(PERIPH) ((((PERIPH) & 0x81106800) == 0x00) && ((PERIPH) != 0x00))

/**
  * @}
  */ 
  
/** @defgroup RCC_AHB2_Peripherals 
  * @{
  */  
#define RCC_AHB2Periph_DCMI              ((uint32_t)0x00000001)
#define RCC_AHB2Periph_CRYP              ((uint32_t)0x00000010)
#define RCC_AHB2Periph_HASH              ((uint32_t)0x00000020)
#define RCC_AHB2Periph_RNG               ((uint32_t)0x00000040)
#define RCC_AHB2Periph_OTG_FS            ((uint32_t)0x00000080)
#define IS_RCC_AHB2_PERIPH(PERIPH) ((((PERIPH) & 0xFFFFFF0E) == 0x00) && ((PERIPH) != 0x00))
/**
  * @}
  */ 
  
/** @defgroup RCC_AHB3_Peripherals 
  * @{
  */ 
#if defined(STM32F40_41xxx)
#define RCC_AHB3Periph_FSMC                ((uint32_t)0x00000001)
#define IS_RCC_AHB3_PERIPH(PERIPH) ((((PERIPH) & 0xFFFFFFFE) == 0x00) && ((PERIPH) != 0x00))
#endif /* STM32F40_41xxx */

#if defined(STM32F427_437xx) || defined(STM32F429xx)
#define RCC_AHB3Periph_FMC                 ((uint32_t)0x00000001)
#define IS_RCC_AHB3_PERIPH(PERIPH) ((((PERIPH) & 0xFFFFFFFE) == 0x00) && ((PERIPH) != 0x00))
#endif /* STM32F427_437xx ||  STM32F429xx */

#if defined(STM32F446xx)
#define RCC_AHB3Periph_FMC                 ((uint32_t)0x00000001)
#define RCC_AHB3Periph_QSPI                ((uint32_t)0x00000002)
#define IS_RCC_AHB3_PERIPH(PERIPH) ((((PERIPH) & 0xFFFFFFFC) == 0x00) && ((PERIPH) != 0x00))
#endif /* STM32F446xx */

/**
  * @}
  */ 
  
/** @defgroup RCC_APB1_Peripherals 
  * @{
  */ 
#define RCC_APB1Periph_TIM2              ((uint32_t)0x00000001)
#define RCC_APB1Periph_TIM3              ((uint32_t)0x00000002)
#define RCC_APB1Periph_TIM4              ((uint32_t)0x00000004)
#define RCC_APB1Periph_TIM5              ((uint32_t)0x00000008)
#define RCC_APB1Periph_TIM6              ((uint32_t)0x00000010)
#define RCC_APB1Periph_TIM7              ((uint32_t)0x00000020)
#define RCC_APB1Periph_TIM12             ((uint32_t)0x00000040)
#define RCC_APB1Periph_TIM13             ((uint32_t)0x00000080)
#define RCC_APB1Periph_TIM14             ((uint32_t)0x00000100)
#define RCC_APB1Periph_WWDG              ((uint32_t)0x00000800)
#define RCC_APB1Periph_SPI2              ((uint32_t)0x00004000)
#define RCC_APB1Periph_SPI3              ((uint32_t)0x00008000)
#if defined(STM32F446xx)
#define RCC_APB1Periph_SPDIFRX           ((uint32_t)0x00010000)
#endif /* STM32F446xx */ 
#define RCC_APB1Periph_USART2            ((uint32_t)0x00020000)
#define RCC_APB1Periph_USART3            ((uint32_t)0x00040000)
#define RCC_APB1Periph_UART4             ((uint32_t)0x00080000)
#define RCC_APB1Periph_UART5             ((uint32_t)0x00100000)
#define RCC_APB1Periph_I2C1              ((uint32_t)0x00200000)
#define RCC_APB1Periph_I2C2              ((uint32_t)0x00400000)
#define RCC_APB1Periph_I2C3              ((uint32_t)0x00800000)
#if defined(STM32F446xx)
#define RCC_APB1Periph_FMPI2C1           ((uint32_t)0x01000000)
#endif /* STM32F446xx */ 
#define RCC_APB1Periph_CAN1              ((uint32_t)0x02000000)
#define RCC_APB1Periph_CAN2              ((uint32_t)0x04000000)
#if defined(STM32F446xx)
#define RCC_APB1Periph_CEC               ((uint32_t)0x08000000)
#endif /* STM32F446xx */ 
#define RCC_APB1Periph_PWR               ((uint32_t)0x10000000)
#define RCC_APB1Periph_DAC               ((uint32_t)0x20000000)
#define RCC_APB1Periph_UART7             ((uint32_t)0x40000000)
#define RCC_APB1Periph_UART8             ((uint32_t)0x80000000)
#define IS_RCC_APB1_PERIPH(PERIPH) ((((PERIPH) & 0x00003600) == 0x00) && ((PERIPH) != 0x00))
/**
  * @}
  */ 
  
/** @defgroup RCC_APB2_Peripherals 
  * @{
  */ 
#define RCC_APB2Periph_TIM1              ((uint32_t)0x00000001)
#define RCC_APB2Periph_TIM8              ((uint32_t)0x00000002)
#define RCC_APB2Periph_USART1            ((uint32_t)0x00000010)
#define RCC_APB2Periph_USART6            ((uint32_t)0x00000020)
#define RCC_APB2Periph_ADC               ((uint32_t)0x00000100)
#define RCC_APB2Periph_ADC1              ((uint32_t)0x00000100)
#define RCC_APB2Periph_ADC2              ((uint32_t)0x00000200)
#define RCC_APB2Periph_ADC3              ((uint32_t)0x00000400)
#define RCC_APB2Periph_SDIO              ((uint32_t)0x00000800)
#define RCC_APB2Periph_SPI1              ((uint32_t)0x00001000)
#define RCC_APB2Periph_SPI4              ((uint32_t)0x00002000)
#define RCC_APB2Periph_SYSCFG            ((uint32_t)0x00004000)
#define RCC_APB2Periph_TIM9              ((uint32_t)0x00010000)
#define RCC_APB2Periph_TIM10             ((uint32_t)0x00020000)
#define RCC_APB2Periph_TIM11             ((uint32_t)0x00040000)
#define RCC_APB2Periph_SPI5              ((uint32_t)0x00100000)
#define RCC_APB2Periph_SPI6              ((uint32_t)0x00200000)
#define RCC_APB2Periph_SAI1              ((uint32_t)0x00400000)
#if defined(STM32F446xx)
#define RCC_APB2Periph_SAI2              ((uint32_t)0x00800000)
#endif /* STM32F446xx */
#define RCC_APB2Periph_LTDC              ((uint32_t)0x04000000)

#define IS_RCC_APB2_PERIPH(PERIPH) ((((PERIPH) & 0xF30880CC) == 0x00) && ((PERIPH) != 0x00))
#define IS_RCC_APB2_RESET_PERIPH(PERIPH) ((((PERIPH) & 0xF30886CC) == 0x00) && ((PERIPH) != 0x00))

/**
  * @}
  */ 

/** @defgroup RCC_MCO1_Clock_Source_Prescaler
  * @{
  */
#define RCC_MCO1Source_HSI               ((uint32_t)0x00000000)
#define RCC_MCO1Source_LSE               ((uint32_t)0x00200000)
#define RCC_MCO1Source_HSE               ((uint32_t)0x00400000)
#define RCC_MCO1Source_PLLCLK            ((uint32_t)0x00600000)
#define RCC_MCO1Div_1                    ((uint32_t)0x00000000)
#define RCC_MCO1Div_2                    ((uint32_t)0x04000000)
#define RCC_MCO1Div_3                    ((uint32_t)0x05000000)
#define RCC_MCO1Div_4                    ((uint32_t)0x06000000)
#define RCC_MCO1Div_5                    ((uint32_t)0x07000000)
#define IS_RCC_MCO1SOURCE(SOURCE) (((SOURCE) == RCC_MCO1Source_HSI) || ((SOURCE) == RCC_MCO1Source_LSE) || \
                                   ((SOURCE) == RCC_MCO1Source_HSE) || ((SOURCE) == RCC_MCO1Source_PLLCLK))
                                   
#define IS_RCC_MCO1DIV(DIV) (((DIV) == RCC_MCO1Div_1) || ((DIV) == RCC_MCO1Div_2) || \
                             ((DIV) == RCC_MCO1Div_3) || ((DIV) == RCC_MCO1Div_4) || \
                             ((DIV) == RCC_MCO1Div_5)) 
/**
  * @}
  */ 
  
/** @defgroup RCC_MCO2_Clock_Source_Prescaler
  * @{
  */
#define RCC_MCO2Source_SYSCLK            ((uint32_t)0x00000000)
#define RCC_MCO2Source_PLLI2SCLK         ((uint32_t)0x40000000)
#define RCC_MCO2Source_HSE               ((uint32_t)0x80000000)
#define RCC_MCO2Source_PLLCLK            ((uint32_t)0xC0000000)
#define RCC_MCO2Div_1                    ((uint32_t)0x00000000)
#define RCC_MCO2Div_2                    ((uint32_t)0x20000000)
#define RCC_MCO2Div_3                    ((uint32_t)0x28000000)
#define RCC_MCO2Div_4                    ((uint32_t)0x30000000)
#define RCC_MCO2Div_5                    ((uint32_t)0x38000000)
#define IS_RCC_MCO2SOURCE(SOURCE) (((SOURCE) == RCC_MCO2Source_SYSCLK) || ((SOURCE) == RCC_MCO2Source_PLLI2SCLK)|| \
                                   ((SOURCE) == RCC_MCO2Source_HSE) || ((SOURCE) == RCC_MCO2Source_PLLCLK))
                                   
#define IS_RCC_MCO2DIV(DIV) (((DIV) == RCC_MCO2Div_1) || ((DIV) == RCC_MCO2Div_2) || \
                             ((DIV) == RCC_MCO2Div_3) || ((DIV) == RCC_MCO2Div_4) || \
                             ((DIV) == RCC_MCO2Div_5))                             
/**
  * @}
  */ 
  
/** @defgroup RCC_Flag 
  * @{
  */
#define RCC_FLAG_HSIRDY                  ((uint8_t)0x21)
#define RCC_FLAG_HSERDY                  ((uint8_t)0x31)
#define RCC_FLAG_PLLRDY                  ((uint8_t)0x39)
#define RCC_FLAG_PLLI2SRDY               ((uint8_t)0x3B)
#define RCC_FLAG_PLLSAIRDY               ((uint8_t)0x3D)
#define RCC_FLAG_LSERDY                  ((uint8_t)0x41)
#define RCC_FLAG_LSIRDY                  ((uint8_t)0x61)
#define RCC_FLAG_BORRST                  ((uint8_t)0x79)
#define RCC_FLAG_PINRST                  ((uint8_t)0x7A)
#define RCC_FLAG_PORRST                  ((uint8_t)0x7B)
#define RCC_FLAG_SFTRST                  ((uint8_t)0x7C)
#define RCC_FLAG_IWDGRST                 ((uint8_t)0x7D)
#define RCC_FLAG_WWDGRST                 ((uint8_t)0x7E)
#define RCC_FLAG_LPWRRST                 ((uint8_t)0x7F)

#define IS_RCC_FLAG(FLAG) (((FLAG) == RCC_FLAG_HSIRDY)   || ((FLAG) == RCC_FLAG_HSERDY) || \
                           ((FLAG) == RCC_FLAG_PLLRDY)   || ((FLAG) == RCC_FLAG_LSERDY) || \
                           ((FLAG) == RCC_FLAG_LSIRDY)   || ((FLAG) == RCC_FLAG_BORRST) || \
                           ((FLAG) == RCC_FLAG_PINRST)   || ((FLAG) == RCC_FLAG_PORRST) || \
                           ((FLAG) == RCC_FLAG_SFTRST)   || ((FLAG) == RCC_FLAG_IWDGRST)|| \
                           ((FLAG) == RCC_FLAG_WWDGRST)  || ((FLAG) == RCC_FLAG_LPWRRST)|| \
                           ((FLAG) == RCC_FLAG_PLLI2SRDY)|| ((FLAG) == RCC_FLAG_PLLSAIRDY))

#define IS_RCC_CALIBRATION_VALUE(VALUE) ((VALUE) <= 0x1F)
/**
  * @}
  */ 

/**
  * @}
  */ 

/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/ 

/* Function used to set the RCC clock configuration to the default reset state */
void        RCC_DeInit(void);

/* Internal/external clocks, PLL, CSS and MCO configuration functions *********/
void        RCC_HSEConfig(uint8_t RCC_HSE);
ErrorStatus RCC_WaitForHSEStartUp(void);
void        RCC_AdjustHSICalibrationValue(uint8_t HSICalibrationValue);
void        RCC_HSICmd(FunctionalState NewState);
void        RCC_LSEConfig(uint8_t RCC_LSE);
void        RCC_LSICmd(FunctionalState NewState);

void        RCC_PLLCmd(FunctionalState NewState);
#if defined(STM32F446xx)
void        RCC_PLLConfig(uint32_t RCC_PLLSource, uint32_t PLLM, uint32_t PLLN, uint32_t PLLP, uint32_t PLLQ, uint32_t PLLR);
#endif /* STM32F446xx */

#if defined(STM32F40_41xxx) || defined(STM32F427_437xx) || defined(STM32F429xx) || defined(STM32F401xC) || defined(STM32F411xE)
void        RCC_PLLConfig(uint32_t RCC_PLLSource, uint32_t PLLM, uint32_t PLLN, uint32_t PLLP, uint32_t PLLQ);
#endif /* STM32F40_41xxx || STM32F427_437xx || STM32F429xx || STM32F401xC || STM32F411xE */

void        RCC_PLLI2SCmd(FunctionalState NewState);
#if defined(STM32F40_41xxx) || defined(STM32F401xC)
void        RCC_PLLI2SConfig(uint32_t PLLI2SN, uint32_t PLLI2SR);
#endif /* STM32F40_41xxx || STM32F401xC */
#if defined(STM32F411xE)
void        RCC_PLLI2SConfig(uint32_t PLLI2SN, uint32_t PLLI2SR, uint32_t PLLI2SM);
#endif /* STM32F411xE */
#if defined(STM32F427_437xx) || defined(STM32F429xx)
void        RCC_PLLI2SConfig(uint32_t PLLI2SN, uint32_t PLLI2SQ, uint32_t PLLI2SR);
#endif /* STM32F427_437xx || STM32F429xx */
#if defined(STM32F446xx)
void        RCC_PLLI2SConfig(uint32_t PLLI2SM, uint32_t PLLI2SN, uint32_t PLLI2SP, uint32_t PLLI2SQ, uint32_t PLLI2SR);
#endif /* STM32F446xx */

void        RCC_PLLSAICmd(FunctionalState NewState);
#if defined(STM32F446xx)
void        RCC_PLLSAIConfig(uint32_t PLLSAIM, uint32_t PLLSAIN, uint32_t PLLSAIP, uint32_t PLLSAIQ);
#endif /* STM32F446xx */
#if defined(STM32F40_41xxx) || defined(STM32F427_437xx) || defined(STM32F429xx) || defined(STM32F401xC) || defined(STM32F411xE)
void        RCC_PLLSAIConfig(uint32_t PLLSAIN, uint32_t PLLSAIQ, uint32_t PLLSAIR);
#endif /* STM32F40_41xxx || STM32F427_437xx || STM32F429xx || STM32F401xC || STM32F411xE */

void        RCC_ClockSecuritySystemCmd(FunctionalState NewState);
void        RCC_MCO1Config(uint32_t RCC_MCO1Source, uint32_t RCC_MCO1Div);
void        RCC_MCO2Config(uint32_t RCC_MCO2Source, uint32_t RCC_MCO2Div);

/* System, AHB and APB busses clocks configuration functions ******************/
void        RCC_SYSCLKConfig(uint32_t RCC_SYSCLKSource);
uint8_t     RCC_GetSYSCLKSource(void);
void        RCC_HCLKConfig(uint32_t RCC_SYSCLK);
void        RCC_PCLK1Config(uint32_t RCC_HCLK);
void        RCC_PCLK2Config(uint32_t RCC_HCLK);
void        RCC_GetClocksFreq(RCC_ClocksTypeDef* RCC_Clocks);

/* Peripheral clocks configuration functions **********************************/
void        RCC_RTCCLKConfig(uint32_t RCC_RTCCLKSource);
void        RCC_RTCCLKCmd(FunctionalState NewState);
void        RCC_BackupResetCmd(FunctionalState NewState);

#if defined(STM32F446xx)
void        RCC_I2SCLKConfig(uint32_t RCC_I2SAPBx, uint32_t RCC_I2SCLKSource);
void        RCC_SAICLKConfig(uint32_t RCC_SAIInstance, uint32_t RCC_SAICLKSource);
#endif /* STM32F446xx */

#if defined(STM32F40_41xxx) || defined(STM32F427_437xx) || defined(STM32F429xx) || defined(STM32F401xC) || defined(STM32F411xE)
void        RCC_I2SCLKConfig(uint32_t RCC_I2SCLKSource);
void        RCC_SAIBlockACLKConfig(uint32_t RCC_SAIBlockACLKSource);
void        RCC_SAIBlockBCLKConfig(uint32_t RCC_SAIBlockBCLKSource);
#endif /* STM32F40_41xxx || STM32F427_437xx || STM32F429xx || STM32F401xC || STM32F411xE */

void        RCC_SAIPLLI2SClkDivConfig(uint32_t RCC_PLLI2SDivQ);
void        RCC_SAIPLLSAIClkDivConfig(uint32_t RCC_PLLSAIDivQ);

void        RCC_LTDCCLKDivConfig(uint32_t RCC_PLLSAIDivR);
void        RCC_TIMCLKPresConfig(uint32_t RCC_TIMCLKPrescaler);

void        RCC_AHB1PeriphClockCmd(uint32_t RCC_AHB1Periph, FunctionalState NewState);
void        RCC_AHB2PeriphClockCmd(uint32_t RCC_AHB2Periph, FunctionalState NewState);
void        RCC_AHB3PeriphClockCmd(uint32_t RCC_AHB3Periph, FunctionalState NewState);
void        RCC_APB1PeriphClockCmd(uint32_t RCC_APB1Periph, FunctionalState NewState);
void        RCC_APB2PeriphClockCmd(uint32_t RCC_APB2Periph, FunctionalState NewState);

void        RCC_AHB1PeriphResetCmd(uint32_t RCC_AHB1Periph, FunctionalState NewState);
void        RCC_AHB2PeriphResetCmd(uint32_t RCC_AHB2Periph, FunctionalState NewState);
void        RCC_AHB3PeriphResetCmd(uint32_t RCC_AHB3Periph, FunctionalState NewState);
void        RCC_APB1PeriphResetCmd(uint32_t RCC_APB1Periph, FunctionalState NewState);
void        RCC_APB2PeriphResetCmd(uint32_t RCC_APB2Periph, FunctionalState NewState);

void        RCC_AHB1PeriphClockLPModeCmd(uint32_t RCC_AHB1Periph, FunctionalState NewState);
void        RCC_AHB2PeriphClockLPModeCmd(uint32_t RCC_AHB2Periph, FunctionalState NewState);
void        RCC_AHB3PeriphClockLPModeCmd(uint32_t RCC_AHB3Periph, FunctionalState NewState);
void        RCC_APB1PeriphClockLPModeCmd(uint32_t RCC_APB1Periph, FunctionalState NewState);
void        RCC_APB2PeriphClockLPModeCmd(uint32_t RCC_APB2Periph, FunctionalState NewState);

/* Features available only for STM32F411xx/STM32F446xx devices */
void        RCC_LSEModeConfig(uint8_t RCC_Mode);

/* Features available only for STM32F446xx devices */
#if defined(STM32F446xx)
void        RCC_48MHzClockSourceConfig(uint8_t RCC_ClockSource);
void        RCC_SDIOClockSourceConfig(uint8_t RCC_ClockSource);
#endif /* STM32F446xx */

/* Features available only for STM32F446xx devices */
#if defined(STM32F446xx)
void        RCC_AHB1ClockGatingCmd(uint32_t RCC_AHB1ClockGating, FunctionalState NewState);
void        RCC_SPDIFRXClockSourceConfig(uint8_t RCC_ClockSource);
void        RCC_CECClockSourceConfig(uint8_t RCC_ClockSource);
void        RCC_FMPI2C1ClockSourceConfig(uint32_t RCC_ClockSource);
#endif /* STM32F446xx */

/* Interrupts and flags management functions **********************************/
void        RCC_ITConfig(uint8_t RCC_IT, FunctionalState NewState);
FlagStatus  RCC_GetFlagStatus(uint8_t RCC_FLAG);
void        RCC_ClearFlag(void);
ITStatus    RCC_GetITStatus(uint8_t RCC_IT);
void        RCC_ClearITPendingBit(uint8_t RCC_IT);

/** @addtogroup STM32F4xx_StdPeriph_Driver
  * @{
  */

/** @addtogroup GPIO
  * @{
  */ 

/* Exported types ------------------------------------------------------------*/

#define IS_GPIO_ALL_PERIPH(PERIPH) (((PERIPH) == GPIOA) || \
                                    ((PERIPH) == GPIOB) || \
                                    ((PERIPH) == GPIOC) || \
                                    ((PERIPH) == GPIOD) || \
                                    ((PERIPH) == GPIOE) || \
                                    ((PERIPH) == GPIOF) || \
                                    ((PERIPH) == GPIOG) || \
                                    ((PERIPH) == GPIOH) || \
                                    ((PERIPH) == GPIOI) || \
                                    ((PERIPH) == GPIOJ) || \
                                    ((PERIPH) == GPIOK))

/** 
  * @brief  GPIO Configuration Mode enumeration 
  */   
typedef enum
{ 
  GPIO_Mode_IN   = 0x00, /*!< GPIO Input Mode */
  GPIO_Mode_OUT  = 0x01, /*!< GPIO Output Mode */
  GPIO_Mode_AF   = 0x02, /*!< GPIO Alternate function Mode */
  GPIO_Mode_AN   = 0x03  /*!< GPIO Analog Mode */
}GPIOMode_TypeDef;
#define IS_GPIO_MODE(MODE) (((MODE) == GPIO_Mode_IN)  || ((MODE) == GPIO_Mode_OUT) || \
                            ((MODE) == GPIO_Mode_AF)|| ((MODE) == GPIO_Mode_AN))

/** 
  * @brief  GPIO Output type enumeration 
  */  
typedef enum
{ 
  GPIO_OType_PP = 0x00,
  GPIO_OType_OD = 0x01
}GPIOOType_TypeDef;
#define IS_GPIO_OTYPE(OTYPE) (((OTYPE) == GPIO_OType_PP) || ((OTYPE) == GPIO_OType_OD))


/** 
  * @brief  GPIO Output Maximum frequency enumeration 
  */  
typedef enum
{ 
  GPIO_Low_Speed     = 0x00, /*!< Low speed    */
  GPIO_Medium_Speed  = 0x01, /*!< Medium speed */
  GPIO_Fast_Speed    = 0x02, /*!< Fast speed   */
  GPIO_High_Speed    = 0x03  /*!< High speed   */
}GPIOSpeed_TypeDef;

/* Add legacy definition */
#define  GPIO_Speed_2MHz    GPIO_Low_Speed    
#define  GPIO_Speed_25MHz   GPIO_Medium_Speed 
#define  GPIO_Speed_50MHz   GPIO_Fast_Speed 
#define  GPIO_Speed_100MHz  GPIO_High_Speed  
  
#define IS_GPIO_SPEED(SPEED) (((SPEED) == GPIO_Low_Speed) || ((SPEED) == GPIO_Medium_Speed) || \
                              ((SPEED) == GPIO_Fast_Speed)||  ((SPEED) == GPIO_High_Speed)) 

/** 
  * @brief  GPIO Configuration PullUp PullDown enumeration 
  */ 
typedef enum
{ 
  GPIO_PuPd_NOPULL = 0x00,
  GPIO_PuPd_UP     = 0x01,
  GPIO_PuPd_DOWN   = 0x02
}GPIOPuPd_TypeDef;
#define IS_GPIO_PUPD(PUPD) (((PUPD) == GPIO_PuPd_NOPULL) || ((PUPD) == GPIO_PuPd_UP) || \
                            ((PUPD) == GPIO_PuPd_DOWN))

/** 
  * @brief  GPIO Bit SET and Bit RESET enumeration 
  */ 
typedef enum
{ 
  Bit_RESET = 0,
  Bit_SET
}BitAction;
#define IS_GPIO_BIT_ACTION(ACTION) (((ACTION) == Bit_RESET) || ((ACTION) == Bit_SET))


/** 
  * @brief   GPIO Init structure definition  
  */ 
typedef struct
{
  uint32_t GPIO_Pin;              /*!< Specifies the GPIO pins to be configured.
                                       This parameter can be any value of @ref GPIO_pins_define */

  GPIOMode_TypeDef GPIO_Mode;     /*!< Specifies the operating mode for the selected pins.
                                       This parameter can be a value of @ref GPIOMode_TypeDef */

  GPIOSpeed_TypeDef GPIO_Speed;   /*!< Specifies the speed for the selected pins.
                                       This parameter can be a value of @ref GPIOSpeed_TypeDef */

  GPIOOType_TypeDef GPIO_OType;   /*!< Specifies the operating output type for the selected pins.
                                       This parameter can be a value of @ref GPIOOType_TypeDef */

  GPIOPuPd_TypeDef GPIO_PuPd;     /*!< Specifies the operating Pull-up/Pull down for the selected pins.
                                       This parameter can be a value of @ref GPIOPuPd_TypeDef */
}GPIO_InitTypeDef;

/* Exported constants --------------------------------------------------------*/

/** @defgroup GPIO_Exported_Constants
  * @{
  */ 

/** @defgroup GPIO_pins_define 
  * @{
  */ 
#define GPIO_Pin_0                 ((uint16_t)0x0001)  /* Pin 0 selected */
#define GPIO_Pin_1                 ((uint16_t)0x0002)  /* Pin 1 selected */
#define GPIO_Pin_2                 ((uint16_t)0x0004)  /* Pin 2 selected */
#define GPIO_Pin_3                 ((uint16_t)0x0008)  /* Pin 3 selected */
#define GPIO_Pin_4                 ((uint16_t)0x0010)  /* Pin 4 selected */
#define GPIO_Pin_5                 ((uint16_t)0x0020)  /* Pin 5 selected */
#define GPIO_Pin_6                 ((uint16_t)0x0040)  /* Pin 6 selected */
#define GPIO_Pin_7                 ((uint16_t)0x0080)  /* Pin 7 selected */
#define GPIO_Pin_8                 ((uint16_t)0x0100)  /* Pin 8 selected */
#define GPIO_Pin_9                 ((uint16_t)0x0200)  /* Pin 9 selected */
#define GPIO_Pin_10                ((uint16_t)0x0400)  /* Pin 10 selected */
#define GPIO_Pin_11                ((uint16_t)0x0800)  /* Pin 11 selected */
#define GPIO_Pin_12                ((uint16_t)0x1000)  /* Pin 12 selected */
#define GPIO_Pin_13                ((uint16_t)0x2000)  /* Pin 13 selected */
#define GPIO_Pin_14                ((uint16_t)0x4000)  /* Pin 14 selected */
#define GPIO_Pin_15                ((uint16_t)0x8000)  /* Pin 15 selected */
#define GPIO_Pin_All               ((uint16_t)0xFFFF)  /* All pins selected */

#define GPIO_PIN_MASK              ((uint32_t)0x0000FFFF) /* PIN mask for assert test */
#define IS_GPIO_PIN(PIN)           (((PIN) & GPIO_PIN_MASK ) != (uint32_t)0x00)
#define IS_GET_GPIO_PIN(PIN) (((PIN) == GPIO_Pin_0) || \
                              ((PIN) == GPIO_Pin_1) || \
                              ((PIN) == GPIO_Pin_2) || \
                              ((PIN) == GPIO_Pin_3) || \
                              ((PIN) == GPIO_Pin_4) || \
                              ((PIN) == GPIO_Pin_5) || \
                              ((PIN) == GPIO_Pin_6) || \
                              ((PIN) == GPIO_Pin_7) || \
                              ((PIN) == GPIO_Pin_8) || \
                              ((PIN) == GPIO_Pin_9) || \
                              ((PIN) == GPIO_Pin_10) || \
                              ((PIN) == GPIO_Pin_11) || \
                              ((PIN) == GPIO_Pin_12) || \
                              ((PIN) == GPIO_Pin_13) || \
                              ((PIN) == GPIO_Pin_14) || \
                              ((PIN) == GPIO_Pin_15))
/**
  * @}
  */ 


/** @defgroup GPIO_Pin_sources 
  * @{
  */ 
#define GPIO_PinSource0            ((uint8_t)0x00)
#define GPIO_PinSource1            ((uint8_t)0x01)
#define GPIO_PinSource2            ((uint8_t)0x02)
#define GPIO_PinSource3            ((uint8_t)0x03)
#define GPIO_PinSource4            ((uint8_t)0x04)
#define GPIO_PinSource5            ((uint8_t)0x05)
#define GPIO_PinSource6            ((uint8_t)0x06)
#define GPIO_PinSource7            ((uint8_t)0x07)
#define GPIO_PinSource8            ((uint8_t)0x08)
#define GPIO_PinSource9            ((uint8_t)0x09)
#define GPIO_PinSource10           ((uint8_t)0x0A)
#define GPIO_PinSource11           ((uint8_t)0x0B)
#define GPIO_PinSource12           ((uint8_t)0x0C)
#define GPIO_PinSource13           ((uint8_t)0x0D)
#define GPIO_PinSource14           ((uint8_t)0x0E)
#define GPIO_PinSource15           ((uint8_t)0x0F)

#define IS_GPIO_PIN_SOURCE(PINSOURCE) (((PINSOURCE) == GPIO_PinSource0) || \
                                       ((PINSOURCE) == GPIO_PinSource1) || \
                                       ((PINSOURCE) == GPIO_PinSource2) || \
                                       ((PINSOURCE) == GPIO_PinSource3) || \
                                       ((PINSOURCE) == GPIO_PinSource4) || \
                                       ((PINSOURCE) == GPIO_PinSource5) || \
                                       ((PINSOURCE) == GPIO_PinSource6) || \
                                       ((PINSOURCE) == GPIO_PinSource7) || \
                                       ((PINSOURCE) == GPIO_PinSource8) || \
                                       ((PINSOURCE) == GPIO_PinSource9) || \
                                       ((PINSOURCE) == GPIO_PinSource10) || \
                                       ((PINSOURCE) == GPIO_PinSource11) || \
                                       ((PINSOURCE) == GPIO_PinSource12) || \
                                       ((PINSOURCE) == GPIO_PinSource13) || \
                                       ((PINSOURCE) == GPIO_PinSource14) || \
                                       ((PINSOURCE) == GPIO_PinSource15))
/**
  * @}
  */ 

/** @defgroup GPIO_Alternat_function_selection_define 
  * @{
  */ 
/** 
  * @brief   AF 0 selection  
  */ 
#define GPIO_AF_RTC_50Hz      ((uint8_t)0x00)  /* RTC_50Hz Alternate Function mapping */
#define GPIO_AF_MCO           ((uint8_t)0x00)  /* MCO (MCO1 and MCO2) Alternate Function mapping */
#define GPIO_AF_TAMPER        ((uint8_t)0x00)  /* TAMPER (TAMPER_1 and TAMPER_2) Alternate Function mapping */
#define GPIO_AF_SWJ           ((uint8_t)0x00)  /* SWJ (SWD and JTAG) Alternate Function mapping */
#define GPIO_AF_TRACE         ((uint8_t)0x00)  /* TRACE Alternate Function mapping */
#if defined (STM32F446xx)
#define GPIO_AF0_TIM2         ((uint8_t)0x00)  /* TIM2 Alternate Function mapping */
#endif /* STM32F446xx */

/** 
  * @brief   AF 1 selection  
  */ 
#define GPIO_AF_TIM1          ((uint8_t)0x01)  /* TIM1 Alternate Function mapping */
#define GPIO_AF_TIM2          ((uint8_t)0x01)  /* TIM2 Alternate Function mapping */

/** 
  * @brief   AF 2 selection  
  */ 
#define GPIO_AF_TIM3          ((uint8_t)0x02)  /* TIM3 Alternate Function mapping */
#define GPIO_AF_TIM4          ((uint8_t)0x02)  /* TIM4 Alternate Function mapping */
#define GPIO_AF_TIM5          ((uint8_t)0x02)  /* TIM5 Alternate Function mapping */

/** 
  * @brief   AF 3 selection  
  */ 
#define GPIO_AF_TIM8          ((uint8_t)0x03)  /* TIM8 Alternate Function mapping */
#define GPIO_AF_TIM9          ((uint8_t)0x03)  /* TIM9 Alternate Function mapping */
#define GPIO_AF_TIM10         ((uint8_t)0x03)  /* TIM10 Alternate Function mapping */
#define GPIO_AF_TIM11         ((uint8_t)0x03)  /* TIM11 Alternate Function mapping */
#if defined (STM32F446xx)
#define GPIO_AF3_CEC          ((uint8_t)0x03)  /* CEC Alternate Function mapping */
#endif /* STM32F446xx */
/** 
  * @brief   AF 4 selection  
  */ 
#define GPIO_AF_I2C1          ((uint8_t)0x04)  /* I2C1 Alternate Function mapping */
#define GPIO_AF_I2C2          ((uint8_t)0x04)  /* I2C2 Alternate Function mapping */
#define GPIO_AF_I2C3          ((uint8_t)0x04)  /* I2C3 Alternate Function mapping */
#if defined (STM32F446xx)
#define GPIO_AF4_CEC          ((uint8_t)0x04)  /* CEC Alternate Function mapping */
#define GPIO_AF_FMPI2C        ((uint8_t)0x04)  /* FMPI2C Alternate Function mapping */
#endif /* STM32F446xx */

/** 
  * @brief   AF 5 selection  
  */ 
#define GPIO_AF_SPI1          ((uint8_t)0x05)  /* SPI1/I2S1 Alternate Function mapping */
#define GPIO_AF_SPI2          ((uint8_t)0x05)  /* SPI2/I2S2 Alternate Function mapping */
#define GPIO_AF5_SPI3         ((uint8_t)0x05)  /* SPI3/I2S3 Alternate Function mapping (Only for STM32F411xE Devices) */
#define GPIO_AF_SPI4          ((uint8_t)0x05)  /* SPI4/I2S4 Alternate Function mapping */
#define GPIO_AF_SPI5          ((uint8_t)0x05)  /* SPI5 Alternate Function mapping      */
#define GPIO_AF_SPI6          ((uint8_t)0x05)  /* SPI6 Alternate Function mapping      */

/** 
  * @brief   AF 6 selection  
  */ 
#define GPIO_AF_SPI3          ((uint8_t)0x06)  /* SPI3/I2S3 Alternate Function mapping */
#define GPIO_AF6_SPI2         ((uint8_t)0x06)  /* SPI2 Alternate Function mapping (Only for STM32F411xE Devices) */
#define GPIO_AF6_SPI4         ((uint8_t)0x06)  /* SPI4 Alternate Function mapping (Only for STM32F411xE Devices) */
#define GPIO_AF6_SPI5         ((uint8_t)0x06)  /* SPI5 Alternate Function mapping (Only for STM32F411xE Devices) */
#define GPIO_AF_SAI1          ((uint8_t)0x06)  /* SAI1 Alternate Function mapping      */

/** 
  * @brief   AF 7 selection  
  */ 
#define GPIO_AF_USART1         ((uint8_t)0x07)  /* USART1 Alternate Function mapping  */
#define GPIO_AF_USART2         ((uint8_t)0x07)  /* USART2 Alternate Function mapping  */
#define GPIO_AF_USART3         ((uint8_t)0x07)  /* USART3 Alternate Function mapping  */
#define GPIO_AF7_SPI3          ((uint8_t)0x07)  /* SPI3/I2S3ext Alternate Function mapping */

/** 
  * @brief   AF 7 selection Legacy 
  */ 
#define GPIO_AF_I2S3ext   GPIO_AF7_SPI3

/** 
  * @brief   AF 8 selection  
  */ 
#define GPIO_AF_UART4         ((uint8_t)0x08)  /* UART4 Alternate Function mapping  */
#define GPIO_AF_UART5         ((uint8_t)0x08)  /* UART5 Alternate Function mapping  */
#define GPIO_AF_USART6        ((uint8_t)0x08)  /* USART6 Alternate Function mapping */
#define GPIO_AF_UART7         ((uint8_t)0x08)  /* UART7 Alternate Function mapping  */
#define GPIO_AF_UART8         ((uint8_t)0x08)  /* UART8 Alternate Function mapping  */
#if defined (STM32F446xx)
#define GPIO_AF8_SAI2          ((uint8_t)0x08)  /* SAI2 Alternate Function mapping */
#define GPIO_AF_SPDIF         ((uint8_t)0x08)   /* SPDIF Alternate Function mapping */
#endif /* STM32F446xx */

/** 
  * @brief   AF 9 selection 
  */ 
#define GPIO_AF_CAN1          ((uint8_t)0x09)  /* CAN1 Alternate Function mapping  */
#define GPIO_AF_CAN2          ((uint8_t)0x09)  /* CAN2 Alternate Function mapping  */
#define GPIO_AF_TIM12         ((uint8_t)0x09)  /* TIM12 Alternate Function mapping */
#define GPIO_AF_TIM13         ((uint8_t)0x09)  /* TIM13 Alternate Function mapping */
#define GPIO_AF_TIM14         ((uint8_t)0x09)  /* TIM14 Alternate Function mapping */

#define GPIO_AF9_I2C2         ((uint8_t)0x09)  /* I2C2 Alternate Function mapping (Only for STM32F401xC/STM32F411xE Devices) */
#define GPIO_AF9_I2C3         ((uint8_t)0x09)  /* I2C3 Alternate Function mapping (Only for STM32F401xC/STM32F411xE Devices) */
#if defined (STM32F446xx)
#define GPIO_AF9_SAI2         ((uint8_t)0x09)  /* SAI2 Alternate Function mapping */
#endif /* STM32F446xx */
#define GPIO_AF9_LTDC         ((uint8_t)0x09)  /* LTDC Alternate Function mapping */
#if defined (STM32F446xx)
#define GPIO_AF9_QUADSPI      ((uint8_t)0x09)  /* QuadSPI Alternate Function mapping */
#endif /* STM32F446xx */
/** 
  * @brief   AF 10 selection  
  */ 
#define GPIO_AF_OTG_FS         ((uint8_t)0xA)  /* OTG_FS Alternate Function mapping */
#define GPIO_AF_OTG_HS         ((uint8_t)0xA)  /* OTG_HS Alternate Function mapping */
#if defined (STM32F446xx)
#define GPIO_AF10_SAI2         ((uint8_t)0x0A)  /* SAI2 Alternate Function mapping */
#endif /* STM32F446xx */
#if defined (STM32F446xx)
#define GPIO_AF10_QUADSPI      ((uint8_t)0x0A)  /* QuadSPI Alternate Function mapping */
#endif /* STM32F446xx */
/** 
  * @brief   AF 11 selection  
  */ 
#define GPIO_AF_ETH             ((uint8_t)0x0B)  /* ETHERNET Alternate Function mapping */

/** 
  * @brief   AF 12 selection  
  */ 
#if defined (STM32F40_41xxx)
#define GPIO_AF_FSMC             ((uint8_t)0xC)  /* FSMC Alternate Function mapping                     */
#endif /* STM32F40_41xxx */

#if defined (STM32F427_437xx) || defined (STM32F429xx) || defined (STM32F446xx)
#define GPIO_AF_FMC              ((uint8_t)0xC)  /* FMC Alternate Function mapping                      */
#endif /* STM32F427_437xx || STM32F429xx || STM32F446xx */

#define GPIO_AF_OTG_HS_FS        ((uint8_t)0xC)  /* OTG HS configured in FS, Alternate Function mapping */
#define GPIO_AF_SDIO             ((uint8_t)0xC)  /* SDIO Alternate Function mapping                     */

/** 
  * @brief   AF 13 selection  
  */ 
#define GPIO_AF_DCMI          ((uint8_t)0x0D)  /* DCMI Alternate Function mapping */

/** 
  * @brief   AF 14 selection  
  */
//#define GPIO_AF_LTDC          ((uint8_t)0x0E)  /* LCD-TFT Alternate Function mapping */

/** 
  * @brief   AF 15 selection  
  */ 
#define GPIO_AF_EVENTOUT      ((uint8_t)0x0F)  /* EVENTOUT Alternate Function mapping */

#if defined (STM32F40_41xxx)
#define IS_GPIO_AF(AF)   (((AF) == GPIO_AF_RTC_50Hz)  || ((AF) == GPIO_AF_TIM14)     || \
                          ((AF) == GPIO_AF_MCO)       || ((AF) == GPIO_AF_TAMPER)    || \
                          ((AF) == GPIO_AF_SWJ)       || ((AF) == GPIO_AF_TRACE)     || \
                          ((AF) == GPIO_AF_TIM1)      || ((AF) == GPIO_AF_TIM2)      || \
                          ((AF) == GPIO_AF_TIM3)      || ((AF) == GPIO_AF_TIM4)      || \
                          ((AF) == GPIO_AF_TIM5)      || ((AF) == GPIO_AF_TIM8)      || \
                          ((AF) == GPIO_AF_I2C1)      || ((AF) == GPIO_AF_I2C2)      || \
                          ((AF) == GPIO_AF_I2C3)      || ((AF) == GPIO_AF_SPI1)      || \
                          ((AF) == GPIO_AF_SPI2)      || ((AF) == GPIO_AF_TIM13)     || \
                          ((AF) == GPIO_AF_SPI3)      || ((AF) == GPIO_AF_TIM14)     || \
                          ((AF) == GPIO_AF_USART1)    || ((AF) == GPIO_AF_USART2)    || \
                          ((AF) == GPIO_AF_USART3)    || ((AF) == GPIO_AF_UART4)     || \
                          ((AF) == GPIO_AF_UART5)     || ((AF) == GPIO_AF_USART6)    || \
                          ((AF) == GPIO_AF_CAN1)      || ((AF) == GPIO_AF_CAN2)      || \
                          ((AF) == GPIO_AF_OTG_FS)    || ((AF) == GPIO_AF_OTG_HS)    || \
                          ((AF) == GPIO_AF_ETH)       || ((AF) == GPIO_AF_OTG_HS_FS) || \
                          ((AF) == GPIO_AF_SDIO)      || ((AF) == GPIO_AF_DCMI)      || \
                          ((AF) == GPIO_AF_EVENTOUT)  || ((AF) == GPIO_AF_FSMC))
#endif /* STM32F40_41xxx */

#if defined (STM32F401xC)
#define IS_GPIO_AF(AF)   (((AF) == GPIO_AF_RTC_50Hz)  || ((AF) == GPIO_AF_TIM14)     || \
                          ((AF) == GPIO_AF_MCO)       || ((AF) == GPIO_AF_TAMPER)    || \
                          ((AF) == GPIO_AF_SWJ)       || ((AF) == GPIO_AF_TRACE)     || \
                          ((AF) == GPIO_AF_TIM1)      || ((AF) == GPIO_AF_TIM2)      || \
                          ((AF) == GPIO_AF_TIM3)      || ((AF) == GPIO_AF_TIM4)      || \
                          ((AF) == GPIO_AF_TIM5)      || ((AF) == GPIO_AF_TIM8)      || \
                          ((AF) == GPIO_AF_I2C1)      || ((AF) == GPIO_AF_I2C2)      || \
                          ((AF) == GPIO_AF_I2C3)      || ((AF) == GPIO_AF_SPI1)      || \
                          ((AF) == GPIO_AF_SPI2)      || ((AF) == GPIO_AF_TIM13)     || \
                          ((AF) == GPIO_AF_SPI3)      || ((AF) == GPIO_AF_TIM14)     || \
                          ((AF) == GPIO_AF_USART1)    || ((AF) == GPIO_AF_USART2)    || \
                          ((AF) == GPIO_AF_SDIO)      || ((AF) == GPIO_AF_USART6)    || \
                          ((AF) == GPIO_AF_OTG_FS)    || ((AF) == GPIO_AF_OTG_HS)    || \
                          ((AF) == GPIO_AF_EVENTOUT)  || ((AF) == GPIO_AF_SPI4))
#endif /* STM32F401xC */

#if defined (STM32F411xE)
#define IS_GPIO_AF(AF)   (((AF) < 16) && ((AF) != 11) && ((AF) != 13) && ((AF) != 14))
#endif /* STM32F411xE */

#if defined (STM32F427_437xx) || defined (STM32F429xx)
#define IS_GPIO_AF(AF)   (((AF) == GPIO_AF_RTC_50Hz)  || ((AF) == GPIO_AF_TIM14)     || \
                          ((AF) == GPIO_AF_MCO)       || ((AF) == GPIO_AF_TAMPER)    || \
                          ((AF) == GPIO_AF_SWJ)       || ((AF) == GPIO_AF_TRACE)     || \
                          ((AF) == GPIO_AF_TIM1)      || ((AF) == GPIO_AF_TIM2)      || \
                          ((AF) == GPIO_AF_TIM3)      || ((AF) == GPIO_AF_TIM4)      || \
                          ((AF) == GPIO_AF_TIM5)      || ((AF) == GPIO_AF_TIM8)      || \
                          ((AF) == GPIO_AF_I2C1)      || ((AF) == GPIO_AF_I2C2)      || \
                          ((AF) == GPIO_AF_I2C3)      || ((AF) == GPIO_AF_SPI1)      || \
                          ((AF) == GPIO_AF_SPI2)      || ((AF) == GPIO_AF_TIM13)     || \
                          ((AF) == GPIO_AF_SPI3)      || ((AF) == GPIO_AF_TIM14)     || \
                          ((AF) == GPIO_AF_USART1)    || ((AF) == GPIO_AF_USART2)    || \
                          ((AF) == GPIO_AF_USART3)    || ((AF) == GPIO_AF_UART4)     || \
                          ((AF) == GPIO_AF_UART5)     || ((AF) == GPIO_AF_USART6)    || \
                          ((AF) == GPIO_AF_CAN1)      || ((AF) == GPIO_AF_CAN2)      || \
                          ((AF) == GPIO_AF_OTG_FS)    || ((AF) == GPIO_AF_OTG_HS)    || \
                          ((AF) == GPIO_AF_ETH)       || ((AF) == GPIO_AF_OTG_HS_FS) || \
                          ((AF) == GPIO_AF_SDIO)      || ((AF) == GPIO_AF_DCMI)      || \
                          ((AF) == GPIO_AF_EVENTOUT)  || ((AF) == GPIO_AF_SPI4)      || \
                          ((AF) == GPIO_AF_SPI5)      || ((AF) == GPIO_AF_SPI6)      || \
                          ((AF) == GPIO_AF_UART7)     || ((AF) == GPIO_AF_UART8)     || \
                          ((AF) == GPIO_AF_FMC)       ||  ((AF) == GPIO_AF_SAI1)     || \
                          ((AF) == GPIO_AF_LTDC))
#endif /* STM32F427_437xx ||  STM32F429xx */

#if defined (STM32F446xx)
#define IS_GPIO_AF(AF)   (((AF) < 16) && ((AF) != 11) && ((AF) != 14))
#endif /* STM32F446xx */

/**
  * @}
  */ 

/** @defgroup GPIO_Legacy 
  * @{
  */
    
#define GPIO_Mode_AIN           GPIO_Mode_AN

#define GPIO_AF_OTG1_FS         GPIO_AF_OTG_FS
#define GPIO_AF_OTG2_HS         GPIO_AF_OTG_HS
#define GPIO_AF_OTG2_FS         GPIO_AF_OTG_HS_FS

/**
  * @}
  */

/**
  * @}
  */

/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/*  Function used to set the GPIO configuration to the default reset state ****/
void GPIO_DeInit(GPIO_TypeDef* GPIOx);

/* Initialization and Configuration functions *********************************/
void GPIO_Init(GPIO_TypeDef* GPIOx, GPIO_InitTypeDef* GPIO_InitStruct);
void GPIO_StructInit(GPIO_InitTypeDef* GPIO_InitStruct);
void GPIO_PinLockConfig(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);

/* GPIO Read and Write functions **********************************************/
uint8_t GPIO_ReadInputDataBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
uint16_t GPIO_ReadInputData(GPIO_TypeDef* GPIOx);
uint8_t GPIO_ReadOutputDataBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
uint16_t GPIO_ReadOutputData(GPIO_TypeDef* GPIOx);
void GPIO_SetBits(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
void GPIO_ResetBits(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
void GPIO_WriteBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, BitAction BitVal);
void GPIO_Write(GPIO_TypeDef* GPIOx, uint16_t PortVal);
void GPIO_ToggleBits(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);

/* GPIO Alternate functions configuration function ****************************/
void GPIO_PinAFConfig(GPIO_TypeDef* GPIOx, uint16_t GPIO_PinSource, uint8_t GPIO_AF);

void GPIO_PinAFConfig(GPIO_TypeDef* GPIOx, uint16_t GPIO_PinSource, uint8_t GPIO_AF)
{
  uint32_t temp = 0x00;
  uint32_t temp_2 = 0x00;
  
  /* Check the parameters */
  assert_param(IS_GPIO_ALL_PERIPH(GPIOx));
  assert_param(IS_GPIO_PIN_SOURCE(GPIO_PinSource));
  assert_param(IS_GPIO_AF(GPIO_AF));
  
  temp = ((uint32_t)(GPIO_AF) << ((uint32_t)((uint32_t)GPIO_PinSource & (uint32_t)0x07) * 4)) ;
  GPIOx->AFR[GPIO_PinSource >> 0x03] &= ~((uint32_t)0xF << ((uint32_t)((uint32_t)GPIO_PinSource & (uint32_t)0x07) * 4)) ;
  temp_2 = GPIOx->AFR[GPIO_PinSource >> 0x03] | temp;
  GPIOx->AFR[GPIO_PinSource >> 0x03] = temp_2;
}
void GPIO_Init(GPIO_TypeDef* GPIOx, GPIO_InitTypeDef* GPIO_InitStruct)
{
  uint32_t pinpos = 0x00, pos = 0x00 , currentpin = 0x00;

  /* Check the parameters */
  assert_param(IS_GPIO_ALL_PERIPH(GPIOx));
  assert_param(IS_GPIO_PIN(GPIO_InitStruct->GPIO_Pin));
  assert_param(IS_GPIO_MODE(GPIO_InitStruct->GPIO_Mode));
  assert_param(IS_GPIO_PUPD(GPIO_InitStruct->GPIO_PuPd));

  /* ------------------------- Configure the port pins ---------------- */
  /*-- GPIO Mode Configuration --*/
  for (pinpos = 0x00; pinpos < 0x10; pinpos++)
  {
    pos = ((uint32_t)0x01) << pinpos;
    /* Get the port pins position */
    currentpin = (GPIO_InitStruct->GPIO_Pin) & pos;

    if (currentpin == pos)
    {
      GPIOx->MODER  &= ~(GPIO_MODER_MODER0 << (pinpos * 2));
      GPIOx->MODER |= (((uint32_t)GPIO_InitStruct->GPIO_Mode) << (pinpos * 2));

      if ((GPIO_InitStruct->GPIO_Mode == GPIO_Mode_OUT) || (GPIO_InitStruct->GPIO_Mode == GPIO_Mode_AF))
      {
        /* Check Speed mode parameters */
        assert_param(IS_GPIO_SPEED(GPIO_InitStruct->GPIO_Speed));

        /* Speed mode configuration */
        GPIOx->OSPEEDR &= ~(GPIO_OSPEEDER_OSPEEDR0 << (pinpos * 2));
        GPIOx->OSPEEDR |= ((uint32_t)(GPIO_InitStruct->GPIO_Speed) << (pinpos * 2));

        /* Check Output mode parameters */
        assert_param(IS_GPIO_OTYPE(GPIO_InitStruct->GPIO_OType));

        /* Output mode configuration*/
        GPIOx->OTYPER  &= ~((GPIO_OTYPER_OT_0) << ((uint16_t)pinpos)) ;
        GPIOx->OTYPER |= (uint16_t)(((uint16_t)GPIO_InitStruct->GPIO_OType) << ((uint16_t)pinpos));
      }

      /* Pull-up Pull down resistor configuration*/
      GPIOx->PUPDR &= ~(GPIO_PUPDR_PUPDR0 << ((uint16_t)pinpos * 2));
      GPIOx->PUPDR |= (((uint32_t)GPIO_InitStruct->GPIO_PuPd) << (pinpos * 2));
    }
  }
}
void GPIO_StructInit(GPIO_InitTypeDef* GPIO_InitStruct)
{
  /* Reset GPIO init structure parameters values */
  GPIO_InitStruct->GPIO_Pin  = GPIO_Pin_All;
  GPIO_InitStruct->GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStruct->GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStruct->GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct->GPIO_PuPd = GPIO_PuPd_NOPULL;
}
void FMC_SDRAMInit(FMC_SDRAMInitTypeDef* FMC_SDRAMInitStruct)
{ 
  /* temporary registers */
  uint32_t tmpr1 = 0, tmpr2 = 0, tmpr3 = 0, tmpr4 = 0;
  
  /* Check the parameters */
  
  /* Control parameters */
  assert_param(IS_FMC_SDRAM_BANK(FMC_SDRAMInitStruct->FMC_Bank));
  assert_param(IS_FMC_COLUMNBITS_NUMBER(FMC_SDRAMInitStruct->FMC_ColumnBitsNumber)); 
  assert_param(IS_FMC_ROWBITS_NUMBER(FMC_SDRAMInitStruct->FMC_RowBitsNumber));
  assert_param(IS_FMC_SDMEMORY_WIDTH(FMC_SDRAMInitStruct->FMC_SDMemoryDataWidth));
  assert_param(IS_FMC_INTERNALBANK_NUMBER(FMC_SDRAMInitStruct->FMC_InternalBankNumber)); 
  assert_param(IS_FMC_CAS_LATENCY(FMC_SDRAMInitStruct->FMC_CASLatency));
  assert_param(IS_FMC_WRITE_PROTECTION(FMC_SDRAMInitStruct->FMC_WriteProtection));
  assert_param(IS_FMC_SDCLOCK_PERIOD(FMC_SDRAMInitStruct->FMC_SDClockPeriod));
  assert_param(IS_FMC_READ_BURST(FMC_SDRAMInitStruct->FMC_ReadBurst));
  assert_param(IS_FMC_READPIPE_DELAY(FMC_SDRAMInitStruct->FMC_ReadPipeDelay));   
  
  /* Timing parameters */
  assert_param(IS_FMC_LOADTOACTIVE_DELAY(FMC_SDRAMInitStruct->FMC_SDRAMTimingStruct->FMC_LoadToActiveDelay)); 
  assert_param(IS_FMC_EXITSELFREFRESH_DELAY(FMC_SDRAMInitStruct->FMC_SDRAMTimingStruct->FMC_ExitSelfRefreshDelay));
  assert_param(IS_FMC_SELFREFRESH_TIME(FMC_SDRAMInitStruct->FMC_SDRAMTimingStruct->FMC_SelfRefreshTime));
  assert_param(IS_FMC_ROWCYCLE_DELAY(FMC_SDRAMInitStruct->FMC_SDRAMTimingStruct->FMC_RowCycleDelay));
  assert_param(IS_FMC_WRITE_RECOVERY_TIME(FMC_SDRAMInitStruct->FMC_SDRAMTimingStruct->FMC_WriteRecoveryTime)); 
  assert_param(IS_FMC_RP_DELAY(FMC_SDRAMInitStruct->FMC_SDRAMTimingStruct->FMC_RPDelay)); 
  assert_param(IS_FMC_RCD_DELAY(FMC_SDRAMInitStruct->FMC_SDRAMTimingStruct->FMC_RCDDelay));    
  
  /* Get SDRAM register value */
  tmpr1 = FMC_Bank5_6->SDCR[FMC_SDRAMInitStruct->FMC_Bank];

  /* Clear NC, NR, MWID, NB, CAS, WP, SDCLK, RBURST, and RPIPE bits */
  tmpr1 &= ((uint32_t)~(FMC_SDCR1_NC | FMC_SDCR1_NR | FMC_SDCR1_MWID | \
                        FMC_SDCR1_NB | FMC_SDCR1_CAS | FMC_SDCR1_WP | \
                        FMC_SDCR1_SDCLK | FMC_SDCR1_RBURST | FMC_SDCR1_RPIPE));

  /* SDRAM bank control register configuration */ 
  tmpr1 |=   (uint32_t)FMC_SDRAMInitStruct->FMC_ColumnBitsNumber |
                       FMC_SDRAMInitStruct->FMC_RowBitsNumber |
                       FMC_SDRAMInitStruct->FMC_SDMemoryDataWidth |
                       FMC_SDRAMInitStruct->FMC_InternalBankNumber |           
                       FMC_SDRAMInitStruct->FMC_CASLatency |
                       FMC_SDRAMInitStruct->FMC_WriteProtection |
                       FMC_SDRAMInitStruct->FMC_SDClockPeriod |
                       FMC_SDRAMInitStruct->FMC_ReadBurst | 
                       FMC_SDRAMInitStruct->FMC_ReadPipeDelay;
            
  if(FMC_SDRAMInitStruct->FMC_Bank == FMC_Bank1_SDRAM )
  {
    FMC_Bank5_6->SDCR[FMC_SDRAMInitStruct->FMC_Bank] = tmpr1;
  }
  else   /* SDCR2 "don't care" bits configuration */
  {
    /* Get SDCR register value */
    tmpr3 = FMC_Bank5_6->SDCR[FMC_Bank1_SDRAM];

    /* Clear NC, NR, MWID, NB, CAS, WP, SDCLK, RBURST, and RPIPE bits */
    tmpr3 &= ((uint32_t)~(FMC_SDCR1_NC  | FMC_SDCR1_NR | FMC_SDCR1_MWID | \
                          FMC_SDCR1_NB  | FMC_SDCR1_CAS | FMC_SDCR1_WP | \
                          FMC_SDCR1_SDCLK | FMC_SDCR1_RBURST | FMC_SDCR1_RPIPE));

    tmpr3 |= (uint32_t)FMC_SDRAMInitStruct->FMC_SDClockPeriod |
                       FMC_SDRAMInitStruct->FMC_ReadBurst | 
                       FMC_SDRAMInitStruct->FMC_ReadPipeDelay;
    
    FMC_Bank5_6->SDCR[FMC_Bank1_SDRAM] = tmpr3;
    FMC_Bank5_6->SDCR[FMC_SDRAMInitStruct->FMC_Bank] = tmpr1;
  }
  /* SDRAM bank timing register configuration */
  if(FMC_SDRAMInitStruct->FMC_Bank == FMC_Bank1_SDRAM )
  {
    /* Get SDTR register value */
    tmpr2 = FMC_Bank5_6->SDTR[FMC_SDRAMInitStruct->FMC_Bank];

    /* Clear TMRD, TXSR, TRAS, TRC, TWR, TRP and TRCD bits */
    tmpr2 &= ((uint32_t)~(FMC_SDTR1_TMRD  | FMC_SDTR1_TXSR | FMC_SDTR1_TRAS | \
                          FMC_SDTR1_TRC  | FMC_SDTR1_TWR | FMC_SDTR1_TRP | \
                          FMC_SDTR1_TRCD));

    tmpr2 |=   (uint32_t)((FMC_SDRAMInitStruct->FMC_SDRAMTimingStruct->FMC_LoadToActiveDelay)-1) |
                          (((FMC_SDRAMInitStruct->FMC_SDRAMTimingStruct->FMC_ExitSelfRefreshDelay)-1) << 4) |
                          (((FMC_SDRAMInitStruct->FMC_SDRAMTimingStruct->FMC_SelfRefreshTime)-1) << 8) |
                          (((FMC_SDRAMInitStruct->FMC_SDRAMTimingStruct->FMC_RowCycleDelay)-1) << 12) |
                          (((FMC_SDRAMInitStruct->FMC_SDRAMTimingStruct->FMC_WriteRecoveryTime)-1) << 16) |
                          (((FMC_SDRAMInitStruct->FMC_SDRAMTimingStruct->FMC_RPDelay)-1) << 20) |
                          (((FMC_SDRAMInitStruct->FMC_SDRAMTimingStruct->FMC_RCDDelay)-1) << 24);
            
            FMC_Bank5_6->SDTR[FMC_SDRAMInitStruct->FMC_Bank] = tmpr2;
  }
  else   /* SDTR "don't care bits configuration */
  {
    /* Get SDTR register value */
    tmpr2 = FMC_Bank5_6->SDTR[FMC_SDRAMInitStruct->FMC_Bank];

    /* Clear TMRD, TXSR, TRAS, TRC, TWR, TRP and TRCD bits */
    tmpr2 &= ((uint32_t)~(FMC_SDTR1_TMRD  | FMC_SDTR1_TXSR | FMC_SDTR1_TRAS | \
                          FMC_SDTR1_TRC  | FMC_SDTR1_TWR | FMC_SDTR1_TRP | \
                          FMC_SDTR1_TRCD));

    tmpr2 |=   (uint32_t)((FMC_SDRAMInitStruct->FMC_SDRAMTimingStruct->FMC_LoadToActiveDelay)-1) |
                          (((FMC_SDRAMInitStruct->FMC_SDRAMTimingStruct->FMC_ExitSelfRefreshDelay)-1) << 4) |
                          (((FMC_SDRAMInitStruct->FMC_SDRAMTimingStruct->FMC_SelfRefreshTime)-1) << 8) |
                          (((FMC_SDRAMInitStruct->FMC_SDRAMTimingStruct->FMC_WriteRecoveryTime)-1) << 16);
    
    /* Get SDTR register value */
    tmpr4 = FMC_Bank5_6->SDTR[FMC_Bank1_SDRAM];

    /* Clear TMRD, TXSR, TRAS, TRC, TWR, TRP and TRCD bits */
    tmpr4 &= ((uint32_t)~(FMC_SDTR1_TMRD  | FMC_SDTR1_TXSR | FMC_SDTR1_TRAS | \
                          FMC_SDTR1_TRC  | FMC_SDTR1_TWR | FMC_SDTR1_TRP | \
                          FMC_SDTR1_TRCD));

    tmpr4 |=   (uint32_t)(((FMC_SDRAMInitStruct->FMC_SDRAMTimingStruct->FMC_RowCycleDelay)-1) << 12) |
                          (((FMC_SDRAMInitStruct->FMC_SDRAMTimingStruct->FMC_RPDelay)-1) << 20);
            
            FMC_Bank5_6->SDTR[FMC_Bank1_SDRAM] = tmpr4;
            FMC_Bank5_6->SDTR[FMC_SDRAMInitStruct->FMC_Bank] = tmpr2;
  }
  
}

void FMC_SDRAMStructInit(FMC_SDRAMInitTypeDef* FMC_SDRAMInitStruct)  
{  
  /* Reset SDRAM Init structure parameters values */
  FMC_SDRAMInitStruct->FMC_Bank = FMC_Bank1_SDRAM;
  FMC_SDRAMInitStruct->FMC_ColumnBitsNumber = FMC_ColumnBits_Number_8b;
  FMC_SDRAMInitStruct->FMC_RowBitsNumber = FMC_RowBits_Number_11b; 
  FMC_SDRAMInitStruct->FMC_SDMemoryDataWidth = FMC_SDMemory_Width_16b;
  FMC_SDRAMInitStruct->FMC_InternalBankNumber = FMC_InternalBank_Number_4; 
  FMC_SDRAMInitStruct->FMC_CASLatency = FMC_CAS_Latency_1;  
  FMC_SDRAMInitStruct->FMC_WriteProtection = FMC_Write_Protection_Enable;
  FMC_SDRAMInitStruct->FMC_SDClockPeriod = FMC_SDClock_Disable;
  FMC_SDRAMInitStruct->FMC_ReadBurst = FMC_Read_Burst_Disable;
  FMC_SDRAMInitStruct->FMC_ReadPipeDelay = FMC_ReadPipe_Delay_0; 
   
  FMC_SDRAMInitStruct->FMC_SDRAMTimingStruct->FMC_LoadToActiveDelay = 16;
  FMC_SDRAMInitStruct->FMC_SDRAMTimingStruct->FMC_ExitSelfRefreshDelay = 16;
  FMC_SDRAMInitStruct->FMC_SDRAMTimingStruct->FMC_SelfRefreshTime = 16;
  FMC_SDRAMInitStruct->FMC_SDRAMTimingStruct->FMC_RowCycleDelay = 16;
  FMC_SDRAMInitStruct->FMC_SDRAMTimingStruct->FMC_WriteRecoveryTime = 16;
  FMC_SDRAMInitStruct->FMC_SDRAMTimingStruct->FMC_RPDelay = 16;
  FMC_SDRAMInitStruct->FMC_SDRAMTimingStruct->FMC_RCDDelay = 16;
  
}
void FMC_SDRAMCmdConfig(FMC_SDRAMCommandTypeDef* FMC_SDRAMCommandStruct)
{
  uint32_t tmpr = 0x0;
    
  /* check parameters */
  assert_param(IS_FMC_COMMAND_MODE(FMC_SDRAMCommandStruct->FMC_CommandMode));
  assert_param(IS_FMC_COMMAND_TARGET(FMC_SDRAMCommandStruct->FMC_CommandTarget));
  assert_param(IS_FMC_AUTOREFRESH_NUMBER(FMC_SDRAMCommandStruct->FMC_AutoRefreshNumber));
  assert_param(IS_FMC_MODE_REGISTER(FMC_SDRAMCommandStruct->FMC_ModeRegisterDefinition));
  
  tmpr =   (uint32_t)(FMC_SDRAMCommandStruct->FMC_CommandMode |
                      FMC_SDRAMCommandStruct->FMC_CommandTarget |
                     (((FMC_SDRAMCommandStruct->FMC_AutoRefreshNumber)-1)<<5) |
                     ((FMC_SDRAMCommandStruct->FMC_ModeRegisterDefinition)<<9));
  
  FMC_Bank5_6->SDCMR = tmpr;

}


void RCC_AHB1PeriphClockCmd(uint32_t RCC_AHB1Periph, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_RCC_AHB1_CLOCK_PERIPH(RCC_AHB1Periph));

  assert_param(IS_FUNCTIONAL_STATE(NewState));
  if (NewState != DISABLE)
  {
    RCC->AHB1ENR |= RCC_AHB1Periph;
  }
  else
  {
    RCC->AHB1ENR &= ~RCC_AHB1Periph;
  }
}

/**
  * @brief  Configures all SDRAM memory I/Os pins. 
  * @param  None. 
  * @retval None.
  */
void SDRAM_GPIOConfig(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  /* Enable GPIOs clock */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD |
                         RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOF | RCC_AHB1Periph_GPIOG, ENABLE);
                            
/*-- GPIOs Configuration -----------------------------------------------------*/
/*
 +-------------------+--------------------+--------------------+--------------------+
 +                       SDRAM pins assignment                                      +
 +-------------------+--------------------+--------------------+--------------------+
 | PD0  <-> FMC_D2   | PE0  <-> FMC_NBL0  | PF0  <-> FMC_A0    | PG0  <-> FMC_A10   |
 | PD1  <-> FMC_D3   | PE1  <-> FMC_NBL1  | PF1  <-> FMC_A1    | PG1  <-> FMC_A11   |
 | PD8  <-> FMC_D13  | PE7  <-> FMC_D4    | PF2  <-> FMC_A2    | PG8  <-> FMC_SDCLK |
 | PD9  <-> FMC_D14  | PE8  <-> FMC_D5    | PF3  <-> FMC_A3    | PG15 <-> FMC_NCAS  |
 | PD10 <-> FMC_D15  | PE9  <-> FMC_D6    | PF4  <-> FMC_A4    |--------------------+ 
 | PD14 <-> FMC_D0   | PE10 <-> FMC_D7    | PF5  <-> FMC_A5    |   
 | PD15 <-> FMC_D1   | PE11 <-> FMC_D8    | PF11 <-> FMC_NRAS  | 
 +-------------------| PE12 <-> FMC_D9    | PF12 <-> FMC_A6    | 
                     | PE13 <-> FMC_D10   | PF13 <-> FMC_A7    |    
                     | PE14 <-> FMC_D11   | PF14 <-> FMC_A8    |
                     | PE15 <-> FMC_D12   | PF15 <-> FMC_A9    |
 +-------------------+--------------------+--------------------+
 | PB5 <-> FMC_SDCKE1| 
 | PB6 <-> FMC_SDNE1 | 
 | PC0 <-> FMC_SDNWE |
 +-------------------+  
  
*/
  
  /* Common GPIO configuration */
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;

  /* GPIOB configuration */
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource5 , GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource6 , GPIO_AF_FMC);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5  | GPIO_Pin_6;      

  GPIO_Init(GPIOB, &GPIO_InitStructure);  

  /* GPIOC configuration */
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource0 , GPIO_AF_FMC);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;      

  GPIO_Init(GPIOC, &GPIO_InitStructure);  
  
  /* GPIOD configuration */
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource0, GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource1, GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource10, GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_FMC);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1  | GPIO_Pin_8 |
                                GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_14 |
                                GPIO_Pin_15;

  GPIO_Init(GPIOD, &GPIO_InitStructure);

  /* GPIOE configuration */
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource0 , GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource1 , GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource7 , GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource8 , GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource9 , GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource10 , GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource11 , GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource12 , GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource13 , GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource14 , GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource15 , GPIO_AF_FMC);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0  | GPIO_Pin_1  | GPIO_Pin_7 |
                                GPIO_Pin_8  | GPIO_Pin_9  | GPIO_Pin_10 |
                                GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 |
                                GPIO_Pin_14 | GPIO_Pin_15;

  GPIO_Init(GPIOE, &GPIO_InitStructure);

  /* GPIOF configuration */
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource0 , GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource1 , GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource2 , GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource3 , GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource4 , GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource5 , GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource11 , GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource12 , GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource13 , GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource14 , GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource15 , GPIO_AF_FMC);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0  | GPIO_Pin_1 | GPIO_Pin_2 | 
                                GPIO_Pin_3  | GPIO_Pin_4 | GPIO_Pin_5 |
                                GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 |
                                GPIO_Pin_14 | GPIO_Pin_15;      

  GPIO_Init(GPIOF, &GPIO_InitStructure);

  /* GPIOG configuration */
  GPIO_PinAFConfig(GPIOG, GPIO_PinSource0 , GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOG, GPIO_PinSource1 , GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOG, GPIO_PinSource4 , GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOG, GPIO_PinSource5 , GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOG, GPIO_PinSource8 , GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOG, GPIO_PinSource15 , GPIO_AF_FMC);
  

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 |
                                GPIO_Pin_5 | GPIO_Pin_8 | GPIO_Pin_15;

  GPIO_Init(GPIOG, &GPIO_InitStructure);    
}

void FMC_SetRefreshCount(uint32_t FMC_Count)
{
  /* check the parameters */
  assert_param(IS_FMC_REFRESH_COUNT(FMC_Count));
  
  FMC_Bank5_6->SDRTR |= (FMC_Count<<1);
   
}

FlagStatus FMC_GetFlagStatus(uint32_t FMC_Bank, uint32_t FMC_FLAG)
{
  FlagStatus bitstatus = RESET;
  uint32_t tmpsr = 0x00000000;
  
  /* Check the parameters */
  assert_param(IS_FMC_GETFLAG_BANK(FMC_Bank));
  assert_param(IS_FMC_GET_FLAG(FMC_FLAG));
  
  if(FMC_Bank == FMC_Bank2_NAND)
  {
    tmpsr = FMC_Bank2_3->SR2;
  }  
  else if(FMC_Bank == FMC_Bank3_NAND)
  {
    tmpsr = FMC_Bank2_3->SR3;
  }
  else if(FMC_Bank == FMC_Bank4_PCCARD)
  {
    tmpsr = FMC_Bank4->SR4;
  }
  else 
  {
    tmpsr = FMC_Bank5_6->SDSR;
  }
  
  /* Get the flag status */
  if ((tmpsr & FMC_FLAG) != FMC_FLAG )
  {
    bitstatus = RESET;
  }
  else
  {
    bitstatus = SET;
  }
  /* Return the flag status */
  return bitstatus;
}

/**
  * @brief  Executes the SDRAM memory initialization sequence. 
  * @param  None. 
  * @retval None.
  */
void SDRAM_InitSequence(void)
{
  FMC_SDRAMCommandTypeDef FMC_SDRAMCommandStructure;
  uint32_t tmpr = 0;
  
/* Step 3 --------------------------------------------------------------------*/
  /* Configure a clock configuration enable command */
  FMC_SDRAMCommandStructure.FMC_CommandMode = FMC_Command_Mode_CLK_Enabled;
  FMC_SDRAMCommandStructure.FMC_CommandTarget = FMC_Command_Target_bank2;
  FMC_SDRAMCommandStructure.FMC_AutoRefreshNumber = 1;
  FMC_SDRAMCommandStructure.FMC_ModeRegisterDefinition = 0;
  /* Wait until the SDRAM controller is ready */ 
  while(FMC_GetFlagStatus(FMC_Bank2_SDRAM, FMC_FLAG_Busy) != RESET)
  {
  }
  /* Send the command */
  FMC_SDRAMCmdConfig(&FMC_SDRAMCommandStructure);  
  
/* Step 4 --------------------------------------------------------------------*/
  /* Insert 100 ms delay */
  __Delay(10);
    
/* Step 5 --------------------------------------------------------------------*/
  /* Configure a PALL (precharge all) command */ 
  FMC_SDRAMCommandStructure.FMC_CommandMode = FMC_Command_Mode_PALL;
  FMC_SDRAMCommandStructure.FMC_CommandTarget = FMC_Command_Target_bank2;
  FMC_SDRAMCommandStructure.FMC_AutoRefreshNumber = 1;
  FMC_SDRAMCommandStructure.FMC_ModeRegisterDefinition = 0;
  /* Wait until the SDRAM controller is ready */ 
  while(FMC_GetFlagStatus(FMC_Bank2_SDRAM, FMC_FLAG_Busy) != RESET)
  {
  }
  /* Send the command */
  FMC_SDRAMCmdConfig(&FMC_SDRAMCommandStructure);
  
/* Step 6 --------------------------------------------------------------------*/
  /* Configure a Auto-Refresh command */ 
  FMC_SDRAMCommandStructure.FMC_CommandMode = FMC_Command_Mode_AutoRefresh;
  FMC_SDRAMCommandStructure.FMC_CommandTarget = FMC_Command_Target_bank2;
  FMC_SDRAMCommandStructure.FMC_AutoRefreshNumber = 4;
  FMC_SDRAMCommandStructure.FMC_ModeRegisterDefinition = 0;
  /* Wait until the SDRAM controller is ready */ 
  while(FMC_GetFlagStatus(FMC_Bank2_SDRAM, FMC_FLAG_Busy) != RESET)
  {
  }
  /* Send the  first command */
  FMC_SDRAMCmdConfig(&FMC_SDRAMCommandStructure);
  
  /* Wait until the SDRAM controller is ready */ 
  while(FMC_GetFlagStatus(FMC_Bank2_SDRAM, FMC_FLAG_Busy) != RESET)
  {
  }
  /* Send the second command */
  FMC_SDRAMCmdConfig(&FMC_SDRAMCommandStructure);
  
/* Step 7 --------------------------------------------------------------------*/
  /* Program the external memory mode register */
  tmpr = (uint32_t)SDRAM_MODEREG_BURST_LENGTH_2          |
                   SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL   |
                   SDRAM_MODEREG_CAS_LATENCY_3           |
                   SDRAM_MODEREG_OPERATING_MODE_STANDARD |
                   SDRAM_MODEREG_WRITEBURST_MODE_SINGLE;
  
  /* Configure a load Mode register command*/ 
  FMC_SDRAMCommandStructure.FMC_CommandMode = FMC_Command_Mode_LoadMode;
  FMC_SDRAMCommandStructure.FMC_CommandTarget = FMC_Command_Target_bank2;
  FMC_SDRAMCommandStructure.FMC_AutoRefreshNumber = 1;
  FMC_SDRAMCommandStructure.FMC_ModeRegisterDefinition = tmpr;
  /* Wait until the SDRAM controller is ready */ 
  while(FMC_GetFlagStatus(FMC_Bank2_SDRAM, FMC_FLAG_Busy) != RESET)
  {
  }
  /* Send the command */
  FMC_SDRAMCmdConfig(&FMC_SDRAMCommandStructure);
  
/* Step 8 --------------------------------------------------------------------*/

  /* Set the refresh rate counter */
  /* (7.81 us x Freq) - 20 */
  /* Set the device refresh counter */
  FMC_SetRefreshCount(683);
  /* Wait until the SDRAM controller is ready */ 
  while(FMC_GetFlagStatus(FMC_Bank2_SDRAM, FMC_FLAG_Busy) != RESET)
  {
  }
}

/**
  * @brief  Configures the FMC and GPIOs to interface with the SDRAM memory.
  *         This function must be called before any read/write operation
  *         on the SDRAM.
  * @param  None
  * @retval None

  SDRAM_Init
  */
void arm_hardware_sdram_initialize(void)
{
  FMC_SDRAMInitTypeDef  FMC_SDRAMInitStructure;
  FMC_SDRAMTimingInitTypeDef  FMC_SDRAMTimingInitStructure; 
  
  /* GPIO configuration for FMC SDRAM bank */
  SDRAM_GPIOConfig();
  
  /* Enable FMC clock */
  //RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FMC, ENABLE);
  
	RCC->AHB3ENR |= RCC_AHB3ENR_FMCEN;	/* FMC clock enable */
	__DSB();
 
/* FMC Configuration ---------------------------------------------------------*/
/* FMC SDRAM Bank configuration */   
  /* Timing configuration for 84 Mhz of SD clock frequency (168Mhz/2) */
  /* TMRD: 2 Clock cycles */
  FMC_SDRAMTimingInitStructure.FMC_LoadToActiveDelay = 2;      
  /* TXSR: min=70ns (6x11.90ns) */
  FMC_SDRAMTimingInitStructure.FMC_ExitSelfRefreshDelay = 7;
  /* TRAS: min=42ns (4x11.90ns) max=120k (ns) */
  FMC_SDRAMTimingInitStructure.FMC_SelfRefreshTime = 4;
  /* TRC:  min=63 (6x11.90ns) */        
  FMC_SDRAMTimingInitStructure.FMC_RowCycleDelay = 7;         
  /* TWR:  2 Clock cycles */
  FMC_SDRAMTimingInitStructure.FMC_WriteRecoveryTime = 2;      
  /* TRP:  15ns => 2x11.90ns */
  FMC_SDRAMTimingInitStructure.FMC_RPDelay = 2;                
  /* TRCD: 15ns => 2x11.90ns */
  FMC_SDRAMTimingInitStructure.FMC_RCDDelay = 2;

/* FMC SDRAM control configuration */
  FMC_SDRAMInitStructure.FMC_Bank = FMC_Bank2_SDRAM;
  /* Row addressing: [7:0] */
  FMC_SDRAMInitStructure.FMC_ColumnBitsNumber = FMC_ColumnBits_Number_8b;
  /* Column addressing: [11:0] */
  FMC_SDRAMInitStructure.FMC_RowBitsNumber = FMC_RowBits_Number_12b;
  FMC_SDRAMInitStructure.FMC_SDMemoryDataWidth = SDRAM_MEMORY_WIDTH;
  FMC_SDRAMInitStructure.FMC_InternalBankNumber = FMC_InternalBank_Number_4;
  FMC_SDRAMInitStructure.FMC_CASLatency = SDRAM_CAS_LATENCY; 
  FMC_SDRAMInitStructure.FMC_WriteProtection = FMC_Write_Protection_Disable;
  FMC_SDRAMInitStructure.FMC_SDClockPeriod = SDCLOCK_PERIOD;  
  FMC_SDRAMInitStructure.FMC_ReadBurst = SDRAM_READBURST;
  FMC_SDRAMInitStructure.FMC_ReadPipeDelay = FMC_ReadPipe_Delay_1;
  FMC_SDRAMInitStructure.FMC_SDRAMTimingStruct = &FMC_SDRAMTimingInitStructure;
  
  /* FMC SDRAM bank initialization */
  FMC_SDRAMInit(&FMC_SDRAMInitStructure); 
  
  /* FMC SDRAM device initialization sequence */
  SDRAM_InitSequence(); 
  
}

#elif CPUSTYLE_STM32MP157A

void FLASHMEMINITFUNC arm_hardware_sdram_initialize(void)
{

}

#endif

#endif /* WITHSDRAMHW */
