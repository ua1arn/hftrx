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
#include "gpio.h"

#if WITHSDRAMHW

#include "sdram.h"

void sdram_test_pattern(uint_fast32_t addr, uint_fast16_t buffer_size, uint_fast16_t pattern)
{
	for (uint32_t i = 0; i < buffer_size; i++)
		*(volatile uint16_t*) (addr + 2 * i) = pattern;

	volatile uint16_t r;
	for (uint32_t i = 0; i < buffer_size; i++)
	{
		r = *(volatile uint16_t*) (addr + 2 * i);
		if(r !=  pattern)
			PRINTF("ERROR! %X - fill pattern: %04X, read: %04X\n", addr + 2 * i,  pattern, r);
	}

}

void sdram_test_increment(uint_fast32_t addr, uint_fast16_t buffer_size, uint_fast16_t seed)
{
	for (uint32_t i = 0; i < buffer_size; i++)
		*(volatile uint16_t*) (addr + 2 * i) = seed + i;

	volatile uint16_t r;
	for (uint32_t i = 0; i < buffer_size; i++)
	{
		r = *(volatile uint16_t*) (addr + 2 * i);
		if(r !=  seed + i)
			PRINTF("ERROR! %X - fill increment: %04X, read: %04X\n", addr + 2 * i,  seed + i, r);
	}

}

void sdram_test_random(uint_fast32_t addr, uint_fast16_t buffer_size)
{
	volatile uint16_t aTxBuffer[buffer_size];
	volatile uint16_t r;

	for (uint32_t i = 0; i < buffer_size; i++)
	{
		aTxBuffer[i] = rand() & 0xFFFF;
		*(volatile uint16_t*) (addr + 2 * i) = aTxBuffer[i];
	}

	for (uint32_t i = 0; i < buffer_size; i++)
	{
		r = *(volatile uint16_t*) (addr + 2 * i);
		if(aTxBuffer[i] !=  r)
			PRINTF("ERROR! %X - fill random: %04X, read: %04X\n", addr + 2 * i, aTxBuffer[i], r);
	}

}

#if CPUSTYLE_STM32F

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

/**
  * @brief  Configures all SDRAM memory I/Os pins. 
  * @param  None. 
  * @retval None.
  */
void SDRAM_GPIOConfig(void)
{
#if defined CTLSTYLE_V1D	/* Плата STM32F429I-DISCO с процессором STM32F429ZIT6	*/
                            
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
  enum { GPIO_AF_FMC = 12 };

  arm_hardware_piob_altfn50((1U << 5), GPIO_AF_FMC);
  arm_hardware_piob_altfn50((1U << 6), GPIO_AF_FMC);
  arm_hardware_pioc_altfn50((1U << 0), GPIO_AF_FMC);
  arm_hardware_piod_altfn50((1U << 0), GPIO_AF_FMC);
  arm_hardware_piod_altfn50((1U << 1), GPIO_AF_FMC);
  arm_hardware_piod_altfn50((1U << 8), GPIO_AF_FMC);
  arm_hardware_piod_altfn50((1U << 9), GPIO_AF_FMC);
  arm_hardware_piod_altfn50((1U << 10), GPIO_AF_FMC);
  arm_hardware_piod_altfn50((1U << 14), GPIO_AF_FMC);
  arm_hardware_piod_altfn50((1U << 15), GPIO_AF_FMC);
  arm_hardware_pioe_altfn50((1U << 0), GPIO_AF_FMC);
  arm_hardware_pioe_altfn50((1U << 1), GPIO_AF_FMC);
  arm_hardware_pioe_altfn50((1U << 7), GPIO_AF_FMC);
  arm_hardware_pioe_altfn50((1U << 8), GPIO_AF_FMC);
  arm_hardware_pioe_altfn50((1U << 9), GPIO_AF_FMC);
  arm_hardware_pioe_altfn50((1U << 10), GPIO_AF_FMC);
  arm_hardware_pioe_altfn50((1U << 11), GPIO_AF_FMC);
  arm_hardware_pioe_altfn50((1U << 12), GPIO_AF_FMC);
  arm_hardware_pioe_altfn50((1U << 13), GPIO_AF_FMC);
  arm_hardware_pioe_altfn50((1U << 14), GPIO_AF_FMC);
  arm_hardware_pioe_altfn50((1U << 15), GPIO_AF_FMC);
  arm_hardware_piof_altfn50((1U << 0), GPIO_AF_FMC);
  arm_hardware_piof_altfn50((1U << 1), GPIO_AF_FMC);
  arm_hardware_piof_altfn50((1U << 2), GPIO_AF_FMC);
  arm_hardware_piof_altfn50((1U << 3), GPIO_AF_FMC);
  arm_hardware_piof_altfn50((1U << 4), GPIO_AF_FMC);
  arm_hardware_piof_altfn50((1U << 5), GPIO_AF_FMC);
  arm_hardware_piof_altfn50((1U << 11), GPIO_AF_FMC);
  arm_hardware_piof_altfn50((1U << 12), GPIO_AF_FMC);
  arm_hardware_piof_altfn50((1U << 13), GPIO_AF_FMC);
  arm_hardware_piof_altfn50((1U << 14), GPIO_AF_FMC);
  arm_hardware_piof_altfn50((1U << 15), GPIO_AF_FMC);
  arm_hardware_piog_altfn50((1U << 0), GPIO_AF_FMC);
  arm_hardware_piog_altfn50((1U << 1), GPIO_AF_FMC);
  arm_hardware_piog_altfn50((1U << 4), GPIO_AF_FMC);
  arm_hardware_piog_altfn50((1U << 5), GPIO_AF_FMC);
  arm_hardware_piog_altfn50((1U << 8), GPIO_AF_FMC);
  arm_hardware_piog_altfn50((1U << 15), GPIO_AF_FMC);

#elif defined CTLSTYLE_V3D	/* Плата STM32F746G-DISCO с процессором STM32F746NGH6	*/

  /*
   D0 -> PD14*		A0 -> PF0*		SDNWE  -> PH5*
   D1 -> PD15*		A1 -> PF1*		SDNRAS -> PF11*
   D2 -> PD0*		A2 -> PF2*		SDNCAS -> PG15*
   D3 -> PD1*		A3 -> PF3*		SDCLK  -> PG8*
   D4 -> PE7*		A4 -> PF4*		BA0	   -> PG4*
   D5 -> PE8*		A5 -> PF5*		BA1	   -> PG5*
   D6 -> PE9*		A6 -> PF12*		SDNE0  -> PH3*
   D7 -> PE10*		A7 -> PF13*		SDCKE0 -> PC3*
   D8 -> PE11*		A8 -> PF14*		NBL0   -> PE0*
   D9 -> PE12*		A9 -> PF15*		NBL1   -> PE1*
   D10 -> PE13*		A10 -> PG0*
   D11 -> PE14*		A11 -> PG1*
   D12 -> PE15*
   D13 -> PD8*
   D14 -> PD9*
   D15 -> PD10*
  */

	enum { GPIO_AF_FMC = 12 };

	arm_hardware_pioc_altfn50((1U << 3), GPIO_AF_FMC);
	arm_hardware_piod_altfn50((1U << 14), GPIO_AF_FMC);
	arm_hardware_piod_altfn50((1U << 15), GPIO_AF_FMC);
	arm_hardware_piod_altfn50((1U << 0), GPIO_AF_FMC);
	arm_hardware_piod_altfn50((1U << 1), GPIO_AF_FMC);
	arm_hardware_piod_altfn50((1U << 8), GPIO_AF_FMC);
	arm_hardware_piod_altfn50((1U << 9), GPIO_AF_FMC);
	arm_hardware_piod_altfn50((1U << 10), GPIO_AF_FMC);
	arm_hardware_pioe_altfn50((1U << 7), GPIO_AF_FMC);
	arm_hardware_pioe_altfn50((1U << 8), GPIO_AF_FMC);
	arm_hardware_pioe_altfn50((1U << 9), GPIO_AF_FMC);
	arm_hardware_pioe_altfn50((1U << 10), GPIO_AF_FMC);
	arm_hardware_pioe_altfn50((1U << 11), GPIO_AF_FMC);
	arm_hardware_pioe_altfn50((1U << 12), GPIO_AF_FMC);
	arm_hardware_pioe_altfn50((1U << 13), GPIO_AF_FMC);
	arm_hardware_pioe_altfn50((1U << 14), GPIO_AF_FMC);
	arm_hardware_pioe_altfn50((1U << 15), GPIO_AF_FMC);
	arm_hardware_pioe_altfn50((1U << 0), GPIO_AF_FMC);
	arm_hardware_pioe_altfn50((1U << 1), GPIO_AF_FMC);
	arm_hardware_piof_altfn50((1U << 0), GPIO_AF_FMC);
	arm_hardware_piof_altfn50((1U << 1), GPIO_AF_FMC);
	arm_hardware_piof_altfn50((1U << 2), GPIO_AF_FMC);
	arm_hardware_piof_altfn50((1U << 3), GPIO_AF_FMC);
	arm_hardware_piof_altfn50((1U << 4), GPIO_AF_FMC);
	arm_hardware_piof_altfn50((1U << 5), GPIO_AF_FMC);
	arm_hardware_piof_altfn50((1U << 12), GPIO_AF_FMC);
	arm_hardware_piof_altfn50((1U << 13), GPIO_AF_FMC);
	arm_hardware_piof_altfn50((1U << 14), GPIO_AF_FMC);
	arm_hardware_piof_altfn50((1U << 15), GPIO_AF_FMC);
	arm_hardware_piof_altfn50((1U << 11), GPIO_AF_FMC);
	arm_hardware_piog_altfn50((1U << 0), GPIO_AF_FMC);
	arm_hardware_piog_altfn50((1U << 1), GPIO_AF_FMC);
	arm_hardware_piog_altfn50((1U << 15), GPIO_AF_FMC);
	arm_hardware_piog_altfn50((1U << 8), GPIO_AF_FMC);
	arm_hardware_piog_altfn50((1U << 4), GPIO_AF_FMC);
	arm_hardware_piog_altfn50((1U << 5), GPIO_AF_FMC);
	arm_hardware_pioh_altfn50((1U << 3), GPIO_AF_FMC);
	arm_hardware_pioh_altfn50((1U << 5), GPIO_AF_FMC);

#endif
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
  
#if defined CTLSTYLE_V1D	/* Плата STM32F429I-DISCO с процессором STM32F429ZIT6	*/
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
#endif
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
  
#if defined CTLSTYLE_V1D	/* Плата STM32F429I-DISCO с процессором STM32F429ZIT6	*/
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
#elif defined CTLSTYLE_V3D	/* Плата STM32F746G-DISCO с процессором STM32F746NGH6	*/
	FMC_SDRAMCommandStructure.FMC_CommandMode = FMC_Command_Mode_CLK_Enabled;
	FMC_SDRAMCommandStructure.FMC_CommandTarget = FMC_Command_Target_bank1;
	FMC_SDRAMCommandStructure.FMC_AutoRefreshNumber = 1;
	FMC_SDRAMCommandStructure.FMC_ModeRegisterDefinition = 0;

	while(FMC_GetFlagStatus(FMC_Bank1_SDRAM, FMC_FLAG_Busy) != RESET)
	{
	}

	FMC_SDRAMCmdConfig(&FMC_SDRAMCommandStructure);
	__Delay(10);

	FMC_SDRAMCommandStructure.FMC_CommandMode = FMC_Command_Mode_PALL;
	FMC_SDRAMCommandStructure.FMC_CommandTarget = FMC_Command_Target_bank1;
	FMC_SDRAMCommandStructure.FMC_AutoRefreshNumber = 1;
	FMC_SDRAMCommandStructure.FMC_ModeRegisterDefinition = 0;

	while(FMC_GetFlagStatus(FMC_Bank1_SDRAM, FMC_FLAG_Busy) != RESET)
	{
	}

	FMC_SDRAMCmdConfig(&FMC_SDRAMCommandStructure);

	FMC_SDRAMCommandStructure.FMC_CommandMode = FMC_Command_Mode_AutoRefresh;
	FMC_SDRAMCommandStructure.FMC_CommandTarget = FMC_Command_Target_bank1;
	FMC_SDRAMCommandStructure.FMC_AutoRefreshNumber = 8;
	FMC_SDRAMCommandStructure.FMC_ModeRegisterDefinition = 0;

	while(FMC_GetFlagStatus(FMC_Bank1_SDRAM, FMC_FLAG_Busy) != RESET)
	{
	}

	FMC_SDRAMCmdConfig(&FMC_SDRAMCommandStructure);

	tmpr = (uint32_t)SDRAM_MODEREG_BURST_LENGTH_1          |
				   SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL   |
				   SDRAM_MODEREG_CAS_LATENCY_2           |
				   SDRAM_MODEREG_OPERATING_MODE_STANDARD |
				   SDRAM_MODEREG_WRITEBURST_MODE_SINGLE;

	FMC_SDRAMCommandStructure.FMC_CommandMode = FMC_Command_Mode_LoadMode;
	FMC_SDRAMCommandStructure.FMC_CommandTarget = FMC_Command_Target_bank1;
	FMC_SDRAMCommandStructure.FMC_AutoRefreshNumber = 1;
	FMC_SDRAMCommandStructure.FMC_ModeRegisterDefinition = tmpr;

	while(FMC_GetFlagStatus(FMC_Bank1_SDRAM, FMC_FLAG_Busy) != RESET)
	{
	}

	FMC_SDRAMCmdConfig(&FMC_SDRAMCommandStructure);

	FMC_SetRefreshCount(1292);

	while(FMC_GetFlagStatus(FMC_Bank1_SDRAM, FMC_FLAG_Busy) != RESET)
	{
	}

	PRINTF("sdram_MT48LC4M32B2_initialize done\n");
#endif
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
  
#if defined CTLSTYLE_V1D	/* Плата STM32F429I-DISCO с процессором STM32F429ZIT6	*/
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
  
#elif defined CTLSTYLE_V3D	/* Плата STM32F746G-DISCO с процессором STM32F746NGH6	*/

	RCC->AHB3ENR |= RCC_AHB3ENR_FMCEN;	/* FMC clock enable */
	__DSB();

	FMC_SDRAMTimingInitStructure.FMC_LoadToActiveDelay = 2;
	FMC_SDRAMTimingInitStructure.FMC_ExitSelfRefreshDelay = 6;
	FMC_SDRAMTimingInitStructure.FMC_SelfRefreshTime = 4;
	FMC_SDRAMTimingInitStructure.FMC_RowCycleDelay = 6;
	FMC_SDRAMTimingInitStructure.FMC_WriteRecoveryTime = 2;
	FMC_SDRAMTimingInitStructure.FMC_RPDelay = 2;
	FMC_SDRAMTimingInitStructure.FMC_RCDDelay = 2;

	FMC_SDRAMInitStructure.FMC_Bank = FMC_Bank1_SDRAM;
	FMC_SDRAMInitStructure.FMC_ColumnBitsNumber = FMC_ColumnBits_Number_8b;
	FMC_SDRAMInitStructure.FMC_RowBitsNumber = FMC_RowBits_Number_12b;
	FMC_SDRAMInitStructure.FMC_SDMemoryDataWidth = FMC_SDMemory_Width_16b;
	FMC_SDRAMInitStructure.FMC_InternalBankNumber = FMC_InternalBank_Number_2;
	FMC_SDRAMInitStructure.FMC_CASLatency = FMC_CAS_Latency_2;
	FMC_SDRAMInitStructure.FMC_WriteProtection = FMC_Write_Protection_Disable;
	FMC_SDRAMInitStructure.FMC_SDClockPeriod = FMC_SDClock_Period_2;
	FMC_SDRAMInitStructure.FMC_ReadBurst = FMC_Read_Burst_Enable;
	FMC_SDRAMInitStructure.FMC_ReadPipeDelay = FMC_ReadPipe_Delay_1;
	FMC_SDRAMInitStructure.FMC_SDRAMTimingStruct = &FMC_SDRAMTimingInitStructure;

#endif

  /* FMC SDRAM bank initialization */
  FMC_SDRAMInit(&FMC_SDRAMInitStructure); 

  /* FMC SDRAM device initialization sequence */
  SDRAM_InitSequence(); 

#if 0		// Тест памяти
	#if defined (SDRAM_BANK_ADDR)
		#define SDRAM_ADDR		SDRAM_BANK_ADDR
		#define SDRAM_SIZE 		0x8000uL
	#else
		#define SDRAM_ADDR 		0xC0000000uL
		#define SDRAM_SIZE 		0x800000uL
	#endif
	#define BUFFER_SIZE 		0x1000uL

	uint32_t addr = SDRAM_ADDR;

	PRINTF("sdram testing...\n");

	do
	{
		sdram_test_pattern(addr, BUFFER_SIZE, 0x5555);
		sdram_test_pattern(addr, BUFFER_SIZE, 0xCCCC);
		sdram_test_increment(addr, BUFFER_SIZE, 0x7800);
		sdram_test_random(addr, BUFFER_SIZE);

		addr += BUFFER_SIZE;
	} while(addr < SDRAM_ADDR + SDRAM_SIZE - BUFFER_SIZE);

	PRINTF("sdram testing done\n");
	for(;;)
		;
#endif
}

#elif CPUSTYLE_STM32MP1

// Taken from https://github.com/ARM-software/arm-trusted-firmware

/*
#define INT8_C(x)  x
#define INT16_C(x) x
#define INT32_C(x) x
#define INT64_C(x) x ## LL

#define UINT8_C(x)  x
#define UINT16_C(x) x
#define UINT32_C(x) x ## U
#define UINT64_C(x) x ## ULL

#define INTMAX_C(x)  x ## LL
#define UINTMAX_C(x) x ## ULL
*/

#define IS_POWER_OF_TWO(x)			\
	(((x) & ((x) - 1)) == 0)

#define SIZE_FROM_LOG2_WORDS(n)		(4 << (n))

#define ULL(v) ((unsigned long long) (v))
#define UL(v) ((unsigned long) (v))
#define U(v) ((unsigned) (v))

#define BIT_32(nr)			(U(1) << (nr))
#define BIT_64(nr)			(ULL(1) << (nr))

#ifdef __aarch64__
#define BIT				BIT_64
#else
#define BIT				BIT_32
#endif

/*
 * Create a contiguous bitmask starting at bit position @l and ending at
 * position @h. For example
 * GENMASK_64(39, 21) gives us the 64bit vector 0x000000ffffe00000.
 */
#if defined(__LINKER__) || defined(__ASSEMBLER__)
#define GENMASK_32(h, l) \
	(((0xFFFFFFFF) << (l)) & (0xFFFFFFFF >> (32 - 1 - (h))))

#define GENMASK_64(h, l) \
	((~0 << (l)) & (~0 >> (64 - 1 - (h))))
#else
#define GENMASK_32(h, l) \
	(((~UINT32_C(0)) << (l)) & (~UINT32_C(0) >> (32 - 1 - (h))))

#define GENMASK_64(h, l) \
	(((~UINT64_C(0)) << (l)) & (~UINT64_C(0) >> (64 - 1 - (h))))
#endif

#ifdef __aarch64__
#define GENMASK				GENMASK_64
#else
#define GENMASK				GENMASK_32
#endif

/*
 * This variant of div_round_up can be used in macro definition but should not
 * be used in C code as the `div` parameter is evaluated twice.
 */
#define DIV_ROUND_UP_2EVAL(n, d)	(((n) + (d) - 1) / (d))

#define div_round_up(val, div) __extension__ ({	\
	__typeof__(div) _div = (div);		\
	((val) + _div - (__typeof__(div)) 1) / _div;		\
})

#define MIN(x, y) __extension__ ({	\
	__typeof__(x) _x = (x);		\
	__typeof__(y) _y = (y);		\
	(void)(&_x == &_y);		\
	_x < _y ? _x : _y;		\
})

#define MAX(x, y) __extension__ ({	\
	__typeof__(x) _x = (x);		\
	__typeof__(y) _y = (y);		\
	(void)(&_x == &_y);		\
	_x > _y ? _x : _y;		\
})

/*
 * The round_up() macro rounds up a value to the given boundary in a
 * type-agnostic yet type-safe manner. The boundary must be a power of two.
 * In other words, it computes the smallest multiple of boundary which is
 * greater than or equal to value.
 *
 * round_down() is similar but rounds the value down instead.
 */
#define round_boundary(value, boundary)		\
	((__typeof__(value))((boundary) - 1))

#define round_up(value, boundary)		\
	((((value) - 1) | round_boundary(value, boundary)) + 1)

#define round_down(value, boundary)		\
	((value) & ~round_boundary(value, boundary))

/* DDR Controller Register fields */
#define DDRCTRL_MSTR_DDR3			BIT(0)
#define DDRCTRL_MSTR_LPDDR2			BIT(2)
#define DDRCTRL_MSTR_LPDDR3			BIT(3)
#define DDRCTRL_MSTR_DATA_BUS_WIDTH_MASK	GENMASK(13, 12)
#define DDRCTRL_MSTR_DATA_BUS_WIDTH_FULL	0
#define DDRCTRL_MSTR_DATA_BUS_WIDTH_HALF	BIT(12)
#define DDRCTRL_MSTR_DATA_BUS_WIDTH_QUARTER	BIT(13)
#define DDRCTRL_MSTR_DLL_OFF_MODE		BIT(15)

#define DDRCTRL_STAT_OPERATING_MODE_MASK	GENMASK(2, 0)
#define DDRCTRL_STAT_OPERATING_MODE_NORMAL	BIT(0)
#define DDRCTRL_STAT_OPERATING_MODE_SR		(BIT(0) | BIT(1))
#define DDRCTRL_STAT_SELFREF_TYPE_MASK		GENMASK(5, 4)
#define DDRCTRL_STAT_SELFREF_TYPE_ASR		(BIT(4) | BIT(5))
#define DDRCTRL_STAT_SELFREF_TYPE_SR		BIT(5)

#define DDRCTRL_MRCTRL0_MR_TYPE_WRITE		U(0)
/* Only one rank supported */
#define DDRCTRL_MRCTRL0_MR_RANK_SHIFT		4
#define DDRCTRL_MRCTRL0_MR_RANK_ALL \
					BIT(DDRCTRL_MRCTRL0_MR_RANK_SHIFT)
#define DDRCTRL_MRCTRL0_MR_ADDR_SHIFT		12
#define DDRCTRL_MRCTRL0_MR_ADDR_MASK		GENMASK(15, 12)
#define DDRCTRL_MRCTRL0_MR_WR			BIT(31)

#define DDRCTRL_MRSTAT_MR_WR_BUSY		BIT(0)

#define DDRCTRL_PWRCTL_SELFREF_EN		BIT(0)
#define DDRCTRL_PWRCTL_POWERDOWN_EN		BIT(1)
#define DDRCTRL_PWRCTL_EN_DFI_DRAM_CLK_DISABLE	BIT(3)
#define DDRCTRL_PWRCTL_SELFREF_SW		BIT(5)

#define DDRCTRL_PWRTMG_SELFREF_TO_X32_MASK	GENMASK(19, 12)
#define DDRCTRL_PWRTMG_SELFREF_TO_X32_0		BIT(16)

#define DDRCTRL_RFSHCTL3_DIS_AUTO_REFRESH	BIT(0)

#define DDRCTRL_HWLPCTL_HW_LP_EN		BIT(0)

#define DDRCTRL_RFSHTMG_T_RFC_NOM_X1_X32_MASK	GENMASK(27, 16)
#define DDRCTRL_RFSHTMG_T_RFC_NOM_X1_X32_SHIFT	16

#define DDRCTRL_INIT0_SKIP_DRAM_INIT_MASK	GENMASK(31, 30)
#define DDRCTRL_INIT0_SKIP_DRAM_INIT_NORMAL	BIT(30)

#define DDRCTRL_DFIMISC_DFI_INIT_COMPLETE_EN	BIT(0)

#define DDRCTRL_DBG1_DIS_HIF			BIT(1)

#define DDRCTRL_DBGCAM_WR_DATA_PIPELINE_EMPTY	BIT(29)
#define DDRCTRL_DBGCAM_RD_DATA_PIPELINE_EMPTY	BIT(28)
#define DDRCTRL_DBGCAM_DBG_WR_Q_EMPTY		BIT(26)
#define DDRCTRL_DBGCAM_DBG_LPR_Q_DEPTH		GENMASK(12, 8)
#define DDRCTRL_DBGCAM_DBG_HPR_Q_DEPTH		GENMASK(4, 0)
#define DDRCTRL_DBGCAM_DATA_PIPELINE_EMPTY \
		(DDRCTRL_DBGCAM_WR_DATA_PIPELINE_EMPTY | \
		 DDRCTRL_DBGCAM_RD_DATA_PIPELINE_EMPTY)
#define DDRCTRL_DBGCAM_DBG_Q_DEPTH \
		(DDRCTRL_DBGCAM_DBG_WR_Q_EMPTY | \
		 DDRCTRL_DBGCAM_DBG_LPR_Q_DEPTH | \
		 DDRCTRL_DBGCAM_DBG_HPR_Q_DEPTH)

#define DDRCTRL_DBGCMD_RANK0_REFRESH		BIT(0)

#define DDRCTRL_DBGSTAT_RANK0_REFRESH_BUSY	BIT(0)

#define DDRCTRL_SWCTL_SW_DONE			BIT(0)

#define DDRCTRL_SWSTAT_SW_DONE_ACK		BIT(0)

#define DDRCTRL_PCTRL_N_PORT_EN			BIT(0)

/* DDR PHY registers offsets */
#define DDRPHYC_PIR				0x004
#define DDRPHYC_PGCR				0x008
#define DDRPHYC_PGSR				0x00C
#define DDRPHYC_DLLGCR				0x010
#define DDRPHYC_ACDLLCR				0x014
#define DDRPHYC_PTR0				0x018
#define DDRPHYC_ACIOCR				0x024
#define DDRPHYC_DXCCR				0x028
#define DDRPHYC_DSGCR				0x02C
#define DDRPHYC_ZQ0CR0				0x180
#define DDRPHYC_DX0GCR				0x1C0
#define DDRPHYC_DX0DLLCR			0x1CC
#define DDRPHYC_DX1GCR				0x200
#define DDRPHYC_DX1DLLCR			0x20C
#define DDRPHYC_DX2GCR				0x240
#define DDRPHYC_DX2DLLCR			0x24C
#define DDRPHYC_DX3GCR				0x280
#define DDRPHYC_DX3DLLCR			0x28C

/* DDR PHY Register fields */
#define DDRPHYC_PIR_INIT			BIT(0)
#define DDRPHYC_PIR_DLLSRST			BIT(1)
#define DDRPHYC_PIR_DLLLOCK			BIT(2)
#define DDRPHYC_PIR_ZCAL			BIT(3)
#define DDRPHYC_PIR_ITMSRST			BIT(4)
#define DDRPHYC_PIR_DRAMRST			BIT(5)
#define DDRPHYC_PIR_DRAMINIT			BIT(6)
#define DDRPHYC_PIR_QSTRN			BIT(7)
#define DDRPHYC_PIR_ICPC			BIT(16)
#define DDRPHYC_PIR_ZCALBYP			BIT(30)
#define DDRPHYC_PIR_INITSTEPS_MASK		GENMASK(31, 7)

#define DDRPHYC_PGCR_DFTCMP			BIT(2)
#define DDRPHYC_PGCR_PDDISDX			BIT(24)
#define DDRPHYC_PGCR_RFSHDT_MASK		GENMASK(28, 25)

#define DDRPHYC_PGSR_IDONE			BIT(0)
#define DDRPHYC_PGSR_DTERR			BIT(5)
#define DDRPHYC_PGSR_DTIERR			BIT(6)
#define DDRPHYC_PGSR_DFTERR			BIT(7)
#define DDRPHYC_PGSR_RVERR			BIT(8)
#define DDRPHYC_PGSR_RVEIRR			BIT(9)

#define DDRPHYC_DLLGCR_BPS200			BIT(23)

#define DDRPHYC_ACDLLCR_DLLSRST			BIT(30)
#define DDRPHYC_ACDLLCR_DLLDIS			BIT(31)

#define DDRPHYC_PTR0_TDLLSRST_OFFSET		0
#define DDRPHYC_PTR0_TDLLSRST_MASK		GENMASK(5, 0)
#define DDRPHYC_PTR0_TDLLLOCK_OFFSET		6
#define DDRPHYC_PTR0_TDLLLOCK_MASK		GENMASK(17, 6)
#define DDRPHYC_PTR0_TITMSRST_OFFSET		18
#define DDRPHYC_PTR0_TITMSRST_MASK		GENMASK(21, 18)

#define DDRPHYC_ACIOCR_ACPDD			BIT(3)
#define DDRPHYC_ACIOCR_ACPDR			BIT(4)
#define DDRPHYC_ACIOCR_CKPDD_MASK		GENMASK(10, 8)
#define DDRPHYC_ACIOCR_CKPDD_0			BIT(8)
#define DDRPHYC_ACIOCR_CKPDR_MASK		GENMASK(13, 11)
#define DDRPHYC_ACIOCR_CKPDR_0			BIT(11)
#define DDRPHYC_ACIOCR_CSPDD_MASK		GENMASK(21, 18)
#define DDRPHYC_ACIOCR_CSPDD_0			BIT(18)
#define DDRPHYC_ACIOCR_RSTPDD			BIT(27)
#define DDRPHYC_ACIOCR_RSTPDR			BIT(28)

#define DDRPHYC_DXCCR_DXPDD			BIT(2)
#define DDRPHYC_DXCCR_DXPDR			BIT(3)

#define DDRPHYC_DSGCR_CKEPDD_MASK		GENMASK(19, 16)
#define DDRPHYC_DSGCR_CKEPDD_0			BIT(16)
#define DDRPHYC_DSGCR_ODTPDD_MASK		GENMASK(23, 20)
#define DDRPHYC_DSGCR_ODTPDD_0			BIT(20)
#define DDRPHYC_DSGCR_NL2PD			BIT(24)

#define DDRPHYC_ZQ0CRN_ZDATA_MASK		GENMASK(27, 0)
#define DDRPHYC_ZQ0CRN_ZDATA_SHIFT		0
#define DDRPHYC_ZQ0CRN_ZDEN			BIT(28)
#define DDRPHYC_ZQ0CRN_ZQPD			BIT(31)

#define DDRPHYC_DXNGCR_DXEN			BIT(0)

#define DDRPHYC_DXNDLLCR_DLLSRST		BIT(30)
#define DDRPHYC_DXNDLLCR_DLLDIS			BIT(31)
#define DDRPHYC_DXNDLLCR_SDPHASE_MASK		GENMASK(17, 14)
#define DDRPHYC_DXNDLLCR_SDPHASE_SHIFT		14

#define RCC_DDRITFCR			U(0xD8)

static void
mmio_write_32(uintptr_t addr, uint32_t value)
{
	volatile uint32_t * const reg = (volatile uint32_t * const) addr;
	* reg = value;
	(void) * reg;
}

static uint32_t
mmio_read_32(uintptr_t addr)
{
	volatile uint32_t * const reg = (volatile uint32_t * const) addr;
	return * reg;
}

static void
mmio_clrbits_32(uintptr_t addr, uint32_t mask)
{
	volatile uint32_t * const reg = (volatile uint32_t * const) addr;
	* reg &= ~ mask;
	(void) * reg;
}

static void
mmio_setbits_32(uintptr_t addr, uint32_t mask)
{
	volatile uint32_t * const reg = (volatile uint32_t * const) addr;
	* reg |= mask;
	(void) * reg;
}


static void
mmio_clrsetbits_32(uintptr_t addr, uint32_t cmask, uint32_t smask)
{
	volatile uint32_t * const reg = (volatile uint32_t * const) addr;
	* reg = (* reg & ~ cmask) | smask;
	(void) * reg;
}

////////////////////////

#define VERBOSE PRINTF
#define ERROR PRINTF
#define INFO PRINTF

static void panic(void)
{
	PRINTF("sdram: panic.\n");
	for (;;)
		;
}

// DDR clock in Hz
unsigned long stm32mp_ddr_clk_get_rate(unsigned long id)
{
	return DDR_FREQ;
/*
	int p = stm32mp1_clk_get_parent(id);

	if (p < 0) {
		return 0;
	}

	return get_clock_rate(p);
*/
}


/*******************************************************************************
 * CHIP ID
 ******************************************************************************/
#define STM32MP157C_PART_NB	U(0x05000000)
#define STM32MP157A_PART_NB	U(0x05000001)
#define STM32MP153C_PART_NB	U(0x05000024)
#define STM32MP153A_PART_NB	U(0x05000025)
#define STM32MP151C_PART_NB	U(0x0500002E)
#define STM32MP151A_PART_NB	U(0x0500002F)

#define STM32MP1_REV_B		U(0x2000)

/*******************************************************************************
 * PACKAGE ID
 ******************************************************************************/
#define PKG_AA_LFBGA448		U(4)
#define PKG_AB_LFBGA354		U(3)
#define PKG_AC_TFBGA361		U(2)
#define PKG_AD_TFBGA257		U(1)

/*******************************************************************************
 * STM32MP1 memory map related constants
 ******************************************************************************/
#define STM32MP_ROM_BASE		U(0x00000000)
#define STM32MP_ROM_SIZE		U(0x00020000)

#define STM32MP_SYSRAM_BASE		U(0x2FFC0000)
#define STM32MP_SYSRAM_SIZE		U(0x00040000)

/* DDR configuration */
#define STM32MP_DDR_BASE		U(0xC0000000)
#define STM32MP_DDR_MAX_SIZE		U(0x40000000)	/* Max 1GB */
#ifdef AARCH32_SP_OPTEE
#define STM32MP_DDR_S_SIZE		U(0x01E00000)	/* 30 MB */
#define STM32MP_DDR_SHMEM_SIZE		U(0x00200000)	/* 2 MB */
#endif

/* DDR power initializations */
#ifndef __ASSEMBLER__
enum ddr_type {
	STM32MP_DDR3,
	STM32MP_LPDDR2,
	STM32MP_LPDDR3
};
#endif

/* DDR3/LPDDR2/LPDDR3 Controller (DDRCTRL) registers */
struct stm32mp1_ddrctl {
	uint32_t mstr ;		/* 0x0 Master */
	uint32_t stat;		/* 0x4 Operating Mode Status */
	uint8_t reserved008[0x10 - 0x8];
	uint32_t mrctrl0;	/* 0x10 Control 0 */
	uint32_t mrctrl1;	/* 0x14 Control 1 */
	uint32_t mrstat;	/* 0x18 Status */
	uint32_t reserved01c;	/* 0x1c */
	uint32_t derateen;	/* 0x20 Temperature Derate Enable */
	uint32_t derateint;	/* 0x24 Temperature Derate Interval */
	uint8_t reserved028[0x30 - 0x28];
	uint32_t pwrctl;	/* 0x30 Low Power Control */
	uint32_t pwrtmg;	/* 0x34 Low Power Timing */
	uint32_t hwlpctl;	/* 0x38 Hardware Low Power Control */
	uint8_t reserved03c[0x50 - 0x3C];
	uint32_t rfshctl0;	/* 0x50 Refresh Control 0 */
	uint32_t reserved054;	/* 0x54 Refresh Control 1 */
	uint32_t reserved058;	/* 0x58 Refresh Control 2 */
	uint32_t reserved05C;
	uint32_t rfshctl3;	/* 0x60 Refresh Control 0 */
	uint32_t rfshtmg;	/* 0x64 Refresh Timing */
	uint8_t reserved068[0xc0 - 0x68];
	uint32_t crcparctl0;		/* 0xc0 CRC Parity Control0 */
	uint32_t reserved0c4;	/* 0xc4 CRC Parity Control1 */
	uint32_t reserved0c8;	/* 0xc8 CRC Parity Control2 */
	uint32_t crcparstat;		/* 0xcc CRC Parity Status */
	uint32_t init0;		/* 0xd0 SDRAM Initialization 0 */
	uint32_t init1;		/* 0xd4 SDRAM Initialization 1 */
	uint32_t init2;		/* 0xd8 SDRAM Initialization 2 */
	uint32_t init3;		/* 0xdc SDRAM Initialization 3 */
	uint32_t init4;		/* 0xe0 SDRAM Initialization 4 */
	uint32_t init5;		/* 0xe4 SDRAM Initialization 5 */
	uint32_t reserved0e8;
	uint32_t reserved0ec;
	uint32_t dimmctl;	/* 0xf0 DIMM Control */
	uint8_t reserved0f4[0x100 - 0xf4];
	uint32_t dramtmg0;	/* 0x100 SDRAM Timing 0 */
	uint32_t dramtmg1;	/* 0x104 SDRAM Timing 1 */
	uint32_t dramtmg2;	/* 0x108 SDRAM Timing 2 */
	uint32_t dramtmg3;	/* 0x10c SDRAM Timing 3 */
	uint32_t dramtmg4;	/* 0x110 SDRAM Timing 4 */
	uint32_t dramtmg5;	/* 0x114 SDRAM Timing 5 */
	uint32_t dramtmg6;	/* 0x118 SDRAM Timing 6 */
	uint32_t dramtmg7;	/* 0x11c SDRAM Timing 7 */
	uint32_t dramtmg8;	/* 0x120 SDRAM Timing 8 */
	uint8_t reserved124[0x138 - 0x124];
	uint32_t dramtmg14;	/* 0x138 SDRAM Timing 14 */
	uint32_t dramtmg15;	/* 0x13C SDRAM Timing 15 */
	uint8_t reserved140[0x180 - 0x140];
	uint32_t zqctl0;	/* 0x180 ZQ Control 0 */
	uint32_t zqctl1;	/* 0x184 ZQ Control 1 */
	uint32_t zqctl2;	/* 0x188 ZQ Control 2 */
	uint32_t zqstat;	/* 0x18c ZQ Status */
	uint32_t dfitmg0;	/* 0x190 DFI Timing 0 */
	uint32_t dfitmg1;	/* 0x194 DFI Timing 1 */
	uint32_t dfilpcfg0;	/* 0x198 DFI Low Power Configuration 0 */
	uint32_t reserved19c;
	uint32_t dfiupd0;	/* 0x1a0 DFI Update 0 */
	uint32_t dfiupd1;	/* 0x1a4 DFI Update 1 */
	uint32_t dfiupd2;	/* 0x1a8 DFI Update 2 */
	uint32_t reserved1ac;
	uint32_t dfimisc;	/* 0x1b0 DFI Miscellaneous Control */
	uint8_t reserved1b4[0x1bc - 0x1b4];
	uint32_t dfistat;	/* 0x1bc DFI Miscellaneous Control */
	uint8_t reserved1c0[0x1c4 - 0x1c0];
	uint32_t dfiphymstr;	/* 0x1c4 DFI PHY Master interface */
	uint8_t reserved1c8[0x204 - 0x1c8];
	uint32_t addrmap1;	/* 0x204 Address Map 1 */
	uint32_t addrmap2;	/* 0x208 Address Map 2 */
	uint32_t addrmap3;	/* 0x20c Address Map 3 */
	uint32_t addrmap4;	/* 0x210 Address Map 4 */
	uint32_t addrmap5;	/* 0x214 Address Map 5 */
	uint32_t addrmap6;	/* 0x218 Address Map 6 */
	uint8_t reserved21c[0x224 - 0x21c];
	uint32_t addrmap9;	/* 0x224 Address Map 9 */
	uint32_t addrmap10;	/* 0x228 Address Map 10 */
	uint32_t addrmap11;	/* 0x22C Address Map 11 */
	uint8_t reserved230[0x240 - 0x230];
	uint32_t odtcfg;	/* 0x240 ODT Configuration */
	uint32_t odtmap;	/* 0x244 ODT/Rank Map */
	uint8_t reserved248[0x250 - 0x248];
	uint32_t sched;		/* 0x250 Scheduler Control */
	uint32_t sched1;	/* 0x254 Scheduler Control 1 */
	uint32_t reserved258;
	uint32_t perfhpr1;	/* 0x25c High Priority Read CAM 1 */
	uint32_t reserved260;
	uint32_t perflpr1;	/* 0x264 Low Priority Read CAM 1 */
	uint32_t reserved268;
	uint32_t perfwr1;	/* 0x26c Write CAM 1 */
	uint8_t reserved27c[0x300 - 0x270];
	uint32_t dbg0;		/* 0x300 Debug 0 */
	uint32_t dbg1;		/* 0x304 Debug 1 */
	uint32_t dbgcam;	/* 0x308 CAM Debug */
	uint32_t dbgcmd;	/* 0x30c Command Debug */
	uint32_t dbgstat;	/* 0x310 Status Debug */
	uint8_t reserved314[0x320 - 0x314];
	uint32_t swctl;		/* 0x320 Software Programming Control Enable */
	uint32_t swstat;	/* 0x324 Software Programming Control Status */
	uint8_t reserved328[0x36c - 0x328];
	uint32_t poisoncfg;	/* 0x36c AXI Poison Configuration Register */
	uint32_t poisonstat;	/* 0x370 AXI Poison Status Register */
	uint8_t reserved374[0x3fc - 0x374];

	/* Multi Port registers */
	uint32_t pstat;		/* 0x3fc Port Status */
	uint32_t pccfg;		/* 0x400 Port Common Configuration */

	/* PORT 0 */
	uint32_t pcfgr_0;	/* 0x404 Configuration Read */
	uint32_t pcfgw_0;	/* 0x408 Configuration Write */
	uint8_t reserved40c[0x490 - 0x40c];
	uint32_t pctrl_0;	/* 0x490 Port Control Register */
	uint32_t pcfgqos0_0;	/* 0x494 Read QoS Configuration 0 */
	uint32_t pcfgqos1_0;	/* 0x498 Read QoS Configuration 1 */
	uint32_t pcfgwqos0_0;	/* 0x49c Write QoS Configuration 0 */
	uint32_t pcfgwqos1_0;	/* 0x4a0 Write QoS Configuration 1 */
	uint8_t reserved4a4[0x4b4 - 0x4a4];

	/* PORT 1 */
	uint32_t pcfgr_1;	/* 0x4b4 Configuration Read */
	uint32_t pcfgw_1;	/* 0x4b8 Configuration Write */
	uint8_t reserved4bc[0x540 - 0x4bc];
	uint32_t pctrl_1;	/* 0x540 Port 2 Control Register */
	uint32_t pcfgqos0_1;	/* 0x544 Read QoS Configuration 0 */
	uint32_t pcfgqos1_1;	/* 0x548 Read QoS Configuration 1 */
	uint32_t pcfgwqos0_1;	/* 0x54c Write QoS Configuration 0 */
	uint32_t pcfgwqos1_1;	/* 0x550 Write QoS Configuration 1 */
} __packed;

/* DDR Physical Interface Control (DDRPHYC) registers*/
struct stm32mp1_ddrphy {
	uint32_t ridr;		/* 0x00 R Revision Identification */
	uint32_t pir;		/* 0x04 R/W PHY Initialization */
	uint32_t pgcr;		/* 0x08 R/W PHY General Configuration */
	uint32_t pgsr;		/* 0x0C PHY General Status */
	uint32_t dllgcr;	/* 0x10 R/W DLL General Control */
	uint32_t acdllcr;	/* 0x14 R/W AC DLL Control */
	uint32_t ptr0;		/* 0x18 R/W PHY Timing 0 */
	uint32_t ptr1;		/* 0x1C R/W PHY Timing 1 */
	uint32_t ptr2;		/* 0x20 R/W PHY Timing 2 */
	uint32_t aciocr;	/* 0x24 AC I/O Configuration */
	uint32_t dxccr;		/* 0x28 DATX8 Common Configuration */
	uint32_t dsgcr;		/* 0x2C DDR System General Configuration */
	uint32_t dcr;		/* 0x30 DRAM Configuration */
	uint32_t dtpr0;		/* 0x34 DRAM Timing Parameters0 */
	uint32_t dtpr1;		/* 0x38 DRAM Timing Parameters1 */
	uint32_t dtpr2;		/* 0x3C DRAM Timing Parameters2 */
	uint32_t mr0;		/* 0x40 Mode 0 */
	uint32_t mr1;		/* 0x44 Mode 1 */
	uint32_t mr2;		/* 0x48 Mode 2 */
	uint32_t mr3;		/* 0x4C Mode 3 */
	uint32_t odtcr;		/* 0x50 ODT Configuration */
	uint32_t dtar;		/* 0x54 data training address */
	uint32_t dtdr0;		/* 0x58 */
	uint32_t dtdr1;		/* 0x5c */
	uint8_t res1[0x0c0 - 0x060];	/* 0x60 */
	uint32_t dcuar;		/* 0xc0 Address */
	uint32_t dcudr;		/* 0xc4 DCU Data */
	uint32_t dcurr;		/* 0xc8 DCU Run */
	uint32_t dculr;		/* 0xcc DCU Loop */
	uint32_t dcugcr;	/* 0xd0 DCU General Configuration */
	uint32_t dcutpr;	/* 0xd4 DCU Timing Parameters */
	uint32_t dcusr0;	/* 0xd8 DCU Status 0 */
	uint32_t dcusr1;	/* 0xdc DCU Status 1 */
	uint8_t res2[0x100 - 0xe0];	/* 0xe0 */
	uint32_t bistrr;	/* 0x100 BIST Run */
	uint32_t bistmskr0;	/* 0x104 BIST Mask 0 */
	uint32_t bistmskr1;	/* 0x108 BIST Mask 0 */
	uint32_t bistwcr;	/* 0x10c BIST Word Count */
	uint32_t bistlsr;	/* 0x110 BIST LFSR Seed */
	uint32_t bistar0;	/* 0x114 BIST Address 0 */
	uint32_t bistar1;	/* 0x118 BIST Address 1 */
	uint32_t bistar2;	/* 0x11c BIST Address 2 */
	uint32_t bistupdr;	/* 0x120 BIST User Data Pattern */
	uint32_t bistgsr;	/* 0x124 BIST General Status */
	uint32_t bistwer;	/* 0x128 BIST Word Error */
	uint32_t bistber0;	/* 0x12c BIST Bit Error 0 */
	uint32_t bistber1;	/* 0x130 BIST Bit Error 1 */
	uint32_t bistber2;	/* 0x134 BIST Bit Error 2 */
	uint32_t bistwcsr;	/* 0x138 BIST Word Count Status */
	uint32_t bistfwr0;	/* 0x13c BIST Fail Word 0 */
	uint32_t bistfwr1;	/* 0x140 BIST Fail Word 1 */
	uint8_t res3[0x178 - 0x144];	/* 0x144 */
	uint32_t gpr0;		/* 0x178 General Purpose 0 (GPR0) */
	uint32_t gpr1;		/* 0x17C General Purpose 1 (GPR1) */
	uint32_t zq0cr0;	/* 0x180 zq 0 control 0 */
	uint32_t zq0cr1;	/* 0x184 zq 0 control 1 */
	uint32_t zq0sr0;	/* 0x188 zq 0 status 0 */
	uint32_t zq0sr1;	/* 0x18C zq 0 status 1 */
	uint8_t res4[0x1C0 - 0x190];	/* 0x190 */
	uint32_t dx0gcr;	/* 0x1c0 Byte lane 0 General Configuration */
	uint32_t dx0gsr0;	/* 0x1c4 Byte lane 0 General Status 0 */
	uint32_t dx0gsr1;	/* 0x1c8 Byte lane 0 General Status 1 */
	uint32_t dx0dllcr;	/* 0x1cc Byte lane 0 DLL Control */
	uint32_t dx0dqtr;	/* 0x1d0 Byte lane 0 DQ Timing */
	uint32_t dx0dqstr;	/* 0x1d4 Byte lane 0 DQS Timing */
	uint8_t res5[0x200 - 0x1d8];	/* 0x1d8 */
	uint32_t dx1gcr;	/* 0x200 Byte lane 1 General Configuration */
	uint32_t dx1gsr0;	/* 0x204 Byte lane 1 General Status 0 */
	uint32_t dx1gsr1;	/* 0x208 Byte lane 1 General Status 1 */
	uint32_t dx1dllcr;	/* 0x20c Byte lane 1 DLL Control */
	uint32_t dx1dqtr;	/* 0x210 Byte lane 1 DQ Timing */
	uint32_t dx1dqstr;	/* 0x214 Byte lane 1 QS Timing */
	uint8_t res6[0x240 - 0x218];	/* 0x218 */
	uint32_t dx2gcr;	/* 0x240 Byte lane 2 General Configuration */
	uint32_t dx2gsr0;	/* 0x244 Byte lane 2 General Status 0 */
	uint32_t dx2gsr1;	/* 0x248 Byte lane 2 General Status 1 */
	uint32_t dx2dllcr;	/* 0x24c Byte lane 2 DLL Control */
	uint32_t dx2dqtr;	/* 0x250 Byte lane 2 DQ Timing */
	uint32_t dx2dqstr;	/* 0x254 Byte lane 2 QS Timing */
	uint8_t res7[0x280 - 0x258];	/* 0x258 */
	uint32_t dx3gcr;	/* 0x280 Byte lane 3 General Configuration */
	uint32_t dx3gsr0;	/* 0x284 Byte lane 3 General Status 0 */
	uint32_t dx3gsr1;	/* 0x288 Byte lane 3 General Status 1 */
	uint32_t dx3dllcr;	/* 0x28c Byte lane 3 DLL Control */
	uint32_t dx3dqtr;	/* 0x290 Byte lane 3 DQ Timing */
	uint32_t dx3dqstr;	/* 0x294 Byte lane 3 QS Timing */
} __packed;

struct stm32mp1_ddr_size {
	uint64_t base;
	uint64_t size;
};

/**
 * struct ddr_info
 *
 * @dev: pointer for the device
 * @info: UCLASS RAM information
 * @ctl: DDR controleur base address
 * @phy: DDR PHY base address
 * @syscfg: syscfg base address
 */
struct ddr_info {
	struct stm32mp1_ddr_size info;
	struct stm32mp1_ddrctl *ctl;
	struct stm32mp1_ddrphy *phy;
	uintptr_t pwr;
	uintptr_t rcc;
};

struct stm32mp1_ddrctrl_reg {
	uint32_t mstr;
	uint32_t mrctrl0;
	uint32_t mrctrl1;
	uint32_t derateen;
	uint32_t derateint;
	uint32_t pwrctl;
	uint32_t pwrtmg;
	uint32_t hwlpctl;
	uint32_t rfshctl0;
	uint32_t rfshctl3;
	uint32_t crcparctl0;
	uint32_t zqctl0;
	uint32_t dfitmg0;
	uint32_t dfitmg1;
	uint32_t dfilpcfg0;
	uint32_t dfiupd0;
	uint32_t dfiupd1;
	uint32_t dfiupd2;
	uint32_t dfiphymstr;
	uint32_t odtmap;
	uint32_t dbg0;
	uint32_t dbg1;
	uint32_t dbgcmd;
	uint32_t poisoncfg;
	uint32_t pccfg;
};

struct stm32mp1_ddrctrl_timing {
	uint32_t rfshtmg;
	uint32_t dramtmg0;
	uint32_t dramtmg1;
	uint32_t dramtmg2;
	uint32_t dramtmg3;
	uint32_t dramtmg4;
	uint32_t dramtmg5;
	uint32_t dramtmg6;
	uint32_t dramtmg7;
	uint32_t dramtmg8;
	uint32_t dramtmg14;
	uint32_t odtcfg;
};

struct stm32mp1_ddrctrl_map {
	uint32_t addrmap1;
	uint32_t addrmap2;
	uint32_t addrmap3;
	uint32_t addrmap4;
	uint32_t addrmap5;
	uint32_t addrmap6;
	uint32_t addrmap9;
	uint32_t addrmap10;
	uint32_t addrmap11;
};

struct stm32mp1_ddrctrl_perf {
	uint32_t sched;
	uint32_t sched1;
	uint32_t perfhpr1;
	uint32_t perflpr1;
	uint32_t perfwr1;
	uint32_t pcfgr_0;
	uint32_t pcfgw_0;
	uint32_t pcfgqos0_0;
	uint32_t pcfgqos1_0;
	uint32_t pcfgwqos0_0;
	uint32_t pcfgwqos1_0;
	uint32_t pcfgr_1;
	uint32_t pcfgw_1;
	uint32_t pcfgqos0_1;
	uint32_t pcfgqos1_1;
	uint32_t pcfgwqos0_1;
	uint32_t pcfgwqos1_1;
};

struct stm32mp1_ddrphy_reg {
	uint32_t pgcr;
	uint32_t aciocr;
	uint32_t dxccr;
	uint32_t dsgcr;
	uint32_t dcr;
	uint32_t odtcr;
	uint32_t zq0cr1;
	uint32_t dx0gcr;
	uint32_t dx1gcr;
	uint32_t dx2gcr;
	uint32_t dx3gcr;
};

struct stm32mp1_ddrphy_timing {
	uint32_t ptr0;
	uint32_t ptr1;
	uint32_t ptr2;
	uint32_t dtpr0;
	uint32_t dtpr1;
	uint32_t dtpr2;
	uint32_t mr0;
	uint32_t mr1;
	uint32_t mr2;
	uint32_t mr3;
};

struct stm32mp1_ddrphy_cal {
	uint32_t dx0dllcr;
	uint32_t dx0dqtr;
	uint32_t dx0dqstr;
	uint32_t dx1dllcr;
	uint32_t dx1dqtr;
	uint32_t dx1dqstr;
	uint32_t dx2dllcr;
	uint32_t dx2dqtr;
	uint32_t dx2dqstr;
	uint32_t dx3dllcr;
	uint32_t dx3dqtr;
	uint32_t dx3dqstr;
};

struct stm32mp1_ddr_info {
	const char *name;
	uint32_t speed; /* in kHZ */
	uint32_t size;  /* Memory size in byte = col * row * width */
};

struct stm32mp1_ddr_config {
	struct stm32mp1_ddr_info info;
	struct stm32mp1_ddrctrl_reg c_reg;
	struct stm32mp1_ddrctrl_timing c_timing;
	struct stm32mp1_ddrctrl_map c_map;
	struct stm32mp1_ddrctrl_perf c_perf;
	struct stm32mp1_ddrphy_reg p_reg;
	struct stm32mp1_ddrphy_timing p_timing;
	struct stm32mp1_ddrphy_cal p_cal;
};

void ddr_enable_clock(void)
{
	//stm32mp1_clk_rcc_regs_lock();

	mmio_setbits_32(RCC_BASE + RCC_DDRITFCR,
			RCC_DDRITFCR_DDRC1EN | RCC_DDRITFCR_DDRC1LPEN |
			RCC_DDRITFCR_DDRC2EN | RCC_DDRITFCR_DDRC2LPEN |
			RCC_DDRITFCR_DDRPHYCEN | RCC_DDRITFCR_DDRPHYCLPEN |
			RCC_DDRITFCR_DDRPHYCAPBEN | RCC_DDRITFCR_DDRCAPBLPEN |
			RCC_DDRITFCR_DDRCAPBEN | RCC_DDRITFCR_DDRPHYCAPBLPEN
			);

	//stm32mp1_clk_rcc_regs_unlock();
}

int stm32mp1_ddr_clk_enable(struct ddr_info *priv, uint32_t mem_speed)
{
	unsigned long ddrphy_clk, ddr_clk, mem_speed_hz;

	ddr_enable_clock();

	ddrphy_clk = stm32mp_ddr_clk_get_rate(0 /*DDRPHYC */);

	VERBOSE("DDR: mem_speed (%lu kHz), RCC %lu kHz\n",
		(unsigned long) mem_speed, ddrphy_clk / 1000lU);

	mem_speed_hz = mem_speed * 1000U;

	/* Max 10% frequency delta */
	if (ddrphy_clk > mem_speed_hz) {
		ddr_clk = ddrphy_clk - mem_speed_hz;
	} else {
		ddr_clk = mem_speed_hz - ddrphy_clk;
	}
	if (ddr_clk > (mem_speed_hz / 10)) {
		ERROR("DDR expected freq %d kHz, current is %ld kHz\n",
		      mem_speed, ddrphy_clk / 1000U);
		//return -1;
	}
	return 0;
}


struct reg_desc {
	const char *name;
	uint16_t offset;	/* Offset for base address */
	uint8_t par_offset;	/* Offset for parameter array */
};

#define INVALID_OFFSET	0xFFU

#define TIMEOUT_US_1S	1000000U

#define DDRCTL_REG(x, y)					\
	{							\
		.name = #x,					\
		.offset = offsetof(struct stm32mp1_ddrctl, x),	\
		.par_offset = offsetof(struct y, x)		\
	}

#define DDRPHY_REG(x, y)					\
	{							\
		.name = #x,					\
		.offset = offsetof(struct stm32mp1_ddrphy, x),	\
		.par_offset = offsetof(struct y, x)		\
	}

#define DDRCTL_REG_REG(x)	DDRCTL_REG(x, stm32mp1_ddrctrl_reg)
static const struct reg_desc ddr_reg[] = {
	DDRCTL_REG_REG(mstr),
	DDRCTL_REG_REG(mrctrl0),
	DDRCTL_REG_REG(mrctrl1),
	DDRCTL_REG_REG(derateen),
	DDRCTL_REG_REG(derateint),
	DDRCTL_REG_REG(pwrctl),
	DDRCTL_REG_REG(pwrtmg),
	DDRCTL_REG_REG(hwlpctl),
	DDRCTL_REG_REG(rfshctl0),
	DDRCTL_REG_REG(rfshctl3),
	DDRCTL_REG_REG(crcparctl0),
	DDRCTL_REG_REG(zqctl0),
	DDRCTL_REG_REG(dfitmg0),
	DDRCTL_REG_REG(dfitmg1),
	DDRCTL_REG_REG(dfilpcfg0),
	DDRCTL_REG_REG(dfiupd0),
	DDRCTL_REG_REG(dfiupd1),
	DDRCTL_REG_REG(dfiupd2),
	DDRCTL_REG_REG(dfiphymstr),
	DDRCTL_REG_REG(odtmap),
	DDRCTL_REG_REG(dbg0),
	DDRCTL_REG_REG(dbg1),
	DDRCTL_REG_REG(dbgcmd),
	DDRCTL_REG_REG(poisoncfg),
	DDRCTL_REG_REG(pccfg),
};

#define DDRCTL_REG_TIMING(x)	DDRCTL_REG(x, stm32mp1_ddrctrl_timing)
static const struct reg_desc ddr_timing[] = {
	DDRCTL_REG_TIMING(rfshtmg),
	DDRCTL_REG_TIMING(dramtmg0),
	DDRCTL_REG_TIMING(dramtmg1),
	DDRCTL_REG_TIMING(dramtmg2),
	DDRCTL_REG_TIMING(dramtmg3),
	DDRCTL_REG_TIMING(dramtmg4),
	DDRCTL_REG_TIMING(dramtmg5),
	DDRCTL_REG_TIMING(dramtmg6),
	DDRCTL_REG_TIMING(dramtmg7),
	DDRCTL_REG_TIMING(dramtmg8),
	DDRCTL_REG_TIMING(dramtmg14),
	DDRCTL_REG_TIMING(odtcfg),
};

#define DDRCTL_REG_MAP(x)	DDRCTL_REG(x, stm32mp1_ddrctrl_map)
static const struct reg_desc ddr_map[] = {
	DDRCTL_REG_MAP(addrmap1),
	DDRCTL_REG_MAP(addrmap2),
	DDRCTL_REG_MAP(addrmap3),
	DDRCTL_REG_MAP(addrmap4),
	DDRCTL_REG_MAP(addrmap5),
	DDRCTL_REG_MAP(addrmap6),
	DDRCTL_REG_MAP(addrmap9),
	DDRCTL_REG_MAP(addrmap10),
	DDRCTL_REG_MAP(addrmap11),
};

#define DDRCTL_REG_PERF(x)	DDRCTL_REG(x, stm32mp1_ddrctrl_perf)
static const struct reg_desc ddr_perf[] = {
	DDRCTL_REG_PERF(sched),
	DDRCTL_REG_PERF(sched1),
	DDRCTL_REG_PERF(perfhpr1),
	DDRCTL_REG_PERF(perflpr1),
	DDRCTL_REG_PERF(perfwr1),
	DDRCTL_REG_PERF(pcfgr_0),
	DDRCTL_REG_PERF(pcfgw_0),
	DDRCTL_REG_PERF(pcfgqos0_0),
	DDRCTL_REG_PERF(pcfgqos1_0),
	DDRCTL_REG_PERF(pcfgwqos0_0),
	DDRCTL_REG_PERF(pcfgwqos1_0),
	DDRCTL_REG_PERF(pcfgr_1),
	DDRCTL_REG_PERF(pcfgw_1),
	DDRCTL_REG_PERF(pcfgqos0_1),
	DDRCTL_REG_PERF(pcfgqos1_1),
	DDRCTL_REG_PERF(pcfgwqos0_1),
	DDRCTL_REG_PERF(pcfgwqos1_1),
};

#define DDRPHY_REG_REG(x)	DDRPHY_REG(x, stm32mp1_ddrphy_reg)
static const struct reg_desc ddrphy_reg[] = {
	DDRPHY_REG_REG(pgcr),
	DDRPHY_REG_REG(aciocr),
	DDRPHY_REG_REG(dxccr),
	DDRPHY_REG_REG(dsgcr),
	DDRPHY_REG_REG(dcr),
	DDRPHY_REG_REG(odtcr),
	DDRPHY_REG_REG(zq0cr1),
	DDRPHY_REG_REG(dx0gcr),
	DDRPHY_REG_REG(dx1gcr),
	DDRPHY_REG_REG(dx2gcr),
	DDRPHY_REG_REG(dx3gcr),
};

#define DDRPHY_REG_TIMING(x)	DDRPHY_REG(x, stm32mp1_ddrphy_timing)
static const struct reg_desc ddrphy_timing[] = {
	DDRPHY_REG_TIMING(ptr0),
	DDRPHY_REG_TIMING(ptr1),
	DDRPHY_REG_TIMING(ptr2),
	DDRPHY_REG_TIMING(dtpr0),
	DDRPHY_REG_TIMING(dtpr1),
	DDRPHY_REG_TIMING(dtpr2),
	DDRPHY_REG_TIMING(mr0),
	DDRPHY_REG_TIMING(mr1),
	DDRPHY_REG_TIMING(mr2),
	DDRPHY_REG_TIMING(mr3),
};

#define DDRPHY_REG_CAL(x)	DDRPHY_REG(x, stm32mp1_ddrphy_cal)
static const struct reg_desc ddrphy_cal[] = {
	DDRPHY_REG_CAL(dx0dllcr),
	DDRPHY_REG_CAL(dx0dqtr),
	DDRPHY_REG_CAL(dx0dqstr),
	DDRPHY_REG_CAL(dx1dllcr),
	DDRPHY_REG_CAL(dx1dqtr),
	DDRPHY_REG_CAL(dx1dqstr),
	DDRPHY_REG_CAL(dx2dllcr),
	DDRPHY_REG_CAL(dx2dqtr),
	DDRPHY_REG_CAL(dx2dqstr),
	DDRPHY_REG_CAL(dx3dllcr),
	DDRPHY_REG_CAL(dx3dqtr),
	DDRPHY_REG_CAL(dx3dqstr),
};

#define DDR_REG_DYN(x)						\
	{							\
		.name = #x,					\
		.offset = offsetof(struct stm32mp1_ddrctl, x),	\
		.par_offset = INVALID_OFFSET \
	}

static const struct reg_desc ddr_dyn[] = {
	DDR_REG_DYN(stat),
	DDR_REG_DYN(init0),
	DDR_REG_DYN(dfimisc),
	DDR_REG_DYN(dfistat),
	DDR_REG_DYN(swctl),
	DDR_REG_DYN(swstat),
	DDR_REG_DYN(pctrl_0),
	DDR_REG_DYN(pctrl_1),
};

#define DDRPHY_REG_DYN(x)					\
	{							\
		.name = #x,					\
		.offset = offsetof(struct stm32mp1_ddrphy, x),	\
		.par_offset = INVALID_OFFSET			\
	}

static const struct reg_desc ddrphy_dyn[] = {
	DDRPHY_REG_DYN(pir),
	DDRPHY_REG_DYN(pgsr),
};

enum reg_type {
	REG_REG,
	REG_TIMING,
	REG_PERF,
	REG_MAP,
	REGPHY_REG,
	REGPHY_TIMING,
	REGPHY_CAL,
/*
 * Dynamic registers => managed in driver or not changed,
 * can be dumped in interactive mode.
 */
	REG_DYN,
	REGPHY_DYN,
	REG_TYPE_NB
};

enum base_type {
	DDR_BASE,
	DDRPHY_BASE,
	NONE_BASE
};

struct ddr_reg_info {
	const char *name;
	const struct reg_desc *desc;
	uint8_t size;
	enum base_type base;
};

static const struct ddr_reg_info ddr_registers[REG_TYPE_NB] = {
	[REG_REG] = {
		.name = "static",
		.desc = ddr_reg,
		.size = ARRAY_SIZE(ddr_reg),
		.base = DDR_BASE
	},
	[REG_TIMING] = {
		.name = "timing",
		.desc = ddr_timing,
		.size = ARRAY_SIZE(ddr_timing),
		.base = DDR_BASE
	},
	[REG_PERF] = {
		.name = "perf",
		.desc = ddr_perf,
		.size = ARRAY_SIZE(ddr_perf),
		.base = DDR_BASE
	},
	[REG_MAP] = {
		.name = "map",
		.desc = ddr_map,
		.size = ARRAY_SIZE(ddr_map),
		.base = DDR_BASE
	},
	[REGPHY_REG] = {
		.name = "static",
		.desc = ddrphy_reg,
		.size = ARRAY_SIZE(ddrphy_reg),
		.base = DDRPHY_BASE
	},
	[REGPHY_TIMING] = {
		.name = "timing",
		.desc = ddrphy_timing,
		.size = ARRAY_SIZE(ddrphy_timing),
		.base = DDRPHY_BASE
	},
	[REGPHY_CAL] = {
		.name = "cal",
		.desc = ddrphy_cal,
		.size = ARRAY_SIZE(ddrphy_cal),
		.base = DDRPHY_BASE
	},
	[REG_DYN] = {
		.name = "dyn",
		.desc = ddr_dyn,
		.size = ARRAY_SIZE(ddr_dyn),
		.base = DDR_BASE
	},
	[REGPHY_DYN] = {
		.name = "dyn",
		.desc = ddrphy_dyn,
		.size = ARRAY_SIZE(ddrphy_dyn),
		.base = DDRPHY_BASE
	},
};

static uintptr_t get_base_addr(const struct ddr_info *priv, enum base_type base)
{
	if (base == DDRPHY_BASE) {
		return (uintptr_t)priv->phy;
	} else {
		return (uintptr_t)priv->ctl;
	}
}

static void set_reg(const struct ddr_info *priv,
		    enum reg_type type,
		    const void *param)
{
	unsigned int i;
	unsigned int value;
	enum base_type base = ddr_registers[type].base;
	uintptr_t base_addr = get_base_addr(priv, base);
	const struct reg_desc *desc = ddr_registers[type].desc;

	VERBOSE("init %s\n", ddr_registers[type].name);
	for (i = 0; i < ddr_registers[type].size; i++) {
		uintptr_t ptr = base_addr + desc[i].offset;

		if (desc[i].par_offset == INVALID_OFFSET) {
			ERROR("invalid parameter offset for %s", desc[i].name);
			panic();
		} else {
			value = *((uint32_t *)((uintptr_t)param +
					       desc[i].par_offset));
			mmio_write_32(ptr, value);
		}
	}
}

static void stm32mp1_ddrphy_idone_wait(struct stm32mp1_ddrphy *phy)
{
	uint32_t pgsr;
	int error = 0;
//	uint64_t timeout = timeout_init_us(TIMEOUT_US_1S);

	do {
		pgsr = mmio_read_32((uintptr_t)&phy->pgsr);

		VERBOSE("  > [0x%lx] pgsr = 0x%x &",
			(uintptr_t)&phy->pgsr, pgsr);

//		if (timeout_elapsed(timeout)) {
//			panic();
//		}

		if ((pgsr & DDRPHYC_PGSR_DTERR) != 0U) {
			VERBOSE("DQS Gate Training Error\n");
			error++;
		}

		if ((pgsr & DDRPHYC_PGSR_DTIERR) != 0U) {
			VERBOSE("DQS Gate Training Intermittent Error\n");
			error++;
		}

		if ((pgsr & DDRPHYC_PGSR_DFTERR) != 0U) {
			VERBOSE("DQS Drift Error\n");
			error++;
		}

		if ((pgsr & DDRPHYC_PGSR_RVERR) != 0U) {
			VERBOSE("Read Valid Training Error\n");
			error++;
		}

		if ((pgsr & DDRPHYC_PGSR_RVEIRR) != 0U) {
			VERBOSE("Read Valid Training Intermittent Error\n");
			error++;
		}
	} while (((pgsr & DDRPHYC_PGSR_IDONE) == 0U) && (error == 0));
	VERBOSE("\n[0x%lx] pgsr = 0x%x\n",
		(uintptr_t)&phy->pgsr, pgsr);
}

static void stm32mp1_ddrphy_init(struct stm32mp1_ddrphy *phy, uint32_t pir)
{
	uint32_t pir_init = pir | DDRPHYC_PIR_INIT;

	mmio_write_32((uintptr_t)&phy->pir, pir_init);
	VERBOSE("[0x%lx] pir = 0x%x -> 0x%x\n",
		(uintptr_t)&phy->pir, pir_init,
		mmio_read_32((uintptr_t)&phy->pir));

	/* Need to wait 10 configuration clock before start polling */
	local_delay_us(10);

	/* Wait DRAM initialization and Gate Training Evaluation complete */
	stm32mp1_ddrphy_idone_wait(phy);
}

/* Start quasi dynamic register update */
static void stm32mp1_start_sw_done(struct stm32mp1_ddrctl *ctl)
{
	mmio_clrbits_32((uintptr_t)&ctl->swctl, DDRCTRL_SWCTL_SW_DONE);
	VERBOSE("[0x%lx] swctl = 0x%x\n",
		(uintptr_t)&ctl->swctl,  mmio_read_32((uintptr_t)&ctl->swctl));
}

/* Wait quasi dynamic register update */
static void stm32mp1_wait_sw_done_ack(struct stm32mp1_ddrctl *ctl)
{
//	uint64_t timeout;
	uint32_t swstat;

	mmio_setbits_32((uintptr_t)&ctl->swctl, DDRCTRL_SWCTL_SW_DONE);
	VERBOSE("[0x%lx] swctl = 0x%x\n",
		(uintptr_t)&ctl->swctl, mmio_read_32((uintptr_t)&ctl->swctl));

//	timeout = timeout_init_us(TIMEOUT_US_1S);
	do {
		swstat = mmio_read_32((uintptr_t)&ctl->swstat);
		VERBOSE("1 [0x%lx] swstat = 0x%x ",
			(uintptr_t)&ctl->swstat, swstat);
//		if (timeout_elapsed(timeout)) {
//			panic();
//		}
	} while ((swstat & DDRCTRL_SWSTAT_SW_DONE_ACK) == 0U);

	VERBOSE("2 [0x%lx] swstat = 0x%x\n",
		(uintptr_t)&ctl->swstat, swstat);
}

/* Wait quasi dynamic register update */
static void stm32mp1_wait_operating_mode(struct ddr_info *priv, uint32_t mode)
{
//	uint64_t timeout;
	uint32_t stat;
	int break_loop = 0;

//	timeout = timeout_init_us(TIMEOUT_US_1S);
	for ( ; ; ) {
		uint32_t operating_mode;
		uint32_t selref_type;

		stat = mmio_read_32((uintptr_t)&priv->ctl->stat);
		operating_mode = stat & DDRCTRL_STAT_OPERATING_MODE_MASK;
		selref_type = stat & DDRCTRL_STAT_SELFREF_TYPE_MASK;
		VERBOSE("1 [0x%lx] stat = 0x%x\n",
			(uintptr_t)&priv->ctl->stat, stat);
//		if (timeout_elapsed(timeout)) {
//			panic();
//		}

		if (mode == DDRCTRL_STAT_OPERATING_MODE_SR) {
			/*
			 * Self-refresh due to software
			 * => checking also STAT.selfref_type.
			 */
			if ((operating_mode ==
			     DDRCTRL_STAT_OPERATING_MODE_SR) &&
			    (selref_type == DDRCTRL_STAT_SELFREF_TYPE_SR)) {
				break_loop = 1;
			}
		} else if (operating_mode == mode) {
			break_loop = 1;
		} else if ((mode == DDRCTRL_STAT_OPERATING_MODE_NORMAL) &&
			   (operating_mode == DDRCTRL_STAT_OPERATING_MODE_SR) &&
			   (selref_type == DDRCTRL_STAT_SELFREF_TYPE_ASR)) {
			/* Normal mode: handle also automatic self refresh */
			break_loop = 1;
		}

		if (break_loop == 1) {
			break;
		}
	}

	VERBOSE("2 [0x%lx] stat = 0x%x\n",
		(uintptr_t)&priv->ctl->stat, stat);
}

/* Mode Register Writes (MRW or MRS) */
static void stm32mp1_mode_register_write(struct ddr_info *priv, uint8_t addr,
					 uint32_t data)
{
	uint32_t mrctrl0;

	VERBOSE("MRS: %d = %x\n", addr, data);

	/*
	 * 1. Poll MRSTAT.mr_wr_busy until it is '0'.
	 *    This checks that there is no outstanding MR transaction.
	 *    No write should be performed to MRCTRL0 and MRCTRL1
	 *    if MRSTAT.mr_wr_busy = 1.
	 */
	while ((mmio_read_32((uintptr_t)&priv->ctl->mrstat) &
		DDRCTRL_MRSTAT_MR_WR_BUSY) != 0U) {
		;
	}

	/*
	 * 2. Write the MRCTRL0.mr_type, MRCTRL0.mr_addr, MRCTRL0.mr_rank
	 *    and (for MRWs) MRCTRL1.mr_data to define the MR transaction.
	 */
	mrctrl0 = DDRCTRL_MRCTRL0_MR_TYPE_WRITE |
		  DDRCTRL_MRCTRL0_MR_RANK_ALL |
		  (((uint32_t)addr << DDRCTRL_MRCTRL0_MR_ADDR_SHIFT) &
		   DDRCTRL_MRCTRL0_MR_ADDR_MASK);
	mmio_write_32((uintptr_t)&priv->ctl->mrctrl0, mrctrl0);
	VERBOSE("[0x%lx] mrctrl0 = 0x%x (0x%x)\n",
		(uintptr_t)&priv->ctl->mrctrl0,
		mmio_read_32((uintptr_t)&priv->ctl->mrctrl0), mrctrl0);
	mmio_write_32((uintptr_t)&priv->ctl->mrctrl1, data);
	VERBOSE("[0x%lx] mrctrl1 = 0x%x\n",
		(uintptr_t)&priv->ctl->mrctrl1,
		mmio_read_32((uintptr_t)&priv->ctl->mrctrl1));

	/*
	 * 3. In a separate APB transaction, write the MRCTRL0.mr_wr to 1. This
	 *    bit is self-clearing, and triggers the MR transaction.
	 *    The uMCTL2 then asserts the MRSTAT.mr_wr_busy while it performs
	 *    the MR transaction to SDRAM, and no further access can be
	 *    initiated until it is deasserted.
	 */
	mrctrl0 |= DDRCTRL_MRCTRL0_MR_WR;
	mmio_write_32((uintptr_t)&priv->ctl->mrctrl0, mrctrl0);

	while ((mmio_read_32((uintptr_t)&priv->ctl->mrstat) &
	       DDRCTRL_MRSTAT_MR_WR_BUSY) != 0U) {
		;
	}

	VERBOSE("[0x%lx] mrctrl0 = 0x%x\n",
		(uintptr_t)&priv->ctl->mrctrl0, mrctrl0);
}

/* Switch DDR3 from DLL-on to DLL-off */
static void stm32mp1_ddr3_dll_off(struct ddr_info *priv)
{
	uint32_t mr1 = mmio_read_32((uintptr_t)&priv->phy->mr1);
	uint32_t mr2 = mmio_read_32((uintptr_t)&priv->phy->mr2);
	uint32_t dbgcam;

	VERBOSE("mr1: 0x%x\n", mr1);
	VERBOSE("mr2: 0x%x\n", mr2);

	/*
	 * 1. Set the DBG1.dis_hif = 1.
	 *    This prevents further reads/writes being received on the HIF.
	 */
	mmio_setbits_32((uintptr_t)&priv->ctl->dbg1, DDRCTRL_DBG1_DIS_HIF);
	VERBOSE("[0x%lx] dbg1 = 0x%x\n",
		(uintptr_t)&priv->ctl->dbg1,
		mmio_read_32((uintptr_t)&priv->ctl->dbg1));

	/*
	 * 2. Ensure all commands have been flushed from the uMCTL2 by polling
	 *    DBGCAM.wr_data_pipeline_empty = 1,
	 *    DBGCAM.rd_data_pipeline_empty = 1,
	 *    DBGCAM.dbg_wr_q_depth = 0 ,
	 *    DBGCAM.dbg_lpr_q_depth = 0, and
	 *    DBGCAM.dbg_hpr_q_depth = 0.
	 */
	do {
		dbgcam = mmio_read_32((uintptr_t)&priv->ctl->dbgcam);
		VERBOSE("[0x%lx] dbgcam = 0x%x\n",
			(uintptr_t)&priv->ctl->dbgcam, dbgcam);
	} while ((((dbgcam & DDRCTRL_DBGCAM_DATA_PIPELINE_EMPTY) ==
		   DDRCTRL_DBGCAM_DATA_PIPELINE_EMPTY)) &&
		 ((dbgcam & DDRCTRL_DBGCAM_DBG_Q_DEPTH) == 0U));

	/*
	 * 3. Perform an MRS command (using MRCTRL0 and MRCTRL1 registers)
	 *    to disable RTT_NOM:
	 *    a. DDR3: Write to MR1[9], MR1[6] and MR1[2]
	 *    b. DDR4: Write to MR1[10:8]
	 */
	mr1 &= ~(BIT(9) | BIT(6) | BIT(2));
	stm32mp1_mode_register_write(priv, 1, mr1);

	/*
	 * 4. For DDR4 only: Perform an MRS command
	 *    (using MRCTRL0 and MRCTRL1 registers) to write to MR5[8:6]
	 *    to disable RTT_PARK
	 */

	/*
	 * 5. Perform an MRS command (using MRCTRL0 and MRCTRL1 registers)
	 *    to write to MR2[10:9], to disable RTT_WR
	 *    (and therefore disable dynamic ODT).
	 *    This applies for both DDR3 and DDR4.
	 */
	mr2 &= ~GENMASK(10, 9);
	stm32mp1_mode_register_write(priv, 2, mr2);

	/*
	 * 6. Perform an MRS command (using MRCTRL0 and MRCTRL1 registers)
	 *    to disable the DLL. The timing of this MRS is automatically
	 *    handled by the uMCTL2.
	 *    a. DDR3: Write to MR1[0]
	 *    b. DDR4: Write to MR1[0]
	 */
	mr1 |= BIT(0);
	stm32mp1_mode_register_write(priv, 1, mr1);

	/*
	 * 7. Put the SDRAM into self-refresh mode by setting
	 *    PWRCTL.selfref_sw = 1, and polling STAT.operating_mode to ensure
	 *    the DDRC has entered self-refresh.
	 */
	mmio_setbits_32((uintptr_t)&priv->ctl->pwrctl,
			DDRCTRL_PWRCTL_SELFREF_SW);
	VERBOSE("[0x%lx] pwrctl = 0x%x\n",
		(uintptr_t)&priv->ctl->pwrctl,
		mmio_read_32((uintptr_t)&priv->ctl->pwrctl));

	/*
	 * 8. Wait until STAT.operating_mode[1:0]==11 indicating that the
	 *    DWC_ddr_umctl2 core is in self-refresh mode.
	 *    Ensure transition to self-refresh was due to software
	 *    by checking that STAT.selfref_type[1:0]=2.
	 */
	stm32mp1_wait_operating_mode(priv, DDRCTRL_STAT_OPERATING_MODE_SR);

	/*
	 * 9. Set the MSTR.dll_off_mode = 1.
	 *    warning: MSTR.dll_off_mode is a quasi-dynamic type 2 field
	 */
	stm32mp1_start_sw_done(priv->ctl);

	mmio_setbits_32((uintptr_t)&priv->ctl->mstr, DDRCTRL_MSTR_DLL_OFF_MODE);
	VERBOSE("[0x%lx] mstr = 0x%x\n",
		(uintptr_t)&priv->ctl->mstr,
		mmio_read_32((uintptr_t)&priv->ctl->mstr));

	stm32mp1_wait_sw_done_ack(priv->ctl);

	/* 10. Change the clock frequency to the desired value. */

	/*
	 * 11. Update any registers which may be required to change for the new
	 *     frequency. This includes static and dynamic registers.
	 *     This includes both uMCTL2 registers and PHY registers.
	 */

	/* Change Bypass Mode Frequency Range */
	if (stm32mp_ddr_clk_get_rate(0/*DDRPHYC*/) < 100000000U) {
		mmio_clrbits_32((uintptr_t)&priv->phy->dllgcr,
				DDRPHYC_DLLGCR_BPS200);
	} else {
		mmio_setbits_32((uintptr_t)&priv->phy->dllgcr,
				DDRPHYC_DLLGCR_BPS200);
	}

	mmio_setbits_32((uintptr_t)&priv->phy->acdllcr, DDRPHYC_ACDLLCR_DLLDIS);

	mmio_setbits_32((uintptr_t)&priv->phy->dx0dllcr,
			DDRPHYC_DXNDLLCR_DLLDIS);
	mmio_setbits_32((uintptr_t)&priv->phy->dx1dllcr,
			DDRPHYC_DXNDLLCR_DLLDIS);
	mmio_setbits_32((uintptr_t)&priv->phy->dx2dllcr,
			DDRPHYC_DXNDLLCR_DLLDIS);
	mmio_setbits_32((uintptr_t)&priv->phy->dx3dllcr,
			DDRPHYC_DXNDLLCR_DLLDIS);

	/* 12. Exit the self-refresh state by setting PWRCTL.selfref_sw = 0. */
	mmio_clrbits_32((uintptr_t)&priv->ctl->pwrctl,
			DDRCTRL_PWRCTL_SELFREF_SW);
	stm32mp1_wait_operating_mode(priv, DDRCTRL_STAT_OPERATING_MODE_NORMAL);

	/*
	 * 13. If ZQCTL0.dis_srx_zqcl = 0, the uMCTL2 performs a ZQCL command
	 *     at this point.
	 */

	/*
	 * 14. Perform MRS commands as required to re-program timing registers
	 *     in the SDRAM for the new frequency
	 *     (in particular, CL, CWL and WR may need to be changed).
	 */

	/* 15. Write DBG1.dis_hif = 0 to re-enable reads and writes. */
	mmio_clrbits_32((uintptr_t)&priv->ctl->dbg1, DDRCTRL_DBG1_DIS_HIF);
	VERBOSE("[0x%lx] dbg1 = 0x%x\n",
		(uintptr_t)&priv->ctl->dbg1,
		mmio_read_32((uintptr_t)&priv->ctl->dbg1));
}

static void stm32mp1_refresh_disable(struct stm32mp1_ddrctl *ctl)
{
	stm32mp1_start_sw_done(ctl);
	/* Quasi-dynamic register update*/
	mmio_setbits_32((uintptr_t)&ctl->rfshctl3,
			DDRCTRL_RFSHCTL3_DIS_AUTO_REFRESH);
	mmio_clrbits_32((uintptr_t)&ctl->pwrctl, DDRCTRL_PWRCTL_POWERDOWN_EN);
	mmio_clrbits_32((uintptr_t)&ctl->dfimisc,
			DDRCTRL_DFIMISC_DFI_INIT_COMPLETE_EN);
	stm32mp1_wait_sw_done_ack(ctl);
}

static void stm32mp1_refresh_restore(struct stm32mp1_ddrctl *ctl,
				     uint32_t rfshctl3, uint32_t pwrctl)
{
	stm32mp1_start_sw_done(ctl);
	if ((rfshctl3 & DDRCTRL_RFSHCTL3_DIS_AUTO_REFRESH) == 0U) {
		mmio_clrbits_32((uintptr_t)&ctl->rfshctl3,
				DDRCTRL_RFSHCTL3_DIS_AUTO_REFRESH);
	}
	if ((pwrctl & DDRCTRL_PWRCTL_POWERDOWN_EN) != 0U) {
		mmio_setbits_32((uintptr_t)&ctl->pwrctl,
				DDRCTRL_PWRCTL_POWERDOWN_EN);
	}
	mmio_setbits_32((uintptr_t)&ctl->dfimisc,
			DDRCTRL_DFIMISC_DFI_INIT_COMPLETE_EN);
	stm32mp1_wait_sw_done_ack(ctl);
}

#if WITHSDRAM_PMC1

#define I2C_TIMEOUT_MS		25

struct regul_struct {
	const char *dt_node_name;
	const uint16_t *voltage_table;
	uint8_t voltage_table_size;
	uint8_t control_reg;
	uint8_t low_power_reg;
	uint8_t pull_down_reg;
	uint8_t pull_down;
	uint8_t mask_reset_reg;
	uint8_t mask_reset;
};

//static struct i2c_handle_s *pmic_i2c_handle;
static const uint16_t pmic_i2c_addr = (0x33 << 1);


#define TURN_ON_REG			0x1U
#define TURN_OFF_REG			0x2U
#define ICC_LDO_TURN_OFF_REG		0x3U
#define ICC_BUCK_TURN_OFF_REG		0x4U
#define RESET_STATUS_REG		0x5U
#define VERSION_STATUS_REG		0x6U
#define MAIN_CONTROL_REG		0x10U
#define PADS_PULL_REG			0x11U
#define BUCK_PULL_DOWN_REG		0x12U
#define LDO14_PULL_DOWN_REG		0x13U
#define LDO56_PULL_DOWN_REG		0x14U
#define VIN_CONTROL_REG			0x15U
#define PONKEY_TIMER_REG		0x16U
#define MASK_RANK_BUCK_REG		0x17U
#define MASK_RESET_BUCK_REG		0x18U
#define MASK_RANK_LDO_REG		0x19U
#define MASK_RESET_LDO_REG		0x1AU
#define WATCHDOG_CONTROL_REG		0x1BU
#define WATCHDOG_TIMER_REG		0x1CU
#define BUCK_ICC_TURNOFF_REG		0x1DU
#define LDO_ICC_TURNOFF_REG		0x1EU
#define BUCK_APM_CONTROL_REG		0x1FU
#define BUCK1_CONTROL_REG		0x20U
#define BUCK2_CONTROL_REG		0x21U
#define BUCK3_CONTROL_REG		0x22U
#define BUCK4_CONTROL_REG		0x23U
#define VREF_DDR_CONTROL_REG		0x24U
#define LDO1_CONTROL_REG		0x25U
#define LDO2_CONTROL_REG		0x26U
#define LDO3_CONTROL_REG		0x27U
#define LDO4_CONTROL_REG		0x28U
#define LDO5_CONTROL_REG		0x29U
#define LDO6_CONTROL_REG		0x2AU
#define BUCK1_PWRCTRL_REG		0x30U
#define BUCK2_PWRCTRL_REG		0x31U
#define BUCK3_PWRCTRL_REG		0x32U
#define BUCK4_PWRCTRL_REG		0x33U
#define VREF_DDR_PWRCTRL_REG		0x34U
#define LDO1_PWRCTRL_REG		0x35U
#define LDO2_PWRCTRL_REG		0x36U
#define LDO3_PWRCTRL_REG		0x37U
#define LDO4_PWRCTRL_REG		0x38U
#define LDO5_PWRCTRL_REG		0x39U
#define LDO6_PWRCTRL_REG		0x3AU
#define FREQUENCY_SPREADING_REG		0x3BU
#define USB_CONTROL_REG			0x40U
#define ITLATCH1_REG			0x50U
#define ITLATCH2_REG			0x51U
#define ITLATCH3_REG			0x52U
#define ITLATCH4_REG			0x53U
#define ITSETLATCH1_REG			0x60U
#define ITSETLATCH2_REG			0x61U
#define ITSETLATCH3_REG			0x62U
#define ITSETLATCH4_REG			0x63U
#define ITCLEARLATCH1_REG		0x70U
#define ITCLEARLATCH2_REG		0x71U
#define ITCLEARLATCH3_REG		0x72U
#define ITCLEARLATCH4_REG		0x73U
#define ITMASK1_REG			0x80U
#define ITMASK2_REG			0x81U
#define ITMASK3_REG			0x82U
#define ITMASK4_REG			0x83U
#define ITSETMASK1_REG			0x90U
#define ITSETMASK2_REG			0x91U
#define ITSETMASK3_REG			0x92U
#define ITSETMASK4_REG			0x93U
#define ITCLEARMASK1_REG		0xA0U
#define ITCLEARMASK2_REG		0xA1U
#define ITCLEARMASK3_REG		0xA2U
#define ITCLEARMASK4_REG		0xA3U
#define ITSOURCE1_REG			0xB0U
#define ITSOURCE2_REG			0xB1U
#define ITSOURCE3_REG			0xB2U
#define ITSOURCE4_REG			0xB3U

/* Registers masks */
#define LDO_VOLTAGE_MASK		0x7CU
#define BUCK_VOLTAGE_MASK		0xFCU
#define LDO_BUCK_VOLTAGE_SHIFT		2
#define LDO_BUCK_ENABLE_MASK		0x01U
#define LDO_BUCK_HPLP_ENABLE_MASK	0x02U
#define LDO_BUCK_HPLP_SHIFT		1
#define LDO_BUCK_RANK_MASK		0x01U
#define LDO_BUCK_RESET_MASK		0x01U
#define LDO_BUCK_PULL_DOWN_MASK		0x03U

/* Pull down register */
#define BUCK1_PULL_DOWN_SHIFT		0
#define BUCK2_PULL_DOWN_SHIFT		2
#define BUCK3_PULL_DOWN_SHIFT		4
#define BUCK4_PULL_DOWN_SHIFT		6
#define VREF_DDR_PULL_DOWN_SHIFT	4

/* Buck Mask reset register */
#define BUCK1_MASK_RESET		0
#define BUCK2_MASK_RESET		1
#define BUCK3_MASK_RESET		2
#define BUCK4_MASK_RESET		3

/* LDO Mask reset register */
#define LDO1_MASK_RESET			0
#define LDO2_MASK_RESET			1
#define LDO3_MASK_RESET			2
#define LDO4_MASK_RESET			3
#define LDO5_MASK_RESET			4
#define LDO6_MASK_RESET			5
#define VREF_DDR_MASK_RESET		6

/* Main PMIC Control Register (MAIN_CONTROL_REG) */
#define ICC_EVENT_ENABLED		BIT(4)
#define PWRCTRL_POLARITY_HIGH		BIT(3)
#define PWRCTRL_PIN_VALID		BIT(2)
#define RESTART_REQUEST_ENABLED		BIT(1)
#define SOFTWARE_SWITCH_OFF_ENABLED	BIT(0)

/* Main PMIC PADS Control Register (PADS_PULL_REG) */
#define WAKEUP_DETECTOR_DISABLED	BIT(4)
#define PWRCTRL_PD_ACTIVE		BIT(3)
#define PWRCTRL_PU_ACTIVE		BIT(2)
#define WAKEUP_PD_ACTIVE		BIT(1)
#define PONKEY_PU_ACTIVE		BIT(0)

/* Main PMIC VINLOW Control Register (VIN_CONTROL_REGC DMSC) */
#define SWIN_DETECTOR_ENABLED		BIT(7)
#define SWOUT_DETECTOR_ENABLED          BIT(6)
#define VINLOW_HYST_MASK		0x3
#define VINLOW_HYST_SHIFT		4
#define VINLOW_THRESHOLD_MASK		0x7
#define VINLOW_THRESHOLD_SHIFT		1
#define VINLOW_ENABLED			0x01
#define VINLOW_CTRL_REG_MASK		0xFF

/* USB Control Register */
#define BOOST_OVP_DISABLED		BIT(7)
#define VBUS_OTG_DETECTION_DISABLED	BIT(6)
#define OCP_LIMIT_HIGH			BIT(3)
#define SWIN_SWOUT_ENABLED		BIT(2)
#define USBSW_OTG_SWITCH_ENABLED	BIT(1)


#define STPMIC1_LDO12356_OUTPUT_MASK	(uint8_t)(GENMASK(6, 2))
#define STPMIC1_LDO12356_OUTPUT_SHIFT	2
#define STPMIC1_LDO3_MODE		(uint8_t)(BIT(7))
#define STPMIC1_LDO3_DDR_SEL		31U
#define STPMIC1_LDO3_1800000		(9U << STPMIC1_LDO12356_OUTPUT_SHIFT)

#define STPMIC1_BUCK_OUTPUT_SHIFT	2
#define STPMIC1_BUCK3_1V8		(39U << STPMIC1_BUCK_OUTPUT_SHIFT)

#define STPMIC1_DEFAULT_START_UP_DELAY_MS	1

/* Voltage tables in mV */
static const uint16_t buck1_voltage_table[] = {
	725,
	725,
	725,
	725,
	725,
	725,
	750,
	775,
	800,
	825,
	850,
	875,
	900,
	925,
	950,
	975,
	1000,
	1025,
	1050,
	1075,
	1100,
	1125,
	1150,
	1175,
	1200,
	1225,
	1250,
	1275,
	1300,
	1325,
	1350,
	1375,
	1400,
	1425,
	1450,
	1475,
	1500,
	1500,
	1500,
	1500,
	1500,
	1500,
	1500,
	1500,
	1500,
	1500,
	1500,
	1500,
	1500,
	1500,
	1500,
	1500,
	1500,
	1500,
	1500,
	1500,
	1500,
	1500,
	1500,
	1500,
	1500,
	1500,
	1500,
	1500,
};

static const uint16_t buck2_voltage_table[] = {
	1000,
	1000,
	1000,
	1000,
	1000,
	1000,
	1000,
	1000,
	1000,
	1000,
	1000,
	1000,
	1000,
	1000,
	1000,
	1000,
	1000,
	1000,
	1050,
	1050,
	1100,
	1100,
	1150,
	1150,
	1200,
	1200,
	1250,
	1250,
	1300,
	1300,
	1350,
	1350,
	1400,
	1400,
	1450,
	1450,
	1500,
};

static const uint16_t buck3_voltage_table[] = {
	1000,
	1000,
	1000,
	1000,
	1000,
	1000,
	1000,
	1000,
	1000,
	1000,
	1000,
	1000,
	1000,
	1000,
	1000,
	1000,
	1000,
	1000,
	1000,
	1000,
	1100,
	1100,
	1100,
	1100,
	1200,
	1200,
	1200,
	1200,
	1300,
	1300,
	1300,
	1300,
	1400,
	1400,
	1400,
	1400,
	1500,
	1600,
	1700,
	1800,
	1900,
	2000,
	2100,
	2200,
	2300,
	2400,
	2500,
	2600,
	2700,
	2800,
	2900,
	3000,
	3100,
	3200,
	3300,
	3400,
};

static const uint16_t buck4_voltage_table[] = {
	600,
	625,
	650,
	675,
	700,
	725,
	750,
	775,
	800,
	825,
	850,
	875,
	900,
	925,
	950,
	975,
	1000,
	1025,
	1050,
	1075,
	1100,
	1125,
	1150,
	1175,
	1200,
	1225,
	1250,
	1275,
	1300,
	1300,
	1350,
	1350,
	1400,
	1400,
	1450,
	1450,
	1500,
	1600,
	1700,
	1800,
	1900,
	2000,
	2100,
	2200,
	2300,
	2400,
	2500,
	2600,
	2700,
	2800,
	2900,
	3000,
	3100,
	3200,
	3300,
	3400,
	3500,
	3600,
	3700,
	3800,
	3900,
};

static const uint16_t ldo1_voltage_table[] = {
	1700,
	1700,
	1700,
	1700,
	1700,
	1700,
	1700,
	1700,
	1700,
	1800,
	1900,
	2000,
	2100,
	2200,
	2300,
	2400,
	2500,
	2600,
	2700,
	2800,
	2900,
	3000,
	3100,
	3200,
	3300,
};

static const uint16_t ldo2_voltage_table[] = {
	1700,
	1700,
	1700,
	1700,
	1700,
	1700,
	1700,
	1700,
	1700,
	1800,
	1900,
	2000,
	2100,
	2200,
	2300,
	2400,
	2500,
	2600,
	2700,
	2800,
	2900,
	3000,
	3100,
	3200,
	3300,
};

static const uint16_t ldo3_voltage_table[] = {
	1700,
	1700,
	1700,
	1700,
	1700,
	1700,
	1700,
	1700,
	1700,
	1800,
	1900,
	2000,
	2100,
	2200,
	2300,
	2400,
	2500,
	2600,
	2700,
	2800,
	2900,
	3000,
	3100,
	3200,
	3300,
	3300,
	3300,
	3300,
	3300,
	3300,
	3300,
	500,
	0xFFFF, /* VREFDDR */
};

static const uint16_t ldo5_voltage_table[] = {
	1700,
	1700,
	1700,
	1700,
	1700,
	1700,
	1700,
	1700,
	1700,
	1800,
	1900,
	2000,
	2100,
	2200,
	2300,
	2400,
	2500,
	2600,
	2700,
	2800,
	2900,
	3000,
	3100,
	3200,
	3300,
	3400,
	3500,
	3600,
	3700,
	3800,
	3900,
};

static const uint16_t ldo6_voltage_table[] = {
	900,
	1000,
	1100,
	1200,
	1300,
	1400,
	1500,
	1600,
	1700,
	1800,
	1900,
	2000,
	2100,
	2200,
	2300,
	2400,
	2500,
	2600,
	2700,
	2800,
	2900,
	3000,
	3100,
	3200,
	3300,
};

static const uint16_t ldo4_voltage_table[] = {
	3300,
};

static const uint16_t vref_ddr_voltage_table[] = {
	3300,
};

/* Table of Regulators in PMIC SoC */
static const struct regul_struct regulators_table[] = {
	{
		.dt_node_name	= "buck1",
		.voltage_table	= buck1_voltage_table,
		.voltage_table_size = ARRAY_SIZE(buck1_voltage_table),
		.control_reg	= BUCK1_CONTROL_REG,
		.low_power_reg	= BUCK1_PWRCTRL_REG,
		.pull_down_reg	= BUCK_PULL_DOWN_REG,
		.pull_down	= BUCK1_PULL_DOWN_SHIFT,
		.mask_reset_reg	= MASK_RESET_BUCK_REG,
		.mask_reset	= BUCK1_MASK_RESET,
	},
	{
		.dt_node_name	= "buck2",
		.voltage_table	= buck2_voltage_table,
		.voltage_table_size = ARRAY_SIZE(buck2_voltage_table),
		.control_reg	= BUCK2_CONTROL_REG,
		.low_power_reg	= BUCK2_PWRCTRL_REG,
		.pull_down_reg	= BUCK_PULL_DOWN_REG,
		.pull_down	= BUCK2_PULL_DOWN_SHIFT,
		.mask_reset_reg	= MASK_RESET_BUCK_REG,
		.mask_reset	= BUCK2_MASK_RESET,
	},
	{
		.dt_node_name	= "buck3",
		.voltage_table	= buck3_voltage_table,
		.voltage_table_size = ARRAY_SIZE(buck3_voltage_table),
		.control_reg	= BUCK3_CONTROL_REG,
		.low_power_reg	= BUCK3_PWRCTRL_REG,
		.pull_down_reg	= BUCK_PULL_DOWN_REG,
		.pull_down	= BUCK3_PULL_DOWN_SHIFT,
		.mask_reset_reg	= MASK_RESET_BUCK_REG,
		.mask_reset	= BUCK3_MASK_RESET,
	},
	{
		.dt_node_name	= "buck4",
		.voltage_table	= buck4_voltage_table,
		.voltage_table_size = ARRAY_SIZE(buck4_voltage_table),
		.control_reg	= BUCK4_CONTROL_REG,
		.low_power_reg	= BUCK4_PWRCTRL_REG,
		.pull_down_reg	= BUCK_PULL_DOWN_REG,
		.pull_down	= BUCK4_PULL_DOWN_SHIFT,
		.mask_reset_reg	= MASK_RESET_BUCK_REG,
		.mask_reset	= BUCK4_MASK_RESET,
	},
	{
		.dt_node_name	= "ldo1",
		.voltage_table	= ldo1_voltage_table,
		.voltage_table_size = ARRAY_SIZE(ldo1_voltage_table),
		.control_reg	= LDO1_CONTROL_REG,
		.low_power_reg	= LDO1_PWRCTRL_REG,
		.mask_reset_reg	= MASK_RESET_LDO_REG,
		.mask_reset	= LDO1_MASK_RESET,
	},
	{
		.dt_node_name	= "ldo2",
		.voltage_table	= ldo2_voltage_table,
		.voltage_table_size = ARRAY_SIZE(ldo2_voltage_table),
		.control_reg	= LDO2_CONTROL_REG,
		.low_power_reg	= LDO2_PWRCTRL_REG,
		.mask_reset_reg	= MASK_RESET_LDO_REG,
		.mask_reset	= LDO2_MASK_RESET,
	},
	{
		.dt_node_name	= "ldo3",
		.voltage_table	= ldo3_voltage_table,
		.voltage_table_size = ARRAY_SIZE(ldo3_voltage_table),
		.control_reg	= LDO3_CONTROL_REG,
		.low_power_reg	= LDO3_PWRCTRL_REG,
		.mask_reset_reg	= MASK_RESET_LDO_REG,
		.mask_reset	= LDO3_MASK_RESET,
	},
	{
		.dt_node_name	= "ldo4",
		.voltage_table	= ldo4_voltage_table,
		.voltage_table_size = ARRAY_SIZE(ldo4_voltage_table),
		.control_reg	= LDO4_CONTROL_REG,
		.low_power_reg	= LDO4_PWRCTRL_REG,
		.mask_reset_reg	= MASK_RESET_LDO_REG,
		.mask_reset	= LDO4_MASK_RESET,
	},
	{
		.dt_node_name	= "ldo5",
		.voltage_table	= ldo5_voltage_table,
		.voltage_table_size = ARRAY_SIZE(ldo5_voltage_table),
		.control_reg	= LDO5_CONTROL_REG,
		.low_power_reg	= LDO5_PWRCTRL_REG,
		.mask_reset_reg	= MASK_RESET_LDO_REG,
		.mask_reset	= LDO5_MASK_RESET,
	},
	{
		.dt_node_name	= "ldo6",
		.voltage_table	= ldo6_voltage_table,
		.voltage_table_size = ARRAY_SIZE(ldo6_voltage_table),
		.control_reg	= LDO6_CONTROL_REG,
		.low_power_reg	= LDO6_PWRCTRL_REG,
		.mask_reset_reg	= MASK_RESET_LDO_REG,
		.mask_reset	= LDO6_MASK_RESET,
	},
	{
		.dt_node_name	= "vref_ddr",
		.voltage_table	= vref_ddr_voltage_table,
		.voltage_table_size = ARRAY_SIZE(vref_ddr_voltage_table),
		.control_reg	= VREF_DDR_CONTROL_REG,
		.low_power_reg	= VREF_DDR_PWRCTRL_REG,
		.mask_reset_reg	= MASK_RESET_LDO_REG,
		.mask_reset	= VREF_DDR_MASK_RESET,
	},
};

#define MAX_REGUL	ARRAY_SIZE(regulators_table)

static const struct regul_struct *get_regulator_data(const char *name)
{
	uint8_t i;

	for (i = 0 ; i < MAX_REGUL ; i++) {
		if (strncmp(name, regulators_table[i].dt_node_name,
			    strlen(regulators_table[i].dt_node_name)) == 0) {
			return &regulators_table[i];
		}
	}

	/* Regulator not found */
	panic();
	return NULL;
}

static uint8_t voltage_to_index(const char *name, uint16_t millivolts)
{
	const struct regul_struct *regul = get_regulator_data(name);
	uint8_t i;

	for (i = 0 ; i < regul->voltage_table_size ; i++) {
		if (regul->voltage_table[i] == millivolts) {
			return i;
		}
	}

	/* Voltage not found */
	panic();

	return 0;
}


static int stpmic1_register_update(uint8_t register_id, uint8_t value, uint8_t mask);
static int stpmic1_register_read(uint8_t register_id,  uint8_t *value);

static int stpmic1_powerctrl_on(void)
{
	return stpmic1_register_update(MAIN_CONTROL_REG, PWRCTRL_PIN_VALID,
				       PWRCTRL_PIN_VALID);
}

static int stpmic1_switch_off(void)
{
	return stpmic1_register_update(MAIN_CONTROL_REG, 1,
				       SOFTWARE_SWITCH_OFF_ENABLED);
}

static int stpmic1_regulator_enable(const char *name)
{
	const struct regul_struct *regul = get_regulator_data(name);

	return stpmic1_register_update(regul->control_reg, BIT(0), BIT(0));
}

static int stpmic1_regulator_disable(const char *name)
{
	const struct regul_struct *regul = get_regulator_data(name);

	return stpmic1_register_update(regul->control_reg, 0, BIT(0));
}

static uint8_t stpmic1_is_regulator_enabled(const char *name)
{
	uint8_t val;
	const struct regul_struct *regul = get_regulator_data(name);

	if (stpmic1_register_read(regul->control_reg, &val) != 0) {
		panic();
	}

	return (val & 0x1U);
}

static int stpmic1_regulator_voltage_set(const char *name, uint16_t millivolts)
{
	uint8_t voltage_index = voltage_to_index(name, millivolts);
	const struct regul_struct *regul = get_regulator_data(name);
	uint8_t mask;

	/* Voltage can be set for buck<N> or ldo<N> (except ldo4) regulators */
	if (strncmp(name, "buck", 4) == 0) {
		mask = BUCK_VOLTAGE_MASK;
	} else if ((strncmp(name, "ldo", 3) == 0) &&
		   (strncmp(name, "ldo4", 4) != 0)) {
		mask = LDO_VOLTAGE_MASK;
	} else {
		return 0;
	}

	return stpmic1_register_update(regul->control_reg,
				       voltage_index << LDO_BUCK_VOLTAGE_SHIFT,
				       mask);
}

static int stpmic1_regulator_pull_down_set(const char *name)
{
	const struct regul_struct *regul = get_regulator_data(name);

	if (regul->pull_down_reg != 0) {
		return stpmic1_register_update(regul->pull_down_reg,
					       BIT(regul->pull_down),
					       LDO_BUCK_PULL_DOWN_MASK <<
					       regul->pull_down);
	}

	return 0;
}

static int stpmic1_regulator_mask_reset_set(const char *name)
{
	const struct regul_struct *regul = get_regulator_data(name);

	return stpmic1_register_update(regul->mask_reset_reg,
				       BIT(regul->mask_reset),
				       LDO_BUCK_RESET_MASK <<
				       regul->mask_reset);
}

static int stpmic1_regulator_voltage_get(const char *name)
{
	const struct regul_struct *regul = get_regulator_data(name);
	uint8_t value;
	uint8_t mask;

	/* Voltage can be set for buck<N> or ldo<N> (except ldo4) regulators */
	if (strncmp(name, "buck", 4) == 0) {
		mask = BUCK_VOLTAGE_MASK;
	} else if ((strncmp(name, "ldo", 3) == 0) &&
		   (strncmp(name, "ldo4", 4) != 0)) {
		mask = LDO_VOLTAGE_MASK;
	} else {
		return 0;
	}

	if (stpmic1_register_read(regul->control_reg, &value))
		return -1;

	value = (value & mask) >> LDO_BUCK_VOLTAGE_SHIFT;

	if (value > regul->voltage_table_size)
		return -1;

	return (int)regul->voltage_table[value];
}

static int stpmic1_register_read(uint8_t register_id,  uint8_t *value)
{
	uint8_t v;

	i2c_start(pmic_i2c_addr | 0x00);
	i2c_write_withrestart(register_id);
	i2c_start(pmic_i2c_addr | 0x01);
	i2c_read(& v, I2C_READ_ACK_NACK);	/* чтение первого и единственного байта ответа */

	* value = v;
	return 0;
/*
	return stm32_i2c_mem_read(pmic_i2c_handle, pmic_i2c_addr,
				  (uint16_t)register_id,
				  I2C_MEMADD_SIZE_8BIT, value,
				  1, I2C_TIMEOUT_MS);
*/
}

static int stpmic1_register_write(uint8_t register_id, uint8_t value)
{
	int status = 0;

	i2c_start(pmic_i2c_addr | 0x00);
	i2c_write(register_id);
	i2c_write(value);
	i2c_waitsend();
	i2c_stop();

/*

	status = stm32_i2c_mem_write(pmic_i2c_handle, pmic_i2c_addr,
				     (uint16_t)register_id,
				     I2C_MEMADD_SIZE_8BIT, &value,
				     1, I2C_TIMEOUT_MS);
*/

#if ENABLE_ASSERTIONS
	if (status != 0) {
		return status;
	}

	if ((register_id != WATCHDOG_CONTROL_REG) && (register_id <= 0x40U)) {
		uint8_t readval;

		status = stpmic1_register_read(register_id, &readval);
		if (status != 0) {
			return status;
		}

		if (readval != value) {
			return -1;
		}
	}
#endif

	return status;
}

static int stpmic1_register_update(uint8_t register_id, uint8_t value, uint8_t mask)
{
	int status;
	uint8_t val;

	status = stpmic1_register_read(register_id, &val);
	if (status != 0) {
		return status;
	}

	val = (val & ~mask) | (value & mask);

	return stpmic1_register_write(register_id, val);
}

static void stpmic1_dump_regulators(void)
{
	uint32_t i;

	for (i = 0U; i < MAX_REGUL; i++) {
		const char *name __unused = regulators_table[i].dt_node_name;

		VERBOSE("PMIC regul %s: %sable, %d mV\n",
			name,
			stpmic1_is_regulator_enabled(name) ? "en" : "dis",
			stpmic1_regulator_voltage_get(name));
	}
}

static int stpmic1_get_version(unsigned long *version)
{
	int rc;
	uint8_t read_val = 0xDD;

	rc = stpmic1_register_read(VERSION_STATUS_REG, &read_val);
	if (rc) {
		return -1;
	}

	*version = (unsigned long)read_val;

	return 0;
}


static int initialize_pmic_i2c(void)
{

	return 1;
}

static void initialize_pmic(void)
{
	unsigned long pmic_version;

	if (!initialize_pmic_i2c()) {
		VERBOSE("No PMIC\n");
		return;
	}

	if (stpmic1_get_version(&pmic_version) != 0) {
		ERROR("Failed to access PMIC\n");
		panic();
	}

	INFO("PMIC version = 0x%02lx\n", pmic_version);
	stpmic1_dump_regulators();

#if defined(IMAGE_BL2)
	if (dt_pmic_configure_boot_on_regulators() != 0) {
		panic();
	};
#endif
}


int pmic_ddr_power_init(enum ddr_type ddr_type)
{
	int buck3_at_1v8 = 0;
	uint8_t read_val;
	int status;

	switch (ddr_type) {
	case STM32MP_DDR3:
		/* Set LDO3 to sync mode */
		status = stpmic1_register_read(LDO3_CONTROL_REG, &read_val);
		if (status != 0) {
			return status;
		}

		read_val &= ~STPMIC1_LDO3_MODE;
		read_val &= ~STPMIC1_LDO12356_OUTPUT_MASK;
		read_val |= STPMIC1_LDO3_DDR_SEL <<
			    STPMIC1_LDO12356_OUTPUT_SHIFT;

		status = stpmic1_register_write(LDO3_CONTROL_REG, read_val);
		if (status != 0) {
			return status;
		}

		status = stpmic1_regulator_voltage_set("buck2", 1350);
		if (status != 0) {
			return status;
		}

		status = stpmic1_regulator_enable("buck2");
		if (status != 0) {
			return status;
		}

		local_delay_ms(STPMIC1_DEFAULT_START_UP_DELAY_MS);

		status = stpmic1_regulator_enable("vref_ddr");
		if (status != 0) {
			return status;
		}

		local_delay_ms(STPMIC1_DEFAULT_START_UP_DELAY_MS);

		status = stpmic1_regulator_enable("ldo3");
		if (status != 0) {
			return status;
		}

		local_delay_ms(STPMIC1_DEFAULT_START_UP_DELAY_MS);
		break;

	case STM32MP_LPDDR2:
	case STM32MP_LPDDR3:
		/*
		 * Set LDO3 to 1.8V
		 * Set LDO3 to bypass mode if BUCK3 = 1.8V
		 * Set LDO3 to normal mode if BUCK3 != 1.8V
		 */
		status = stpmic1_register_read(BUCK3_CONTROL_REG, &read_val);
		if (status != 0) {
			return status;
		}

		if ((read_val & STPMIC1_BUCK3_1V8) == STPMIC1_BUCK3_1V8) {
			buck3_at_1v8 = 1;
		}

		status = stpmic1_register_read(LDO3_CONTROL_REG, &read_val);
		if (status != 0) {
			return status;
		}

		read_val &= ~STPMIC1_LDO3_MODE;
		read_val &= ~STPMIC1_LDO12356_OUTPUT_MASK;
		read_val |= STPMIC1_LDO3_1800000;
		if (buck3_at_1v8) {
			read_val |= STPMIC1_LDO3_MODE;
		}

		status = stpmic1_register_write(LDO3_CONTROL_REG, read_val);
		if (status != 0) {
			return status;
		}

		status = stpmic1_regulator_voltage_set("buck2", 1200);
		if (status != 0) {
			return status;
		}

		status = stpmic1_regulator_enable("ldo3");
		if (status != 0) {
			return status;
		}

		local_delay_ms(STPMIC1_DEFAULT_START_UP_DELAY_MS);

		status = stpmic1_regulator_enable("buck2");
		if (status != 0) {
			return status;
		}

		local_delay_ms(STPMIC1_DEFAULT_START_UP_DELAY_MS);

		status = stpmic1_regulator_enable("vref_ddr");
		if (status != 0) {
			return status;
		}

		local_delay_ms(STPMIC1_DEFAULT_START_UP_DELAY_MS);
		break;

	default:
		break;
	};

	return 0;
}

#endif /* WITHSDRAM_PMC1 */

static int board_ddr_power_init(enum ddr_type ddr_type)
{
#if WITHSDRAM_PMC1
	pmic_ddr_power_init(ddr_type);
#endif /* WITHSDRAM_PMC1 */
//	if (dt_pmic_status() > 0) {
//		return pmic_ddr_power_init(ddr_type);
//	}

	return 0;
}

//	The initialization steps for DDR3 SDRAMs are as follows:
//	1. Optionally maintain RESET# low for a minimum of either 200 us (power-up
//	initialization) or 100ns (power-on initialization).
//	The PUBL drives RESET# low from the beginning of reset assertion and therefore this
//	step may be skipped if enough time may already have expired to satisfy the RESET#
//	low time.
//	2. After RESET# is de-asserted, wait a minimum of 500 us with CKE low.
//	3. Apply NOP and drive CKE high.
//	4. Wait a minimum of tXPR
//	Caution: PUBL registers, SDRAM mode registers, and equivalent register fields inside
//	the memory controller must be uniformly programmed. Mismatches between the
//	register fields can cause transaction failures. Verify all register fields are consistently
//	programmed before starting any SDRAM transaction.
//	5. Issue a load Mode Register 2 (MR2) command.
//	6. Issue a load Mode Register 3 (MR3) command.
//	7. Issue a load Mode Register (MR1) command (to set parameters and enable DLL).
//	8. Issue a load Mode Register (MR0) command to set parameters and reset DLL.
//	9. Issue ZQ calibration command.
//	10. Wait 512 SDRAM clock cycles for the DLL to lock (tDLLK) and ZQ calibration (tZQinit)
//	to finish.
//	This wait time is relative to Step 8, i.e. relative to when the DLL reset command was
//	issued onto the SDRAM command bus.
static void stm32mp1_ddr_init(struct ddr_info *priv,
	       struct stm32mp1_ddr_config *config)
{
	uint32_t pir;
	int ret = - 1;

	if ((config->c_reg.mstr & DDRCTRL_MSTR_DDR3) != 0U) {
		ret = board_ddr_power_init(STM32MP_DDR3);
	} else if ((config->c_reg.mstr & DDRCTRL_MSTR_LPDDR2) != 0U) {
		ret = board_ddr_power_init(STM32MP_LPDDR2);
	} else if ((config->c_reg.mstr & DDRCTRL_MSTR_LPDDR3) != 0U) {
		ret = board_ddr_power_init(STM32MP_LPDDR3);
	} else {
		ERROR("DDR type not supported\n");
	}

	if (ret != 0) {
		panic();
	}

	//stpmic1_dump_regulators();

	VERBOSE("name = %s\n", config->info.name);
	VERBOSE("speed = %d kHz\n", config->info.speed);
	VERBOSE("size  = 0x%x\n", config->info.size);

	/* DDR INIT SEQUENCE */
	/*
	 * 1. Program the DWC_ddr_umctl2 registers
	 *     nota: check DFIMISC.dfi_init_complete = 0
	 */
	/* 1.1 RESETS: presetn, core_ddrc_rstn, aresetn */
	mmio_setbits_32(priv->rcc + RCC_DDRITFCR, RCC_DDRITFCR_DDRCAPBRST);
	mmio_setbits_32(priv->rcc + RCC_DDRITFCR, RCC_DDRITFCR_DDRCAXIRST);
	mmio_setbits_32(priv->rcc + RCC_DDRITFCR, RCC_DDRITFCR_DDRCORERST);
	mmio_setbits_32(priv->rcc + RCC_DDRITFCR, RCC_DDRITFCR_DPHYAPBRST);
	mmio_setbits_32(priv->rcc + RCC_DDRITFCR, RCC_DDRITFCR_DPHYRST);
	mmio_setbits_32(priv->rcc + RCC_DDRITFCR, RCC_DDRITFCR_DPHYCTLRST);

	/* 1.2. start CLOCK */
	if (stm32mp1_ddr_clk_enable(priv, config->info.speed) != 0) {
		panic();
	}

	/* 1.3. deassert reset */
	/* De-assert PHY rstn and ctl_rstn via DPHYRST and DPHYCTLRST. */
	mmio_clrbits_32(priv->rcc + RCC_DDRITFCR, RCC_DDRITFCR_DPHYRST);
	mmio_clrbits_32(priv->rcc + RCC_DDRITFCR, RCC_DDRITFCR_DPHYCTLRST);
	/*
	 * De-assert presetn once the clocks are active
	 * and stable via DDRCAPBRST bit.
	 */
	mmio_clrbits_32(priv->rcc + RCC_DDRITFCR, RCC_DDRITFCR_DDRCAPBRST);

	/* 1.4. wait 128 cycles to permit initialization of end logic */
	local_delay_us(2);
	/* For PCLK = 133MHz => 1 us is enough, 2 to allow lower frequency */

	/* 1.5. initialize registers ddr_umctl2 */
	/* Stop uMCTL2 before PHY is ready */
	mmio_clrbits_32((uintptr_t)&priv->ctl->dfimisc,
			DDRCTRL_DFIMISC_DFI_INIT_COMPLETE_EN);
	VERBOSE("[0x%lx] dfimisc = 0x%x\n",
		(uintptr_t)&priv->ctl->dfimisc,
		mmio_read_32((uintptr_t)&priv->ctl->dfimisc));

	set_reg(priv, REG_REG, &config->c_reg);

	/* DDR3 = don't set DLLOFF for init mode */
	if ((config->c_reg.mstr &
	     (DDRCTRL_MSTR_DDR3 | DDRCTRL_MSTR_DLL_OFF_MODE))
	    == (DDRCTRL_MSTR_DDR3 | DDRCTRL_MSTR_DLL_OFF_MODE)) {
		VERBOSE("deactivate DLL OFF in mstr\n");
		mmio_clrbits_32((uintptr_t)&priv->ctl->mstr,
				DDRCTRL_MSTR_DLL_OFF_MODE);
		VERBOSE("[0x%lx] mstr = 0x%x\n",
			(uintptr_t)&priv->ctl->mstr,
			mmio_read_32((uintptr_t)&priv->ctl->mstr));
	}

	set_reg(priv, REG_TIMING, &config->c_timing);
	set_reg(priv, REG_MAP, &config->c_map);

	/* Skip CTRL init, SDRAM init is done by PHY PUBL */
	mmio_clrsetbits_32((uintptr_t)&priv->ctl->init0,
			   DDRCTRL_INIT0_SKIP_DRAM_INIT_MASK,
			   DDRCTRL_INIT0_SKIP_DRAM_INIT_NORMAL);
	VERBOSE("[0x%lx] init0 = 0x%x\n",
		(uintptr_t)&priv->ctl->init0,
		mmio_read_32((uintptr_t)&priv->ctl->init0));

	set_reg(priv, REG_PERF, &config->c_perf);

	/*  2. deassert reset signal core_ddrc_rstn, aresetn and presetn */
	mmio_clrbits_32(priv->rcc + RCC_DDRITFCR, RCC_DDRITFCR_DDRCORERST);
	mmio_clrbits_32(priv->rcc + RCC_DDRITFCR, RCC_DDRITFCR_DDRCAXIRST);
	mmio_clrbits_32(priv->rcc + RCC_DDRITFCR, RCC_DDRITFCR_DPHYAPBRST);

	/*
	 * 3. start PHY init by accessing relevant PUBL registers
	 *    (DXGCR, DCR, PTR*, MR*, DTPR*)
	 */
	set_reg(priv, REGPHY_REG, &config->p_reg);
	set_reg(priv, REGPHY_TIMING, &config->p_timing);
	set_reg(priv, REGPHY_CAL, &config->p_cal);

	/* DDR3 = don't set DLLOFF for init mode */
	if ((config->c_reg.mstr &
	     (DDRCTRL_MSTR_DDR3 | DDRCTRL_MSTR_DLL_OFF_MODE))
	    == (DDRCTRL_MSTR_DDR3 | DDRCTRL_MSTR_DLL_OFF_MODE)) {
		VERBOSE("deactivate DLL OFF in mr1\n");
		mmio_clrbits_32((uintptr_t)&priv->phy->mr1, BIT(0));
		VERBOSE("[0x%lx] mr1 = 0x%x\n",
			(uintptr_t)&priv->phy->mr1,
			mmio_read_32((uintptr_t)&priv->phy->mr1));
	}

	/*
	 *  4. Monitor PHY init status by polling PUBL register PGSR.IDONE
	 *     Perform DDR PHY DRAM initialization and Gate Training Evaluation
	 */
	stm32mp1_ddrphy_idone_wait(priv->phy);

	/*
	 *  5. Indicate to PUBL that controller performs SDRAM initialization
	 *     by setting PIR.INIT and PIR CTLDINIT and pool PGSR.IDONE
	 *     DRAM init is done by PHY, init0.skip_dram.init = 1
	 */

	pir = DDRPHYC_PIR_DLLSRST | DDRPHYC_PIR_DLLLOCK | DDRPHYC_PIR_ZCAL |
	      DDRPHYC_PIR_ITMSRST | DDRPHYC_PIR_DRAMINIT | DDRPHYC_PIR_ICPC;

	if ((config->c_reg.mstr & DDRCTRL_MSTR_DDR3) != 0U) {
		pir |= DDRPHYC_PIR_DRAMRST; /* Only for DDR3 */
	}

	stm32mp1_ddrphy_init(priv->phy, pir);

	/*
	 *  6. SET DFIMISC.dfi_init_complete_en to 1
	 *  Enable quasi-dynamic register programming.
	 */
	stm32mp1_start_sw_done(priv->ctl);

	mmio_setbits_32((uintptr_t)&priv->ctl->dfimisc,
			DDRCTRL_DFIMISC_DFI_INIT_COMPLETE_EN);
	VERBOSE("[0x%lx] dfimisc = 0x%x\n",
		(uintptr_t)&priv->ctl->dfimisc,
		mmio_read_32((uintptr_t)&priv->ctl->dfimisc));

	stm32mp1_wait_sw_done_ack(priv->ctl);

	/*
	 *  7. Wait for DWC_ddr_umctl2 to move to normal operation mode
	 *     by monitoring STAT.operating_mode signal
	 */

	/* Wait uMCTL2 ready */
	stm32mp1_wait_operating_mode(priv, DDRCTRL_STAT_OPERATING_MODE_NORMAL);

	/* Switch to DLL OFF mode */
	if ((config->c_reg.mstr & DDRCTRL_MSTR_DLL_OFF_MODE) != 0U) {
		stm32mp1_ddr3_dll_off(priv);
	}

	VERBOSE("DDR DQS training : ");

	/*
	 *  8. Disable Auto refresh and power down by setting
	 *    - RFSHCTL3.dis_au_refresh = 1
	 *    - PWRCTL.powerdown_en = 0
	 *    - DFIMISC.dfiinit_complete_en = 0
	 */
	stm32mp1_refresh_disable(priv->ctl);

	/*
	 *  9. Program PUBL PGCR to enable refresh during training
	 *     and rank to train
	 *     not done => keep the programed value in PGCR
	 */

	/*
	 * 10. configure PUBL PIR register to specify which training step
	 * to run
	 * Warning : RVTRN  is not supported by this PUBL
	 */
	stm32mp1_ddrphy_init(priv->phy, DDRPHYC_PIR_QSTRN);

	/* 11. monitor PUB PGSR.IDONE to poll cpmpletion of training sequence */
	stm32mp1_ddrphy_idone_wait(priv->phy);

	/*
	 * 12. set back registers in step 8 to the orginal values if desidered
	 */
	stm32mp1_refresh_restore(priv->ctl, config->c_reg.rfshctl3,
				 config->c_reg.pwrctl);

	/* Enable uMCTL2 AXI port 0 */
	mmio_setbits_32((uintptr_t)&priv->ctl->pctrl_0,
			DDRCTRL_PCTRL_N_PORT_EN);
	VERBOSE("[0x%lx] pctrl_0 = 0x%x\n",
		(uintptr_t)&priv->ctl->pctrl_0,
		mmio_read_32((uintptr_t)&priv->ctl->pctrl_0));

	/* Enable uMCTL2 AXI port 1 */
	mmio_setbits_32((uintptr_t)&priv->ctl->pctrl_1,
			DDRCTRL_PCTRL_N_PORT_EN);
	VERBOSE("[0x%lx] pctrl_1 = 0x%x\n",
		(uintptr_t)&priv->ctl->pctrl_1,
		mmio_read_32((uintptr_t)&priv->ctl->pctrl_1));
}

//#include "stm32mp15-mx_1G.dtsi"	// 64k*16
#include "stm32mp15-mx_2G.dtsi"	// 128k*16
//#include "stm32mp15-mx_4G.dtsi"		// 256k*16
//#include "stm32mp15-mx_8G.dtsi"	// 512k*16

// NT5CC128M16IP-DI BGA DDR3 NT5CC128M16IP DI
void stm32mp1_ddr_get_config(struct stm32mp1_ddr_config * cfg)
{
	cfg->info.speed = DDR_MEM_SPEED; // kHz
	cfg->info.size = DDR_MEM_SIZE;
	cfg->info.name = DDR_MEM_NAME;

	cfg->c_reg.mstr = 	 	DDR_MSTR;
	cfg->c_reg.mrctrl0 = 	DDR_MRCTRL0;
	cfg->c_reg.mrctrl1 = 	DDR_MRCTRL1;
	cfg->c_reg.derateen =  	DDR_DERATEEN;
	cfg->c_reg.derateint =	DDR_DERATEINT;
	cfg->c_reg.pwrctl = 	DDR_PWRCTL;
	cfg->c_reg.pwrtmg = 	DDR_PWRTMG;
	cfg->c_reg.hwlpctl = 	DDR_HWLPCTL;
	cfg->c_reg.rfshctl0 =  	DDR_RFSHCTL0;
	cfg->c_reg.rfshctl3 =  	DDR_RFSHCTL3;
	cfg->c_reg.crcparctl0  = DDR_CRCPARCTL0;
	cfg->c_reg.zqctl0 = 	 DDR_ZQCTL0;
	cfg->c_reg.dfitmg0 = 	 DDR_DFITMG0;
	cfg->c_reg.dfitmg1 = 	 DDR_DFITMG1;
	cfg->c_reg.dfilpcfg0 = 	DDR_DFILPCFG0;
	cfg->c_reg.dfiupd0 = 	 DDR_DFIUPD0;
	cfg->c_reg.dfiupd1 = 	 DDR_DFIUPD1;
	cfg->c_reg.dfiupd2 = 	 DDR_DFIUPD2;
	cfg->c_reg.dfiphymstr = DDR_DFIPHYMSTR;
	cfg->c_reg.odtmap = 	DDR_ODTMAP;
	cfg->c_reg.dbg0 = 	 	DDR_DBG0;
	cfg->c_reg.dbg1 = 	 	DDR_DBG1;
	cfg->c_reg.dbgcmd = 	DDR_DBGCMD;
	cfg->c_reg.poisoncfg = DDR_POISONCFG;
	cfg->c_reg.pccfg = 	 DDR_PCCFG;

	cfg->c_timing.rfshtmg = 	 DDR_RFSHTMG;
	cfg->c_timing.dramtmg0 =  DDR_DRAMTMG0;
	cfg->c_timing.dramtmg1 =  DDR_DRAMTMG1;
	cfg->c_timing.dramtmg2 =  DDR_DRAMTMG2;
	cfg->c_timing.dramtmg3 =  DDR_DRAMTMG3;
	cfg->c_timing.dramtmg4 =  DDR_DRAMTMG4;
	cfg->c_timing.dramtmg5 =  DDR_DRAMTMG5;
	cfg->c_timing.dramtmg6 =  DDR_DRAMTMG6;
	cfg->c_timing.dramtmg7 =  DDR_DRAMTMG7;
	cfg->c_timing.dramtmg8 =  DDR_DRAMTMG8;
	cfg->c_timing.dramtmg14 = DDR_DRAMTMG14;
	cfg->c_timing.odtcfg = 	 DDR_ODTCFG;

	cfg->c_perf.sched = 	 DDR_SCHED;
	cfg->c_perf.sched1 = 	 DDR_SCHED1;
	cfg->c_perf.perfhpr1 =  DDR_PERFHPR1;
	cfg->c_perf.perflpr1 =  DDR_PERFLPR1;
	cfg->c_perf.perfwr1 = 	 DDR_PERFWR1;
	cfg->c_perf.pcfgr_0 = 	 DDR_PCFGR_0;
	cfg->c_perf.pcfgw_0 = 	 DDR_PCFGW_0;
	cfg->c_perf.pcfgqos0_0  = DDR_PCFGQOS0_0;
	cfg->c_perf.pcfgqos1_0  = DDR_PCFGQOS1_0;
	cfg->c_perf.pcfgwqos0_0 = DDR_PCFGWQOS0_0;
	cfg->c_perf.pcfgwqos1_0 = DDR_PCFGWQOS1_0;
	cfg->c_perf.pcfgr_1 = 	 DDR_PCFGR_1;
	cfg->c_perf.pcfgw_1 = 	 DDR_PCFGW_1;
	cfg->c_perf.pcfgqos0_1  = DDR_PCFGQOS0_1;
	cfg->c_perf.pcfgqos1_1  = DDR_PCFGQOS1_1;
	cfg->c_perf.pcfgwqos0_1 = DDR_PCFGWQOS0_1;
	cfg->c_perf.pcfgwqos1_1 = DDR_PCFGWQOS1_1;

	cfg->c_map.addrmap1 =  DDR_ADDRMAP1;
	cfg->c_map.addrmap2 =  DDR_ADDRMAP2;
	cfg->c_map.addrmap3 =  DDR_ADDRMAP3;
	cfg->c_map.addrmap4 =  DDR_ADDRMAP4;
	cfg->c_map.addrmap5 =  DDR_ADDRMAP5;
	cfg->c_map.addrmap6 =  DDR_ADDRMAP6;
	cfg->c_map.addrmap9 =  DDR_ADDRMAP9;
	cfg->c_map.addrmap10 = DDR_ADDRMAP10;
	cfg->c_map.addrmap11 = DDR_ADDRMAP11;

	cfg->p_reg.pgcr = 	 DDR_PGCR;
	cfg->p_reg.aciocr = 	 DDR_ACIOCR;
	cfg->p_reg.dxccr = 	 DDR_DXCCR;
	cfg->p_reg.dsgcr = 	 DDR_DSGCR;
	cfg->p_reg.dcr = 		 DDR_DCR;
	cfg->p_reg.odtcr = 	 DDR_ODTCR;

	cfg->p_timing.ptr0 = 	 DDR_PTR0;
	cfg->p_timing.ptr1 = 	 DDR_PTR1;
	cfg->p_timing.ptr2 = 	 DDR_PTR2;
	cfg->p_timing.dtpr0 = 	 DDR_DTPR0;
	cfg->p_timing.dtpr1 = 	 DDR_DTPR1;
	cfg->p_timing.dtpr2 = 	 DDR_DTPR2;
	cfg->p_timing.mr0 = 		 DDR_MR0;
	cfg->p_timing.mr1 = 		 DDR_MR1;
	cfg->p_timing.mr2 = 		 DDR_MR2;
	cfg->p_timing.mr3 = 		 DDR_MR3;

	cfg->p_reg.zq0cr1 = 	 DDR_ZQ0CR1;
	cfg->p_reg.dx0gcr = 	 DDR_DX0GCR;
	cfg->p_cal.dx0dllcr =  DDR_DX0DLLCR;
	cfg->p_cal.dx0dqtr = 	 DDR_DX0DQTR;
	cfg->p_cal.dx0dqstr =  DDR_DX0DQSTR;
	cfg->p_reg.dx1gcr = 	 DDR_DX1GCR;
	cfg->p_cal.dx1dllcr =  DDR_DX1DLLCR;
	cfg->p_cal.dx1dqtr = 	 DDR_DX1DQTR;
	cfg->p_cal.dx1dqstr =  	DDR_DX1DQSTR;
	cfg->p_reg.dx2gcr = 	DDR_DX2GCR;
	cfg->p_cal.dx2dllcr =  	DDR_DX2DLLCR;
	cfg->p_cal.dx2dqtr = 	DDR_DX2DQTR;
	cfg->p_cal.dx2dqstr =  	DDR_DX2DQSTR;
	cfg->p_reg.dx3gcr = 	DDR_DX3GCR;
	cfg->p_cal.dx3dllcr =  	DDR_DX3DLLCR;
	cfg->p_cal.dx3dqtr = 	DDR_DX3DQTR;
	cfg->p_cal.dx3dqstr =  	DDR_DX3DQSTR;
}

#define DDR_PATTERN	0xAAAAAAAAU
#define DDR_ANTIPATTERN	0x55555555U


/*******************************************************************************
 * This function tests the DDR data bus wiring.
 * This is inspired from the Data Bus Test algorithm written by Michael Barr
 * in "Programming Embedded Systems in C and C++" book.
 * resources.oreilly.com/examples/9781565923546/blob/master/Chapter6/
 * File: memtest.c - This source code belongs to Public Domain.
 * Returns 0 if success, and address value else.
 ******************************************************************************/
static uint32_t ddr_test_data_bus(void)
{
	uint32_t pattern;

	for (pattern = 1U; pattern != 0U; pattern <<= 1) {
		mmio_write_32(STM32MP_DDR_BASE, pattern);

		if (mmio_read_32(STM32MP_DDR_BASE) != pattern) {
			return (uint32_t)STM32MP_DDR_BASE;
		}
	}

	return 0;
}

/*******************************************************************************
 * This function tests the DDR address bus wiring.
 * This is inspired from the Data Bus Test algorithm written by Michael Barr
 * in "Programming Embedded Systems in C and C++" book.
 * resources.oreilly.com/examples/9781565923546/blob/master/Chapter6/
 * File: memtest.c - This source code belongs to Public Domain.
 * Returns 0 if success, and address value else.
 ******************************************************************************/
static uint32_t ddr_test_addr_bus(void)
{
	uint64_t addressmask = (DDR_MEM_SIZE - 1U);
	uint64_t offset;
	uint64_t testoffset = 0;

	/* Write the default pattern at each of the power-of-two offsets. */
	for (offset = sizeof(uint32_t); (offset & addressmask) != 0U;
	     offset <<= 1) {
		mmio_write_32(STM32MP_DDR_BASE + (uint32_t)offset,
			      DDR_PATTERN);
	}

	/* Check for address bits stuck high. */
	mmio_write_32(STM32MP_DDR_BASE + (uint32_t)testoffset,
		      DDR_ANTIPATTERN);

	for (offset = sizeof(uint32_t); (offset & addressmask) != 0U;
	     offset <<= 1) {
		if (mmio_read_32(STM32MP_DDR_BASE + (uint32_t)offset) !=
		    DDR_PATTERN) {
			return (uint32_t)(STM32MP_DDR_BASE + offset);
		}
	}

	mmio_write_32(STM32MP_DDR_BASE + (uint32_t)testoffset, DDR_PATTERN);

	/* Check for address bits stuck low or shorted. */
	for (testoffset = sizeof(uint32_t); (testoffset & addressmask) != 0U;
	     testoffset <<= 1) {
		mmio_write_32(STM32MP_DDR_BASE + (uint32_t)testoffset,
			      DDR_ANTIPATTERN);

		if (mmio_read_32(STM32MP_DDR_BASE) != DDR_PATTERN) {
			return STM32MP_DDR_BASE;
		}

		for (offset = sizeof(uint32_t); (offset & addressmask) != 0U;
		     offset <<= 1) {
			if ((mmio_read_32(STM32MP_DDR_BASE +
					  (uint32_t)offset) != DDR_PATTERN) &&
			    (offset != testoffset)) {
				return (uint32_t)(STM32MP_DDR_BASE + offset);
			}
		}

		mmio_write_32(STM32MP_DDR_BASE + (uint32_t)testoffset,
			      DDR_PATTERN);
	}

	return 0;
}

/*******************************************************************************
 * This function checks the DDR size. It has to be run with Data Cache off.
 * This test is run before data have been put in DDR, and is only done for
 * cold boot. The DDR data can then be overwritten, and it is not useful to
 * restore its content.
 * Returns DDR computed size.
 ******************************************************************************/
static uint32_t ddr_check_size(void)
{
	uint32_t offset = sizeof(uint32_t);

	mmio_write_32(STM32MP_DDR_BASE, DDR_PATTERN);

	while (offset < STM32MP_DDR_MAX_SIZE) {
		mmio_write_32(STM32MP_DDR_BASE + offset, DDR_ANTIPATTERN);
		__DSB();

		if (mmio_read_32(STM32MP_DDR_BASE) != DDR_PATTERN) {
			break;
		}

		offset <<= 1;
	}


	return offset;
}

static unsigned long rand_val = 123456UL;

static void local_random_init(void)
{
	rand_val = 123456UL;
}

static unsigned long local_random(void)
{


	if (rand_val & 0x80000000UL)
		rand_val = (rand_val << 1);
	else
		rand_val = (rand_val << 1) ^ 0x201051UL;

	return (rand_val);
}
static int oldstate;

static void ddr_check_progress(uint32_t addr)
{
	int state = (addr >> 22) & 0x01;
#if defined (BOARD_BLINK_SETSTATE)
	if (oldstate != state)
	{
		oldstate = state;
		BOARD_BLINK_SETSTATE(state);
	}
#endif /* defined (BOARD_BLINK_SETSTATE) */
}

static uint32_t ddr_check_rand(unsigned long sizeee)
{
	typedef uint8_t test_t;
	//uint64_t addressmask = (DDR_MEM_SIZE - 1U);
	const uint32_t sizeN = sizeee / sizeof (test_t);
	volatile test_t * const p = (volatile test_t *) STM32MP_DDR_BASE;
	uint32_t i;

	// fill
	//local_random_init();
	uint32_t seed = rand_val;

	for (i = 0; i < sizeN; ++ i)
	{
		ddr_check_progress(i);
		p [i] = local_random();
	}
	// compare
	//local_random_init();
	rand_val = seed;
	//p = (volatile uint16_t *) STM32MP_DDR_BASE;
	for (i = 0; i < sizeN; ++ i)
	{
		ddr_check_progress(i);
		if (p [i] != (test_t) local_random())
			return i * sizeof (test_t);
	}

	return sizeee;	// OK
}
// NT5CC128M16IP-DI BGA DDR3 NT5CC128M16IP DI
void FLASHMEMINITFUNC arm_hardware_sdram_initialize(void)
{
	PRINTF("arm_hardware_sdram_initialize start\n");

	if (1)
	{
		// TrustZone address space controller for DDR (TZC)

		// TZC AXI port 1 clocks enable
		RCC->MP_APB5ENSETR = RCC_MC_APB5ENSETR_TZC1EN;
		(void) RCC->MP_APB5ENSETR;
		RCC->MP_APB5LPENSETR = RCC_MC_APB5LPENSETR_TZC1LPEN;
		(void) RCC->MP_APB5LPENSETR;

		// TZC AXI port 2 clocks enable
		RCC->MP_APB5ENSETR = RCC_MC_APB5ENSETR_TZC2EN;
		(void) RCC->MP_APB5ENSETR;
		RCC->MP_APB5LPENSETR = RCC_MC_APB5LPENSETR_TZC2LPEN;
		(void) RCC->MP_APB5LPENSETR;

		const uint_fast8_t lastfilrer = (TZC->BUILD_CONFIG >> 24) & 0x03;
		const uint_fast32_t mask = (1uL << (lastfilrer + 1)) - 1;
		TZC->GATE_KEEPER |= mask;	// Gate open request
		(void) TZC->GATE_KEEPER;
		while (((TZC->GATE_KEEPER >> 16) & mask) != mask)
			;
		TZC->REG_ATTRIBUTESO |= 0xC0000000;	// All (read and write) permitted
		(void) TZC->REG_ATTRIBUTESO;
		TZC->REG_ID_ACCESSO = 0xFFFFFFFF; // permits read and write non-secure to the region for all NSAIDs
		(void) TZC->REG_ID_ACCESSO;
	}
	if (1)
	{
        // 0x01001F08
        //PRINTF("TZC->BUILD_CONFIG=%08lX\n", TZC->BUILD_CONFIG);
        //PRINTF("TZC->ACTION=%08lX\n", TZC->ACTION);

        const uint_fast8_t lastregion = TZC->BUILD_CONFIG & 0x0f;
        uint_fast8_t i;
        for (i = 1; i <= lastregion; ++ i)
        {
            volatile uint32_t * const REG_ATTRIBUTESx = & TZC->REG_ATTRIBUTESO + (i * 8);
            //volatile uint32_t * const REG_BASE_LOWx = & TZC->REG_BASE_LOWO + (i * 8);
            //volatile uint32_t * const REG_BASE_HIGHx = & TZC->REG_BASE_HIGHO + (i * 8);
            //volatile uint32_t * const REG_TOP_LOWx = & TZC->REG_TOP_LOWO + (i * 8);
            //volatile uint32_t * const REG_TOP_HIGHx = & TZC->REG_TOP_HIGHO + (i * 8);

            //PRINTF("TZC->REG_BASE_LOW%d=%08lX ", i, * REG_BASE_LOWx);
            //PRINTF("REG_BASE_HIGH%d=%08lX ", i, * REG_BASE_HIGHx);
            //PRINTF("REG_TOP_LOW%d=%08lX ", i, * REG_TOP_LOWx);
            //PRINTF("REG_TOP_HIGH%d=%08lX ", i, * REG_TOP_HIGHx);
            //PRINTF("REG_ATTRIBUTES%d=%08lX\n", i, * REG_ATTRIBUTESx);

             * REG_ATTRIBUTESx &= ~ 0x03uL;

            //PRINTF("TZC->REG_ATTRIBUTES%d=%08lX\n", i, * REG_ATTRIBUTESx);
        }
	}
	if (0)
	{
		/* SYSCFG clock enable */
		RCC->MP_APB3ENSETR = RCC_MC_APB3ENSETR_SYSCFGEN;
		(void) RCC->MP_APB3ENSETR;
		RCC->MP_APB3LPENSETR = RCC_MC_APB3LPENSETR_SYSCFGLPEN;
		(void) RCC->MP_APB3LPENSETR;
		/*
		 * Interconnect update : select master using the port 1.
		 * MCU interconnect (OTG_HS) = AXI_M0.
		 */
//		SYSCFG->ICNR |= SYSCFG_ICNR_AXI_M0;
//		(void) SYSCFG->ICNR;
	}

#if WITHSDRAM_PMC1
	initialize_pmic();
#endif /* WITHSDRAM_PMC1 */

	struct ddr_info ddr_priv_data;
	struct ddr_info * const priv = &ddr_priv_data;
	int ret;
	struct stm32mp1_ddr_config config;
	int node, len;
	uint32_t uret, idx;
	void *fdt;

#define PARAM(x, y)							\
	{								\
		.name = x,						\
		.offset = offsetof(struct stm32mp1_ddr_config, y),	\
		.size = sizeof(config.y) / sizeof(uint32_t)		\
	}

#define CTL_PARAM(x) PARAM("st,ctl-"#x, c_##x)
#define PHY_PARAM(x) PARAM("st,phy-"#x, p_##x)

	const struct {
		const char *name; /* Name in DT */
		const uint32_t offset; /* Offset in config struct */
		const uint32_t size;   /* Size of parameters */
	} param[] = {
		CTL_PARAM(reg),
		CTL_PARAM(timing),
		CTL_PARAM(map),
		CTL_PARAM(perf),
		PHY_PARAM(reg),
		PHY_PARAM(timing),
		PHY_PARAM(cal)
	};

	priv->ctl = (struct stm32mp1_ddrctl *) DDRC_BASE;
	priv->phy = (struct stm32mp1_ddrphy *) DDRPHYC_BASE;
	priv->pwr = PWR_BASE;
	priv->rcc = RCC_BASE;
	priv->info.base = STM32MP_DDR_BASE;
	priv->info.size = DDR_MEM_SIZE;

	stm32mp1_ddr_get_config(& config);

	/* Software Self-Refresh mode (SSR) during DDR initilialization */
	mmio_clrsetbits_32(RCC_BASE + RCC_DDRITFCR,
			   RCC_DDRITFCR_DDRCKMOD_Msk,
			   (0) << RCC_DDRITFCR_DDRCKMOD_Pos); // RCC_DDRITFCR_DDRCKMOD_SSR

	/* Disable axidcg clock gating during init */
	mmio_clrbits_32(priv->rcc + RCC_DDRITFCR, RCC_DDRITFCR_AXIDCGEN);

	stm32mp1_ddr_init(priv, & config);

	/* Enable axidcg clock gating */
	mmio_setbits_32(RCC_BASE + RCC_DDRITFCR, RCC_DDRITFCR_AXIDCGEN);

	// инициализация выполняетмя еще до включения MMU
	//__set_SCTLR(__get_SCTLR() & ~ SCTLR_C_Msk);

	uret = ddr_test_data_bus();
	if (uret != 0U) {
		ERROR("DDR data bus test: can't access memory @ 0x%x\n",
		      uret);
		//panic();
	}
	uret = ddr_test_addr_bus();
	if (uret != 0U) {
		ERROR("DDR addr bus test: can't access memory @ 0x%x\n",
		      uret);
		//panic();
	}

	uret = ddr_check_size();
	if (uret < config.info.size) {
		ERROR("DDR size: 0x%x does not match DT config: 0x%x\n",
		      uret, config.info.size);
		//panic();
	}
	INFO("Memory size = 0x%x (%d MB)\n", uret, uret / (1024U * 1024U));

#if 0
	// Бесконечный тест памяти.
	PRINTF("DDR memory tests:\n");
#if defined (BOARD_BLINK_INITIALIZE)
	BOARD_BLINK_INITIALIZE();
#endif /* defined (BOARD_BLINK_INITIALIZE) */

	for (;;)
	{
		PRINTF("rand_val = %08lX ", (unsigned long) rand_val);

		uret = ddr_test_data_bus();
		if (uret != 0U) {
			ERROR("DDR data bus test: can't access memory @ 0x%x\n",
			      uret);
			panic();
		}
		uret = ddr_test_addr_bus();
		if (uret != 0U) {
			ERROR("DDR addr bus test: can't access memory @ 0x%x\n",
			      uret);
			panic();
		}
		uret = ddr_check_size();
		if (uret != config.info.size) {
			ERROR("DDR size: 0x%x does not match DT config: 0x%x\n",
			      uret, config.info.size);
			panic();
		}
		uret = ddr_check_rand(config.info.size / 32);
		if (uret != (config.info.size / 32)) {
			ERROR("DDR random test: 0x%x does not match DT config: 0x%x\n",
			      uret, config.info.size / 32);
			panic();
		}
		PRINTF(".");
		//char c;
		//if (dbg_getchar(& c)/* && c == 0x1b*/)
		//	break;
	}
#endif
	//__set_SCTLR(__get_SCTLR() | SCTLR_C_Msk);
	//PRINTF("TZC->INT_STATUS=%08lX\n", TZC->INT_STATUS);

	/* Software Self-Refresh mode (SSR) during DDR initilialization */
	mmio_clrsetbits_32(RCC_BASE + RCC_DDRITFCR,
			   RCC_DDRITFCR_DDRCKMOD_Msk,
			   (0) << RCC_DDRITFCR_DDRCKMOD_Pos);

	PRINTF("arm_hardware_sdram_initialize done\n");
}

#endif

#endif /* WITHSDRAMHW */
