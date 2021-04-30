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
	#define BUFFER_SIZE 		0x100uL

  	uint_fast32_t addr = SDRAM_ADDR;

	PRINTF("sdram testing...\n");

	for (; addr < SDRAM_ADDR + SDRAM_SIZE - BUFFER_SIZE; addr += BUFFER_SIZE)
	{
		PRINTF("addr %lx\n", addr);
		sdram_test_pattern(addr, BUFFER_SIZE, 0x5555);
//		sdram_test_pattern(addr, BUFFER_SIZE, 0xCCCC);
//		sdram_test_increment(addr, BUFFER_SIZE, 0x7800);
//		sdram_test_random(addr, BUFFER_SIZE);
	}

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
/*
#define DDRCTRL_MSTR_DDR3			BIT(0)
#define DDRCTRL_MSTR_LPDDR2			BIT(2)
#define DDRCTRL_MSTR_LPDDR3			BIT(3)
#define DDRCTRL_MSTR_DATA_BUS_WIDTH_MASK	GENMASK(13, 12)
#define DDRCTRL_MSTR_DATA_BUS_WIDTH_FULL	0
#define DDRCTRL_MSTR_DATA_BUS_WIDTH_HALF	BIT(12)
#define DDRCTRL_MSTR_DATA_BUS_WIDTH_QUARTER	BIT(13)
#define DDRCTRL_MSTR_DLL_OFF_MODE		BIT(15)
*/
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
//#define DDRCTRL_MRCTRL0_MR_WR			BIT(31)

//#define DDRCTRL_MRSTAT_MR_WR_BUSY		BIT(0)

//#define DDRCTRL_PWRCTL_SELFREF_EN		BIT(0)
//#define DDRCTRL_PWRCTL_POWERDOWN_EN		BIT(1)
//#define DDRCTRL_PWRCTL_EN_DFI_DRAM_CLK_DISABLE	BIT(3)
//#define DDRCTRL_PWRCTL_SELFREF_SW		BIT(5)

#define DDRCTRL_PWRTMG_SELFREF_TO_X32_MASK	GENMASK(19, 12)
//#define DDRCTRL_PWRTMG_SELFREF_TO_X32_0		BIT(16)

//#define DDRCTRL_RFSHCTL3_DIS_AUTO_REFRESH	BIT(0)

//#define DDRCTRL_HWLPCTL_HW_LP_EN		BIT(0)

#define DDRCTRL_RFSHTMG_T_RFC_NOM_X1_X32_MASK	GENMASK(27, 16)
#define DDRCTRL_RFSHTMG_T_RFC_NOM_X1_X32_SHIFT	16

#define DDRCTRL_INIT0_SKIP_DRAM_INIT_MASK	GENMASK(31, 30)
#define DDRCTRL_INIT0_SKIP_DRAM_INIT_NORMAL	BIT(30)

//#define DDRCTRL_DFIMISC_DFI_INIT_COMPLETE_EN	BIT(0)

//#define DDRCTRL_DBG1_DIS_HIF			BIT(1)

//#define DDRCTRL_DBGCAM_WR_DATA_PIPELINE_EMPTY	BIT(29)
//#define DDRCTRL_DBGCAM_RD_DATA_PIPELINE_EMPTY	BIT(28)
//#define DDRCTRL_DBGCAM_DBG_WR_Q_EMPTY		BIT(26)
//#define DDRCTRL_DBGCAM_DBG_LPR_Q_DEPTH		GENMASK(12, 8)
//#define DDRCTRL_DBGCAM_DBG_HPR_Q_DEPTH		GENMASK(4, 0)
#define DDRCTRL_DBGCAM_DATA_PIPELINE_EMPTY \
		(DDRCTRL_DBGCAM_WR_DATA_PIPELINE_EMPTY | \
		 DDRCTRL_DBGCAM_RD_DATA_PIPELINE_EMPTY)
#define DDRCTRL_DBGCAM_DBG_Q_DEPTH \
		(DDRCTRL_DBGCAM_DBG_WR_Q_EMPTY | \
		 DDRCTRL_DBGCAM_DBG_LPR_Q_DEPTH | \
		 DDRCTRL_DBGCAM_DBG_HPR_Q_DEPTH)

//#define DDRCTRL_DBGCMD_RANK0_REFRESH		BIT(0)

//#define DDRCTRL_DBGSTAT_RANK0_REFRESH_BUSY	BIT(0)

//#define DDRCTRL_SWCTL_SW_DONE			BIT(0)

//#define DDRCTRL_SWSTAT_SW_DONE_ACK		BIT(0)

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
/*
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
#define DDRPHYC_DXNDLLCR_SDPHASE_MASK		GENMASK(17, 14)
#define DDRPHYC_DXNDLLCR_SDPHASE_SHIFT		14
*/
#define DDRPHYC_DXNDLLCR_DLLDIS			BIT(31)

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
//#define VERBOSE(...) // PRINTF
#define ERROR PRINTF
#define INFO PRINTF

static void panic(void)
{
	PRINTF("sdram: panic.\n");
	return;
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
} KEYWORDPACKED;

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
} KEYWORDPACKED;

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

void stpmic1_dump_regulators(void)
{
	uint32_t i;

	for (i = 0U; i < MAX_REGUL; i++) {
		const char *name __unused = regulators_table[i].dt_node_name;

		PRINTF("PMIC regul %s: %sable, %d mV\n",
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

	i2c_initialize();

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

	PRINTF("PMIC version = 0x%02lx\n", pmic_version);
	//stpmic1_dump_regulators();

#if defined(IMAGE_BL2)
	if (dt_pmic_configure_boot_on_regulators() != 0) {
		panic();
	};
#endif
}


static int pmic_ddr_power_init(enum ddr_type ddr_type)
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


// LDO1 = 1.8 Volt
// LDO6 = 1.2 Volt
// LDO2 = 3.3 Volt

int toshiba_ddr_power_init(void)
{
	{
		uint8_t read_val;
		int status;
		// LDO1 = 1.8 Volt
		status = stpmic1_register_read(LDO1_CONTROL_REG, &read_val);
		if (status != 0) {
			return status;
		}

		read_val &= ~ 0x01;	// enable

		status = stpmic1_register_write(LDO1_CONTROL_REG, read_val);
		if (status != 0) {
			return status;
		}

		status = stpmic1_regulator_voltage_set("ldo1", 1800);
		if (status != 0) {
			return status;
		}

		status = stpmic1_regulator_enable("ldo1");
		if (status != 0) {
			return status;
		}

		local_delay_ms(STPMIC1_DEFAULT_START_UP_DELAY_MS);
	}

	{
		uint8_t read_val;
		int status;
		// LDO6 = 1.2 Volt (toshiba supplay)
		status = stpmic1_register_read(LDO6_CONTROL_REG, &read_val);
		if (status != 0) {
			return status;
		}

		read_val &= ~ 0x01;	// enable

		status = stpmic1_register_write(LDO6_CONTROL_REG, read_val);
		if (status != 0) {
			return status;
		}

		status = stpmic1_regulator_voltage_set("ldo6", 1200);
		if (status != 0) {
			return status;
		}

		status = stpmic1_regulator_enable("ldo6");
		if (status != 0) {
			return status;
		}

		local_delay_ms(STPMIC1_DEFAULT_START_UP_DELAY_MS);
	}

	{
		uint8_t read_val;
		int status;
		// LDO2 = 3.3 Volt
		status = stpmic1_register_read(LDO2_CONTROL_REG, &read_val);
		if (status != 0) {
			return status;
		}

		read_val &= ~ 0x01;	// enable

		status = stpmic1_register_write(LDO2_CONTROL_REG, read_val);
		if (status != 0) {
			return status;
		}

		status = stpmic1_regulator_voltage_set("ldo2", 3300);
		if (status != 0) {
			return status;
		}

		status = stpmic1_regulator_enable("ldo2");
		if (status != 0) {
			return status;
		}

		local_delay_ms(STPMIC1_DEFAULT_START_UP_DELAY_MS);
	}
	{
		uint8_t read_val;
		int status;
		status = stpmic1_regulator_disable("ldo5");
		if (status != 0) {
			return status;
		}
	}

	return 0;
}

#endif /* WITHSDRAM_PMC1 */

static int board_ddr_power_init(enum ddr_type ddr_type)
{
#if WITHSDRAM_PMC1
	if (pmic_ddr_power_init(ddr_type))
		PRINTF("ddr power init failure\n");
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

#if WITHSDRAM_PMC1
	//stpmic1_dump_regulators();
#endif /* WITHSDRAM_PMC1 */

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

#if DDR_FREQ <= 300000000uL
	// less or equal 300 MHz
	#if WITHHWDDR3_1GBIT
		#include "stm32mp15-mx_300MHz_1G.dtsi"	// 64k*16
	#elif WITHHWDDR3_2GBIT
		#include "stm32mp15-mx_300MHz_2G.dtsi"	// 128k*16
	#elif WITHHWDDR3_4GBIT
		#include "stm32mp15-mx_300MHz_4G.dtsi"	// 256k*16
	#elif WITHHWDDR3_8GBIT
		#include "stm32mp15-mx_300MHz_8G.dtsi"	// 512k*16
	#else
		#error Please select DDR3 chip size
	#endif
#else
	#if WITHHWDDR3_1GBIT
		#include "stm32mp15-mx_1G.dtsi"	// 64k*16
	#elif WITHHWDDR3_2GBIT
		#include "stm32mp15-mx_2G.dtsi"	// 128k*16
	#elif WITHHWDDR3_4GBIT
		#include "stm32mp15-mx_4G.dtsi"		// 256k*16
	#elif WITHHWDDR3_8GBIT
		#include "stm32mp15-mx_8G.dtsi"	// 512k*16
	#else
		#error Please select DDR3 chip size
	#endif
#endif

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
	typedef uint16_t test_t;
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
		RCC->MP_APB5ENSETR = RCC_MP_APB5ENSETR_TZC1EN;
		(void) RCC->MP_APB5ENSETR;
		RCC->MP_APB5LPENSETR = RCC_MP_APB5LPENSETR_TZC1LPEN;
		(void) RCC->MP_APB5LPENSETR;

		// TZC AXI port 2 clocks enable
		RCC->MP_APB5ENSETR = RCC_MP_APB5ENSETR_TZC2EN;
		(void) RCC->MP_APB5ENSETR;
		RCC->MP_APB5LPENSETR = RCC_MP_APB5LPENSETR_TZC2LPEN;
		(void) RCC->MP_APB5LPENSETR;

        // 0x01001F08
		// no_of_filters=1 - Two filter units.
		// address_width=31 - 32 bits.
		// no_of_regions=8 - Nine regions.
        //PRINTF("TZC->BUILD_CONFIG=%08lX\n", TZC->BUILD_CONFIG);
        //PRINTF("TZC->ACTION=%08lX\n", TZC->ACTION);
        //PRINTF("TZC->GATE_KEEPER=%08lX\n", TZC->GATE_KEEPER);

		TZC->ACTION = 0x00;
		(void) TZC->ACTION;
		const uint_fast8_t lastfilter = (TZC->BUILD_CONFIG >> 24) & 0x03;
		const uint_fast32_t mask = (1uL << (lastfilter + 1)) - 1;
		TZC->GATE_KEEPER = mask;	// Gate open request
		(void) TZC->GATE_KEEPER;
		// Check open status
		while (((TZC->GATE_KEEPER >> 16) & mask) != mask)
			;
		TZC->REG_ATTRIBUTESO |= 0xC0000000;	// All (read and write) permitted
		(void) TZC->REG_ATTRIBUTESO;
		TZC->REG_ID_ACCESSO = 0xFFFFFFFF; // NSAID_WR_EN[15:0], NSAID_RD_EN[15:0] - permits read and write non-secure to the region for all NSAIDs
		(void) TZC->REG_ID_ACCESSO;
        //PRINTF("TZC->REG_ID_ACCESSO=%08lX\n", TZC->REG_ID_ACCESSO);
        //PRINTF("TZC->REG_ATTRIBUTESO=%08lX\n", TZC->REG_ATTRIBUTESO);
	}
	if (1)
	{

        const uint_fast8_t lastregion = TZC->BUILD_CONFIG & 0x1f;
        uint_fast8_t i;
        for (i = 1; i <= lastregion; ++ i)
        {
            volatile uint32_t * const REG_ATTRIBUTESx = & TZC->REG_ATTRIBUTESO + (i * 8);
            volatile uint32_t * const REG_ID_ACCESSx = & TZC->REG_ID_ACCESSO + (i * 8);
            volatile uint32_t * const REG_BASE_LOWx = & TZC->REG_BASE_LOWO + (i * 8);
            volatile uint32_t * const REG_BASE_HIGHx = & TZC->REG_BASE_HIGHO + (i * 8);
            volatile uint32_t * const REG_TOP_LOWx = & TZC->REG_TOP_LOWO + (i * 8);
            volatile uint32_t * const REG_TOP_HIGHx = & TZC->REG_TOP_HIGHO + (i * 8);

            //PRINTF("TZC->REG_BASE_LOW%d=%08lX ", i, * REG_BASE_LOWx);
            //PRINTF("REG_BASE_HIGH%d=%08lX ", i, * REG_BASE_HIGHx);
            //PRINTF("REG_TOP_LOW%d=%08lX ", i, * REG_TOP_LOWx);
            //PRINTF("REG_TOP_HIGH%d=%08lX ", i, * REG_TOP_HIGHx);
            //PRINTF("REG_ATTRIBUTES%d=%08lX\n", i, * REG_ATTRIBUTESx);
            //PRINTF("REG_ID_ACCESS%d=%08lX\n", i, * REG_ID_ACCESSx);

             * REG_ATTRIBUTESx &= ~ 0x03uL;

            //PRINTF("TZC->REG_ATTRIBUTES%d=%08lX\n", i, * REG_ATTRIBUTESx);
        }
	}
	if (0)
	{
		/* SYSCFG clock enable */
		RCC->MP_APB3ENSETR = RCC_MP_APB3ENSETR_SYSCFGEN;
		(void) RCC->MP_APB3ENSETR;
		RCC->MP_APB3LPENSETR = RCC_MP_APB3LPENSETR_SYSCFGLPEN;
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

	static const struct {
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

	priv->ctl = (struct stm32mp1_ddrctl *) DDRCTRL_BASE;
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

	//for (;;)
	{
		PRINTF("rand_val = %08lX ", (unsigned long) rand_val);

		uret = ddr_test_data_bus();
		if (uret != 0U) {
			ERROR("DDR data bus test: can't access memory @ 0x%08x\n",
			      uret);
			panic();
		}
		uret = ddr_test_addr_bus();
		if (uret != 0U) {
			ERROR("DDR addr bus test: can't access memory @ 0x%08x\n",
			      uret);
			panic();
		}
		uret = ddr_check_size();
		if (uret != config.info.size) {
			ERROR("DDR size: 0x%x does not match DT config: 0x%08x\n",
			      uret, config.info.size);
			panic();
		}
		int partfortest = 128;
		uret = ddr_check_rand(config.info.size / partfortest);
		if (uret != (config.info.size / partfortest)) {
			ERROR("DDR random test: 0x%08x does not match DT config: 0x%08x\n",
			      uret, config.info.size / partfortest);
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

#if WITHDEBUG
	{
		unsigned size4 = config.info.size / 4;
		volatile uint32_t * base4 = (volatile uint32_t *) STM32MP_DDR_BASE;
		while (size4 --)
			* base4 ++ = 0xABBA1998uL;
	}
#endif /* WITHDEBUG */

	/* Software Self-Refresh mode (SSR) during DDR initilialization */
	mmio_clrsetbits_32(RCC_BASE + RCC_DDRITFCR,
			   RCC_DDRITFCR_DDRCKMOD_Msk,
			   (0) << RCC_DDRITFCR_DDRCKMOD_Pos);

	PRINTF("arm_hardware_sdram_initialize done\n");


}

#elif CPUSTYLE_XC7Z

/* Opcode exit is 0 all the time */
#define OPCODE_EXIT       0U
#define OPCODE_CLEAR      1U
#define OPCODE_WRITE      2U
#define OPCODE_MASKWRITE  3U
#define OPCODE_MASKPOLL   4U
#define OPCODE_MASKDELAY  5U
#define NEW_PS7_ERR_CODE 1

/* Encode number of arguments in last nibble */
#define EMIT_EXIT()                   ( (OPCODE_EXIT      << 4 ) | 0 )
#define EMIT_CLEAR(addr)              ( (OPCODE_CLEAR     << 4 ) | 1 ) , addr
#define EMIT_WRITE(addr,val)          ( (OPCODE_WRITE     << 4 ) | 2 ) , addr, val
#define EMIT_MASKWRITE(addr,mask,val) ( (OPCODE_MASKWRITE << 4 ) | 3 ) , addr, mask, val
#define EMIT_MASKPOLL(addr,mask)      ( (OPCODE_MASKPOLL  << 4 ) | 2 ) , addr, mask
#define EMIT_MASKDELAY(addr,mask)      ( (OPCODE_MASKDELAY << 4 ) | 2 ) , addr, mask

/* Returns codes of ps7_init* */
#define PS7_INIT_SUCCESS		(0)
#define PS7_INIT_CORRUPT		(1)
#define PS7_INIT_TIMEOUT		(2)
#define PS7_POLL_FAILED_DDR_INIT	(3)
#define PS7_POLL_FAILED_DMA		(4)
#define PS7_POLL_FAILED_PLL		(5)

#define PCW_SILICON_VERSION_1	0
#define PCW_SILICON_VERSION_2	1
#define PCW_SILICON_VERSION_3	2

/* For delay calculation using global registers*/
#define SCU_GLOBAL_TIMER_COUNT_L32	0xF8F00200
#define SCU_GLOBAL_TIMER_COUNT_U32	0xF8F00204
#define SCU_GLOBAL_TIMER_CONTROL	0xF8F00208
#define SCU_GLOBAL_TIMER_AUTO_INC	0xF8F00218
#define APU_FREQ  666666666

#define PS7_MASK_POLL_TIME 100000000

#define __arch_getb(a)			(*(volatile uint8_t *)(a))
#define __arch_getw(a)			(*(volatile uint16_t *)(a))
#define __arch_getl(a)			(*(volatile uint32_t *)(a))
#define __arch_getq(a)			(*(volatile uint64_t *)(a))

#define __arch_putb(v,a)		(*(volatile uint8_t *)(a) = (v))
#define __arch_putw(v,a)		(*(volatile uint16_t *)(a) = (v))
#define __arch_putl(v,a)		(*(volatile uint32_t *)(a) = (v))
#define __arch_putq(v,a)		(*(volatile uint64_t *)(a) = (v))

#define __raw_writeb(v,a)	__arch_putb(v,a)
#define __raw_writew(v,a)	__arch_putw(v,a)
#define __raw_writel(v,a)	__arch_putl(v,a)
#define __raw_writeq(v,a)	__arch_putq(v,a)

#define __raw_readb(a)		__arch_getb(a)
#define __raw_readw(a)		__arch_getw(a)
#define __raw_readl(a)		__arch_getl(a)
#define __raw_readq(a)		__arch_getq(a)

/* IO accessors. No memory barriers desired. */
static inline void iowrite(unsigned long val, uintptr_t addr)
{
	__raw_writel(val, addr);
}

static inline unsigned long ioread(uintptr_t addr)
{
	return __raw_readl(addr);
}

/* start timer */
static void perf_start_clock(void)
{
	iowrite((1 << 0) | /* Timer Enable */
		(1 << 3) | /* Auto-increment */
		(0 << 8), /* Pre-scale */
		SCU_GLOBAL_TIMER_CONTROL);
}

/* Compute mask for given delay in miliseconds*/
static unsigned long get_number_of_cycles_for_delay(unsigned long delay)
{
	return (APU_FREQ / (2 * 1000)) * delay;
}

/* stop timer */
static void perf_disable_clock(void)
{
	iowrite(0, SCU_GLOBAL_TIMER_CONTROL);
}

/* stop timer and reset timer count regs */
static void perf_reset_clock(void)
{
	perf_disable_clock();
	iowrite(0, SCU_GLOBAL_TIMER_COUNT_L32);
	iowrite(0, SCU_GLOBAL_TIMER_COUNT_U32);
}

static void perf_reset_and_start_timer(void)
{
	perf_reset_clock();
	perf_start_clock();
}

int ps7_config(const unsigned long * ps7_config_init)
{
	const unsigned long *ptr = ps7_config_init;

    unsigned long  opcode;            // current instruction ..
    unsigned long  args[16];           // no opcode has so many args ...
    int  numargs;           // number of arguments of this instruction
    int  j;                 // general purpose index

    volatile uint32_t *addr;         // some variable to make code readable
    unsigned long  val,mask;              // some variable to make code readable

    int finish = -1 ;           // loop while this is negative !
    int i = 0;                  // Timeout variable

    while( finish < 0 ) {
        numargs = ptr[0] & 0xF;
        opcode = ptr[0] >> 4;

        for( j = 0 ; j < numargs ; j ++ )
            args[j] = ptr[j+1];
        ptr += numargs + 1;


        switch ( opcode ) {

        case OPCODE_EXIT:
            finish = PS7_INIT_SUCCESS;
            break;

        case OPCODE_CLEAR:
            addr = (volatile uint32_t*) args[0];
            *addr = 0;
            break;

        case OPCODE_WRITE:
            addr = (volatile uint32_t*) args[0];
            val = args[1];
            *addr = val;
            break;

        case OPCODE_MASKWRITE:
            addr = (volatile uint32_t*) args[0];
            mask = args[1];
            val = args[2];
            *addr = ( val & mask ) | ( *addr & ~mask);
            break;

        case OPCODE_MASKPOLL:
            addr = (volatile uint32_t*) args[0];
            mask = args[1];
            i = 0;
            while (!(*addr & mask)) {
                if (i == PS7_MASK_POLL_TIME) {
                    finish = PS7_INIT_TIMEOUT;
                    break;
                }
                i++;
            }
            break;
        case OPCODE_MASKDELAY:
            addr = (volatile uint32_t*) args[0];
            mask = args[1];
            int delay = get_number_of_cycles_for_delay(mask);
            perf_reset_and_start_timer();
            while ((*addr < delay)) {
            }
            break;
        default:
            finish = PS7_INIT_CORRUPT;
            break;
        }
    }
    return finish;
}

#if WITHPS7BOARD_EBAZ4205
// EBAZ
static const unsigned long ps7_clock_init_data_3_0[] = {
		EMIT_MASKWRITE(0XF8000128, 0x03F03F01U ,0x00700F01U),	// DCI_CLK_CTRL
		EMIT_MASKWRITE(0XF8000168, 0x00003F31U ,0x00000801U),	// PCAP_CLK_CTRL
		//EMIT_MASKWRITE(0XF8000170, 0x03F03F30U ,0x00400800U),	// FPGA0_CLK_CTRL PL Clock 0 Output control
		//EMIT_MASKWRITE(0XF80001C4, 0x00000001U ,0x00000001U),	// CLK_621_TRUE CPU Clock Ratio Mode select
		//EMIT_MASKWRITE(0XF800012C, 0x01FFCCCDU ,0x016C040DU),	// APER_CLK_CTRL AMBA Peripheral Clock Control
		EMIT_EXIT(),
	};

static const unsigned long ps7_ddr_init_data_3_0[] = {
		EMIT_MASKWRITE(0XF8006000, 0x0001FFFFU ,0x00000084U),	// ddrc_ctrl, reg_ddrc_soft_rstb = 0
		EMIT_MASKWRITE(0XF8006004, 0x0007FFFFU ,0x00001082U),	// Two_rank_cfg
		EMIT_MASKWRITE(0XF8006008, 0x03FFFFFFU ,0x03C0780FU),
		EMIT_MASKWRITE(0XF800600C, 0x03FFFFFFU ,0x02001001U),
		EMIT_MASKWRITE(0XF8006010, 0x03FFFFFFU ,0x00014001U),
		EMIT_MASKWRITE(0XF8006014, 0x001FFFFFU ,0x0004159BU),
		EMIT_MASKWRITE(0XF8006018, 0xF7FFFFFFU ,0x44E458D3U),
		EMIT_MASKWRITE(0XF800601C, 0xFFFFFFFFU ,0x7282BCE5U),
		EMIT_MASKWRITE(0XF8006020, 0x7FDFFFFCU ,0x270872D0U),
		EMIT_MASKWRITE(0XF8006024, 0x0FFFFFC3U ,0x00000000U),
		EMIT_MASKWRITE(0XF8006028, 0x00003FFFU ,0x00002007U),
		EMIT_MASKWRITE(0XF800602C, 0xFFFFFFFFU ,0x00000008U),
		EMIT_MASKWRITE(0XF8006030, 0xFFFFFFFFU ,0x00040B30U),
		EMIT_MASKWRITE(0XF8006034, 0x13FF3FFFU ,0x000116D4U),
		EMIT_MASKWRITE(0XF8006038, 0x00000003U ,0x00000000U),
		EMIT_MASKWRITE(0XF800603C, 0x000FFFFFU ,0x00000666U),
		EMIT_MASKWRITE(0XF8006040, 0xFFFFFFFFU ,0xFFFF0000U),
		EMIT_MASKWRITE(0XF8006044, 0x0FFFFFFFU ,0x0FF55555U),
		EMIT_MASKWRITE(0XF8006048, 0x0003F03FU ,0x0003C008U),
		EMIT_MASKWRITE(0XF8006050, 0xFF0F8FFFU ,0x77010800U),
		EMIT_MASKWRITE(0XF8006058, 0x00010000U ,0x00000000U),
		EMIT_MASKWRITE(0XF800605C, 0x0000FFFFU ,0x00005003U),
		EMIT_MASKWRITE(0XF8006060, 0x000017FFU ,0x0000003EU),
		EMIT_MASKWRITE(0XF8006064, 0x00021FE0U ,0x00020000U),
		EMIT_MASKWRITE(0XF8006068, 0x03FFFFFFU ,0x00284141U),
		EMIT_MASKWRITE(0XF800606C, 0x0000FFFFU ,0x00001610U),
		EMIT_MASKWRITE(0XF8006078, 0x03FFFFFFU ,0x00466111U),
		EMIT_MASKWRITE(0XF800607C, 0x000FFFFFU ,0x00032222U),
		EMIT_MASKWRITE(0XF80060A4, 0xFFFFFFFFU ,0x10200802U),
		EMIT_MASKWRITE(0XF80060A8, 0x0FFFFFFFU ,0x0690CB73U),
		EMIT_MASKWRITE(0XF80060AC, 0x000001FFU ,0x000001FEU),
		EMIT_MASKWRITE(0XF80060B0, 0x1FFFFFFFU ,0x1CFFFFFFU),
		EMIT_MASKWRITE(0XF80060B4, 0x00000200U ,0x00000200U),
		EMIT_MASKWRITE(0XF80060B8, 0x01FFFFFFU ,0x00200066U),
		EMIT_MASKWRITE(0XF80060C4, 0x00000003U ,0x00000000U),
		EMIT_MASKWRITE(0XF80060C8, 0x000000FFU ,0x00000000U),
		EMIT_MASKWRITE(0XF80060DC, 0x00000001U ,0x00000000U),
		EMIT_MASKWRITE(0XF80060F0, 0x0000FFFFU ,0x00000000U),
		EMIT_MASKWRITE(0XF80060F4, 0x0000000FU ,0x00000008U),
		EMIT_MASKWRITE(0XF8006114, 0x000000FFU ,0x00000000U),
		EMIT_MASKWRITE(0XF8006118, 0x7FFFFFCFU ,0x40000001U),
		EMIT_MASKWRITE(0XF800611C, 0x7FFFFFCFU ,0x40000001U),
		EMIT_MASKWRITE(0XF8006120, 0x7FFFFFCFU ,0x40000000U),
		EMIT_MASKWRITE(0XF8006124, 0x7FFFFFCFU ,0x40000000U),
		EMIT_MASKWRITE(0XF800612C, 0x000FFFFFU ,0x00029000U),
		EMIT_MASKWRITE(0XF8006130, 0x000FFFFFU ,0x00029000U),
		EMIT_MASKWRITE(0XF8006134, 0x000FFFFFU ,0x00029000U),
		EMIT_MASKWRITE(0XF8006138, 0x000FFFFFU ,0x00029000U),
		EMIT_MASKWRITE(0XF8006140, 0x000FFFFFU ,0x00000035U),
		EMIT_MASKWRITE(0XF8006144, 0x000FFFFFU ,0x00000035U),
		EMIT_MASKWRITE(0XF8006148, 0x000FFFFFU ,0x00000035U),
		EMIT_MASKWRITE(0XF800614C, 0x000FFFFFU ,0x00000035U),
		EMIT_MASKWRITE(0XF8006154, 0x000FFFFFU ,0x00000080U),
		EMIT_MASKWRITE(0XF8006158, 0x000FFFFFU ,0x00000080U),
		EMIT_MASKWRITE(0XF800615C, 0x000FFFFFU ,0x00000080U),
		EMIT_MASKWRITE(0XF8006160, 0x000FFFFFU ,0x00000080U),
		EMIT_MASKWRITE(0XF8006168, 0x001FFFFFU ,0x000000F9U),
		EMIT_MASKWRITE(0XF800616C, 0x001FFFFFU ,0x000000F9U),
		EMIT_MASKWRITE(0XF8006170, 0x001FFFFFU ,0x000000F9U),
		EMIT_MASKWRITE(0XF8006174, 0x001FFFFFU ,0x000000F9U),
		EMIT_MASKWRITE(0XF800617C, 0x000FFFFFU ,0x000000C0U),
		EMIT_MASKWRITE(0XF8006180, 0x000FFFFFU ,0x000000C0U),
		EMIT_MASKWRITE(0XF8006184, 0x000FFFFFU ,0x000000C0U),
		EMIT_MASKWRITE(0XF8006188, 0x000FFFFFU ,0x000000C0U),
		EMIT_MASKWRITE(0XF8006190, 0x6FFFFEFEU ,0x00040080U),
		EMIT_MASKWRITE(0XF8006194, 0x000FFFFFU ,0x0001FC82U),
		EMIT_MASKWRITE(0XF8006204, 0xFFFFFFFFU ,0x00000000U),
		EMIT_MASKWRITE(0XF8006208, 0x000703FFU ,0x000003FFU),
		EMIT_MASKWRITE(0XF800620C, 0x000703FFU ,0x000003FFU),
		EMIT_MASKWRITE(0XF8006210, 0x000703FFU ,0x000003FFU),
		EMIT_MASKWRITE(0XF8006214, 0x000703FFU ,0x000003FFU),
		EMIT_MASKWRITE(0XF8006218, 0x000F03FFU ,0x000003FFU),
		EMIT_MASKWRITE(0XF800621C, 0x000F03FFU ,0x000003FFU),
		EMIT_MASKWRITE(0XF8006220, 0x000F03FFU ,0x000003FFU),
		EMIT_MASKWRITE(0XF8006224, 0x000F03FFU ,0x000003FFU),
		EMIT_MASKWRITE(0XF80062A8, 0x00000FF5U ,0x00000000U),
		EMIT_MASKWRITE(0XF80062AC, 0xFFFFFFFFU ,0x00000000U),
		EMIT_MASKWRITE(0XF80062B0, 0x003FFFFFU ,0x00005125U),
		EMIT_MASKWRITE(0XF80062B4, 0x0003FFFFU ,0x000012A8U),
		EMIT_MASKPOLL(0XF8000B74, 0x00002000U),					// DDRIOB_DCI_STATUS
		EMIT_MASKWRITE(0XF8006000, 0x0001FFFFU ,0x00000085U),	// ddrc_ctrl, reg_ddrc_soft_rstb = 1
		EMIT_MASKPOLL(0XF8006054, 0x00000007U),					// mode_sts_reg
		EMIT_EXIT(),
	};

static const unsigned long ps7_mio_init_data_3_0[] = {
		EMIT_MASKWRITE(0XF8000B40, 0x00000FFFU ,0x00000600U),	// DDRIOB_ADDR0
		EMIT_MASKWRITE(0XF8000B44, 0x00000FFFU ,0x00000600U),	// DDRIOB_ADDR1
		EMIT_MASKWRITE(0XF8000B48, 0x00000FFFU ,0x00000672U),
		EMIT_MASKWRITE(0XF8000B4C, 0x00000FFFU ,0x00000800U),
		EMIT_MASKWRITE(0XF8000B50, 0x00000FFFU ,0x00000674U),
		EMIT_MASKWRITE(0XF8000B54, 0x00000FFFU ,0x00000800U),
		EMIT_MASKWRITE(0XF8000B58, 0x00000FFFU ,0x00000600U),	// DDRIOB_CLOCK
		EMIT_MASKWRITE(0XF8000B5C, 0xFFFFFFFFU ,0x0018C61CU),	// DDRIOB_DRIVE_SLEW_ADDR
		EMIT_MASKWRITE(0XF8000B60, 0xFFFFFFFFU ,0x00F9861CU),
		EMIT_MASKWRITE(0XF8000B64, 0xFFFFFFFFU ,0x00F9861CU),
		EMIT_MASKWRITE(0XF8000B68, 0xFFFFFFFFU ,0x00F9861CU),	// DDRIOB_DRIVE_SLEW_CLOCK
		EMIT_MASKWRITE(0XF8000B6C, 0x00007FFFU ,0x00000220U),	// DDRIOB_DDR_CTRL
		EMIT_MASKWRITE(0XF8000B70, 0x00000001U ,0x00000001U),	// DDRIOB_DCI_CTRL
		EMIT_MASKWRITE(0XF8000B70, 0x00000021U ,0x00000020U),	// DDRIOB_DCI_CTRL
		EMIT_MASKWRITE(0XF8000B70, 0x07FEFFFFU ,0x00000823U),	// DDRIOB_DCI_CTRL
		EMIT_EXIT(),
};

static const unsigned long ps7_peripherals_init_data_3_0[] = {
		EMIT_MASKWRITE(0XF8000B48, 0x00000180U ,0x00000180U),	// DDRIOB_DATA0
		EMIT_MASKWRITE(0XF8000B4C, 0x00000180U ,0x00000000U),	// DDRIOB_DATA1
		EMIT_MASKWRITE(0XF8000B50, 0x00000180U ,0x00000180U),	// DDRIOB_DIFF0
		EMIT_MASKWRITE(0XF8000B54, 0x00000180U ,0x00000000U),	// DDRIOB_DIFF1
		EMIT_EXIT(),
	};

#elif WITHPS7BOARD_ANTMINER

// ANTMINER
unsigned long ps7_pll_init_data_3_0[] = {
    // START: top
    // .. START: SLCR SETTINGS
    // .. UNLOCK_KEY = 0XDF0D
    // .. ==> 0XF8000008[15:0] = 0x0000DF0DU
    // ..     ==> MASK : 0x0000FFFFU    VAL : 0x0000DF0DU
    // ..
    EMIT_WRITE(0XF8000008, 0x0000DF0DU),
    // .. FINISH: SLCR SETTINGS
    // .. START: PLL SLCR REGISTERS
    // .. .. START: ARM PLL INIT
    // .. .. PLL_RES = 0x2
    // .. .. ==> 0XF8000110[7:4] = 0x00000002U
    // .. ..     ==> MASK : 0x000000F0U    VAL : 0x00000020U
    // .. .. PLL_CP = 0x2
    // .. .. ==> 0XF8000110[11:8] = 0x00000002U
    // .. ..     ==> MASK : 0x00000F00U    VAL : 0x00000200U
    // .. .. LOCK_CNT = 0xfa
    // .. .. ==> 0XF8000110[21:12] = 0x000000FAU
    // .. ..     ==> MASK : 0x003FF000U    VAL : 0x000FA000U
    // .. ..
    EMIT_MASKWRITE(0XF8000110, 0x003FFFF0U ,0x000FA220U),
    // .. .. .. START: UPDATE FB_DIV
    // .. .. .. PLL_FDIV = 0x28
    // .. .. .. ==> 0XF8000100[18:12] = 0x00000028U
    // .. .. ..     ==> MASK : 0x0007F000U    VAL : 0x00028000U
    // .. .. ..
    EMIT_MASKWRITE(0XF8000100, 0x0007F000U ,0x00028000U),
    // .. .. .. FINISH: UPDATE FB_DIV
    // .. .. .. START: BY PASS PLL
    // .. .. .. PLL_BYPASS_FORCE = 1
    // .. .. .. ==> 0XF8000100[4:4] = 0x00000001U
    // .. .. ..     ==> MASK : 0x00000010U    VAL : 0x00000010U
    // .. .. ..
    EMIT_MASKWRITE(0XF8000100, 0x00000010U ,0x00000010U),
    // .. .. .. FINISH: BY PASS PLL
    // .. .. .. START: ASSERT RESET
    // .. .. .. PLL_RESET = 1
    // .. .. .. ==> 0XF8000100[0:0] = 0x00000001U
    // .. .. ..     ==> MASK : 0x00000001U    VAL : 0x00000001U
    // .. .. ..
    EMIT_MASKWRITE(0XF8000100, 0x00000001U ,0x00000001U),
    // .. .. .. FINISH: ASSERT RESET
    // .. .. .. START: DEASSERT RESET
    // .. .. .. PLL_RESET = 0
    // .. .. .. ==> 0XF8000100[0:0] = 0x00000000U
    // .. .. ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. .. ..
    EMIT_MASKWRITE(0XF8000100, 0x00000001U ,0x00000000U),
    // .. .. .. FINISH: DEASSERT RESET
    // .. .. .. START: CHECK PLL STATUS
    // .. .. .. ARM_PLL_LOCK = 1
    // .. .. .. ==> 0XF800010C[0:0] = 0x00000001U
    // .. .. ..     ==> MASK : 0x00000001U    VAL : 0x00000001U
    // .. .. ..
    EMIT_MASKPOLL(0XF800010C, 0x00000001U),
    // .. .. .. FINISH: CHECK PLL STATUS
    // .. .. .. START: REMOVE PLL BY PASS
    // .. .. .. PLL_BYPASS_FORCE = 0
    // .. .. .. ==> 0XF8000100[4:4] = 0x00000000U
    // .. .. ..     ==> MASK : 0x00000010U    VAL : 0x00000000U
    // .. .. ..
    EMIT_MASKWRITE(0XF8000100, 0x00000010U ,0x00000000U),
    // .. .. .. FINISH: REMOVE PLL BY PASS
    // .. .. .. SRCSEL = 0x0
    // .. .. .. ==> 0XF8000120[5:4] = 0x00000000U
    // .. .. ..     ==> MASK : 0x00000030U    VAL : 0x00000000U
    // .. .. .. DIVISOR = 0x2
    // .. .. .. ==> 0XF8000120[13:8] = 0x00000002U
    // .. .. ..     ==> MASK : 0x00003F00U    VAL : 0x00000200U
    // .. .. .. CPU_6OR4XCLKACT = 0x1
    // .. .. .. ==> 0XF8000120[24:24] = 0x00000001U
    // .. .. ..     ==> MASK : 0x01000000U    VAL : 0x01000000U
    // .. .. .. CPU_3OR2XCLKACT = 0x1
    // .. .. .. ==> 0XF8000120[25:25] = 0x00000001U
    // .. .. ..     ==> MASK : 0x02000000U    VAL : 0x02000000U
    // .. .. .. CPU_2XCLKACT = 0x1
    // .. .. .. ==> 0XF8000120[26:26] = 0x00000001U
    // .. .. ..     ==> MASK : 0x04000000U    VAL : 0x04000000U
    // .. .. .. CPU_1XCLKACT = 0x1
    // .. .. .. ==> 0XF8000120[27:27] = 0x00000001U
    // .. .. ..     ==> MASK : 0x08000000U    VAL : 0x08000000U
    // .. .. .. CPU_PERI_CLKACT = 0x1
    // .. .. .. ==> 0XF8000120[28:28] = 0x00000001U
    // .. .. ..     ==> MASK : 0x10000000U    VAL : 0x10000000U
    // .. .. ..
    ////EMIT_MASKWRITE(0XF8000120, 0x1F003F30U ,0x1F000200U),
    // .. .. FINISH: ARM PLL INIT
    // .. .. START: DDR PLL INIT
    // .. .. PLL_RES = 0x2
    // .. .. ==> 0XF8000114[7:4] = 0x00000002U
    // .. ..     ==> MASK : 0x000000F0U    VAL : 0x00000020U
    // .. .. PLL_CP = 0x2
    // .. .. ==> 0XF8000114[11:8] = 0x00000002U
    // .. ..     ==> MASK : 0x00000F00U    VAL : 0x00000200U
    // .. .. LOCK_CNT = 0x12c
    // .. .. ==> 0XF8000114[21:12] = 0x0000012CU
    // .. ..     ==> MASK : 0x003FF000U    VAL : 0x0012C000U
    // .. ..
    EMIT_MASKWRITE(0XF8000114, 0x003FFFF0U ,0x0012C220U),
    // .. .. .. START: UPDATE FB_DIV
    // .. .. .. PLL_FDIV = 0x20
    // .. .. .. ==> 0XF8000104[18:12] = 0x00000020U
    // .. .. ..     ==> MASK : 0x0007F000U    VAL : 0x00020000U
    // .. .. ..
    EMIT_MASKWRITE(0XF8000104, 0x0007F000U ,0x00020000U),
    // .. .. .. FINISH: UPDATE FB_DIV
    // .. .. .. START: BY PASS PLL
    // .. .. .. PLL_BYPASS_FORCE = 1
    // .. .. .. ==> 0XF8000104[4:4] = 0x00000001U
    // .. .. ..     ==> MASK : 0x00000010U    VAL : 0x00000010U
    // .. .. ..
    EMIT_MASKWRITE(0XF8000104, 0x00000010U ,0x00000010U),
    // .. .. .. FINISH: BY PASS PLL
    // .. .. .. START: ASSERT RESET
    // .. .. .. PLL_RESET = 1
    // .. .. .. ==> 0XF8000104[0:0] = 0x00000001U
    // .. .. ..     ==> MASK : 0x00000001U    VAL : 0x00000001U
    // .. .. ..
    EMIT_MASKWRITE(0XF8000104, 0x00000001U ,0x00000001U),
    // .. .. .. FINISH: ASSERT RESET
    // .. .. .. START: DEASSERT RESET
    // .. .. .. PLL_RESET = 0
    // .. .. .. ==> 0XF8000104[0:0] = 0x00000000U
    // .. .. ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. .. ..
    EMIT_MASKWRITE(0XF8000104, 0x00000001U ,0x00000000U),
    // .. .. .. FINISH: DEASSERT RESET
    // .. .. .. START: CHECK PLL STATUS
    // .. .. .. DDR_PLL_LOCK = 1
    // .. .. .. ==> 0XF800010C[1:1] = 0x00000001U
    // .. .. ..     ==> MASK : 0x00000002U    VAL : 0x00000002U
    // .. .. ..
    EMIT_MASKPOLL(0XF800010C, 0x00000002U),
    // .. .. .. FINISH: CHECK PLL STATUS
    // .. .. .. START: REMOVE PLL BY PASS
    // .. .. .. PLL_BYPASS_FORCE = 0
    // .. .. .. ==> 0XF8000104[4:4] = 0x00000000U
    // .. .. ..     ==> MASK : 0x00000010U    VAL : 0x00000000U
    // .. .. ..
    EMIT_MASKWRITE(0XF8000104, 0x00000010U ,0x00000000U),
    // .. .. .. FINISH: REMOVE PLL BY PASS
    // .. .. .. DDR_3XCLKACT = 0x1
    // .. .. .. ==> 0XF8000124[0:0] = 0x00000001U
    // .. .. ..     ==> MASK : 0x00000001U    VAL : 0x00000001U
    // .. .. .. DDR_2XCLKACT = 0x1
    // .. .. .. ==> 0XF8000124[1:1] = 0x00000001U
    // .. .. ..     ==> MASK : 0x00000002U    VAL : 0x00000002U
    // .. .. .. DDR_3XCLK_DIVISOR = 0x2
    // .. .. .. ==> 0XF8000124[25:20] = 0x00000002U
    // .. .. ..     ==> MASK : 0x03F00000U    VAL : 0x00200000U
    // .. .. .. DDR_2XCLK_DIVISOR = 0x3
    // .. .. .. ==> 0XF8000124[31:26] = 0x00000003U
    // .. .. ..     ==> MASK : 0xFC000000U    VAL : 0x0C000000U
    // .. .. ..
    EMIT_MASKWRITE(0XF8000124, 0xFFF00003U ,0x0C200003U),
    // .. .. FINISH: DDR PLL INIT
    // .. .. START: IO PLL INIT
    // .. .. PLL_RES = 0x4
    // .. .. ==> 0XF8000118[7:4] = 0x00000004U
    // .. ..     ==> MASK : 0x000000F0U    VAL : 0x00000040U
    // .. .. PLL_CP = 0x2
    // .. .. ==> 0XF8000118[11:8] = 0x00000002U
    // .. ..     ==> MASK : 0x00000F00U    VAL : 0x00000200U
    // .. .. LOCK_CNT = 0xfa
    // .. .. ==> 0XF8000118[21:12] = 0x000000FAU
    // .. ..     ==> MASK : 0x003FF000U    VAL : 0x000FA000U
    // .. ..
    EMIT_MASKWRITE(0XF8000118, 0x003FFFF0U ,0x000FA240U),
    // .. .. .. START: UPDATE FB_DIV
    // .. .. .. PLL_FDIV = 0x30
    // .. .. .. ==> 0XF8000108[18:12] = 0x00000030U
    // .. .. ..     ==> MASK : 0x0007F000U    VAL : 0x00030000U
    // .. .. ..
    EMIT_MASKWRITE(0XF8000108, 0x0007F000U ,0x00030000U),
    // .. .. .. FINISH: UPDATE FB_DIV
    // .. .. .. START: BY PASS PLL
    // .. .. .. PLL_BYPASS_FORCE = 1
    // .. .. .. ==> 0XF8000108[4:4] = 0x00000001U
    // .. .. ..     ==> MASK : 0x00000010U    VAL : 0x00000010U
    // .. .. ..
    EMIT_MASKWRITE(0XF8000108, 0x00000010U ,0x00000010U),
    // .. .. .. FINISH: BY PASS PLL
    // .. .. .. START: ASSERT RESET
    // .. .. .. PLL_RESET = 1
    // .. .. .. ==> 0XF8000108[0:0] = 0x00000001U
    // .. .. ..     ==> MASK : 0x00000001U    VAL : 0x00000001U
    // .. .. ..
    EMIT_MASKWRITE(0XF8000108, 0x00000001U ,0x00000001U),
    // .. .. .. FINISH: ASSERT RESET
    // .. .. .. START: DEASSERT RESET
    // .. .. .. PLL_RESET = 0
    // .. .. .. ==> 0XF8000108[0:0] = 0x00000000U
    // .. .. ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. .. ..
    EMIT_MASKWRITE(0XF8000108, 0x00000001U ,0x00000000U),
    // .. .. .. FINISH: DEASSERT RESET
    // .. .. .. START: CHECK PLL STATUS
    // .. .. .. IO_PLL_LOCK = 1
    // .. .. .. ==> 0XF800010C[2:2] = 0x00000001U
    // .. .. ..     ==> MASK : 0x00000004U    VAL : 0x00000004U
    // .. .. ..
    EMIT_MASKPOLL(0XF800010C, 0x00000004U),
    // .. .. .. FINISH: CHECK PLL STATUS
    // .. .. .. START: REMOVE PLL BY PASS
    // .. .. .. PLL_BYPASS_FORCE = 0
    // .. .. .. ==> 0XF8000108[4:4] = 0x00000000U
    // .. .. ..     ==> MASK : 0x00000010U    VAL : 0x00000000U
    // .. .. ..
    EMIT_MASKWRITE(0XF8000108, 0x00000010U ,0x00000000U),
    // .. .. .. FINISH: REMOVE PLL BY PASS
    // .. .. FINISH: IO PLL INIT
    // .. FINISH: PLL SLCR REGISTERS
    // .. START: LOCK IT BACK
    // .. LOCK_KEY = 0X767B
    // .. ==> 0XF8000004[15:0] = 0x0000767BU
    // ..     ==> MASK : 0x0000FFFFU    VAL : 0x0000767BU
    // ..
    //EMIT_WRITE(0XF8000004, 0x0000767BU),
    // .. FINISH: LOCK IT BACK
    // FINISH: top
    //
    EMIT_EXIT(),

    //
};

unsigned long ps7_clock_init_data_3_0[] = {
    // START: top
    // .. START: SLCR SETTINGS
    // .. UNLOCK_KEY = 0XDF0D
    // .. ==> 0XF8000008[15:0] = 0x0000DF0DU
    // ..     ==> MASK : 0x0000FFFFU    VAL : 0x0000DF0DU
    // ..
    EMIT_WRITE(0XF8000008, 0x0000DF0DU),
    // .. FINISH: SLCR SETTINGS
    // .. START: CLOCK CONTROL SLCR REGISTERS
    // .. CLKACT = 0x1
    // .. ==> 0XF8000128[0:0] = 0x00000001U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000001U
    // .. DIVISOR0 = 0xf
    // .. ==> 0XF8000128[13:8] = 0x0000000FU
    // ..     ==> MASK : 0x00003F00U    VAL : 0x00000F00U
    // .. DIVISOR1 = 0x7
    // .. ==> 0XF8000128[25:20] = 0x00000007U
    // ..     ==> MASK : 0x03F00000U    VAL : 0x00700000U
    // ..
    EMIT_MASKWRITE(0XF8000128, 0x03F03F01U ,0x00700F01U),	// DCI_CLK_CTRL
    // .. CLKACT = 0x1
    // .. ==> 0XF8000148[0:0] = 0x00000001U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000001U
    // .. SRCSEL = 0x0
    // .. ==> 0XF8000148[5:4] = 0x00000000U
    // ..     ==> MASK : 0x00000030U    VAL : 0x00000000U
    // .. DIVISOR = 0x10
    // .. ==> 0XF8000148[13:8] = 0x00000010U
    // ..     ==> MASK : 0x00003F00U    VAL : 0x00001000U
    // ..
    EMIT_MASKWRITE(0XF8000148, 0x00003F31U ,0x00001001U),
    // .. CLKACT0 = 0x0
    // .. ==> 0XF8000154[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. CLKACT1 = 0x1
    // .. ==> 0XF8000154[1:1] = 0x00000001U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000002U
    // .. SRCSEL = 0x0
    // .. ==> 0XF8000154[5:4] = 0x00000000U
    // ..     ==> MASK : 0x00000030U    VAL : 0x00000000U
    // .. DIVISOR = 0x10
    // .. ==> 0XF8000154[13:8] = 0x00000010U
    // ..     ==> MASK : 0x00003F00U    VAL : 0x00001000U
    // ..
    EMIT_MASKWRITE(0XF8000154, 0x00003F33U ,0x00001002U),
    // .. .. START: TRACE CLOCK
    // .. .. FINISH: TRACE CLOCK
    // .. .. CLKACT = 0x1
    // .. .. ==> 0XF8000168[0:0] = 0x00000001U
    // .. ..     ==> MASK : 0x00000001U    VAL : 0x00000001U
    // .. .. SRCSEL = 0x0
    // .. .. ==> 0XF8000168[5:4] = 0x00000000U
    // .. ..     ==> MASK : 0x00000030U    VAL : 0x00000000U
    // .. .. DIVISOR = 0x8
    // .. .. ==> 0XF8000168[13:8] = 0x00000008U
    // .. ..     ==> MASK : 0x00003F00U    VAL : 0x00000800U
    // .. ..
    EMIT_MASKWRITE(0XF8000168, 0x00003F31U ,0x00000801U),	// PCAP_CLK_CTRL
    // .. .. SRCSEL = 0x0
    // .. .. ==> 0XF8000170[5:4] = 0x00000000U
    // .. ..     ==> MASK : 0x00000030U    VAL : 0x00000000U
    // .. .. DIVISOR0 = 0x8
    // .. .. ==> 0XF8000170[13:8] = 0x00000008U
    // .. ..     ==> MASK : 0x00003F00U    VAL : 0x00000800U
    // .. .. DIVISOR1 = 0x4
    // .. .. ==> 0XF8000170[25:20] = 0x00000004U
    // .. ..     ==> MASK : 0x03F00000U    VAL : 0x00400000U
    // .. ..
    //EMIT_MASKWRITE(0XF8000170, 0x03F03F30U ,0x00400800U),	// 	// FPGA0_CLK_CTRL PL Clock 0 Output control
    // .. .. CLK_621_TRUE = 0x1
    // .. .. ==> 0XF80001C4[0:0] = 0x00000001U
    // .. ..     ==> MASK : 0x00000001U    VAL : 0x00000001U
    // .. ..
    //EMIT_MASKWRITE(0XF80001C4, 0x00000001U ,0x00000001U),	// CLK_621_TRUE CPU Clock Ratio Mode select
    // .. .. DMA_CPU_2XCLKACT = 0x1
    // .. .. ==> 0XF800012C[0:0] = 0x00000001U
    // .. ..     ==> MASK : 0x00000001U    VAL : 0x00000001U
    // .. .. USB0_CPU_1XCLKACT = 0x1
    // .. .. ==> 0XF800012C[2:2] = 0x00000001U
    // .. ..     ==> MASK : 0x00000004U    VAL : 0x00000004U
    // .. .. USB1_CPU_1XCLKACT = 0x1
    // .. .. ==> 0XF800012C[3:3] = 0x00000001U
    // .. ..     ==> MASK : 0x00000008U    VAL : 0x00000008U
    // .. .. GEM0_CPU_1XCLKACT = 0x0
    // .. .. ==> 0XF800012C[6:6] = 0x00000000U
    // .. ..     ==> MASK : 0x00000040U    VAL : 0x00000000U
    // .. .. GEM1_CPU_1XCLKACT = 0x0
    // .. .. ==> 0XF800012C[7:7] = 0x00000000U
    // .. ..     ==> MASK : 0x00000080U    VAL : 0x00000000U
    // .. .. SDI0_CPU_1XCLKACT = 0x0
    // .. .. ==> 0XF800012C[10:10] = 0x00000000U
    // .. ..     ==> MASK : 0x00000400U    VAL : 0x00000000U
    // .. .. SDI1_CPU_1XCLKACT = 0x0
    // .. .. ==> 0XF800012C[11:11] = 0x00000000U
    // .. ..     ==> MASK : 0x00000800U    VAL : 0x00000000U
    // .. .. SPI0_CPU_1XCLKACT = 0x0
    // .. .. ==> 0XF800012C[14:14] = 0x00000000U
    // .. ..     ==> MASK : 0x00004000U    VAL : 0x00000000U
    // .. .. SPI1_CPU_1XCLKACT = 0x0
    // .. .. ==> 0XF800012C[15:15] = 0x00000000U
    // .. ..     ==> MASK : 0x00008000U    VAL : 0x00000000U
    // .. .. CAN0_CPU_1XCLKACT = 0x0
    // .. .. ==> 0XF800012C[16:16] = 0x00000000U
    // .. ..     ==> MASK : 0x00010000U    VAL : 0x00000000U
    // .. .. CAN1_CPU_1XCLKACT = 0x0
    // .. .. ==> 0XF800012C[17:17] = 0x00000000U
    // .. ..     ==> MASK : 0x00020000U    VAL : 0x00000000U
    // .. .. I2C0_CPU_1XCLKACT = 0x1
    // .. .. ==> 0XF800012C[18:18] = 0x00000001U
    // .. ..     ==> MASK : 0x00040000U    VAL : 0x00040000U
    // .. .. I2C1_CPU_1XCLKACT = 0x1
    // .. .. ==> 0XF800012C[19:19] = 0x00000001U
    // .. ..     ==> MASK : 0x00080000U    VAL : 0x00080000U
    // .. .. UART0_CPU_1XCLKACT = 0x0
    // .. .. ==> 0XF800012C[20:20] = 0x00000000U
    // .. ..     ==> MASK : 0x00100000U    VAL : 0x00000000U
    // .. .. UART1_CPU_1XCLKACT = 0x1
    // .. .. ==> 0XF800012C[21:21] = 0x00000001U
    // .. ..     ==> MASK : 0x00200000U    VAL : 0x00200000U
    // .. .. GPIO_CPU_1XCLKACT = 0x1
    // .. .. ==> 0XF800012C[22:22] = 0x00000001U
    // .. ..     ==> MASK : 0x00400000U    VAL : 0x00400000U
    // .. .. LQSPI_CPU_1XCLKACT = 0x0
    // .. .. ==> 0XF800012C[23:23] = 0x00000000U
    // .. ..     ==> MASK : 0x00800000U    VAL : 0x00000000U
    // .. .. SMC_CPU_1XCLKACT = 0x1
    // .. .. ==> 0XF800012C[24:24] = 0x00000001U
    // .. ..     ==> MASK : 0x01000000U    VAL : 0x01000000U
    // .. ..
    EMIT_MASKWRITE(0XF800012C, 0x01FFCCCDU ,0x016C000DU),	// APER_CLK_CTRL
    // .. FINISH: CLOCK CONTROL SLCR REGISTERS
    // .. START: THIS SHOULD BE BLANK
    // .. FINISH: THIS SHOULD BE BLANK
    // .. START: LOCK IT BACK
    // .. LOCK_KEY = 0X767B
    // .. ==> 0XF8000004[15:0] = 0x0000767BU
    // ..     ==> MASK : 0x0000FFFFU    VAL : 0x0000767BU
    // ..
    //EMIT_WRITE(0XF8000004, 0x0000767BU),
    // .. FINISH: LOCK IT BACK
    // FINISH: top
    //
    EMIT_EXIT(),

    //
};

unsigned long ps7_ddr_init_data_3_0[] = {
    // START: top
    // .. START: DDR INITIALIZATION
    // .. .. START: LOCK DDR
    // .. .. reg_ddrc_soft_rstb = 0
    // .. .. ==> 0XF8006000[0:0] = 0x00000000U
    // .. ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. .. reg_ddrc_powerdown_en = 0x0
    // .. .. ==> 0XF8006000[1:1] = 0x00000000U
    // .. ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. .. reg_ddrc_data_bus_width = 0x0
    // .. .. ==> 0XF8006000[3:2] = 0x00000000U
    // .. ..     ==> MASK : 0x0000000CU    VAL : 0x00000000U
    // .. .. reg_ddrc_burst8_refresh = 0x0
    // .. .. ==> 0XF8006000[6:4] = 0x00000000U
    // .. ..     ==> MASK : 0x00000070U    VAL : 0x00000000U
    // .. .. reg_ddrc_rdwr_idle_gap = 0x1
    // .. .. ==> 0XF8006000[13:7] = 0x00000001U
    // .. ..     ==> MASK : 0x00003F80U    VAL : 0x00000080U
    // .. .. reg_ddrc_dis_rd_bypass = 0x0
    // .. .. ==> 0XF8006000[14:14] = 0x00000000U
    // .. ..     ==> MASK : 0x00004000U    VAL : 0x00000000U
    // .. .. reg_ddrc_dis_act_bypass = 0x0
    // .. .. ==> 0XF8006000[15:15] = 0x00000000U
    // .. ..     ==> MASK : 0x00008000U    VAL : 0x00000000U
    // .. .. reg_ddrc_dis_auto_refresh = 0x0
    // .. .. ==> 0XF8006000[16:16] = 0x00000000U
    // .. ..     ==> MASK : 0x00010000U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006000, 0x0001FFFFU ,0x00000080U),
    // .. .. FINISH: LOCK DDR
    // .. .. reg_ddrc_t_rfc_nom_x32 = 0x82
    // .. .. ==> 0XF8006004[11:0] = 0x00000082U
    // .. ..     ==> MASK : 0x00000FFFU    VAL : 0x00000082U
    // .. .. reserved_reg_ddrc_active_ranks = 0x1
    // .. .. ==> 0XF8006004[13:12] = 0x00000001U
    // .. ..     ==> MASK : 0x00003000U    VAL : 0x00001000U
    // .. .. reg_ddrc_addrmap_cs_bit0 = 0x0
    // .. .. ==> 0XF8006004[18:14] = 0x00000000U
    // .. ..     ==> MASK : 0x0007C000U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006004, 0x0007FFFFU ,0x00001082U),
    // .. .. reg_ddrc_hpr_min_non_critical_x32 = 0xf
    // .. .. ==> 0XF8006008[10:0] = 0x0000000FU
    // .. ..     ==> MASK : 0x000007FFU    VAL : 0x0000000FU
    // .. .. reg_ddrc_hpr_max_starve_x32 = 0xf
    // .. .. ==> 0XF8006008[21:11] = 0x0000000FU
    // .. ..     ==> MASK : 0x003FF800U    VAL : 0x00007800U
    // .. .. reg_ddrc_hpr_xact_run_length = 0xf
    // .. .. ==> 0XF8006008[25:22] = 0x0000000FU
    // .. ..     ==> MASK : 0x03C00000U    VAL : 0x03C00000U
    // .. ..
    EMIT_MASKWRITE(0XF8006008, 0x03FFFFFFU ,0x03C0780FU),
    // .. .. reg_ddrc_lpr_min_non_critical_x32 = 0x1
    // .. .. ==> 0XF800600C[10:0] = 0x00000001U
    // .. ..     ==> MASK : 0x000007FFU    VAL : 0x00000001U
    // .. .. reg_ddrc_lpr_max_starve_x32 = 0x2
    // .. .. ==> 0XF800600C[21:11] = 0x00000002U
    // .. ..     ==> MASK : 0x003FF800U    VAL : 0x00001000U
    // .. .. reg_ddrc_lpr_xact_run_length = 0x8
    // .. .. ==> 0XF800600C[25:22] = 0x00000008U
    // .. ..     ==> MASK : 0x03C00000U    VAL : 0x02000000U
    // .. ..
    EMIT_MASKWRITE(0XF800600C, 0x03FFFFFFU ,0x02001001U),
    // .. .. reg_ddrc_w_min_non_critical_x32 = 0x1
    // .. .. ==> 0XF8006010[10:0] = 0x00000001U
    // .. ..     ==> MASK : 0x000007FFU    VAL : 0x00000001U
    // .. .. reg_ddrc_w_xact_run_length = 0x8
    // .. .. ==> 0XF8006010[14:11] = 0x00000008U
    // .. ..     ==> MASK : 0x00007800U    VAL : 0x00004000U
    // .. .. reg_ddrc_w_max_starve_x32 = 0x2
    // .. .. ==> 0XF8006010[25:15] = 0x00000002U
    // .. ..     ==> MASK : 0x03FF8000U    VAL : 0x00010000U
    // .. ..
    EMIT_MASKWRITE(0XF8006010, 0x03FFFFFFU ,0x00014001U),
    // .. .. reg_ddrc_t_rc = 0x1b
    // .. .. ==> 0XF8006014[5:0] = 0x0000001BU
    // .. ..     ==> MASK : 0x0000003FU    VAL : 0x0000001BU
    // .. .. reg_ddrc_t_rfc_min = 0x56
    // .. .. ==> 0XF8006014[13:6] = 0x00000056U
    // .. ..     ==> MASK : 0x00003FC0U    VAL : 0x00001580U
    // .. .. reg_ddrc_post_selfref_gap_x32 = 0x10
    // .. .. ==> 0XF8006014[20:14] = 0x00000010U
    // .. ..     ==> MASK : 0x001FC000U    VAL : 0x00040000U
    // .. ..
    EMIT_MASKWRITE(0XF8006014, 0x001FFFFFU ,0x0004159BU),
    // .. .. reg_ddrc_wr2pre = 0x13
    // .. .. ==> 0XF8006018[4:0] = 0x00000013U
    // .. ..     ==> MASK : 0x0000001FU    VAL : 0x00000013U
    // .. .. reg_ddrc_powerdown_to_x32 = 0x6
    // .. .. ==> 0XF8006018[9:5] = 0x00000006U
    // .. ..     ==> MASK : 0x000003E0U    VAL : 0x000000C0U
    // .. .. reg_ddrc_t_faw = 0x16
    // .. .. ==> 0XF8006018[15:10] = 0x00000016U
    // .. ..     ==> MASK : 0x0000FC00U    VAL : 0x00005800U
    // .. .. reg_ddrc_t_ras_max = 0x24
    // .. .. ==> 0XF8006018[21:16] = 0x00000024U
    // .. ..     ==> MASK : 0x003F0000U    VAL : 0x00240000U
    // .. .. reg_ddrc_t_ras_min = 0x13
    // .. .. ==> 0XF8006018[26:22] = 0x00000013U
    // .. ..     ==> MASK : 0x07C00000U    VAL : 0x04C00000U
    // .. .. reg_ddrc_t_cke = 0x4
    // .. .. ==> 0XF8006018[31:28] = 0x00000004U
    // .. ..     ==> MASK : 0xF0000000U    VAL : 0x40000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006018, 0xF7FFFFFFU ,0x44E458D3U),
    // .. .. reg_ddrc_write_latency = 0x5
    // .. .. ==> 0XF800601C[4:0] = 0x00000005U
    // .. ..     ==> MASK : 0x0000001FU    VAL : 0x00000005U
    // .. .. reg_ddrc_rd2wr = 0x7
    // .. .. ==> 0XF800601C[9:5] = 0x00000007U
    // .. ..     ==> MASK : 0x000003E0U    VAL : 0x000000E0U
    // .. .. reg_ddrc_wr2rd = 0xf
    // .. .. ==> 0XF800601C[14:10] = 0x0000000FU
    // .. ..     ==> MASK : 0x00007C00U    VAL : 0x00003C00U
    // .. .. reg_ddrc_t_xp = 0x5
    // .. .. ==> 0XF800601C[19:15] = 0x00000005U
    // .. ..     ==> MASK : 0x000F8000U    VAL : 0x00028000U
    // .. .. reg_ddrc_pad_pd = 0x0
    // .. .. ==> 0XF800601C[22:20] = 0x00000000U
    // .. ..     ==> MASK : 0x00700000U    VAL : 0x00000000U
    // .. .. reg_ddrc_rd2pre = 0x5
    // .. .. ==> 0XF800601C[27:23] = 0x00000005U
    // .. ..     ==> MASK : 0x0F800000U    VAL : 0x02800000U
    // .. .. reg_ddrc_t_rcd = 0x7
    // .. .. ==> 0XF800601C[31:28] = 0x00000007U
    // .. ..     ==> MASK : 0xF0000000U    VAL : 0x70000000U
    // .. ..
    EMIT_MASKWRITE(0XF800601C, 0xFFFFFFFFU ,0x7282BCE5U),
    // .. .. reg_ddrc_t_ccd = 0x4
    // .. .. ==> 0XF8006020[4:2] = 0x00000004U
    // .. ..     ==> MASK : 0x0000001CU    VAL : 0x00000010U
    // .. .. reg_ddrc_t_rrd = 0x6
    // .. .. ==> 0XF8006020[7:5] = 0x00000006U
    // .. ..     ==> MASK : 0x000000E0U    VAL : 0x000000C0U
    // .. .. reg_ddrc_refresh_margin = 0x2
    // .. .. ==> 0XF8006020[11:8] = 0x00000002U
    // .. ..     ==> MASK : 0x00000F00U    VAL : 0x00000200U
    // .. .. reg_ddrc_t_rp = 0x7
    // .. .. ==> 0XF8006020[15:12] = 0x00000007U
    // .. ..     ==> MASK : 0x0000F000U    VAL : 0x00007000U
    // .. .. reg_ddrc_refresh_to_x32 = 0x8
    // .. .. ==> 0XF8006020[20:16] = 0x00000008U
    // .. ..     ==> MASK : 0x001F0000U    VAL : 0x00080000U
    // .. .. reg_ddrc_mobile = 0x0
    // .. .. ==> 0XF8006020[22:22] = 0x00000000U
    // .. ..     ==> MASK : 0x00400000U    VAL : 0x00000000U
    // .. .. reg_ddrc_en_dfi_dram_clk_disable = 0x0
    // .. .. ==> 0XF8006020[23:23] = 0x00000000U
    // .. ..     ==> MASK : 0x00800000U    VAL : 0x00000000U
    // .. .. reg_ddrc_read_latency = 0x7
    // .. .. ==> 0XF8006020[28:24] = 0x00000007U
    // .. ..     ==> MASK : 0x1F000000U    VAL : 0x07000000U
    // .. .. reg_phy_mode_ddr1_ddr2 = 0x1
    // .. .. ==> 0XF8006020[29:29] = 0x00000001U
    // .. ..     ==> MASK : 0x20000000U    VAL : 0x20000000U
    // .. .. reg_ddrc_dis_pad_pd = 0x0
    // .. .. ==> 0XF8006020[30:30] = 0x00000000U
    // .. ..     ==> MASK : 0x40000000U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006020, 0x7FDFFFFCU ,0x270872D0U),
    // .. .. reg_ddrc_en_2t_timing_mode = 0x0
    // .. .. ==> 0XF8006024[0:0] = 0x00000000U
    // .. ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. .. reg_ddrc_prefer_write = 0x0
    // .. .. ==> 0XF8006024[1:1] = 0x00000000U
    // .. ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. .. reg_ddrc_mr_wr = 0x0
    // .. .. ==> 0XF8006024[6:6] = 0x00000000U
    // .. ..     ==> MASK : 0x00000040U    VAL : 0x00000000U
    // .. .. reg_ddrc_mr_addr = 0x0
    // .. .. ==> 0XF8006024[8:7] = 0x00000000U
    // .. ..     ==> MASK : 0x00000180U    VAL : 0x00000000U
    // .. .. reg_ddrc_mr_data = 0x0
    // .. .. ==> 0XF8006024[24:9] = 0x00000000U
    // .. ..     ==> MASK : 0x01FFFE00U    VAL : 0x00000000U
    // .. .. ddrc_reg_mr_wr_busy = 0x0
    // .. .. ==> 0XF8006024[25:25] = 0x00000000U
    // .. ..     ==> MASK : 0x02000000U    VAL : 0x00000000U
    // .. .. reg_ddrc_mr_type = 0x0
    // .. .. ==> 0XF8006024[26:26] = 0x00000000U
    // .. ..     ==> MASK : 0x04000000U    VAL : 0x00000000U
    // .. .. reg_ddrc_mr_rdata_valid = 0x0
    // .. .. ==> 0XF8006024[27:27] = 0x00000000U
    // .. ..     ==> MASK : 0x08000000U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006024, 0x0FFFFFC3U ,0x00000000U),
    // .. .. reg_ddrc_final_wait_x32 = 0x7
    // .. .. ==> 0XF8006028[6:0] = 0x00000007U
    // .. ..     ==> MASK : 0x0000007FU    VAL : 0x00000007U
    // .. .. reg_ddrc_pre_ocd_x32 = 0x0
    // .. .. ==> 0XF8006028[10:7] = 0x00000000U
    // .. ..     ==> MASK : 0x00000780U    VAL : 0x00000000U
    // .. .. reg_ddrc_t_mrd = 0x4
    // .. .. ==> 0XF8006028[13:11] = 0x00000004U
    // .. ..     ==> MASK : 0x00003800U    VAL : 0x00002000U
    // .. ..
    EMIT_MASKWRITE(0XF8006028, 0x00003FFFU ,0x00002007U),
    // .. .. reg_ddrc_emr2 = 0x8
    // .. .. ==> 0XF800602C[15:0] = 0x00000008U
    // .. ..     ==> MASK : 0x0000FFFFU    VAL : 0x00000008U
    // .. .. reg_ddrc_emr3 = 0x0
    // .. .. ==> 0XF800602C[31:16] = 0x00000000U
    // .. ..     ==> MASK : 0xFFFF0000U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF800602C, 0xFFFFFFFFU ,0x00000008U),
    // .. .. reg_ddrc_mr = 0xb30
    // .. .. ==> 0XF8006030[15:0] = 0x00000B30U
    // .. ..     ==> MASK : 0x0000FFFFU    VAL : 0x00000B30U
    // .. .. reg_ddrc_emr = 0x4
    // .. .. ==> 0XF8006030[31:16] = 0x00000004U
    // .. ..     ==> MASK : 0xFFFF0000U    VAL : 0x00040000U
    // .. ..
    EMIT_MASKWRITE(0XF8006030, 0xFFFFFFFFU ,0x00040B30U),
    // .. .. reg_ddrc_burst_rdwr = 0x4
    // .. .. ==> 0XF8006034[3:0] = 0x00000004U
    // .. ..     ==> MASK : 0x0000000FU    VAL : 0x00000004U
    // .. .. reg_ddrc_pre_cke_x1024 = 0x16d
    // .. .. ==> 0XF8006034[13:4] = 0x0000016DU
    // .. ..     ==> MASK : 0x00003FF0U    VAL : 0x000016D0U
    // .. .. reg_ddrc_post_cke_x1024 = 0x1
    // .. .. ==> 0XF8006034[25:16] = 0x00000001U
    // .. ..     ==> MASK : 0x03FF0000U    VAL : 0x00010000U
    // .. .. reg_ddrc_burstchop = 0x0
    // .. .. ==> 0XF8006034[28:28] = 0x00000000U
    // .. ..     ==> MASK : 0x10000000U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006034, 0x13FF3FFFU ,0x000116D4U),
    // .. .. reg_ddrc_force_low_pri_n = 0x0
    // .. .. ==> 0XF8006038[0:0] = 0x00000000U
    // .. ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. .. reg_ddrc_dis_dq = 0x0
    // .. .. ==> 0XF8006038[1:1] = 0x00000000U
    // .. ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006038, 0x00000003U ,0x00000000U),
    // .. .. reg_ddrc_addrmap_bank_b0 = 0x7
    // .. .. ==> 0XF800603C[3:0] = 0x00000007U
    // .. ..     ==> MASK : 0x0000000FU    VAL : 0x00000007U
    // .. .. reg_ddrc_addrmap_bank_b1 = 0x7
    // .. .. ==> 0XF800603C[7:4] = 0x00000007U
    // .. ..     ==> MASK : 0x000000F0U    VAL : 0x00000070U
    // .. .. reg_ddrc_addrmap_bank_b2 = 0x7
    // .. .. ==> 0XF800603C[11:8] = 0x00000007U
    // .. ..     ==> MASK : 0x00000F00U    VAL : 0x00000700U
    // .. .. reg_ddrc_addrmap_col_b5 = 0x0
    // .. .. ==> 0XF800603C[15:12] = 0x00000000U
    // .. ..     ==> MASK : 0x0000F000U    VAL : 0x00000000U
    // .. .. reg_ddrc_addrmap_col_b6 = 0x0
    // .. .. ==> 0XF800603C[19:16] = 0x00000000U
    // .. ..     ==> MASK : 0x000F0000U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF800603C, 0x000FFFFFU ,0x00000777U),
    // .. .. reg_ddrc_addrmap_col_b2 = 0x0
    // .. .. ==> 0XF8006040[3:0] = 0x00000000U
    // .. ..     ==> MASK : 0x0000000FU    VAL : 0x00000000U
    // .. .. reg_ddrc_addrmap_col_b3 = 0x0
    // .. .. ==> 0XF8006040[7:4] = 0x00000000U
    // .. ..     ==> MASK : 0x000000F0U    VAL : 0x00000000U
    // .. .. reg_ddrc_addrmap_col_b4 = 0x0
    // .. .. ==> 0XF8006040[11:8] = 0x00000000U
    // .. ..     ==> MASK : 0x00000F00U    VAL : 0x00000000U
    // .. .. reg_ddrc_addrmap_col_b7 = 0x0
    // .. .. ==> 0XF8006040[15:12] = 0x00000000U
    // .. ..     ==> MASK : 0x0000F000U    VAL : 0x00000000U
    // .. .. reg_ddrc_addrmap_col_b8 = 0x0
    // .. .. ==> 0XF8006040[19:16] = 0x00000000U
    // .. ..     ==> MASK : 0x000F0000U    VAL : 0x00000000U
    // .. .. reg_ddrc_addrmap_col_b9 = 0xf
    // .. .. ==> 0XF8006040[23:20] = 0x0000000FU
    // .. ..     ==> MASK : 0x00F00000U    VAL : 0x00F00000U
    // .. .. reg_ddrc_addrmap_col_b10 = 0xf
    // .. .. ==> 0XF8006040[27:24] = 0x0000000FU
    // .. ..     ==> MASK : 0x0F000000U    VAL : 0x0F000000U
    // .. .. reg_ddrc_addrmap_col_b11 = 0xf
    // .. .. ==> 0XF8006040[31:28] = 0x0000000FU
    // .. ..     ==> MASK : 0xF0000000U    VAL : 0xF0000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006040, 0xFFFFFFFFU ,0xFFF00000U),
    // .. .. reg_ddrc_addrmap_row_b0 = 0x6
    // .. .. ==> 0XF8006044[3:0] = 0x00000006U
    // .. ..     ==> MASK : 0x0000000FU    VAL : 0x00000006U
    // .. .. reg_ddrc_addrmap_row_b1 = 0x6
    // .. .. ==> 0XF8006044[7:4] = 0x00000006U
    // .. ..     ==> MASK : 0x000000F0U    VAL : 0x00000060U
    // .. .. reg_ddrc_addrmap_row_b2_11 = 0x6
    // .. .. ==> 0XF8006044[11:8] = 0x00000006U
    // .. ..     ==> MASK : 0x00000F00U    VAL : 0x00000600U
    // .. .. reg_ddrc_addrmap_row_b12 = 0x6
    // .. .. ==> 0XF8006044[15:12] = 0x00000006U
    // .. ..     ==> MASK : 0x0000F000U    VAL : 0x00006000U
    // .. .. reg_ddrc_addrmap_row_b13 = 0x6
    // .. .. ==> 0XF8006044[19:16] = 0x00000006U
    // .. ..     ==> MASK : 0x000F0000U    VAL : 0x00060000U
    // .. .. reg_ddrc_addrmap_row_b14 = 0xf
    // .. .. ==> 0XF8006044[23:20] = 0x0000000FU
    // .. ..     ==> MASK : 0x00F00000U    VAL : 0x00F00000U
    // .. .. reg_ddrc_addrmap_row_b15 = 0xf
    // .. .. ==> 0XF8006044[27:24] = 0x0000000FU
    // .. ..     ==> MASK : 0x0F000000U    VAL : 0x0F000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006044, 0x0FFFFFFFU ,0x0FF66666U),
    // .. .. reg_phy_rd_local_odt = 0x0
    // .. .. ==> 0XF8006048[13:12] = 0x00000000U
    // .. ..     ==> MASK : 0x00003000U    VAL : 0x00000000U
    // .. .. reg_phy_wr_local_odt = 0x3
    // .. .. ==> 0XF8006048[15:14] = 0x00000003U
    // .. ..     ==> MASK : 0x0000C000U    VAL : 0x0000C000U
    // .. .. reg_phy_idle_local_odt = 0x3
    // .. .. ==> 0XF8006048[17:16] = 0x00000003U
    // .. ..     ==> MASK : 0x00030000U    VAL : 0x00030000U
    // .. .. reserved_reg_ddrc_rank0_wr_odt = 0x1
    // .. .. ==> 0XF8006048[5:3] = 0x00000001U
    // .. ..     ==> MASK : 0x00000038U    VAL : 0x00000008U
    // .. .. reserved_reg_ddrc_rank0_rd_odt = 0x0
    // .. .. ==> 0XF8006048[2:0] = 0x00000000U
    // .. ..     ==> MASK : 0x00000007U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006048, 0x0003F03FU ,0x0003C008U),
    // .. .. reg_phy_rd_cmd_to_data = 0x0
    // .. .. ==> 0XF8006050[3:0] = 0x00000000U
    // .. ..     ==> MASK : 0x0000000FU    VAL : 0x00000000U
    // .. .. reg_phy_wr_cmd_to_data = 0x0
    // .. .. ==> 0XF8006050[7:4] = 0x00000000U
    // .. ..     ==> MASK : 0x000000F0U    VAL : 0x00000000U
    // .. .. reg_phy_rdc_we_to_re_delay = 0x8
    // .. .. ==> 0XF8006050[11:8] = 0x00000008U
    // .. ..     ==> MASK : 0x00000F00U    VAL : 0x00000800U
    // .. .. reg_phy_rdc_fifo_rst_disable = 0x0
    // .. .. ==> 0XF8006050[15:15] = 0x00000000U
    // .. ..     ==> MASK : 0x00008000U    VAL : 0x00000000U
    // .. .. reg_phy_use_fixed_re = 0x1
    // .. .. ==> 0XF8006050[16:16] = 0x00000001U
    // .. ..     ==> MASK : 0x00010000U    VAL : 0x00010000U
    // .. .. reg_phy_rdc_fifo_rst_err_cnt_clr = 0x0
    // .. .. ==> 0XF8006050[17:17] = 0x00000000U
    // .. ..     ==> MASK : 0x00020000U    VAL : 0x00000000U
    // .. .. reg_phy_dis_phy_ctrl_rstn = 0x0
    // .. .. ==> 0XF8006050[18:18] = 0x00000000U
    // .. ..     ==> MASK : 0x00040000U    VAL : 0x00000000U
    // .. .. reg_phy_clk_stall_level = 0x0
    // .. .. ==> 0XF8006050[19:19] = 0x00000000U
    // .. ..     ==> MASK : 0x00080000U    VAL : 0x00000000U
    // .. .. reg_phy_gatelvl_num_of_dq0 = 0x7
    // .. .. ==> 0XF8006050[27:24] = 0x00000007U
    // .. ..     ==> MASK : 0x0F000000U    VAL : 0x07000000U
    // .. .. reg_phy_wrlvl_num_of_dq0 = 0x7
    // .. .. ==> 0XF8006050[31:28] = 0x00000007U
    // .. ..     ==> MASK : 0xF0000000U    VAL : 0x70000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006050, 0xFF0F8FFFU ,0x77010800U),
    // .. .. reg_ddrc_dis_dll_calib = 0x0
    // .. .. ==> 0XF8006058[16:16] = 0x00000000U
    // .. ..     ==> MASK : 0x00010000U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006058, 0x00010000U ,0x00000000U),
    // .. .. reg_ddrc_rd_odt_delay = 0x3
    // .. .. ==> 0XF800605C[3:0] = 0x00000003U
    // .. ..     ==> MASK : 0x0000000FU    VAL : 0x00000003U
    // .. .. reg_ddrc_wr_odt_delay = 0x0
    // .. .. ==> 0XF800605C[7:4] = 0x00000000U
    // .. ..     ==> MASK : 0x000000F0U    VAL : 0x00000000U
    // .. .. reg_ddrc_rd_odt_hold = 0x0
    // .. .. ==> 0XF800605C[11:8] = 0x00000000U
    // .. ..     ==> MASK : 0x00000F00U    VAL : 0x00000000U
    // .. .. reg_ddrc_wr_odt_hold = 0x5
    // .. .. ==> 0XF800605C[15:12] = 0x00000005U
    // .. ..     ==> MASK : 0x0000F000U    VAL : 0x00005000U
    // .. ..
    EMIT_MASKWRITE(0XF800605C, 0x0000FFFFU ,0x00005003U),
    // .. .. reg_ddrc_pageclose = 0x0
    // .. .. ==> 0XF8006060[0:0] = 0x00000000U
    // .. ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. .. reg_ddrc_lpr_num_entries = 0x1f
    // .. .. ==> 0XF8006060[6:1] = 0x0000001FU
    // .. ..     ==> MASK : 0x0000007EU    VAL : 0x0000003EU
    // .. .. reg_ddrc_auto_pre_en = 0x0
    // .. .. ==> 0XF8006060[7:7] = 0x00000000U
    // .. ..     ==> MASK : 0x00000080U    VAL : 0x00000000U
    // .. .. reg_ddrc_refresh_update_level = 0x0
    // .. .. ==> 0XF8006060[8:8] = 0x00000000U
    // .. ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. .. reg_ddrc_dis_wc = 0x0
    // .. .. ==> 0XF8006060[9:9] = 0x00000000U
    // .. ..     ==> MASK : 0x00000200U    VAL : 0x00000000U
    // .. .. reg_ddrc_dis_collision_page_opt = 0x0
    // .. .. ==> 0XF8006060[10:10] = 0x00000000U
    // .. ..     ==> MASK : 0x00000400U    VAL : 0x00000000U
    // .. .. reg_ddrc_selfref_en = 0x0
    // .. .. ==> 0XF8006060[12:12] = 0x00000000U
    // .. ..     ==> MASK : 0x00001000U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006060, 0x000017FFU ,0x0000003EU),
    // .. .. reg_ddrc_go2critical_hysteresis = 0x0
    // .. .. ==> 0XF8006064[12:5] = 0x00000000U
    // .. ..     ==> MASK : 0x00001FE0U    VAL : 0x00000000U
    // .. .. reg_arb_go2critical_en = 0x1
    // .. .. ==> 0XF8006064[17:17] = 0x00000001U
    // .. ..     ==> MASK : 0x00020000U    VAL : 0x00020000U
    // .. ..
    EMIT_MASKWRITE(0XF8006064, 0x00021FE0U ,0x00020000U),
    // .. .. reg_ddrc_wrlvl_ww = 0x41
    // .. .. ==> 0XF8006068[7:0] = 0x00000041U
    // .. ..     ==> MASK : 0x000000FFU    VAL : 0x00000041U
    // .. .. reg_ddrc_rdlvl_rr = 0x41
    // .. .. ==> 0XF8006068[15:8] = 0x00000041U
    // .. ..     ==> MASK : 0x0000FF00U    VAL : 0x00004100U
    // .. .. reg_ddrc_dfi_t_wlmrd = 0x28
    // .. .. ==> 0XF8006068[25:16] = 0x00000028U
    // .. ..     ==> MASK : 0x03FF0000U    VAL : 0x00280000U
    // .. ..
    EMIT_MASKWRITE(0XF8006068, 0x03FFFFFFU ,0x00284141U),
    // .. .. dfi_t_ctrlupd_interval_min_x1024 = 0x10
    // .. .. ==> 0XF800606C[7:0] = 0x00000010U
    // .. ..     ==> MASK : 0x000000FFU    VAL : 0x00000010U
    // .. .. dfi_t_ctrlupd_interval_max_x1024 = 0x16
    // .. .. ==> 0XF800606C[15:8] = 0x00000016U
    // .. ..     ==> MASK : 0x0000FF00U    VAL : 0x00001600U
    // .. ..
    EMIT_MASKWRITE(0XF800606C, 0x0000FFFFU ,0x00001610U),
    // .. .. reg_ddrc_dfi_t_ctrl_delay = 0x1
    // .. .. ==> 0XF8006078[3:0] = 0x00000001U
    // .. ..     ==> MASK : 0x0000000FU    VAL : 0x00000001U
    // .. .. reg_ddrc_dfi_t_dram_clk_disable = 0x1
    // .. .. ==> 0XF8006078[7:4] = 0x00000001U
    // .. ..     ==> MASK : 0x000000F0U    VAL : 0x00000010U
    // .. .. reg_ddrc_dfi_t_dram_clk_enable = 0x1
    // .. .. ==> 0XF8006078[11:8] = 0x00000001U
    // .. ..     ==> MASK : 0x00000F00U    VAL : 0x00000100U
    // .. .. reg_ddrc_t_cksre = 0x6
    // .. .. ==> 0XF8006078[15:12] = 0x00000006U
    // .. ..     ==> MASK : 0x0000F000U    VAL : 0x00006000U
    // .. .. reg_ddrc_t_cksrx = 0x6
    // .. .. ==> 0XF8006078[19:16] = 0x00000006U
    // .. ..     ==> MASK : 0x000F0000U    VAL : 0x00060000U
    // .. .. reg_ddrc_t_ckesr = 0x4
    // .. .. ==> 0XF8006078[25:20] = 0x00000004U
    // .. ..     ==> MASK : 0x03F00000U    VAL : 0x00400000U
    // .. ..
    EMIT_MASKWRITE(0XF8006078, 0x03FFFFFFU ,0x00466111U),
    // .. .. reg_ddrc_t_ckpde = 0x2
    // .. .. ==> 0XF800607C[3:0] = 0x00000002U
    // .. ..     ==> MASK : 0x0000000FU    VAL : 0x00000002U
    // .. .. reg_ddrc_t_ckpdx = 0x2
    // .. .. ==> 0XF800607C[7:4] = 0x00000002U
    // .. ..     ==> MASK : 0x000000F0U    VAL : 0x00000020U
    // .. .. reg_ddrc_t_ckdpde = 0x2
    // .. .. ==> 0XF800607C[11:8] = 0x00000002U
    // .. ..     ==> MASK : 0x00000F00U    VAL : 0x00000200U
    // .. .. reg_ddrc_t_ckdpdx = 0x2
    // .. .. ==> 0XF800607C[15:12] = 0x00000002U
    // .. ..     ==> MASK : 0x0000F000U    VAL : 0x00002000U
    // .. .. reg_ddrc_t_ckcsx = 0x3
    // .. .. ==> 0XF800607C[19:16] = 0x00000003U
    // .. ..     ==> MASK : 0x000F0000U    VAL : 0x00030000U
    // .. ..
    EMIT_MASKWRITE(0XF800607C, 0x000FFFFFU ,0x00032222U),
    // .. .. reg_ddrc_dis_auto_zq = 0x0
    // .. .. ==> 0XF80060A4[0:0] = 0x00000000U
    // .. ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. .. reg_ddrc_ddr3 = 0x1
    // .. .. ==> 0XF80060A4[1:1] = 0x00000001U
    // .. ..     ==> MASK : 0x00000002U    VAL : 0x00000002U
    // .. .. reg_ddrc_t_mod = 0x200
    // .. .. ==> 0XF80060A4[11:2] = 0x00000200U
    // .. ..     ==> MASK : 0x00000FFCU    VAL : 0x00000800U
    // .. .. reg_ddrc_t_zq_long_nop = 0x200
    // .. .. ==> 0XF80060A4[21:12] = 0x00000200U
    // .. ..     ==> MASK : 0x003FF000U    VAL : 0x00200000U
    // .. .. reg_ddrc_t_zq_short_nop = 0x40
    // .. .. ==> 0XF80060A4[31:22] = 0x00000040U
    // .. ..     ==> MASK : 0xFFC00000U    VAL : 0x10000000U
    // .. ..
    EMIT_MASKWRITE(0XF80060A4, 0xFFFFFFFFU ,0x10200802U),
    // .. .. t_zq_short_interval_x1024 = 0xcb73
    // .. .. ==> 0XF80060A8[19:0] = 0x0000CB73U
    // .. ..     ==> MASK : 0x000FFFFFU    VAL : 0x0000CB73U
    // .. .. dram_rstn_x1024 = 0x69
    // .. .. ==> 0XF80060A8[27:20] = 0x00000069U
    // .. ..     ==> MASK : 0x0FF00000U    VAL : 0x06900000U
    // .. ..
    EMIT_MASKWRITE(0XF80060A8, 0x0FFFFFFFU ,0x0690CB73U),
    // .. .. deeppowerdown_en = 0x0
    // .. .. ==> 0XF80060AC[0:0] = 0x00000000U
    // .. ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. .. deeppowerdown_to_x1024 = 0xff
    // .. .. ==> 0XF80060AC[8:1] = 0x000000FFU
    // .. ..     ==> MASK : 0x000001FEU    VAL : 0x000001FEU
    // .. ..
    EMIT_MASKWRITE(0XF80060AC, 0x000001FFU ,0x000001FEU),
    // .. .. dfi_wrlvl_max_x1024 = 0xfff
    // .. .. ==> 0XF80060B0[11:0] = 0x00000FFFU
    // .. ..     ==> MASK : 0x00000FFFU    VAL : 0x00000FFFU
    // .. .. dfi_rdlvl_max_x1024 = 0xfff
    // .. .. ==> 0XF80060B0[23:12] = 0x00000FFFU
    // .. ..     ==> MASK : 0x00FFF000U    VAL : 0x00FFF000U
    // .. .. ddrc_reg_twrlvl_max_error = 0x0
    // .. .. ==> 0XF80060B0[24:24] = 0x00000000U
    // .. ..     ==> MASK : 0x01000000U    VAL : 0x00000000U
    // .. .. ddrc_reg_trdlvl_max_error = 0x0
    // .. .. ==> 0XF80060B0[25:25] = 0x00000000U
    // .. ..     ==> MASK : 0x02000000U    VAL : 0x00000000U
    // .. .. reg_ddrc_dfi_wr_level_en = 0x1
    // .. .. ==> 0XF80060B0[26:26] = 0x00000001U
    // .. ..     ==> MASK : 0x04000000U    VAL : 0x04000000U
    // .. .. reg_ddrc_dfi_rd_dqs_gate_level = 0x1
    // .. .. ==> 0XF80060B0[27:27] = 0x00000001U
    // .. ..     ==> MASK : 0x08000000U    VAL : 0x08000000U
    // .. .. reg_ddrc_dfi_rd_data_eye_train = 0x1
    // .. .. ==> 0XF80060B0[28:28] = 0x00000001U
    // .. ..     ==> MASK : 0x10000000U    VAL : 0x10000000U
    // .. ..
    EMIT_MASKWRITE(0XF80060B0, 0x1FFFFFFFU ,0x1CFFFFFFU),
    // .. .. reg_ddrc_skip_ocd = 0x1
    // .. .. ==> 0XF80060B4[9:9] = 0x00000001U
    // .. ..     ==> MASK : 0x00000200U    VAL : 0x00000200U
    // .. ..
    EMIT_MASKWRITE(0XF80060B4, 0x00000200U ,0x00000200U),
    // .. .. reg_ddrc_dfi_t_rddata_en = 0x6
    // .. .. ==> 0XF80060B8[4:0] = 0x00000006U
    // .. ..     ==> MASK : 0x0000001FU    VAL : 0x00000006U
    // .. .. reg_ddrc_dfi_t_ctrlup_min = 0x3
    // .. .. ==> 0XF80060B8[14:5] = 0x00000003U
    // .. ..     ==> MASK : 0x00007FE0U    VAL : 0x00000060U
    // .. .. reg_ddrc_dfi_t_ctrlup_max = 0x40
    // .. .. ==> 0XF80060B8[24:15] = 0x00000040U
    // .. ..     ==> MASK : 0x01FF8000U    VAL : 0x00200000U
    // .. ..
    EMIT_MASKWRITE(0XF80060B8, 0x01FFFFFFU ,0x00200066U),
    // .. .. Clear_Uncorrectable_DRAM_ECC_error = 0x0
    // .. .. ==> 0XF80060C4[0:0] = 0x00000000U
    // .. ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. .. Clear_Correctable_DRAM_ECC_error = 0x0
    // .. .. ==> 0XF80060C4[1:1] = 0x00000000U
    // .. ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF80060C4, 0x00000003U ,0x00000000U),
    // .. .. CORR_ECC_LOG_VALID = 0x0
    // .. .. ==> 0XF80060C8[0:0] = 0x00000000U
    // .. ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. .. ECC_CORRECTED_BIT_NUM = 0x0
    // .. .. ==> 0XF80060C8[7:1] = 0x00000000U
    // .. ..     ==> MASK : 0x000000FEU    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF80060C8, 0x000000FFU ,0x00000000U),
    // .. .. UNCORR_ECC_LOG_VALID = 0x0
    // .. .. ==> 0XF80060DC[0:0] = 0x00000000U
    // .. ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF80060DC, 0x00000001U ,0x00000000U),
    // .. .. STAT_NUM_CORR_ERR = 0x0
    // .. .. ==> 0XF80060F0[15:8] = 0x00000000U
    // .. ..     ==> MASK : 0x0000FF00U    VAL : 0x00000000U
    // .. .. STAT_NUM_UNCORR_ERR = 0x0
    // .. .. ==> 0XF80060F0[7:0] = 0x00000000U
    // .. ..     ==> MASK : 0x000000FFU    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF80060F0, 0x0000FFFFU ,0x00000000U),
    // .. .. reg_ddrc_ecc_mode = 0x0
    // .. .. ==> 0XF80060F4[2:0] = 0x00000000U
    // .. ..     ==> MASK : 0x00000007U    VAL : 0x00000000U
    // .. .. reg_ddrc_dis_scrub = 0x1
    // .. .. ==> 0XF80060F4[3:3] = 0x00000001U
    // .. ..     ==> MASK : 0x00000008U    VAL : 0x00000008U
    // .. ..
    EMIT_MASKWRITE(0XF80060F4, 0x0000000FU ,0x00000008U),
    // .. .. reg_phy_dif_on = 0x0
    // .. .. ==> 0XF8006114[3:0] = 0x00000000U
    // .. ..     ==> MASK : 0x0000000FU    VAL : 0x00000000U
    // .. .. reg_phy_dif_off = 0x0
    // .. .. ==> 0XF8006114[7:4] = 0x00000000U
    // .. ..     ==> MASK : 0x000000F0U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006114, 0x000000FFU ,0x00000000U),
    // .. .. reg_phy_data_slice_in_use = 0x1
    // .. .. ==> 0XF8006118[0:0] = 0x00000001U
    // .. ..     ==> MASK : 0x00000001U    VAL : 0x00000001U
    // .. .. reg_phy_rdlvl_inc_mode = 0x0
    // .. .. ==> 0XF8006118[1:1] = 0x00000000U
    // .. ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. .. reg_phy_gatelvl_inc_mode = 0x0
    // .. .. ==> 0XF8006118[2:2] = 0x00000000U
    // .. ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. .. reg_phy_wrlvl_inc_mode = 0x0
    // .. .. ==> 0XF8006118[3:3] = 0x00000000U
    // .. ..     ==> MASK : 0x00000008U    VAL : 0x00000000U
    // .. .. reg_phy_bist_shift_dq = 0x0
    // .. .. ==> 0XF8006118[14:6] = 0x00000000U
    // .. ..     ==> MASK : 0x00007FC0U    VAL : 0x00000000U
    // .. .. reg_phy_bist_err_clr = 0x0
    // .. .. ==> 0XF8006118[23:15] = 0x00000000U
    // .. ..     ==> MASK : 0x00FF8000U    VAL : 0x00000000U
    // .. .. reg_phy_dq_offset = 0x40
    // .. .. ==> 0XF8006118[30:24] = 0x00000040U
    // .. ..     ==> MASK : 0x7F000000U    VAL : 0x40000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006118, 0x7FFFFFCFU ,0x40000001U),
    // .. .. reg_phy_data_slice_in_use = 0x1
    // .. .. ==> 0XF800611C[0:0] = 0x00000001U
    // .. ..     ==> MASK : 0x00000001U    VAL : 0x00000001U
    // .. .. reg_phy_rdlvl_inc_mode = 0x0
    // .. .. ==> 0XF800611C[1:1] = 0x00000000U
    // .. ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. .. reg_phy_gatelvl_inc_mode = 0x0
    // .. .. ==> 0XF800611C[2:2] = 0x00000000U
    // .. ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. .. reg_phy_wrlvl_inc_mode = 0x0
    // .. .. ==> 0XF800611C[3:3] = 0x00000000U
    // .. ..     ==> MASK : 0x00000008U    VAL : 0x00000000U
    // .. .. reg_phy_bist_shift_dq = 0x0
    // .. .. ==> 0XF800611C[14:6] = 0x00000000U
    // .. ..     ==> MASK : 0x00007FC0U    VAL : 0x00000000U
    // .. .. reg_phy_bist_err_clr = 0x0
    // .. .. ==> 0XF800611C[23:15] = 0x00000000U
    // .. ..     ==> MASK : 0x00FF8000U    VAL : 0x00000000U
    // .. .. reg_phy_dq_offset = 0x40
    // .. .. ==> 0XF800611C[30:24] = 0x00000040U
    // .. ..     ==> MASK : 0x7F000000U    VAL : 0x40000000U
    // .. ..
    EMIT_MASKWRITE(0XF800611C, 0x7FFFFFCFU ,0x40000001U),
    // .. .. reg_phy_data_slice_in_use = 0x1
    // .. .. ==> 0XF8006120[0:0] = 0x00000001U
    // .. ..     ==> MASK : 0x00000001U    VAL : 0x00000001U
    // .. .. reg_phy_rdlvl_inc_mode = 0x0
    // .. .. ==> 0XF8006120[1:1] = 0x00000000U
    // .. ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. .. reg_phy_gatelvl_inc_mode = 0x0
    // .. .. ==> 0XF8006120[2:2] = 0x00000000U
    // .. ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. .. reg_phy_wrlvl_inc_mode = 0x0
    // .. .. ==> 0XF8006120[3:3] = 0x00000000U
    // .. ..     ==> MASK : 0x00000008U    VAL : 0x00000000U
    // .. .. reg_phy_bist_shift_dq = 0x0
    // .. .. ==> 0XF8006120[14:6] = 0x00000000U
    // .. ..     ==> MASK : 0x00007FC0U    VAL : 0x00000000U
    // .. .. reg_phy_bist_err_clr = 0x0
    // .. .. ==> 0XF8006120[23:15] = 0x00000000U
    // .. ..     ==> MASK : 0x00FF8000U    VAL : 0x00000000U
    // .. .. reg_phy_dq_offset = 0x40
    // .. .. ==> 0XF8006120[30:24] = 0x00000040U
    // .. ..     ==> MASK : 0x7F000000U    VAL : 0x40000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006120, 0x7FFFFFCFU ,0x40000001U),
    // .. .. reg_phy_data_slice_in_use = 0x1
    // .. .. ==> 0XF8006124[0:0] = 0x00000001U
    // .. ..     ==> MASK : 0x00000001U    VAL : 0x00000001U
    // .. .. reg_phy_rdlvl_inc_mode = 0x0
    // .. .. ==> 0XF8006124[1:1] = 0x00000000U
    // .. ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. .. reg_phy_gatelvl_inc_mode = 0x0
    // .. .. ==> 0XF8006124[2:2] = 0x00000000U
    // .. ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. .. reg_phy_wrlvl_inc_mode = 0x0
    // .. .. ==> 0XF8006124[3:3] = 0x00000000U
    // .. ..     ==> MASK : 0x00000008U    VAL : 0x00000000U
    // .. .. reg_phy_bist_shift_dq = 0x0
    // .. .. ==> 0XF8006124[14:6] = 0x00000000U
    // .. ..     ==> MASK : 0x00007FC0U    VAL : 0x00000000U
    // .. .. reg_phy_bist_err_clr = 0x0
    // .. .. ==> 0XF8006124[23:15] = 0x00000000U
    // .. ..     ==> MASK : 0x00FF8000U    VAL : 0x00000000U
    // .. .. reg_phy_dq_offset = 0x40
    // .. .. ==> 0XF8006124[30:24] = 0x00000040U
    // .. ..     ==> MASK : 0x7F000000U    VAL : 0x40000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006124, 0x7FFFFFCFU ,0x40000001U),
    // .. .. reg_phy_wrlvl_init_ratio = 0x0
    // .. .. ==> 0XF800612C[9:0] = 0x00000000U
    // .. ..     ==> MASK : 0x000003FFU    VAL : 0x00000000U
    // .. .. reg_phy_gatelvl_init_ratio = 0xa4
    // .. .. ==> 0XF800612C[19:10] = 0x000000A4U
    // .. ..     ==> MASK : 0x000FFC00U    VAL : 0x00029000U
    // .. ..
    EMIT_MASKWRITE(0XF800612C, 0x000FFFFFU ,0x00029000U),
    // .. .. reg_phy_wrlvl_init_ratio = 0x0
    // .. .. ==> 0XF8006130[9:0] = 0x00000000U
    // .. ..     ==> MASK : 0x000003FFU    VAL : 0x00000000U
    // .. .. reg_phy_gatelvl_init_ratio = 0xa4
    // .. .. ==> 0XF8006130[19:10] = 0x000000A4U
    // .. ..     ==> MASK : 0x000FFC00U    VAL : 0x00029000U
    // .. ..
    EMIT_MASKWRITE(0XF8006130, 0x000FFFFFU ,0x00029000U),
    // .. .. reg_phy_wrlvl_init_ratio = 0x0
    // .. .. ==> 0XF8006134[9:0] = 0x00000000U
    // .. ..     ==> MASK : 0x000003FFU    VAL : 0x00000000U
    // .. .. reg_phy_gatelvl_init_ratio = 0xa4
    // .. .. ==> 0XF8006134[19:10] = 0x000000A4U
    // .. ..     ==> MASK : 0x000FFC00U    VAL : 0x00029000U
    // .. ..
    EMIT_MASKWRITE(0XF8006134, 0x000FFFFFU ,0x00029000U),
    // .. .. reg_phy_wrlvl_init_ratio = 0x0
    // .. .. ==> 0XF8006138[9:0] = 0x00000000U
    // .. ..     ==> MASK : 0x000003FFU    VAL : 0x00000000U
    // .. .. reg_phy_gatelvl_init_ratio = 0xa4
    // .. .. ==> 0XF8006138[19:10] = 0x000000A4U
    // .. ..     ==> MASK : 0x000FFC00U    VAL : 0x00029000U
    // .. ..
    EMIT_MASKWRITE(0XF8006138, 0x000FFFFFU ,0x00029000U),
    // .. .. reg_phy_rd_dqs_slave_ratio = 0x35
    // .. .. ==> 0XF8006140[9:0] = 0x00000035U
    // .. ..     ==> MASK : 0x000003FFU    VAL : 0x00000035U
    // .. .. reg_phy_rd_dqs_slave_force = 0x0
    // .. .. ==> 0XF8006140[10:10] = 0x00000000U
    // .. ..     ==> MASK : 0x00000400U    VAL : 0x00000000U
    // .. .. reg_phy_rd_dqs_slave_delay = 0x0
    // .. .. ==> 0XF8006140[19:11] = 0x00000000U
    // .. ..     ==> MASK : 0x000FF800U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006140, 0x000FFFFFU ,0x00000035U),
    // .. .. reg_phy_rd_dqs_slave_ratio = 0x35
    // .. .. ==> 0XF8006144[9:0] = 0x00000035U
    // .. ..     ==> MASK : 0x000003FFU    VAL : 0x00000035U
    // .. .. reg_phy_rd_dqs_slave_force = 0x0
    // .. .. ==> 0XF8006144[10:10] = 0x00000000U
    // .. ..     ==> MASK : 0x00000400U    VAL : 0x00000000U
    // .. .. reg_phy_rd_dqs_slave_delay = 0x0
    // .. .. ==> 0XF8006144[19:11] = 0x00000000U
    // .. ..     ==> MASK : 0x000FF800U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006144, 0x000FFFFFU ,0x00000035U),
    // .. .. reg_phy_rd_dqs_slave_ratio = 0x35
    // .. .. ==> 0XF8006148[9:0] = 0x00000035U
    // .. ..     ==> MASK : 0x000003FFU    VAL : 0x00000035U
    // .. .. reg_phy_rd_dqs_slave_force = 0x0
    // .. .. ==> 0XF8006148[10:10] = 0x00000000U
    // .. ..     ==> MASK : 0x00000400U    VAL : 0x00000000U
    // .. .. reg_phy_rd_dqs_slave_delay = 0x0
    // .. .. ==> 0XF8006148[19:11] = 0x00000000U
    // .. ..     ==> MASK : 0x000FF800U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006148, 0x000FFFFFU ,0x00000035U),
    // .. .. reg_phy_rd_dqs_slave_ratio = 0x35
    // .. .. ==> 0XF800614C[9:0] = 0x00000035U
    // .. ..     ==> MASK : 0x000003FFU    VAL : 0x00000035U
    // .. .. reg_phy_rd_dqs_slave_force = 0x0
    // .. .. ==> 0XF800614C[10:10] = 0x00000000U
    // .. ..     ==> MASK : 0x00000400U    VAL : 0x00000000U
    // .. .. reg_phy_rd_dqs_slave_delay = 0x0
    // .. .. ==> 0XF800614C[19:11] = 0x00000000U
    // .. ..     ==> MASK : 0x000FF800U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF800614C, 0x000FFFFFU ,0x00000035U),
    // .. .. reg_phy_wr_dqs_slave_ratio = 0x80
    // .. .. ==> 0XF8006154[9:0] = 0x00000080U
    // .. ..     ==> MASK : 0x000003FFU    VAL : 0x00000080U
    // .. .. reg_phy_wr_dqs_slave_force = 0x0
    // .. .. ==> 0XF8006154[10:10] = 0x00000000U
    // .. ..     ==> MASK : 0x00000400U    VAL : 0x00000000U
    // .. .. reg_phy_wr_dqs_slave_delay = 0x0
    // .. .. ==> 0XF8006154[19:11] = 0x00000000U
    // .. ..     ==> MASK : 0x000FF800U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006154, 0x000FFFFFU ,0x00000080U),
    // .. .. reg_phy_wr_dqs_slave_ratio = 0x80
    // .. .. ==> 0XF8006158[9:0] = 0x00000080U
    // .. ..     ==> MASK : 0x000003FFU    VAL : 0x00000080U
    // .. .. reg_phy_wr_dqs_slave_force = 0x0
    // .. .. ==> 0XF8006158[10:10] = 0x00000000U
    // .. ..     ==> MASK : 0x00000400U    VAL : 0x00000000U
    // .. .. reg_phy_wr_dqs_slave_delay = 0x0
    // .. .. ==> 0XF8006158[19:11] = 0x00000000U
    // .. ..     ==> MASK : 0x000FF800U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006158, 0x000FFFFFU ,0x00000080U),
    // .. .. reg_phy_wr_dqs_slave_ratio = 0x80
    // .. .. ==> 0XF800615C[9:0] = 0x00000080U
    // .. ..     ==> MASK : 0x000003FFU    VAL : 0x00000080U
    // .. .. reg_phy_wr_dqs_slave_force = 0x0
    // .. .. ==> 0XF800615C[10:10] = 0x00000000U
    // .. ..     ==> MASK : 0x00000400U    VAL : 0x00000000U
    // .. .. reg_phy_wr_dqs_slave_delay = 0x0
    // .. .. ==> 0XF800615C[19:11] = 0x00000000U
    // .. ..     ==> MASK : 0x000FF800U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF800615C, 0x000FFFFFU ,0x00000080U),
    // .. .. reg_phy_wr_dqs_slave_ratio = 0x80
    // .. .. ==> 0XF8006160[9:0] = 0x00000080U
    // .. ..     ==> MASK : 0x000003FFU    VAL : 0x00000080U
    // .. .. reg_phy_wr_dqs_slave_force = 0x0
    // .. .. ==> 0XF8006160[10:10] = 0x00000000U
    // .. ..     ==> MASK : 0x00000400U    VAL : 0x00000000U
    // .. .. reg_phy_wr_dqs_slave_delay = 0x0
    // .. .. ==> 0XF8006160[19:11] = 0x00000000U
    // .. ..     ==> MASK : 0x000FF800U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006160, 0x000FFFFFU ,0x00000080U),
    // .. .. reg_phy_fifo_we_slave_ratio = 0xf9
    // .. .. ==> 0XF8006168[10:0] = 0x000000F9U
    // .. ..     ==> MASK : 0x000007FFU    VAL : 0x000000F9U
    // .. .. reg_phy_fifo_we_in_force = 0x0
    // .. .. ==> 0XF8006168[11:11] = 0x00000000U
    // .. ..     ==> MASK : 0x00000800U    VAL : 0x00000000U
    // .. .. reg_phy_fifo_we_in_delay = 0x0
    // .. .. ==> 0XF8006168[20:12] = 0x00000000U
    // .. ..     ==> MASK : 0x001FF000U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006168, 0x001FFFFFU ,0x000000F9U),
    // .. .. reg_phy_fifo_we_slave_ratio = 0xf9
    // .. .. ==> 0XF800616C[10:0] = 0x000000F9U
    // .. ..     ==> MASK : 0x000007FFU    VAL : 0x000000F9U
    // .. .. reg_phy_fifo_we_in_force = 0x0
    // .. .. ==> 0XF800616C[11:11] = 0x00000000U
    // .. ..     ==> MASK : 0x00000800U    VAL : 0x00000000U
    // .. .. reg_phy_fifo_we_in_delay = 0x0
    // .. .. ==> 0XF800616C[20:12] = 0x00000000U
    // .. ..     ==> MASK : 0x001FF000U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF800616C, 0x001FFFFFU ,0x000000F9U),
    // .. .. reg_phy_fifo_we_slave_ratio = 0xf9
    // .. .. ==> 0XF8006170[10:0] = 0x000000F9U
    // .. ..     ==> MASK : 0x000007FFU    VAL : 0x000000F9U
    // .. .. reg_phy_fifo_we_in_force = 0x0
    // .. .. ==> 0XF8006170[11:11] = 0x00000000U
    // .. ..     ==> MASK : 0x00000800U    VAL : 0x00000000U
    // .. .. reg_phy_fifo_we_in_delay = 0x0
    // .. .. ==> 0XF8006170[20:12] = 0x00000000U
    // .. ..     ==> MASK : 0x001FF000U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006170, 0x001FFFFFU ,0x000000F9U),
    // .. .. reg_phy_fifo_we_slave_ratio = 0xf9
    // .. .. ==> 0XF8006174[10:0] = 0x000000F9U
    // .. ..     ==> MASK : 0x000007FFU    VAL : 0x000000F9U
    // .. .. reg_phy_fifo_we_in_force = 0x0
    // .. .. ==> 0XF8006174[11:11] = 0x00000000U
    // .. ..     ==> MASK : 0x00000800U    VAL : 0x00000000U
    // .. .. reg_phy_fifo_we_in_delay = 0x0
    // .. .. ==> 0XF8006174[20:12] = 0x00000000U
    // .. ..     ==> MASK : 0x001FF000U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006174, 0x001FFFFFU ,0x000000F9U),
    // .. .. reg_phy_wr_data_slave_ratio = 0xc0
    // .. .. ==> 0XF800617C[9:0] = 0x000000C0U
    // .. ..     ==> MASK : 0x000003FFU    VAL : 0x000000C0U
    // .. .. reg_phy_wr_data_slave_force = 0x0
    // .. .. ==> 0XF800617C[10:10] = 0x00000000U
    // .. ..     ==> MASK : 0x00000400U    VAL : 0x00000000U
    // .. .. reg_phy_wr_data_slave_delay = 0x0
    // .. .. ==> 0XF800617C[19:11] = 0x00000000U
    // .. ..     ==> MASK : 0x000FF800U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF800617C, 0x000FFFFFU ,0x000000C0U),
    // .. .. reg_phy_wr_data_slave_ratio = 0xc0
    // .. .. ==> 0XF8006180[9:0] = 0x000000C0U
    // .. ..     ==> MASK : 0x000003FFU    VAL : 0x000000C0U
    // .. .. reg_phy_wr_data_slave_force = 0x0
    // .. .. ==> 0XF8006180[10:10] = 0x00000000U
    // .. ..     ==> MASK : 0x00000400U    VAL : 0x00000000U
    // .. .. reg_phy_wr_data_slave_delay = 0x0
    // .. .. ==> 0XF8006180[19:11] = 0x00000000U
    // .. ..     ==> MASK : 0x000FF800U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006180, 0x000FFFFFU ,0x000000C0U),
    // .. .. reg_phy_wr_data_slave_ratio = 0xc0
    // .. .. ==> 0XF8006184[9:0] = 0x000000C0U
    // .. ..     ==> MASK : 0x000003FFU    VAL : 0x000000C0U
    // .. .. reg_phy_wr_data_slave_force = 0x0
    // .. .. ==> 0XF8006184[10:10] = 0x00000000U
    // .. ..     ==> MASK : 0x00000400U    VAL : 0x00000000U
    // .. .. reg_phy_wr_data_slave_delay = 0x0
    // .. .. ==> 0XF8006184[19:11] = 0x00000000U
    // .. ..     ==> MASK : 0x000FF800U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006184, 0x000FFFFFU ,0x000000C0U),
    // .. .. reg_phy_wr_data_slave_ratio = 0xc0
    // .. .. ==> 0XF8006188[9:0] = 0x000000C0U
    // .. ..     ==> MASK : 0x000003FFU    VAL : 0x000000C0U
    // .. .. reg_phy_wr_data_slave_force = 0x0
    // .. .. ==> 0XF8006188[10:10] = 0x00000000U
    // .. ..     ==> MASK : 0x00000400U    VAL : 0x00000000U
    // .. .. reg_phy_wr_data_slave_delay = 0x0
    // .. .. ==> 0XF8006188[19:11] = 0x00000000U
    // .. ..     ==> MASK : 0x000FF800U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006188, 0x000FFFFFU ,0x000000C0U),
    // .. .. reg_phy_bl2 = 0x0
    // .. .. ==> 0XF8006190[1:1] = 0x00000000U
    // .. ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. .. reg_phy_at_spd_atpg = 0x0
    // .. .. ==> 0XF8006190[2:2] = 0x00000000U
    // .. ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. .. reg_phy_bist_enable = 0x0
    // .. .. ==> 0XF8006190[3:3] = 0x00000000U
    // .. ..     ==> MASK : 0x00000008U    VAL : 0x00000000U
    // .. .. reg_phy_bist_force_err = 0x0
    // .. .. ==> 0XF8006190[4:4] = 0x00000000U
    // .. ..     ==> MASK : 0x00000010U    VAL : 0x00000000U
    // .. .. reg_phy_bist_mode = 0x0
    // .. .. ==> 0XF8006190[6:5] = 0x00000000U
    // .. ..     ==> MASK : 0x00000060U    VAL : 0x00000000U
    // .. .. reg_phy_invert_clkout = 0x1
    // .. .. ==> 0XF8006190[7:7] = 0x00000001U
    // .. ..     ==> MASK : 0x00000080U    VAL : 0x00000080U
    // .. .. reg_phy_sel_logic = 0x0
    // .. .. ==> 0XF8006190[9:9] = 0x00000000U
    // .. ..     ==> MASK : 0x00000200U    VAL : 0x00000000U
    // .. .. reg_phy_ctrl_slave_ratio = 0x100
    // .. .. ==> 0XF8006190[19:10] = 0x00000100U
    // .. ..     ==> MASK : 0x000FFC00U    VAL : 0x00040000U
    // .. .. reg_phy_ctrl_slave_force = 0x0
    // .. .. ==> 0XF8006190[20:20] = 0x00000000U
    // .. ..     ==> MASK : 0x00100000U    VAL : 0x00000000U
    // .. .. reg_phy_ctrl_slave_delay = 0x0
    // .. .. ==> 0XF8006190[27:21] = 0x00000000U
    // .. ..     ==> MASK : 0x0FE00000U    VAL : 0x00000000U
    // .. .. reg_phy_lpddr = 0x0
    // .. .. ==> 0XF8006190[29:29] = 0x00000000U
    // .. ..     ==> MASK : 0x20000000U    VAL : 0x00000000U
    // .. .. reg_phy_cmd_latency = 0x0
    // .. .. ==> 0XF8006190[30:30] = 0x00000000U
    // .. ..     ==> MASK : 0x40000000U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006190, 0x6FFFFEFEU ,0x00040080U),
    // .. .. reg_phy_wr_rl_delay = 0x2
    // .. .. ==> 0XF8006194[4:0] = 0x00000002U
    // .. ..     ==> MASK : 0x0000001FU    VAL : 0x00000002U
    // .. .. reg_phy_rd_rl_delay = 0x4
    // .. .. ==> 0XF8006194[9:5] = 0x00000004U
    // .. ..     ==> MASK : 0x000003E0U    VAL : 0x00000080U
    // .. .. reg_phy_dll_lock_diff = 0xf
    // .. .. ==> 0XF8006194[13:10] = 0x0000000FU
    // .. ..     ==> MASK : 0x00003C00U    VAL : 0x00003C00U
    // .. .. reg_phy_use_wr_level = 0x1
    // .. .. ==> 0XF8006194[14:14] = 0x00000001U
    // .. ..     ==> MASK : 0x00004000U    VAL : 0x00004000U
    // .. .. reg_phy_use_rd_dqs_gate_level = 0x1
    // .. .. ==> 0XF8006194[15:15] = 0x00000001U
    // .. ..     ==> MASK : 0x00008000U    VAL : 0x00008000U
    // .. .. reg_phy_use_rd_data_eye_level = 0x1
    // .. .. ==> 0XF8006194[16:16] = 0x00000001U
    // .. ..     ==> MASK : 0x00010000U    VAL : 0x00010000U
    // .. .. reg_phy_dis_calib_rst = 0x0
    // .. .. ==> 0XF8006194[17:17] = 0x00000000U
    // .. ..     ==> MASK : 0x00020000U    VAL : 0x00000000U
    // .. .. reg_phy_ctrl_slave_delay = 0x0
    // .. .. ==> 0XF8006194[19:18] = 0x00000000U
    // .. ..     ==> MASK : 0x000C0000U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006194, 0x000FFFFFU ,0x0001FC82U),
    // .. .. reg_arb_page_addr_mask = 0x0
    // .. .. ==> 0XF8006204[31:0] = 0x00000000U
    // .. ..     ==> MASK : 0xFFFFFFFFU    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006204, 0xFFFFFFFFU ,0x00000000U),
    // .. .. reg_arb_pri_wr_portn = 0x3ff
    // .. .. ==> 0XF8006208[9:0] = 0x000003FFU
    // .. ..     ==> MASK : 0x000003FFU    VAL : 0x000003FFU
    // .. .. reg_arb_disable_aging_wr_portn = 0x0
    // .. .. ==> 0XF8006208[16:16] = 0x00000000U
    // .. ..     ==> MASK : 0x00010000U    VAL : 0x00000000U
    // .. .. reg_arb_disable_urgent_wr_portn = 0x0
    // .. .. ==> 0XF8006208[17:17] = 0x00000000U
    // .. ..     ==> MASK : 0x00020000U    VAL : 0x00000000U
    // .. .. reg_arb_dis_page_match_wr_portn = 0x0
    // .. .. ==> 0XF8006208[18:18] = 0x00000000U
    // .. ..     ==> MASK : 0x00040000U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006208, 0x000703FFU ,0x000003FFU),
    // .. .. reg_arb_pri_wr_portn = 0x3ff
    // .. .. ==> 0XF800620C[9:0] = 0x000003FFU
    // .. ..     ==> MASK : 0x000003FFU    VAL : 0x000003FFU
    // .. .. reg_arb_disable_aging_wr_portn = 0x0
    // .. .. ==> 0XF800620C[16:16] = 0x00000000U
    // .. ..     ==> MASK : 0x00010000U    VAL : 0x00000000U
    // .. .. reg_arb_disable_urgent_wr_portn = 0x0
    // .. .. ==> 0XF800620C[17:17] = 0x00000000U
    // .. ..     ==> MASK : 0x00020000U    VAL : 0x00000000U
    // .. .. reg_arb_dis_page_match_wr_portn = 0x0
    // .. .. ==> 0XF800620C[18:18] = 0x00000000U
    // .. ..     ==> MASK : 0x00040000U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF800620C, 0x000703FFU ,0x000003FFU),
    // .. .. reg_arb_pri_wr_portn = 0x3ff
    // .. .. ==> 0XF8006210[9:0] = 0x000003FFU
    // .. ..     ==> MASK : 0x000003FFU    VAL : 0x000003FFU
    // .. .. reg_arb_disable_aging_wr_portn = 0x0
    // .. .. ==> 0XF8006210[16:16] = 0x00000000U
    // .. ..     ==> MASK : 0x00010000U    VAL : 0x00000000U
    // .. .. reg_arb_disable_urgent_wr_portn = 0x0
    // .. .. ==> 0XF8006210[17:17] = 0x00000000U
    // .. ..     ==> MASK : 0x00020000U    VAL : 0x00000000U
    // .. .. reg_arb_dis_page_match_wr_portn = 0x0
    // .. .. ==> 0XF8006210[18:18] = 0x00000000U
    // .. ..     ==> MASK : 0x00040000U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006210, 0x000703FFU ,0x000003FFU),
    // .. .. reg_arb_pri_wr_portn = 0x3ff
    // .. .. ==> 0XF8006214[9:0] = 0x000003FFU
    // .. ..     ==> MASK : 0x000003FFU    VAL : 0x000003FFU
    // .. .. reg_arb_disable_aging_wr_portn = 0x0
    // .. .. ==> 0XF8006214[16:16] = 0x00000000U
    // .. ..     ==> MASK : 0x00010000U    VAL : 0x00000000U
    // .. .. reg_arb_disable_urgent_wr_portn = 0x0
    // .. .. ==> 0XF8006214[17:17] = 0x00000000U
    // .. ..     ==> MASK : 0x00020000U    VAL : 0x00000000U
    // .. .. reg_arb_dis_page_match_wr_portn = 0x0
    // .. .. ==> 0XF8006214[18:18] = 0x00000000U
    // .. ..     ==> MASK : 0x00040000U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006214, 0x000703FFU ,0x000003FFU),
    // .. .. reg_arb_pri_rd_portn = 0x3ff
    // .. .. ==> 0XF8006218[9:0] = 0x000003FFU
    // .. ..     ==> MASK : 0x000003FFU    VAL : 0x000003FFU
    // .. .. reg_arb_disable_aging_rd_portn = 0x0
    // .. .. ==> 0XF8006218[16:16] = 0x00000000U
    // .. ..     ==> MASK : 0x00010000U    VAL : 0x00000000U
    // .. .. reg_arb_disable_urgent_rd_portn = 0x0
    // .. .. ==> 0XF8006218[17:17] = 0x00000000U
    // .. ..     ==> MASK : 0x00020000U    VAL : 0x00000000U
    // .. .. reg_arb_dis_page_match_rd_portn = 0x0
    // .. .. ==> 0XF8006218[18:18] = 0x00000000U
    // .. ..     ==> MASK : 0x00040000U    VAL : 0x00000000U
    // .. .. reg_arb_set_hpr_rd_portn = 0x0
    // .. .. ==> 0XF8006218[19:19] = 0x00000000U
    // .. ..     ==> MASK : 0x00080000U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006218, 0x000F03FFU ,0x000003FFU),
    // .. .. reg_arb_pri_rd_portn = 0x3ff
    // .. .. ==> 0XF800621C[9:0] = 0x000003FFU
    // .. ..     ==> MASK : 0x000003FFU    VAL : 0x000003FFU
    // .. .. reg_arb_disable_aging_rd_portn = 0x0
    // .. .. ==> 0XF800621C[16:16] = 0x00000000U
    // .. ..     ==> MASK : 0x00010000U    VAL : 0x00000000U
    // .. .. reg_arb_disable_urgent_rd_portn = 0x0
    // .. .. ==> 0XF800621C[17:17] = 0x00000000U
    // .. ..     ==> MASK : 0x00020000U    VAL : 0x00000000U
    // .. .. reg_arb_dis_page_match_rd_portn = 0x0
    // .. .. ==> 0XF800621C[18:18] = 0x00000000U
    // .. ..     ==> MASK : 0x00040000U    VAL : 0x00000000U
    // .. .. reg_arb_set_hpr_rd_portn = 0x0
    // .. .. ==> 0XF800621C[19:19] = 0x00000000U
    // .. ..     ==> MASK : 0x00080000U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF800621C, 0x000F03FFU ,0x000003FFU),
    // .. .. reg_arb_pri_rd_portn = 0x3ff
    // .. .. ==> 0XF8006220[9:0] = 0x000003FFU
    // .. ..     ==> MASK : 0x000003FFU    VAL : 0x000003FFU
    // .. .. reg_arb_disable_aging_rd_portn = 0x0
    // .. .. ==> 0XF8006220[16:16] = 0x00000000U
    // .. ..     ==> MASK : 0x00010000U    VAL : 0x00000000U
    // .. .. reg_arb_disable_urgent_rd_portn = 0x0
    // .. .. ==> 0XF8006220[17:17] = 0x00000000U
    // .. ..     ==> MASK : 0x00020000U    VAL : 0x00000000U
    // .. .. reg_arb_dis_page_match_rd_portn = 0x0
    // .. .. ==> 0XF8006220[18:18] = 0x00000000U
    // .. ..     ==> MASK : 0x00040000U    VAL : 0x00000000U
    // .. .. reg_arb_set_hpr_rd_portn = 0x0
    // .. .. ==> 0XF8006220[19:19] = 0x00000000U
    // .. ..     ==> MASK : 0x00080000U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006220, 0x000F03FFU ,0x000003FFU),
    // .. .. reg_arb_pri_rd_portn = 0x3ff
    // .. .. ==> 0XF8006224[9:0] = 0x000003FFU
    // .. ..     ==> MASK : 0x000003FFU    VAL : 0x000003FFU
    // .. .. reg_arb_disable_aging_rd_portn = 0x0
    // .. .. ==> 0XF8006224[16:16] = 0x00000000U
    // .. ..     ==> MASK : 0x00010000U    VAL : 0x00000000U
    // .. .. reg_arb_disable_urgent_rd_portn = 0x0
    // .. .. ==> 0XF8006224[17:17] = 0x00000000U
    // .. ..     ==> MASK : 0x00020000U    VAL : 0x00000000U
    // .. .. reg_arb_dis_page_match_rd_portn = 0x0
    // .. .. ==> 0XF8006224[18:18] = 0x00000000U
    // .. ..     ==> MASK : 0x00040000U    VAL : 0x00000000U
    // .. .. reg_arb_set_hpr_rd_portn = 0x0
    // .. .. ==> 0XF8006224[19:19] = 0x00000000U
    // .. ..     ==> MASK : 0x00080000U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006224, 0x000F03FFU ,0x000003FFU),
    // .. .. reg_ddrc_lpddr2 = 0x0
    // .. .. ==> 0XF80062A8[0:0] = 0x00000000U
    // .. ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. .. reg_ddrc_derate_enable = 0x0
    // .. .. ==> 0XF80062A8[2:2] = 0x00000000U
    // .. ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. .. reg_ddrc_mr4_margin = 0x0
    // .. .. ==> 0XF80062A8[11:4] = 0x00000000U
    // .. ..     ==> MASK : 0x00000FF0U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF80062A8, 0x00000FF5U ,0x00000000U),
    // .. .. reg_ddrc_mr4_read_interval = 0x0
    // .. .. ==> 0XF80062AC[31:0] = 0x00000000U
    // .. ..     ==> MASK : 0xFFFFFFFFU    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF80062AC, 0xFFFFFFFFU ,0x00000000U),
    // .. .. reg_ddrc_min_stable_clock_x1 = 0x5
    // .. .. ==> 0XF80062B0[3:0] = 0x00000005U
    // .. ..     ==> MASK : 0x0000000FU    VAL : 0x00000005U
    // .. .. reg_ddrc_idle_after_reset_x32 = 0x12
    // .. .. ==> 0XF80062B0[11:4] = 0x00000012U
    // .. ..     ==> MASK : 0x00000FF0U    VAL : 0x00000120U
    // .. .. reg_ddrc_t_mrw = 0x5
    // .. .. ==> 0XF80062B0[21:12] = 0x00000005U
    // .. ..     ==> MASK : 0x003FF000U    VAL : 0x00005000U
    // .. ..
    EMIT_MASKWRITE(0XF80062B0, 0x003FFFFFU ,0x00005125U),
    // .. .. reg_ddrc_max_auto_init_x1024 = 0xa8
    // .. .. ==> 0XF80062B4[7:0] = 0x000000A8U
    // .. ..     ==> MASK : 0x000000FFU    VAL : 0x000000A8U
    // .. .. reg_ddrc_dev_zqinit_x32 = 0x12
    // .. .. ==> 0XF80062B4[17:8] = 0x00000012U
    // .. ..     ==> MASK : 0x0003FF00U    VAL : 0x00001200U
    // .. ..
    EMIT_MASKWRITE(0XF80062B4, 0x0003FFFFU ,0x000012A8U),
    // .. .. START: POLL ON DCI STATUS
    // .. .. DONE = 1
    // .. .. ==> 0XF8000B74[13:13] = 0x00000001U
    // .. ..     ==> MASK : 0x00002000U    VAL : 0x00002000U
    // .. ..
    EMIT_MASKPOLL(0XF8000B74, 0x00002000U),
    // .. .. FINISH: POLL ON DCI STATUS
    // .. .. START: UNLOCK DDR
    // .. .. reg_ddrc_soft_rstb = 0x1
    // .. .. ==> 0XF8006000[0:0] = 0x00000001U
    // .. ..     ==> MASK : 0x00000001U    VAL : 0x00000001U
    // .. .. reg_ddrc_powerdown_en = 0x0
    // .. .. ==> 0XF8006000[1:1] = 0x00000000U
    // .. ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. .. reg_ddrc_data_bus_width = 0x0
    // .. .. ==> 0XF8006000[3:2] = 0x00000000U
    // .. ..     ==> MASK : 0x0000000CU    VAL : 0x00000000U
    // .. .. reg_ddrc_burst8_refresh = 0x0
    // .. .. ==> 0XF8006000[6:4] = 0x00000000U
    // .. ..     ==> MASK : 0x00000070U    VAL : 0x00000000U
    // .. .. reg_ddrc_rdwr_idle_gap = 1
    // .. .. ==> 0XF8006000[13:7] = 0x00000001U
    // .. ..     ==> MASK : 0x00003F80U    VAL : 0x00000080U
    // .. .. reg_ddrc_dis_rd_bypass = 0x0
    // .. .. ==> 0XF8006000[14:14] = 0x00000000U
    // .. ..     ==> MASK : 0x00004000U    VAL : 0x00000000U
    // .. .. reg_ddrc_dis_act_bypass = 0x0
    // .. .. ==> 0XF8006000[15:15] = 0x00000000U
    // .. ..     ==> MASK : 0x00008000U    VAL : 0x00000000U
    // .. .. reg_ddrc_dis_auto_refresh = 0x0
    // .. .. ==> 0XF8006000[16:16] = 0x00000000U
    // .. ..     ==> MASK : 0x00010000U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006000, 0x0001FFFFU ,0x00000081U),
    // .. .. FINISH: UNLOCK DDR
    // .. .. START: CHECK DDR STATUS
    // .. .. ddrc_reg_operating_mode = 1
    // .. .. ==> 0XF8006054[2:0] = 0x00000001U
    // .. ..     ==> MASK : 0x00000007U    VAL : 0x00000001U
    // .. ..
    EMIT_MASKPOLL(0XF8006054, 0x00000007U),
    // .. .. FINISH: CHECK DDR STATUS
    // .. FINISH: DDR INITIALIZATION
    // FINISH: top
    //
    EMIT_EXIT(),

    //
};

unsigned long ps7_mio_init_data_3_0[] = {
    // START: top
    // .. START: SLCR SETTINGS
    // .. UNLOCK_KEY = 0XDF0D
    // .. ==> 0XF8000008[15:0] = 0x0000DF0DU
    // ..     ==> MASK : 0x0000FFFFU    VAL : 0x0000DF0DU
    // ..
    EMIT_WRITE(0XF8000008, 0x0000DF0DU),
    // .. FINISH: SLCR SETTINGS
    // .. START: OCM REMAPPING
    // .. FINISH: OCM REMAPPING
    // .. START: DDRIOB SETTINGS
    // .. reserved_INP_POWER = 0x0
    // .. ==> 0XF8000B40[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. INP_TYPE = 0x0
    // .. ==> 0XF8000B40[2:1] = 0x00000000U
    // ..     ==> MASK : 0x00000006U    VAL : 0x00000000U
    // .. DCI_UPDATE_B = 0x0
    // .. ==> 0XF8000B40[3:3] = 0x00000000U
    // ..     ==> MASK : 0x00000008U    VAL : 0x00000000U
    // .. TERM_EN = 0x0
    // .. ==> 0XF8000B40[4:4] = 0x00000000U
    // ..     ==> MASK : 0x00000010U    VAL : 0x00000000U
    // .. DCI_TYPE = 0x0
    // .. ==> 0XF8000B40[6:5] = 0x00000000U
    // ..     ==> MASK : 0x00000060U    VAL : 0x00000000U
    // .. IBUF_DISABLE_MODE = 0x0
    // .. ==> 0XF8000B40[7:7] = 0x00000000U
    // ..     ==> MASK : 0x00000080U    VAL : 0x00000000U
    // .. TERM_DISABLE_MODE = 0x0
    // .. ==> 0XF8000B40[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. OUTPUT_EN = 0x3
    // .. ==> 0XF8000B40[10:9] = 0x00000003U
    // ..     ==> MASK : 0x00000600U    VAL : 0x00000600U
    // .. PULLUP_EN = 0x0
    // .. ==> 0XF8000B40[11:11] = 0x00000000U
    // ..     ==> MASK : 0x00000800U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF8000B40, 0x00000FFFU ,0x00000600U),
    // .. reserved_INP_POWER = 0x0
    // .. ==> 0XF8000B44[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. INP_TYPE = 0x0
    // .. ==> 0XF8000B44[2:1] = 0x00000000U
    // ..     ==> MASK : 0x00000006U    VAL : 0x00000000U
    // .. DCI_UPDATE_B = 0x0
    // .. ==> 0XF8000B44[3:3] = 0x00000000U
    // ..     ==> MASK : 0x00000008U    VAL : 0x00000000U
    // .. TERM_EN = 0x0
    // .. ==> 0XF8000B44[4:4] = 0x00000000U
    // ..     ==> MASK : 0x00000010U    VAL : 0x00000000U
    // .. DCI_TYPE = 0x0
    // .. ==> 0XF8000B44[6:5] = 0x00000000U
    // ..     ==> MASK : 0x00000060U    VAL : 0x00000000U
    // .. IBUF_DISABLE_MODE = 0x0
    // .. ==> 0XF8000B44[7:7] = 0x00000000U
    // ..     ==> MASK : 0x00000080U    VAL : 0x00000000U
    // .. TERM_DISABLE_MODE = 0x0
    // .. ==> 0XF8000B44[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. OUTPUT_EN = 0x3
    // .. ==> 0XF8000B44[10:9] = 0x00000003U
    // ..     ==> MASK : 0x00000600U    VAL : 0x00000600U
    // .. PULLUP_EN = 0x0
    // .. ==> 0XF8000B44[11:11] = 0x00000000U
    // ..     ==> MASK : 0x00000800U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF8000B44, 0x00000FFFU ,0x00000600U),
    // .. reserved_INP_POWER = 0x0
    // .. ==> 0XF8000B48[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. INP_TYPE = 0x1
    // .. ==> 0XF8000B48[2:1] = 0x00000001U
    // ..     ==> MASK : 0x00000006U    VAL : 0x00000002U
    // .. DCI_UPDATE_B = 0x0
    // .. ==> 0XF8000B48[3:3] = 0x00000000U
    // ..     ==> MASK : 0x00000008U    VAL : 0x00000000U
    // .. TERM_EN = 0x1
    // .. ==> 0XF8000B48[4:4] = 0x00000001U
    // ..     ==> MASK : 0x00000010U    VAL : 0x00000010U
    // .. DCI_TYPE = 0x3
    // .. ==> 0XF8000B48[6:5] = 0x00000003U
    // ..     ==> MASK : 0x00000060U    VAL : 0x00000060U
    // .. IBUF_DISABLE_MODE = 0
    // .. ==> 0XF8000B48[7:7] = 0x00000000U
    // ..     ==> MASK : 0x00000080U    VAL : 0x00000000U
    // .. TERM_DISABLE_MODE = 0
    // .. ==> 0XF8000B48[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. OUTPUT_EN = 0x3
    // .. ==> 0XF8000B48[10:9] = 0x00000003U
    // ..     ==> MASK : 0x00000600U    VAL : 0x00000600U
    // .. PULLUP_EN = 0x0
    // .. ==> 0XF8000B48[11:11] = 0x00000000U
    // ..     ==> MASK : 0x00000800U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF8000B48, 0x00000FFFU ,0x00000672U),
    // .. reserved_INP_POWER = 0x0
    // .. ==> 0XF8000B4C[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. INP_TYPE = 0x1
    // .. ==> 0XF8000B4C[2:1] = 0x00000001U
    // ..     ==> MASK : 0x00000006U    VAL : 0x00000002U
    // .. DCI_UPDATE_B = 0x0
    // .. ==> 0XF8000B4C[3:3] = 0x00000000U
    // ..     ==> MASK : 0x00000008U    VAL : 0x00000000U
    // .. TERM_EN = 0x1
    // .. ==> 0XF8000B4C[4:4] = 0x00000001U
    // ..     ==> MASK : 0x00000010U    VAL : 0x00000010U
    // .. DCI_TYPE = 0x3
    // .. ==> 0XF8000B4C[6:5] = 0x00000003U
    // ..     ==> MASK : 0x00000060U    VAL : 0x00000060U
    // .. IBUF_DISABLE_MODE = 0
    // .. ==> 0XF8000B4C[7:7] = 0x00000000U
    // ..     ==> MASK : 0x00000080U    VAL : 0x00000000U
    // .. TERM_DISABLE_MODE = 0
    // .. ==> 0XF8000B4C[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. OUTPUT_EN = 0x3
    // .. ==> 0XF8000B4C[10:9] = 0x00000003U
    // ..     ==> MASK : 0x00000600U    VAL : 0x00000600U
    // .. PULLUP_EN = 0x0
    // .. ==> 0XF8000B4C[11:11] = 0x00000000U
    // ..     ==> MASK : 0x00000800U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF8000B4C, 0x00000FFFU ,0x00000672U),
    // .. reserved_INP_POWER = 0x0
    // .. ==> 0XF8000B50[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. INP_TYPE = 0x2
    // .. ==> 0XF8000B50[2:1] = 0x00000002U
    // ..     ==> MASK : 0x00000006U    VAL : 0x00000004U
    // .. DCI_UPDATE_B = 0x0
    // .. ==> 0XF8000B50[3:3] = 0x00000000U
    // ..     ==> MASK : 0x00000008U    VAL : 0x00000000U
    // .. TERM_EN = 0x1
    // .. ==> 0XF8000B50[4:4] = 0x00000001U
    // ..     ==> MASK : 0x00000010U    VAL : 0x00000010U
    // .. DCI_TYPE = 0x3
    // .. ==> 0XF8000B50[6:5] = 0x00000003U
    // ..     ==> MASK : 0x00000060U    VAL : 0x00000060U
    // .. IBUF_DISABLE_MODE = 0
    // .. ==> 0XF8000B50[7:7] = 0x00000000U
    // ..     ==> MASK : 0x00000080U    VAL : 0x00000000U
    // .. TERM_DISABLE_MODE = 0
    // .. ==> 0XF8000B50[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. OUTPUT_EN = 0x3
    // .. ==> 0XF8000B50[10:9] = 0x00000003U
    // ..     ==> MASK : 0x00000600U    VAL : 0x00000600U
    // .. PULLUP_EN = 0x0
    // .. ==> 0XF8000B50[11:11] = 0x00000000U
    // ..     ==> MASK : 0x00000800U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF8000B50, 0x00000FFFU ,0x00000674U),
    // .. reserved_INP_POWER = 0x0
    // .. ==> 0XF8000B54[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. INP_TYPE = 0x2
    // .. ==> 0XF8000B54[2:1] = 0x00000002U
    // ..     ==> MASK : 0x00000006U    VAL : 0x00000004U
    // .. DCI_UPDATE_B = 0x0
    // .. ==> 0XF8000B54[3:3] = 0x00000000U
    // ..     ==> MASK : 0x00000008U    VAL : 0x00000000U
    // .. TERM_EN = 0x1
    // .. ==> 0XF8000B54[4:4] = 0x00000001U
    // ..     ==> MASK : 0x00000010U    VAL : 0x00000010U
    // .. DCI_TYPE = 0x3
    // .. ==> 0XF8000B54[6:5] = 0x00000003U
    // ..     ==> MASK : 0x00000060U    VAL : 0x00000060U
    // .. IBUF_DISABLE_MODE = 0
    // .. ==> 0XF8000B54[7:7] = 0x00000000U
    // ..     ==> MASK : 0x00000080U    VAL : 0x00000000U
    // .. TERM_DISABLE_MODE = 0
    // .. ==> 0XF8000B54[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. OUTPUT_EN = 0x3
    // .. ==> 0XF8000B54[10:9] = 0x00000003U
    // ..     ==> MASK : 0x00000600U    VAL : 0x00000600U
    // .. PULLUP_EN = 0x0
    // .. ==> 0XF8000B54[11:11] = 0x00000000U
    // ..     ==> MASK : 0x00000800U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF8000B54, 0x00000FFFU ,0x00000674U),
    // .. reserved_INP_POWER = 0x0
    // .. ==> 0XF8000B58[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. INP_TYPE = 0x0
    // .. ==> 0XF8000B58[2:1] = 0x00000000U
    // ..     ==> MASK : 0x00000006U    VAL : 0x00000000U
    // .. DCI_UPDATE_B = 0x0
    // .. ==> 0XF8000B58[3:3] = 0x00000000U
    // ..     ==> MASK : 0x00000008U    VAL : 0x00000000U
    // .. TERM_EN = 0x0
    // .. ==> 0XF8000B58[4:4] = 0x00000000U
    // ..     ==> MASK : 0x00000010U    VAL : 0x00000000U
    // .. DCI_TYPE = 0x0
    // .. ==> 0XF8000B58[6:5] = 0x00000000U
    // ..     ==> MASK : 0x00000060U    VAL : 0x00000000U
    // .. IBUF_DISABLE_MODE = 0x0
    // .. ==> 0XF8000B58[7:7] = 0x00000000U
    // ..     ==> MASK : 0x00000080U    VAL : 0x00000000U
    // .. TERM_DISABLE_MODE = 0x0
    // .. ==> 0XF8000B58[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. OUTPUT_EN = 0x3
    // .. ==> 0XF8000B58[10:9] = 0x00000003U
    // ..     ==> MASK : 0x00000600U    VAL : 0x00000600U
    // .. PULLUP_EN = 0x0
    // .. ==> 0XF8000B58[11:11] = 0x00000000U
    // ..     ==> MASK : 0x00000800U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF8000B58, 0x00000FFFU ,0x00000600U),
    // .. reserved_DRIVE_P = 0x1c
    // .. ==> 0XF8000B5C[6:0] = 0x0000001CU
    // ..     ==> MASK : 0x0000007FU    VAL : 0x0000001CU
    // .. reserved_DRIVE_N = 0xc
    // .. ==> 0XF8000B5C[13:7] = 0x0000000CU
    // ..     ==> MASK : 0x00003F80U    VAL : 0x00000600U
    // .. reserved_SLEW_P = 0x3
    // .. ==> 0XF8000B5C[18:14] = 0x00000003U
    // ..     ==> MASK : 0x0007C000U    VAL : 0x0000C000U
    // .. reserved_SLEW_N = 0x3
    // .. ==> 0XF8000B5C[23:19] = 0x00000003U
    // ..     ==> MASK : 0x00F80000U    VAL : 0x00180000U
    // .. reserved_GTL = 0x0
    // .. ==> 0XF8000B5C[26:24] = 0x00000000U
    // ..     ==> MASK : 0x07000000U    VAL : 0x00000000U
    // .. reserved_RTERM = 0x0
    // .. ==> 0XF8000B5C[31:27] = 0x00000000U
    // ..     ==> MASK : 0xF8000000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF8000B5C, 0xFFFFFFFFU ,0x0018C61CU),
    // .. reserved_DRIVE_P = 0x1c
    // .. ==> 0XF8000B60[6:0] = 0x0000001CU
    // ..     ==> MASK : 0x0000007FU    VAL : 0x0000001CU
    // .. reserved_DRIVE_N = 0xc
    // .. ==> 0XF8000B60[13:7] = 0x0000000CU
    // ..     ==> MASK : 0x00003F80U    VAL : 0x00000600U
    // .. reserved_SLEW_P = 0x6
    // .. ==> 0XF8000B60[18:14] = 0x00000006U
    // ..     ==> MASK : 0x0007C000U    VAL : 0x00018000U
    // .. reserved_SLEW_N = 0x1f
    // .. ==> 0XF8000B60[23:19] = 0x0000001FU
    // ..     ==> MASK : 0x00F80000U    VAL : 0x00F80000U
    // .. reserved_GTL = 0x0
    // .. ==> 0XF8000B60[26:24] = 0x00000000U
    // ..     ==> MASK : 0x07000000U    VAL : 0x00000000U
    // .. reserved_RTERM = 0x0
    // .. ==> 0XF8000B60[31:27] = 0x00000000U
    // ..     ==> MASK : 0xF8000000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF8000B60, 0xFFFFFFFFU ,0x00F9861CU),
    // .. reserved_DRIVE_P = 0x1c
    // .. ==> 0XF8000B64[6:0] = 0x0000001CU
    // ..     ==> MASK : 0x0000007FU    VAL : 0x0000001CU
    // .. reserved_DRIVE_N = 0xc
    // .. ==> 0XF8000B64[13:7] = 0x0000000CU
    // ..     ==> MASK : 0x00003F80U    VAL : 0x00000600U
    // .. reserved_SLEW_P = 0x6
    // .. ==> 0XF8000B64[18:14] = 0x00000006U
    // ..     ==> MASK : 0x0007C000U    VAL : 0x00018000U
    // .. reserved_SLEW_N = 0x1f
    // .. ==> 0XF8000B64[23:19] = 0x0000001FU
    // ..     ==> MASK : 0x00F80000U    VAL : 0x00F80000U
    // .. reserved_GTL = 0x0
    // .. ==> 0XF8000B64[26:24] = 0x00000000U
    // ..     ==> MASK : 0x07000000U    VAL : 0x00000000U
    // .. reserved_RTERM = 0x0
    // .. ==> 0XF8000B64[31:27] = 0x00000000U
    // ..     ==> MASK : 0xF8000000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF8000B64, 0xFFFFFFFFU ,0x00F9861CU),
    // .. reserved_DRIVE_P = 0x1c
    // .. ==> 0XF8000B68[6:0] = 0x0000001CU
    // ..     ==> MASK : 0x0000007FU    VAL : 0x0000001CU
    // .. reserved_DRIVE_N = 0xc
    // .. ==> 0XF8000B68[13:7] = 0x0000000CU
    // ..     ==> MASK : 0x00003F80U    VAL : 0x00000600U
    // .. reserved_SLEW_P = 0x6
    // .. ==> 0XF8000B68[18:14] = 0x00000006U
    // ..     ==> MASK : 0x0007C000U    VAL : 0x00018000U
    // .. reserved_SLEW_N = 0x1f
    // .. ==> 0XF8000B68[23:19] = 0x0000001FU
    // ..     ==> MASK : 0x00F80000U    VAL : 0x00F80000U
    // .. reserved_GTL = 0x0
    // .. ==> 0XF8000B68[26:24] = 0x00000000U
    // ..     ==> MASK : 0x07000000U    VAL : 0x00000000U
    // .. reserved_RTERM = 0x0
    // .. ==> 0XF8000B68[31:27] = 0x00000000U
    // ..     ==> MASK : 0xF8000000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF8000B68, 0xFFFFFFFFU ,0x00F9861CU),
    // .. VREF_INT_EN = 0x0
    // .. ==> 0XF8000B6C[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. VREF_SEL = 0x0
    // .. ==> 0XF8000B6C[4:1] = 0x00000000U
    // ..     ==> MASK : 0x0000001EU    VAL : 0x00000000U
    // .. VREF_EXT_EN = 0x3
    // .. ==> 0XF8000B6C[6:5] = 0x00000003U
    // ..     ==> MASK : 0x00000060U    VAL : 0x00000060U
    // .. reserved_VREF_PULLUP_EN = 0x0
    // .. ==> 0XF8000B6C[8:7] = 0x00000000U
    // ..     ==> MASK : 0x00000180U    VAL : 0x00000000U
    // .. REFIO_EN = 0x1
    // .. ==> 0XF8000B6C[9:9] = 0x00000001U
    // ..     ==> MASK : 0x00000200U    VAL : 0x00000200U
    // .. reserved_REFIO_TEST = 0x0
    // .. ==> 0XF8000B6C[11:10] = 0x00000000U
    // ..     ==> MASK : 0x00000C00U    VAL : 0x00000000U
    // .. reserved_REFIO_PULLUP_EN = 0x0
    // .. ==> 0XF8000B6C[12:12] = 0x00000000U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00000000U
    // .. reserved_DRST_B_PULLUP_EN = 0x0
    // .. ==> 0XF8000B6C[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // .. reserved_CKE_PULLUP_EN = 0x0
    // .. ==> 0XF8000B6C[14:14] = 0x00000000U
    // ..     ==> MASK : 0x00004000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF8000B6C, 0x00007FFFU ,0x00000260U),
    // .. .. START: ASSERT RESET
    // .. .. RESET = 1
    // .. .. ==> 0XF8000B70[0:0] = 0x00000001U
    // .. ..     ==> MASK : 0x00000001U    VAL : 0x00000001U
    // .. ..
    EMIT_MASKWRITE(0XF8000B70, 0x00000001U ,0x00000001U),
    // .. .. FINISH: ASSERT RESET
    // .. .. START: DEASSERT RESET
    // .. .. RESET = 0
    // .. .. ==> 0XF8000B70[0:0] = 0x00000000U
    // .. ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. .. reserved_VRN_OUT = 0x1
    // .. .. ==> 0XF8000B70[5:5] = 0x00000001U
    // .. ..     ==> MASK : 0x00000020U    VAL : 0x00000020U
    // .. ..
    EMIT_MASKWRITE(0XF8000B70, 0x00000021U ,0x00000020U),
    // .. .. FINISH: DEASSERT RESET
    // .. .. RESET = 0x1
    // .. .. ==> 0XF8000B70[0:0] = 0x00000001U
    // .. ..     ==> MASK : 0x00000001U    VAL : 0x00000001U
    // .. .. ENABLE = 0x1
    // .. .. ==> 0XF8000B70[1:1] = 0x00000001U
    // .. ..     ==> MASK : 0x00000002U    VAL : 0x00000002U
    // .. .. reserved_VRP_TRI = 0x0
    // .. .. ==> 0XF8000B70[2:2] = 0x00000000U
    // .. ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. .. reserved_VRN_TRI = 0x0
    // .. .. ==> 0XF8000B70[3:3] = 0x00000000U
    // .. ..     ==> MASK : 0x00000008U    VAL : 0x00000000U
    // .. .. reserved_VRP_OUT = 0x0
    // .. .. ==> 0XF8000B70[4:4] = 0x00000000U
    // .. ..     ==> MASK : 0x00000010U    VAL : 0x00000000U
    // .. .. reserved_VRN_OUT = 0x1
    // .. .. ==> 0XF8000B70[5:5] = 0x00000001U
    // .. ..     ==> MASK : 0x00000020U    VAL : 0x00000020U
    // .. .. NREF_OPT1 = 0x0
    // .. .. ==> 0XF8000B70[7:6] = 0x00000000U
    // .. ..     ==> MASK : 0x000000C0U    VAL : 0x00000000U
    // .. .. NREF_OPT2 = 0x0
    // .. .. ==> 0XF8000B70[10:8] = 0x00000000U
    // .. ..     ==> MASK : 0x00000700U    VAL : 0x00000000U
    // .. .. NREF_OPT4 = 0x1
    // .. .. ==> 0XF8000B70[13:11] = 0x00000001U
    // .. ..     ==> MASK : 0x00003800U    VAL : 0x00000800U
    // .. .. PREF_OPT1 = 0x0
    // .. .. ==> 0XF8000B70[15:14] = 0x00000000U
    // .. ..     ==> MASK : 0x0000C000U    VAL : 0x00000000U
    // .. .. PREF_OPT2 = 0x0
    // .. .. ==> 0XF8000B70[19:17] = 0x00000000U
    // .. ..     ==> MASK : 0x000E0000U    VAL : 0x00000000U
    // .. .. UPDATE_CONTROL = 0x0
    // .. .. ==> 0XF8000B70[20:20] = 0x00000000U
    // .. ..     ==> MASK : 0x00100000U    VAL : 0x00000000U
    // .. .. reserved_INIT_COMPLETE = 0x0
    // .. .. ==> 0XF8000B70[21:21] = 0x00000000U
    // .. ..     ==> MASK : 0x00200000U    VAL : 0x00000000U
    // .. .. reserved_TST_CLK = 0x0
    // .. .. ==> 0XF8000B70[22:22] = 0x00000000U
    // .. ..     ==> MASK : 0x00400000U    VAL : 0x00000000U
    // .. .. reserved_TST_HLN = 0x0
    // .. .. ==> 0XF8000B70[23:23] = 0x00000000U
    // .. ..     ==> MASK : 0x00800000U    VAL : 0x00000000U
    // .. .. reserved_TST_HLP = 0x0
    // .. .. ==> 0XF8000B70[24:24] = 0x00000000U
    // .. ..     ==> MASK : 0x01000000U    VAL : 0x00000000U
    // .. .. reserved_TST_RST = 0x0
    // .. .. ==> 0XF8000B70[25:25] = 0x00000000U
    // .. ..     ==> MASK : 0x02000000U    VAL : 0x00000000U
    // .. .. reserved_INT_DCI_EN = 0x0
    // .. .. ==> 0XF8000B70[26:26] = 0x00000000U
    // .. ..     ==> MASK : 0x04000000U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF8000B70, 0x07FEFFFFU ,0x00000823U),
    // .. FINISH: DDRIOB SETTINGS
    // .. START: MIO PROGRAMMING
    // .. TRI_ENABLE = 0
    // .. ==> 0XF8000700[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF8000700[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF8000700[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 2
    // .. ==> 0XF8000700[4:3] = 0x00000002U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000010U
    // .. L3_SEL = 0
    // .. ==> 0XF8000700[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF8000700[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF8000700[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF8000700[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF8000700[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    //EMIT_MASKWRITE(0XF8000700, 0x00003FFFU ,0x00001610U),	// MIO0
    // .. TRI_ENABLE = 0
    // .. ==> 0XF8000704[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF8000704[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF8000704[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF8000704[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF8000704[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF8000704[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF8000704[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF8000704[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF8000704[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
	//EMIT_MASKWRITE(0XF8000704, 0x00003FFFU ,0x00001600U),	// MIO1
    // .. TRI_ENABLE = 0
    // .. ==> 0XF8000708[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF8000708[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF8000708[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 2
    // .. ==> 0XF8000708[4:3] = 0x00000002U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000010U
    // .. L3_SEL = 0
    // .. ==> 0XF8000708[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF8000708[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF8000708[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 0
    // .. ==> 0XF8000708[12:12] = 0x00000000U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00000000U
    // .. DisableRcvr = 0
    // .. ==> 0XF8000708[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
	//EMIT_MASKWRITE(0XF8000708, 0x00003FFFU ,0x00000610U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF800070C[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF800070C[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF800070C[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 2
    // .. ==> 0XF800070C[4:3] = 0x00000002U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000010U
    // .. L3_SEL = 0
    // .. ==> 0XF800070C[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF800070C[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF800070C[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 0
    // .. ==> 0XF800070C[12:12] = 0x00000000U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00000000U
    // .. DisableRcvr = 0
    // .. ==> 0XF800070C[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
	//EMIT_MASKWRITE(0XF800070C, 0x00003FFFU ,0x00000610U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF8000710[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF8000710[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF8000710[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 2
    // .. ==> 0XF8000710[4:3] = 0x00000002U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000010U
    // .. L3_SEL = 0
    // .. ==> 0XF8000710[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF8000710[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF8000710[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 0
    // .. ==> 0XF8000710[12:12] = 0x00000000U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00000000U
    // .. DisableRcvr = 0
    // .. ==> 0XF8000710[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
	//EMIT_MASKWRITE(0XF8000710, 0x00003FFFU ,0x00000610U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF8000714[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF8000714[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF8000714[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 2
    // .. ==> 0XF8000714[4:3] = 0x00000002U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000010U
    // .. L3_SEL = 0
    // .. ==> 0XF8000714[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF8000714[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF8000714[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 0
    // .. ==> 0XF8000714[12:12] = 0x00000000U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00000000U
    // .. DisableRcvr = 0
    // .. ==> 0XF8000714[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
	//EMIT_MASKWRITE(0XF8000714, 0x00003FFFU ,0x00000610U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF8000718[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF8000718[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF8000718[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 2
    // .. ==> 0XF8000718[4:3] = 0x00000002U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000010U
    // .. L3_SEL = 0
    // .. ==> 0XF8000718[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF8000718[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF8000718[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 0
    // .. ==> 0XF8000718[12:12] = 0x00000000U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00000000U
    // .. DisableRcvr = 0
    // .. ==> 0XF8000718[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
	//EMIT_MASKWRITE(0XF8000718, 0x00003FFFU ,0x00000610U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF800071C[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF800071C[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF800071C[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 2
    // .. ==> 0XF800071C[4:3] = 0x00000002U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000010U
    // .. L3_SEL = 0
    // .. ==> 0XF800071C[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF800071C[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF800071C[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 0
    // .. ==> 0XF800071C[12:12] = 0x00000000U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00000000U
    // .. DisableRcvr = 0
    // .. ==> 0XF800071C[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
	//EMIT_MASKWRITE(0XF800071C, 0x00003FFFU ,0x00000610U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF8000720[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF8000720[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF8000720[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 2
    // .. ==> 0XF8000720[4:3] = 0x00000002U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000010U
    // .. L3_SEL = 0
    // .. ==> 0XF8000720[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF8000720[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF8000720[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 0
    // .. ==> 0XF8000720[12:12] = 0x00000000U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00000000U
    // .. DisableRcvr = 0
    // .. ==> 0XF8000720[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
	//EMIT_MASKWRITE(0XF8000720, 0x00003FFFU ,0x00000610U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF8000724[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF8000724[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF8000724[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 2
    // .. ==> 0XF8000724[4:3] = 0x00000002U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000010U
    // .. L3_SEL = 0
    // .. ==> 0XF8000724[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF8000724[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF8000724[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF8000724[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF8000724[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
	//EMIT_MASKWRITE(0XF8000724, 0x00003FFFU ,0x00001610U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF8000728[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF8000728[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF8000728[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 2
    // .. ==> 0XF8000728[4:3] = 0x00000002U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000010U
    // .. L3_SEL = 0
    // .. ==> 0XF8000728[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF8000728[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF8000728[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF8000728[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF8000728[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
	//EMIT_MASKWRITE(0XF8000728, 0x00003FFFU ,0x00001610U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF800072C[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF800072C[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF800072C[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 2
    // .. ==> 0XF800072C[4:3] = 0x00000002U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000010U
    // .. L3_SEL = 0
    // .. ==> 0XF800072C[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF800072C[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF800072C[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF800072C[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF800072C[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
	//EMIT_MASKWRITE(0XF800072C, 0x00003FFFU ,0x00001610U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF8000730[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF8000730[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF8000730[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 2
    // .. ==> 0XF8000730[4:3] = 0x00000002U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000010U
    // .. L3_SEL = 0
    // .. ==> 0XF8000730[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF8000730[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF8000730[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF8000730[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF8000730[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
	//EMIT_MASKWRITE(0XF8000730, 0x00003FFFU ,0x00001610U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF8000734[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF8000734[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF8000734[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 2
    // .. ==> 0XF8000734[4:3] = 0x00000002U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000010U
    // .. L3_SEL = 0
    // .. ==> 0XF8000734[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF8000734[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF8000734[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF8000734[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF8000734[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
	//EMIT_MASKWRITE(0XF8000734, 0x00003FFFU ,0x00001610U),
    // .. TRI_ENABLE = 1
    // .. ==> 0XF8000738[0:0] = 0x00000001U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000001U
    // .. L0_SEL = 0
    // .. ==> 0XF8000738[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF8000738[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 2
    // .. ==> 0XF8000738[4:3] = 0x00000002U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000010U
    // .. L3_SEL = 0
    // .. ==> 0XF8000738[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF8000738[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF8000738[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF8000738[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF8000738[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF8000738, 0x00003FFFU ,0x00001611U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF800073C[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF800073C[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF800073C[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF800073C[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF800073C[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF800073C[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF800073C[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF800073C[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF800073C[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
	//EMIT_MASKWRITE(0XF800073C, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF8000740[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF8000740[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF8000740[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF8000740[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF8000740[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF8000740[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF8000740[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF8000740[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF8000740[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
	//EMIT_MASKWRITE(0XF8000740, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF8000744[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF8000744[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF8000744[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF8000744[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF8000744[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF8000744[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF8000744[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF8000744[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF8000744[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
	//EMIT_MASKWRITE(0XF8000744, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF8000748[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF8000748[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF8000748[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF8000748[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF8000748[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF8000748[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF8000748[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF8000748[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF8000748[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
	//EMIT_MASKWRITE(0XF8000748, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF800074C[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF800074C[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF800074C[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF800074C[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF800074C[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF800074C[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF800074C[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF800074C[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF800074C[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
	//EMIT_MASKWRITE(0XF800074C, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF8000750[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF8000750[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF8000750[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF8000750[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF8000750[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF8000750[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF8000750[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF8000750[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF8000750[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
	//EMIT_MASKWRITE(0XF8000750, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF8000754[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF8000754[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF8000754[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF8000754[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF8000754[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF8000754[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF8000754[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF8000754[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF8000754[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
	//EMIT_MASKWRITE(0XF8000754, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF8000758[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF8000758[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF8000758[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF8000758[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF8000758[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF8000758[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF8000758[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF8000758[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF8000758[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
	//EMIT_MASKWRITE(0XF8000758, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF800075C[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF800075C[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF800075C[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF800075C[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF800075C[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF800075C[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF800075C[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF800075C[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF800075C[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
	//EMIT_MASKWRITE(0XF800075C, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF8000760[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF8000760[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF8000760[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF8000760[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 7
    // .. ==> 0XF8000760[7:5] = 0x00000007U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x000000E0U
    // .. Speed = 0
    // .. ==> 0XF8000760[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF8000760[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF8000760[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF8000760[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
	//EMIT_MASKWRITE(0XF8000760, 0x00003FFFU ,0x000016E0U),
    // .. TRI_ENABLE = 1
    // .. ==> 0XF8000764[0:0] = 0x00000001U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000001U
    // .. L0_SEL = 0
    // .. ==> 0XF8000764[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF8000764[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF8000764[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 7
    // .. ==> 0XF8000764[7:5] = 0x00000007U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x000000E0U
    // .. Speed = 0
    // .. ==> 0XF8000764[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF8000764[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF8000764[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF8000764[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
	//EMIT_MASKWRITE(0XF8000764, 0x00003FFFU ,0x000016E1U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF8000768[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF8000768[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF8000768[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF8000768[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF8000768[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 1
    // .. ==> 0XF8000768[8:8] = 0x00000001U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000100U
    // .. IO_Type = 3
    // .. ==> 0XF8000768[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 0
    // .. ==> 0XF8000768[12:12] = 0x00000000U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00000000U
    // .. DisableRcvr = 0
    // .. ==> 0XF8000768[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
	//EMIT_MASKWRITE(0XF8000768, 0x00003FFFU ,0x00000700U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF800076C[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF800076C[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF800076C[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF800076C[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF800076C[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 1
    // .. ==> 0XF800076C[8:8] = 0x00000001U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000100U
    // .. IO_Type = 3
    // .. ==> 0XF800076C[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 0
    // .. ==> 0XF800076C[12:12] = 0x00000000U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00000000U
    // .. DisableRcvr = 0
    // .. ==> 0XF800076C[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
	//EMIT_MASKWRITE(0XF800076C, 0x00003FFFU ,0x00000700U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF8000770[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF8000770[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF8000770[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF8000770[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF8000770[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF8000770[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF8000770[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF8000770[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF8000770[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
	//EMIT_MASKWRITE(0XF8000770, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF8000774[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF8000774[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF8000774[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF8000774[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF8000774[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF8000774[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF8000774[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF8000774[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF8000774[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
	//EMIT_MASKWRITE(0XF8000774, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF8000778[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF8000778[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF8000778[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF8000778[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF8000778[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF8000778[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF8000778[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF8000778[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF8000778[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
	//EMIT_MASKWRITE(0XF8000778, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF800077C[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF800077C[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF800077C[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF800077C[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF800077C[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF800077C[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF800077C[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF800077C[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF800077C[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
	//EMIT_MASKWRITE(0XF800077C, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF8000780[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF8000780[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF8000780[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF8000780[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF8000780[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF8000780[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF8000780[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF8000780[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF8000780[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    //EMIT_MASKWRITE(0XF8000780, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF8000784[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF8000784[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF8000784[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF8000784[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF8000784[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF8000784[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF8000784[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF8000784[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF8000784[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
	//EMIT_MASKWRITE(0XF8000784, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF8000788[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF8000788[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF8000788[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF8000788[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF8000788[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF8000788[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF8000788[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF8000788[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF8000788[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
	//EMIT_MASKWRITE(0XF8000788, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF800078C[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF800078C[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF800078C[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF800078C[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF800078C[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF800078C[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF800078C[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF800078C[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF800078C[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
	//EMIT_MASKWRITE(0XF800078C, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF8000790[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF8000790[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF8000790[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF8000790[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF8000790[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF8000790[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF8000790[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF8000790[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF8000790[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
	//EMIT_MASKWRITE(0XF8000790, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF8000794[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF8000794[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF8000794[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF8000794[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF8000794[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF8000794[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF8000794[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF8000794[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF8000794[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
	//EMIT_MASKWRITE(0XF8000794, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF8000798[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF8000798[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF8000798[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF8000798[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF8000798[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF8000798[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF8000798[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF8000798[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF8000798[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
	//EMIT_MASKWRITE(0XF8000798, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF800079C[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF800079C[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF800079C[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF800079C[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF800079C[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF800079C[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF800079C[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF800079C[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF800079C[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
	//EMIT_MASKWRITE(0XF800079C, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF80007A0[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF80007A0[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF80007A0[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF80007A0[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF80007A0[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF80007A0[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF80007A0[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF80007A0[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF80007A0[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
	//EMIT_MASKWRITE(0XF80007A0, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF80007A4[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF80007A4[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF80007A4[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF80007A4[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF80007A4[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF80007A4[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF80007A4[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF80007A4[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF80007A4[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
	//EMIT_MASKWRITE(0XF80007A4, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF80007A8[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF80007A8[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF80007A8[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF80007A8[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF80007A8[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF80007A8[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF80007A8[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF80007A8[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF80007A8[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
	//EMIT_MASKWRITE(0XF80007A8, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF80007AC[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF80007AC[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF80007AC[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF80007AC[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF80007AC[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF80007AC[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF80007AC[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF80007AC[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF80007AC[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
	//EMIT_MASKWRITE(0XF80007AC, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF80007B0[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF80007B0[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF80007B0[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF80007B0[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF80007B0[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF80007B0[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF80007B0[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF80007B0[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF80007B0[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
	//EMIT_MASKWRITE(0XF80007B0, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF80007B4[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF80007B4[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF80007B4[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF80007B4[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF80007B4[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF80007B4[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF80007B4[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF80007B4[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF80007B4[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
	//EMIT_MASKWRITE(0XF80007B4, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF80007B8[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF80007B8[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF80007B8[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF80007B8[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF80007B8[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF80007B8[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF80007B8[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF80007B8[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF80007B8[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
	//EMIT_MASKWRITE(0XF80007B8, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF80007BC[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF80007BC[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF80007BC[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF80007BC[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF80007BC[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF80007BC[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF80007BC[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF80007BC[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF80007BC[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
	//EMIT_MASKWRITE(0XF80007BC, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF80007C0[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF80007C0[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF80007C0[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF80007C0[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF80007C0[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF80007C0[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF80007C0[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF80007C0[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF80007C0[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
	//EMIT_MASKWRITE(0XF80007C0, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF80007C4[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF80007C4[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF80007C4[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF80007C4[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF80007C4[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF80007C4[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF80007C4[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF80007C4[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF80007C4[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
	//EMIT_MASKWRITE(0XF80007C4, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF80007C8[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF80007C8[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF80007C8[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF80007C8[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF80007C8[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF80007C8[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF80007C8[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF80007C8[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF80007C8[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
	//EMIT_MASKWRITE(0XF80007C8, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF80007CC[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF80007CC[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF80007CC[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF80007CC[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF80007CC[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF80007CC[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF80007CC[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF80007CC[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF80007CC[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
	//EMIT_MASKWRITE(0XF80007CC, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF80007D0[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF80007D0[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF80007D0[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF80007D0[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF80007D0[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF80007D0[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF80007D0[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF80007D0[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF80007D0[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
	//EMIT_MASKWRITE(0XF80007D0, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF80007D4[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF80007D4[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF80007D4[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF80007D4[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF80007D4[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF80007D4[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF80007D4[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF80007D4[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF80007D4[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    //EMIT_MASKWRITE(0XF80007D4, 0x00003FFFU ,0x00001600U),	// MIO_PIN_53
    // .. FINISH: MIO PROGRAMMING
    // .. START: LOCK IT BACK
    // .. LOCK_KEY = 0X767B
    // .. ==> 0XF8000004[15:0] = 0x0000767BU
    // ..     ==> MASK : 0x0000FFFFU    VAL : 0x0000767BU
    // ..
    //EMIT_WRITE(0XF8000004, 0x0000767BU),
    // .. FINISH: LOCK IT BACK
    // FINISH: top
    //
    EMIT_EXIT(),

    //
};

unsigned long ps7_peripherals_init_data_3_0[] = {
    // START: top
    // .. START: SLCR SETTINGS
    // .. UNLOCK_KEY = 0XDF0D
    // .. ==> 0XF8000008[15:0] = 0x0000DF0DU
    // ..     ==> MASK : 0x0000FFFFU    VAL : 0x0000DF0DU
    // ..
    EMIT_WRITE(0XF8000008, 0x0000DF0DU),
    // .. FINISH: SLCR SETTINGS
    // .. START: DDR TERM/IBUF_DISABLE_MODE SETTINGS
    // .. IBUF_DISABLE_MODE = 0x1
    // .. ==> 0XF8000B48[7:7] = 0x00000001U
    // ..     ==> MASK : 0x00000080U    VAL : 0x00000080U
    // .. TERM_DISABLE_MODE = 0x1
    // .. ==> 0XF8000B48[8:8] = 0x00000001U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000100U
    // ..
    EMIT_MASKWRITE(0XF8000B48, 0x00000180U ,0x00000180U),
    // .. IBUF_DISABLE_MODE = 0x1
    // .. ==> 0XF8000B4C[7:7] = 0x00000001U
    // ..     ==> MASK : 0x00000080U    VAL : 0x00000080U
    // .. TERM_DISABLE_MODE = 0x1
    // .. ==> 0XF8000B4C[8:8] = 0x00000001U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000100U
    // ..
    EMIT_MASKWRITE(0XF8000B4C, 0x00000180U ,0x00000180U),
    // .. IBUF_DISABLE_MODE = 0x1
    // .. ==> 0XF8000B50[7:7] = 0x00000001U
    // ..     ==> MASK : 0x00000080U    VAL : 0x00000080U
    // .. TERM_DISABLE_MODE = 0x1
    // .. ==> 0XF8000B50[8:8] = 0x00000001U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000100U
    // ..
    EMIT_MASKWRITE(0XF8000B50, 0x00000180U ,0x00000180U),
    // .. IBUF_DISABLE_MODE = 0x1
    // .. ==> 0XF8000B54[7:7] = 0x00000001U
    // ..     ==> MASK : 0x00000080U    VAL : 0x00000080U
    // .. TERM_DISABLE_MODE = 0x1
    // .. ==> 0XF8000B54[8:8] = 0x00000001U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000100U
    // ..
    EMIT_MASKWRITE(0XF8000B54, 0x00000180U ,0x00000180U),
    // .. FINISH: DDR TERM/IBUF_DISABLE_MODE SETTINGS
    // .. START: LOCK IT BACK
    // .. LOCK_KEY = 0X767B
    // .. ==> 0XF8000004[15:0] = 0x0000767BU
    // ..     ==> MASK : 0x0000FFFFU    VAL : 0x0000767BU
    // ..
    //EMIT_WRITE(0XF8000004, 0x0000767BU),
    // .. FINISH: LOCK IT BACK
    // .. START: SRAM/NOR SET OPMODE
    // .. FINISH: SRAM/NOR SET OPMODE
    // .. START: UART REGISTERS
    // .. BDIV = 0x6
    // .. ==> 0XE0001034[7:0] = 0x00000006U
    // ..     ==> MASK : 0x000000FFU    VAL : 0x00000006U
    // ..
    //EMIT_MASKWRITE(0XE0001034, 0x000000FFU ,0x00000006U),	// Baud_rate_divider_reg0
    // .. CD = 0x7c
    // .. ==> 0XE0001018[15:0] = 0x0000007CU
    // ..     ==> MASK : 0x0000FFFFU    VAL : 0x0000007CU
    // ..
    //EMIT_MASKWRITE(0XE0001018, 0x0000FFFFU ,0x0000007CU),
    // .. STPBRK = 0x0
    // .. ==> 0XE0001000[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. STTBRK = 0x0
    // .. ==> 0XE0001000[7:7] = 0x00000000U
    // ..     ==> MASK : 0x00000080U    VAL : 0x00000000U
    // .. RSTTO = 0x0
    // .. ==> 0XE0001000[6:6] = 0x00000000U
    // ..     ==> MASK : 0x00000040U    VAL : 0x00000000U
    // .. TXDIS = 0x0
    // .. ==> 0XE0001000[5:5] = 0x00000000U
    // ..     ==> MASK : 0x00000020U    VAL : 0x00000000U
    // .. TXEN = 0x1
    // .. ==> 0XE0001000[4:4] = 0x00000001U
    // ..     ==> MASK : 0x00000010U    VAL : 0x00000010U
    // .. RXDIS = 0x0
    // .. ==> 0XE0001000[3:3] = 0x00000000U
    // ..     ==> MASK : 0x00000008U    VAL : 0x00000000U
    // .. RXEN = 0x1
    // .. ==> 0XE0001000[2:2] = 0x00000001U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000004U
    // .. TXRES = 0x1
    // .. ==> 0XE0001000[1:1] = 0x00000001U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000002U
    // .. RXRES = 0x1
    // .. ==> 0XE0001000[0:0] = 0x00000001U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000001U
    // ..
    //EMIT_MASKWRITE(0XE0001000, 0x000001FFU ,0x00000017U),
    // .. CHMODE = 0x0
    // .. ==> 0XE0001004[9:8] = 0x00000000U
    // ..     ==> MASK : 0x00000300U    VAL : 0x00000000U
    // .. NBSTOP = 0x0
    // .. ==> 0XE0001004[7:6] = 0x00000000U
    // ..     ==> MASK : 0x000000C0U    VAL : 0x00000000U
    // .. PAR = 0x4
    // .. ==> 0XE0001004[5:3] = 0x00000004U
    // ..     ==> MASK : 0x00000038U    VAL : 0x00000020U
    // .. CHRL = 0x0
    // .. ==> 0XE0001004[2:1] = 0x00000000U
    // ..     ==> MASK : 0x00000006U    VAL : 0x00000000U
    // .. CLKS = 0x0
    // .. ==> 0XE0001004[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // ..
    //EMIT_MASKWRITE(0XE0001004, 0x000003FFU ,0x00000020U),	// XUARTPS_MR_OFFSET
    // .. FINISH: UART REGISTERS
    // .. START: QSPI REGISTERS
    // .. Holdb_dr = 1
    // .. ==> 0XE000D000[19:19] = 0x00000001U
    // ..     ==> MASK : 0x00080000U    VAL : 0x00080000U
    // ..
    //EMIT_MASKWRITE(0XE000D000, 0x00080000U ,0x00080000U),
    // .. FINISH: QSPI REGISTERS
    // .. START: PL POWER ON RESET REGISTERS
    // .. PCFG_POR_CNT_4K = 0
    // .. ==> 0XF8007000[29:29] = 0x00000000U
    // ..     ==> MASK : 0x20000000U    VAL : 0x00000000U
    // ..
    //EMIT_MASKWRITE(0XF8007000, 0x20000000U ,0x00000000U),
    // .. FINISH: PL POWER ON RESET REGISTERS
    // .. START: SMC TIMING CALCULATION REGISTER UPDATE
    // .. .. START: NAND SET CYCLE
    // .. .. Set_t0 = 0x2
    // .. .. ==> 0XE000E014[3:0] = 0x00000002U
    // .. ..     ==> MASK : 0x0000000FU    VAL : 0x00000002U
    // .. .. Set_t1 = 0x2
    // .. .. ==> 0XE000E014[7:4] = 0x00000002U
    // .. ..     ==> MASK : 0x000000F0U    VAL : 0x00000020U
    // .. .. Set_t2 = 0x1
    // .. .. ==> 0XE000E014[10:8] = 0x00000001U
    // .. ..     ==> MASK : 0x00000700U    VAL : 0x00000100U
    // .. .. Set_t3 = 0x1
    // .. .. ==> 0XE000E014[13:11] = 0x00000001U
    // .. ..     ==> MASK : 0x00003800U    VAL : 0x00000800U
    // .. .. Set_t4 = 0x1
    // .. .. ==> 0XE000E014[16:14] = 0x00000001U
    // .. ..     ==> MASK : 0x0001C000U    VAL : 0x00004000U
    // .. .. Set_t5 = 0x1
    // .. .. ==> 0XE000E014[19:17] = 0x00000001U
    // .. ..     ==> MASK : 0x000E0000U    VAL : 0x00020000U
    // .. .. Set_t6 = 0x1
    // .. .. ==> 0XE000E014[23:20] = 0x00000001U
    // .. ..     ==> MASK : 0x00F00000U    VAL : 0x00100000U
    // .. ..
    //EMIT_WRITE(0XE000E014, 0x00124922U),
    // .. .. FINISH: NAND SET CYCLE
    // .. .. START: OPMODE
    // .. .. set_mw = 0x0
    // .. .. ==> 0XE000E018[1:0] = 0x00000000U
    // .. ..     ==> MASK : 0x00000003U    VAL : 0x00000000U
    // .. ..
    //EMIT_MASKWRITE(0XE000E018, 0x00000003U ,0x00000000U),
    // .. .. FINISH: OPMODE
    // .. .. START: DIRECT COMMAND
    // .. .. chip_select = 0x4
    // .. .. ==> 0XE000E010[25:23] = 0x00000004U
    // .. ..     ==> MASK : 0x03800000U    VAL : 0x02000000U
    // .. .. cmd_type = 0x2
    // .. .. ==> 0XE000E010[22:21] = 0x00000002U
    // .. ..     ==> MASK : 0x00600000U    VAL : 0x00400000U
    // .. ..
    EMIT_WRITE(0XE000E010, 0x02400000U),
    // .. .. FINISH: DIRECT COMMAND
    // .. .. START: SRAM/NOR CS0 SET CYCLE
    // .. .. FINISH: SRAM/NOR CS0 SET CYCLE
    // .. .. START: DIRECT COMMAND
    // .. .. FINISH: DIRECT COMMAND
    // .. .. START: NOR CS0 BASE ADDRESS
    // .. .. FINISH: NOR CS0 BASE ADDRESS
    // .. .. START: SRAM/NOR CS1 SET CYCLE
    // .. .. FINISH: SRAM/NOR CS1 SET CYCLE
    // .. .. START: DIRECT COMMAND
    // .. .. FINISH: DIRECT COMMAND
    // .. .. START: NOR CS1 BASE ADDRESS
    // .. .. FINISH: NOR CS1 BASE ADDRESS
    // .. .. START: USB RESET
    // .. .. FINISH: USB RESET
    // .. .. START: ENET RESET
    // .. .. FINISH: ENET RESET
    // .. .. START: I2C RESET
    // .. .. FINISH: I2C RESET
    // .. .. START: NOR CHIP SELECT
    // .. .. .. START: DIR MODE BANK 0
    // .. .. .. FINISH: DIR MODE BANK 0
    // .. .. .. START: MASK_DATA_0_LSW HIGH BANK [15:0]
    // .. .. .. FINISH: MASK_DATA_0_LSW HIGH BANK [15:0]
    // .. .. .. START: OUTPUT ENABLE BANK 0
    // .. .. .. FINISH: OUTPUT ENABLE BANK 0
    // .. .. FINISH: NOR CHIP SELECT
    // .. FINISH: SMC TIMING CALCULATION REGISTER UPDATE
    // FINISH: top
    //
    EMIT_EXIT(),

    //
};

#if 0
unsigned long ps7_post_config_3_0[] = {
    // START: top
    // .. START: SLCR SETTINGS
    // .. UNLOCK_KEY = 0XDF0D
    // .. ==> 0XF8000008[15:0] = 0x0000DF0DU
    // ..     ==> MASK : 0x0000FFFFU    VAL : 0x0000DF0DU
    // ..
    EMIT_WRITE(0XF8000008, 0x0000DF0DU),
    // .. FINISH: SLCR SETTINGS
    // .. START: ENABLING LEVEL SHIFTER
    // .. USER_LVL_INP_EN_0 = 1
    // .. ==> 0XF8000900[3:3] = 0x00000001U
    // ..     ==> MASK : 0x00000008U    VAL : 0x00000008U
    // .. USER_LVL_OUT_EN_0 = 1
    // .. ==> 0XF8000900[2:2] = 0x00000001U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000004U
    // .. USER_LVL_INP_EN_1 = 1
    // .. ==> 0XF8000900[1:1] = 0x00000001U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000002U
    // .. USER_LVL_OUT_EN_1 = 1
    // .. ==> 0XF8000900[0:0] = 0x00000001U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000001U
    // ..
    EMIT_MASKWRITE(0XF8000900, 0x0000000FU ,0x0000000FU),
    // .. FINISH: ENABLING LEVEL SHIFTER
    // .. START: FPGA RESETS TO 0
    // .. reserved_3 = 0
    // .. ==> 0XF8000240[31:25] = 0x00000000U
    // ..     ==> MASK : 0xFE000000U    VAL : 0x00000000U
    // .. reserved_FPGA_ACP_RST = 0
    // .. ==> 0XF8000240[24:24] = 0x00000000U
    // ..     ==> MASK : 0x01000000U    VAL : 0x00000000U
    // .. reserved_FPGA_AXDS3_RST = 0
    // .. ==> 0XF8000240[23:23] = 0x00000000U
    // ..     ==> MASK : 0x00800000U    VAL : 0x00000000U
    // .. reserved_FPGA_AXDS2_RST = 0
    // .. ==> 0XF8000240[22:22] = 0x00000000U
    // ..     ==> MASK : 0x00400000U    VAL : 0x00000000U
    // .. reserved_FPGA_AXDS1_RST = 0
    // .. ==> 0XF8000240[21:21] = 0x00000000U
    // ..     ==> MASK : 0x00200000U    VAL : 0x00000000U
    // .. reserved_FPGA_AXDS0_RST = 0
    // .. ==> 0XF8000240[20:20] = 0x00000000U
    // ..     ==> MASK : 0x00100000U    VAL : 0x00000000U
    // .. reserved_2 = 0
    // .. ==> 0XF8000240[19:18] = 0x00000000U
    // ..     ==> MASK : 0x000C0000U    VAL : 0x00000000U
    // .. reserved_FSSW1_FPGA_RST = 0
    // .. ==> 0XF8000240[17:17] = 0x00000000U
    // ..     ==> MASK : 0x00020000U    VAL : 0x00000000U
    // .. reserved_FSSW0_FPGA_RST = 0
    // .. ==> 0XF8000240[16:16] = 0x00000000U
    // ..     ==> MASK : 0x00010000U    VAL : 0x00000000U
    // .. reserved_1 = 0
    // .. ==> 0XF8000240[15:14] = 0x00000000U
    // ..     ==> MASK : 0x0000C000U    VAL : 0x00000000U
    // .. reserved_FPGA_FMSW1_RST = 0
    // .. ==> 0XF8000240[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // .. reserved_FPGA_FMSW0_RST = 0
    // .. ==> 0XF8000240[12:12] = 0x00000000U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00000000U
    // .. reserved_FPGA_DMA3_RST = 0
    // .. ==> 0XF8000240[11:11] = 0x00000000U
    // ..     ==> MASK : 0x00000800U    VAL : 0x00000000U
    // .. reserved_FPGA_DMA2_RST = 0
    // .. ==> 0XF8000240[10:10] = 0x00000000U
    // ..     ==> MASK : 0x00000400U    VAL : 0x00000000U
    // .. reserved_FPGA_DMA1_RST = 0
    // .. ==> 0XF8000240[9:9] = 0x00000000U
    // ..     ==> MASK : 0x00000200U    VAL : 0x00000000U
    // .. reserved_FPGA_DMA0_RST = 0
    // .. ==> 0XF8000240[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. reserved = 0
    // .. ==> 0XF8000240[7:4] = 0x00000000U
    // ..     ==> MASK : 0x000000F0U    VAL : 0x00000000U
    // .. FPGA3_OUT_RST = 0
    // .. ==> 0XF8000240[3:3] = 0x00000000U
    // ..     ==> MASK : 0x00000008U    VAL : 0x00000000U
    // .. FPGA2_OUT_RST = 0
    // .. ==> 0XF8000240[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. FPGA1_OUT_RST = 0
    // .. ==> 0XF8000240[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. FPGA0_OUT_RST = 0
    // .. ==> 0XF8000240[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF8000240, 0xFFFFFFFFU ,0x00000000U),
    // .. FINISH: FPGA RESETS TO 0
    // .. START: AFI REGISTERS
    // .. .. START: AFI0 REGISTERS
    // .. .. FINISH: AFI0 REGISTERS
    // .. .. START: AFI1 REGISTERS
    // .. .. FINISH: AFI1 REGISTERS
    // .. .. START: AFI2 REGISTERS
    // .. .. FINISH: AFI2 REGISTERS
    // .. .. START: AFI3 REGISTERS
    // .. .. FINISH: AFI3 REGISTERS
    // .. .. START: AFI2 SECURE REGISTER
    // .. .. FINISH: AFI2 SECURE REGISTER
    // .. FINISH: AFI REGISTERS
    // .. START: LOCK IT BACK
    // .. LOCK_KEY = 0X767B
    // .. ==> 0XF8000004[15:0] = 0x0000767BU
    // ..     ==> MASK : 0x0000FFFFU    VAL : 0x0000767BU
    // ..
    //EMIT_WRITE(0XF8000004, 0x0000767BU),
    // .. FINISH: LOCK IT BACK
    // FINISH: top
    //
    EMIT_EXIT(),

    //
};

unsigned long ps7_debug_3_0[] = {
    // START: top
    // .. START: CROSS TRIGGER CONFIGURATIONS
    // .. .. START: UNLOCKING CTI REGISTERS
    // .. .. KEY = 0XC5ACCE55
    // .. .. ==> 0XF8898FB0[31:0] = 0xC5ACCE55U
    // .. ..     ==> MASK : 0xFFFFFFFFU    VAL : 0xC5ACCE55U
    // .. ..
    EMIT_WRITE(0XF8898FB0, 0xC5ACCE55U),
    // .. .. KEY = 0XC5ACCE55
    // .. .. ==> 0XF8899FB0[31:0] = 0xC5ACCE55U
    // .. ..     ==> MASK : 0xFFFFFFFFU    VAL : 0xC5ACCE55U
    // .. ..
    EMIT_WRITE(0XF8899FB0, 0xC5ACCE55U),
    // .. .. KEY = 0XC5ACCE55
    // .. .. ==> 0XF8809FB0[31:0] = 0xC5ACCE55U
    // .. ..     ==> MASK : 0xFFFFFFFFU    VAL : 0xC5ACCE55U
    // .. ..
    EMIT_WRITE(0XF8809FB0, 0xC5ACCE55U),
    // .. .. FINISH: UNLOCKING CTI REGISTERS
    // .. .. START: ENABLING CTI MODULES AND CHANNELS
    // .. .. FINISH: ENABLING CTI MODULES AND CHANNELS
    // .. .. START: MAPPING CPU0, CPU1 AND FTM EVENTS TO CTM CHANNELS
    // .. .. FINISH: MAPPING CPU0, CPU1 AND FTM EVENTS TO CTM CHANNELS
    // .. FINISH: CROSS TRIGGER CONFIGURATIONS
    // FINISH: top
    //
    EMIT_EXIT(),

    //
};

unsigned long ps7_pll_init_data_2_0[] = {
    // START: top
    // .. START: SLCR SETTINGS
    // .. UNLOCK_KEY = 0XDF0D
    // .. ==> 0XF8000008[15:0] = 0x0000DF0DU
    // ..     ==> MASK : 0x0000FFFFU    VAL : 0x0000DF0DU
    // ..
    EMIT_WRITE(0XF8000008, 0x0000DF0DU),
    // .. FINISH: SLCR SETTINGS
    // .. START: PLL SLCR REGISTERS
    // .. .. START: ARM PLL INIT
    // .. .. PLL_RES = 0x2
    // .. .. ==> 0XF8000110[7:4] = 0x00000002U
    // .. ..     ==> MASK : 0x000000F0U    VAL : 0x00000020U
    // .. .. PLL_CP = 0x2
    // .. .. ==> 0XF8000110[11:8] = 0x00000002U
    // .. ..     ==> MASK : 0x00000F00U    VAL : 0x00000200U
    // .. .. LOCK_CNT = 0xfa
    // .. .. ==> 0XF8000110[21:12] = 0x000000FAU
    // .. ..     ==> MASK : 0x003FF000U    VAL : 0x000FA000U
    // .. ..
    EMIT_MASKWRITE(0XF8000110, 0x003FFFF0U ,0x000FA220U),
    // .. .. .. START: UPDATE FB_DIV
    // .. .. .. PLL_FDIV = 0x28
    // .. .. .. ==> 0XF8000100[18:12] = 0x00000028U
    // .. .. ..     ==> MASK : 0x0007F000U    VAL : 0x00028000U
    // .. .. ..
    EMIT_MASKWRITE(0XF8000100, 0x0007F000U ,0x00028000U),
    // .. .. .. FINISH: UPDATE FB_DIV
    // .. .. .. START: BY PASS PLL
    // .. .. .. PLL_BYPASS_FORCE = 1
    // .. .. .. ==> 0XF8000100[4:4] = 0x00000001U
    // .. .. ..     ==> MASK : 0x00000010U    VAL : 0x00000010U
    // .. .. ..
    EMIT_MASKWRITE(0XF8000100, 0x00000010U ,0x00000010U),
    // .. .. .. FINISH: BY PASS PLL
    // .. .. .. START: ASSERT RESET
    // .. .. .. PLL_RESET = 1
    // .. .. .. ==> 0XF8000100[0:0] = 0x00000001U
    // .. .. ..     ==> MASK : 0x00000001U    VAL : 0x00000001U
    // .. .. ..
    EMIT_MASKWRITE(0XF8000100, 0x00000001U ,0x00000001U),
    // .. .. .. FINISH: ASSERT RESET
    // .. .. .. START: DEASSERT RESET
    // .. .. .. PLL_RESET = 0
    // .. .. .. ==> 0XF8000100[0:0] = 0x00000000U
    // .. .. ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. .. ..
    EMIT_MASKWRITE(0XF8000100, 0x00000001U ,0x00000000U),
    // .. .. .. FINISH: DEASSERT RESET
    // .. .. .. START: CHECK PLL STATUS
    // .. .. .. ARM_PLL_LOCK = 1
    // .. .. .. ==> 0XF800010C[0:0] = 0x00000001U
    // .. .. ..     ==> MASK : 0x00000001U    VAL : 0x00000001U
    // .. .. ..
    EMIT_MASKPOLL(0XF800010C, 0x00000001U),
    // .. .. .. FINISH: CHECK PLL STATUS
    // .. .. .. START: REMOVE PLL BY PASS
    // .. .. .. PLL_BYPASS_FORCE = 0
    // .. .. .. ==> 0XF8000100[4:4] = 0x00000000U
    // .. .. ..     ==> MASK : 0x00000010U    VAL : 0x00000000U
    // .. .. ..
    EMIT_MASKWRITE(0XF8000100, 0x00000010U ,0x00000000U),
    // .. .. .. FINISH: REMOVE PLL BY PASS
    // .. .. .. SRCSEL = 0x0
    // .. .. .. ==> 0XF8000120[5:4] = 0x00000000U
    // .. .. ..     ==> MASK : 0x00000030U    VAL : 0x00000000U
    // .. .. .. DIVISOR = 0x2
    // .. .. .. ==> 0XF8000120[13:8] = 0x00000002U
    // .. .. ..     ==> MASK : 0x00003F00U    VAL : 0x00000200U
    // .. .. .. CPU_6OR4XCLKACT = 0x1
    // .. .. .. ==> 0XF8000120[24:24] = 0x00000001U
    // .. .. ..     ==> MASK : 0x01000000U    VAL : 0x01000000U
    // .. .. .. CPU_3OR2XCLKACT = 0x1
    // .. .. .. ==> 0XF8000120[25:25] = 0x00000001U
    // .. .. ..     ==> MASK : 0x02000000U    VAL : 0x02000000U
    // .. .. .. CPU_2XCLKACT = 0x1
    // .. .. .. ==> 0XF8000120[26:26] = 0x00000001U
    // .. .. ..     ==> MASK : 0x04000000U    VAL : 0x04000000U
    // .. .. .. CPU_1XCLKACT = 0x1
    // .. .. .. ==> 0XF8000120[27:27] = 0x00000001U
    // .. .. ..     ==> MASK : 0x08000000U    VAL : 0x08000000U
    // .. .. .. CPU_PERI_CLKACT = 0x1
    // .. .. .. ==> 0XF8000120[28:28] = 0x00000001U
    // .. .. ..     ==> MASK : 0x10000000U    VAL : 0x10000000U
    // .. .. ..
    ////EMIT_MASKWRITE(0XF8000120, 0x1F003F30U ,0x1F000200U),
    // .. .. FINISH: ARM PLL INIT
    // .. .. START: DDR PLL INIT
    // .. .. PLL_RES = 0x2
    // .. .. ==> 0XF8000114[7:4] = 0x00000002U
    // .. ..     ==> MASK : 0x000000F0U    VAL : 0x00000020U
    // .. .. PLL_CP = 0x2
    // .. .. ==> 0XF8000114[11:8] = 0x00000002U
    // .. ..     ==> MASK : 0x00000F00U    VAL : 0x00000200U
    // .. .. LOCK_CNT = 0x12c
    // .. .. ==> 0XF8000114[21:12] = 0x0000012CU
    // .. ..     ==> MASK : 0x003FF000U    VAL : 0x0012C000U
    // .. ..
    EMIT_MASKWRITE(0XF8000114, 0x003FFFF0U ,0x0012C220U),
    // .. .. .. START: UPDATE FB_DIV
    // .. .. .. PLL_FDIV = 0x20
    // .. .. .. ==> 0XF8000104[18:12] = 0x00000020U
    // .. .. ..     ==> MASK : 0x0007F000U    VAL : 0x00020000U
    // .. .. ..
    EMIT_MASKWRITE(0XF8000104, 0x0007F000U ,0x00020000U),
    // .. .. .. FINISH: UPDATE FB_DIV
    // .. .. .. START: BY PASS PLL
    // .. .. .. PLL_BYPASS_FORCE = 1
    // .. .. .. ==> 0XF8000104[4:4] = 0x00000001U
    // .. .. ..     ==> MASK : 0x00000010U    VAL : 0x00000010U
    // .. .. ..
    EMIT_MASKWRITE(0XF8000104, 0x00000010U ,0x00000010U),
    // .. .. .. FINISH: BY PASS PLL
    // .. .. .. START: ASSERT RESET
    // .. .. .. PLL_RESET = 1
    // .. .. .. ==> 0XF8000104[0:0] = 0x00000001U
    // .. .. ..     ==> MASK : 0x00000001U    VAL : 0x00000001U
    // .. .. ..
    EMIT_MASKWRITE(0XF8000104, 0x00000001U ,0x00000001U),
    // .. .. .. FINISH: ASSERT RESET
    // .. .. .. START: DEASSERT RESET
    // .. .. .. PLL_RESET = 0
    // .. .. .. ==> 0XF8000104[0:0] = 0x00000000U
    // .. .. ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. .. ..
    EMIT_MASKWRITE(0XF8000104, 0x00000001U ,0x00000000U),
    // .. .. .. FINISH: DEASSERT RESET
    // .. .. .. START: CHECK PLL STATUS
    // .. .. .. DDR_PLL_LOCK = 1
    // .. .. .. ==> 0XF800010C[1:1] = 0x00000001U
    // .. .. ..     ==> MASK : 0x00000002U    VAL : 0x00000002U
    // .. .. ..
    EMIT_MASKPOLL(0XF800010C, 0x00000002U),
    // .. .. .. FINISH: CHECK PLL STATUS
    // .. .. .. START: REMOVE PLL BY PASS
    // .. .. .. PLL_BYPASS_FORCE = 0
    // .. .. .. ==> 0XF8000104[4:4] = 0x00000000U
    // .. .. ..     ==> MASK : 0x00000010U    VAL : 0x00000000U
    // .. .. ..
    EMIT_MASKWRITE(0XF8000104, 0x00000010U ,0x00000000U),
    // .. .. .. FINISH: REMOVE PLL BY PASS
    // .. .. .. DDR_3XCLKACT = 0x1
    // .. .. .. ==> 0XF8000124[0:0] = 0x00000001U
    // .. .. ..     ==> MASK : 0x00000001U    VAL : 0x00000001U
    // .. .. .. DDR_2XCLKACT = 0x1
    // .. .. .. ==> 0XF8000124[1:1] = 0x00000001U
    // .. .. ..     ==> MASK : 0x00000002U    VAL : 0x00000002U
    // .. .. .. DDR_3XCLK_DIVISOR = 0x2
    // .. .. .. ==> 0XF8000124[25:20] = 0x00000002U
    // .. .. ..     ==> MASK : 0x03F00000U    VAL : 0x00200000U
    // .. .. .. DDR_2XCLK_DIVISOR = 0x3
    // .. .. .. ==> 0XF8000124[31:26] = 0x00000003U
    // .. .. ..     ==> MASK : 0xFC000000U    VAL : 0x0C000000U
    // .. .. ..
    EMIT_MASKWRITE(0XF8000124, 0xFFF00003U ,0x0C200003U),
    // .. .. FINISH: DDR PLL INIT
    // .. .. START: IO PLL INIT
    // .. .. PLL_RES = 0x4
    // .. .. ==> 0XF8000118[7:4] = 0x00000004U
    // .. ..     ==> MASK : 0x000000F0U    VAL : 0x00000040U
    // .. .. PLL_CP = 0x2
    // .. .. ==> 0XF8000118[11:8] = 0x00000002U
    // .. ..     ==> MASK : 0x00000F00U    VAL : 0x00000200U
    // .. .. LOCK_CNT = 0xfa
    // .. .. ==> 0XF8000118[21:12] = 0x000000FAU
    // .. ..     ==> MASK : 0x003FF000U    VAL : 0x000FA000U
    // .. ..
    EMIT_MASKWRITE(0XF8000118, 0x003FFFF0U ,0x000FA240U),
    // .. .. .. START: UPDATE FB_DIV
    // .. .. .. PLL_FDIV = 0x30
    // .. .. .. ==> 0XF8000108[18:12] = 0x00000030U
    // .. .. ..     ==> MASK : 0x0007F000U    VAL : 0x00030000U
    // .. .. ..
    EMIT_MASKWRITE(0XF8000108, 0x0007F000U ,0x00030000U),
    // .. .. .. FINISH: UPDATE FB_DIV
    // .. .. .. START: BY PASS PLL
    // .. .. .. PLL_BYPASS_FORCE = 1
    // .. .. .. ==> 0XF8000108[4:4] = 0x00000001U
    // .. .. ..     ==> MASK : 0x00000010U    VAL : 0x00000010U
    // .. .. ..
    EMIT_MASKWRITE(0XF8000108, 0x00000010U ,0x00000010U),
    // .. .. .. FINISH: BY PASS PLL
    // .. .. .. START: ASSERT RESET
    // .. .. .. PLL_RESET = 1
    // .. .. .. ==> 0XF8000108[0:0] = 0x00000001U
    // .. .. ..     ==> MASK : 0x00000001U    VAL : 0x00000001U
    // .. .. ..
    EMIT_MASKWRITE(0XF8000108, 0x00000001U ,0x00000001U),
    // .. .. .. FINISH: ASSERT RESET
    // .. .. .. START: DEASSERT RESET
    // .. .. .. PLL_RESET = 0
    // .. .. .. ==> 0XF8000108[0:0] = 0x00000000U
    // .. .. ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. .. ..
    EMIT_MASKWRITE(0XF8000108, 0x00000001U ,0x00000000U),
    // .. .. .. FINISH: DEASSERT RESET
    // .. .. .. START: CHECK PLL STATUS
    // .. .. .. IO_PLL_LOCK = 1
    // .. .. .. ==> 0XF800010C[2:2] = 0x00000001U
    // .. .. ..     ==> MASK : 0x00000004U    VAL : 0x00000004U
    // .. .. ..
    EMIT_MASKPOLL(0XF800010C, 0x00000004U),
    // .. .. .. FINISH: CHECK PLL STATUS
    // .. .. .. START: REMOVE PLL BY PASS
    // .. .. .. PLL_BYPASS_FORCE = 0
    // .. .. .. ==> 0XF8000108[4:4] = 0x00000000U
    // .. .. ..     ==> MASK : 0x00000010U    VAL : 0x00000000U
    // .. .. ..
    EMIT_MASKWRITE(0XF8000108, 0x00000010U ,0x00000000U),
    // .. .. .. FINISH: REMOVE PLL BY PASS
    // .. .. FINISH: IO PLL INIT
    // .. FINISH: PLL SLCR REGISTERS
    // .. START: LOCK IT BACK
    // .. LOCK_KEY = 0X767B
    // .. ==> 0XF8000004[15:0] = 0x0000767BU
    // ..     ==> MASK : 0x0000FFFFU    VAL : 0x0000767BU
    // ..
    //EMIT_WRITE(0XF8000004, 0x0000767BU),
    // .. FINISH: LOCK IT BACK
    // FINISH: top
    //
    EMIT_EXIT(),

    //
};

unsigned long ps7_clock_init_data_2_0[] = {
    // START: top
    // .. START: SLCR SETTINGS
    // .. UNLOCK_KEY = 0XDF0D
    // .. ==> 0XF8000008[15:0] = 0x0000DF0DU
    // ..     ==> MASK : 0x0000FFFFU    VAL : 0x0000DF0DU
    // ..
    EMIT_WRITE(0XF8000008, 0x0000DF0DU),
    // .. FINISH: SLCR SETTINGS
    // .. START: CLOCK CONTROL SLCR REGISTERS
    // .. CLKACT = 0x1
    // .. ==> 0XF8000128[0:0] = 0x00000001U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000001U
    // .. DIVISOR0 = 0xf
    // .. ==> 0XF8000128[13:8] = 0x0000000FU
    // ..     ==> MASK : 0x00003F00U    VAL : 0x00000F00U
    // .. DIVISOR1 = 0x7
    // .. ==> 0XF8000128[25:20] = 0x00000007U
    // ..     ==> MASK : 0x03F00000U    VAL : 0x00700000U
    // ..
    EMIT_MASKWRITE(0XF8000128, 0x03F03F01U ,0x00700F01U),
    // .. CLKACT = 0x1
    // .. ==> 0XF8000148[0:0] = 0x00000001U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000001U
    // .. SRCSEL = 0x0
    // .. ==> 0XF8000148[5:4] = 0x00000000U
    // ..     ==> MASK : 0x00000030U    VAL : 0x00000000U
    // .. DIVISOR = 0x10
    // .. ==> 0XF8000148[13:8] = 0x00000010U
    // ..     ==> MASK : 0x00003F00U    VAL : 0x00001000U
    // ..
    EMIT_MASKWRITE(0XF8000148, 0x00003F31U ,0x00001001U),
    // .. CLKACT0 = 0x0
    // .. ==> 0XF8000154[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. CLKACT1 = 0x1
    // .. ==> 0XF8000154[1:1] = 0x00000001U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000002U
    // .. SRCSEL = 0x0
    // .. ==> 0XF8000154[5:4] = 0x00000000U
    // ..     ==> MASK : 0x00000030U    VAL : 0x00000000U
    // .. DIVISOR = 0x10
    // .. ==> 0XF8000154[13:8] = 0x00000010U
    // ..     ==> MASK : 0x00003F00U    VAL : 0x00001000U
    // ..
    EMIT_MASKWRITE(0XF8000154, 0x00003F33U ,0x00001002U),
    // .. .. START: TRACE CLOCK
    // .. .. FINISH: TRACE CLOCK
    // .. .. CLKACT = 0x1
    // .. .. ==> 0XF8000168[0:0] = 0x00000001U
    // .. ..     ==> MASK : 0x00000001U    VAL : 0x00000001U
    // .. .. SRCSEL = 0x0
    // .. .. ==> 0XF8000168[5:4] = 0x00000000U
    // .. ..     ==> MASK : 0x00000030U    VAL : 0x00000000U
    // .. .. DIVISOR = 0x8
    // .. .. ==> 0XF8000168[13:8] = 0x00000008U
    // .. ..     ==> MASK : 0x00003F00U    VAL : 0x00000800U
    // .. ..
    EMIT_MASKWRITE(0XF8000168, 0x00003F31U ,0x00000801U),
    // .. .. SRCSEL = 0x0
    // .. .. ==> 0XF8000170[5:4] = 0x00000000U
    // .. ..     ==> MASK : 0x00000030U    VAL : 0x00000000U
    // .. .. DIVISOR0 = 0x8
    // .. .. ==> 0XF8000170[13:8] = 0x00000008U
    // .. ..     ==> MASK : 0x00003F00U    VAL : 0x00000800U
    // .. .. DIVISOR1 = 0x4
    // .. .. ==> 0XF8000170[25:20] = 0x00000004U
    // .. ..     ==> MASK : 0x03F00000U    VAL : 0x00400000U
    // .. ..
    //EMIT_MASKWRITE(0XF8000170, 0x03F03F30U ,0x00400800U),	// FPGA0_CLK_CTRL PL Clock 0 Output control
    // .. .. CLK_621_TRUE = 0x1
    // .. .. ==> 0XF80001C4[0:0] = 0x00000001U
    // .. ..     ==> MASK : 0x00000001U    VAL : 0x00000001U
    // .. ..
    //EMIT_MASKWRITE(0XF80001C4, 0x00000001U ,0x00000001U),	// CLK_621_TRUE CPU Clock Ratio Mode select
    // .. .. DMA_CPU_2XCLKACT = 0x1
    // .. .. ==> 0XF800012C[0:0] = 0x00000001U
    // .. ..     ==> MASK : 0x00000001U    VAL : 0x00000001U
    // .. .. USB0_CPU_1XCLKACT = 0x1
    // .. .. ==> 0XF800012C[2:2] = 0x00000001U
    // .. ..     ==> MASK : 0x00000004U    VAL : 0x00000004U
    // .. .. USB1_CPU_1XCLKACT = 0x1
    // .. .. ==> 0XF800012C[3:3] = 0x00000001U
    // .. ..     ==> MASK : 0x00000008U    VAL : 0x00000008U
    // .. .. GEM0_CPU_1XCLKACT = 0x0
    // .. .. ==> 0XF800012C[6:6] = 0x00000000U
    // .. ..     ==> MASK : 0x00000040U    VAL : 0x00000000U
    // .. .. GEM1_CPU_1XCLKACT = 0x0
    // .. .. ==> 0XF800012C[7:7] = 0x00000000U
    // .. ..     ==> MASK : 0x00000080U    VAL : 0x00000000U
    // .. .. SDI0_CPU_1XCLKACT = 0x0
    // .. .. ==> 0XF800012C[10:10] = 0x00000000U
    // .. ..     ==> MASK : 0x00000400U    VAL : 0x00000000U
    // .. .. SDI1_CPU_1XCLKACT = 0x0
    // .. .. ==> 0XF800012C[11:11] = 0x00000000U
    // .. ..     ==> MASK : 0x00000800U    VAL : 0x00000000U
    // .. .. SPI0_CPU_1XCLKACT = 0x0
    // .. .. ==> 0XF800012C[14:14] = 0x00000000U
    // .. ..     ==> MASK : 0x00004000U    VAL : 0x00000000U
    // .. .. SPI1_CPU_1XCLKACT = 0x0
    // .. .. ==> 0XF800012C[15:15] = 0x00000000U
    // .. ..     ==> MASK : 0x00008000U    VAL : 0x00000000U
    // .. .. CAN0_CPU_1XCLKACT = 0x0
    // .. .. ==> 0XF800012C[16:16] = 0x00000000U
    // .. ..     ==> MASK : 0x00010000U    VAL : 0x00000000U
    // .. .. CAN1_CPU_1XCLKACT = 0x0
    // .. .. ==> 0XF800012C[17:17] = 0x00000000U
    // .. ..     ==> MASK : 0x00020000U    VAL : 0x00000000U
    // .. .. I2C0_CPU_1XCLKACT = 0x1
    // .. .. ==> 0XF800012C[18:18] = 0x00000001U
    // .. ..     ==> MASK : 0x00040000U    VAL : 0x00040000U
    // .. .. I2C1_CPU_1XCLKACT = 0x1
    // .. .. ==> 0XF800012C[19:19] = 0x00000001U
    // .. ..     ==> MASK : 0x00080000U    VAL : 0x00080000U
    // .. .. UART0_CPU_1XCLKACT = 0x0
    // .. .. ==> 0XF800012C[20:20] = 0x00000000U
    // .. ..     ==> MASK : 0x00100000U    VAL : 0x00000000U
    // .. .. UART1_CPU_1XCLKACT = 0x1
    // .. .. ==> 0XF800012C[21:21] = 0x00000001U
    // .. ..     ==> MASK : 0x00200000U    VAL : 0x00200000U
    // .. .. GPIO_CPU_1XCLKACT = 0x1
    // .. .. ==> 0XF800012C[22:22] = 0x00000001U
    // .. ..     ==> MASK : 0x00400000U    VAL : 0x00400000U
    // .. .. LQSPI_CPU_1XCLKACT = 0x0
    // .. .. ==> 0XF800012C[23:23] = 0x00000000U
    // .. ..     ==> MASK : 0x00800000U    VAL : 0x00000000U
    // .. .. SMC_CPU_1XCLKACT = 0x1
    // .. .. ==> 0XF800012C[24:24] = 0x00000001U
    // .. ..     ==> MASK : 0x01000000U    VAL : 0x01000000U
    // .. ..
    EMIT_MASKWRITE(0XF800012C, 0x01FFCCCDU ,0x016C000DU),
    // .. FINISH: CLOCK CONTROL SLCR REGISTERS
    // .. START: THIS SHOULD BE BLANK
    // .. FINISH: THIS SHOULD BE BLANK
    // .. START: LOCK IT BACK
    // .. LOCK_KEY = 0X767B
    // .. ==> 0XF8000004[15:0] = 0x0000767BU
    // ..     ==> MASK : 0x0000FFFFU    VAL : 0x0000767BU
    // ..
    //EMIT_WRITE(0XF8000004, 0x0000767BU),
    // .. FINISH: LOCK IT BACK
    // FINISH: top
    //
    EMIT_EXIT(),

    //
};

unsigned long ps7_ddr_init_data_2_0[] = {
    // START: top
    // .. START: DDR INITIALIZATION
    // .. .. START: LOCK DDR
    // .. .. reg_ddrc_soft_rstb = 0
    // .. .. ==> 0XF8006000[0:0] = 0x00000000U
    // .. ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. .. reg_ddrc_powerdown_en = 0x0
    // .. .. ==> 0XF8006000[1:1] = 0x00000000U
    // .. ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. .. reg_ddrc_data_bus_width = 0x0
    // .. .. ==> 0XF8006000[3:2] = 0x00000000U
    // .. ..     ==> MASK : 0x0000000CU    VAL : 0x00000000U
    // .. .. reg_ddrc_burst8_refresh = 0x0
    // .. .. ==> 0XF8006000[6:4] = 0x00000000U
    // .. ..     ==> MASK : 0x00000070U    VAL : 0x00000000U
    // .. .. reg_ddrc_rdwr_idle_gap = 0x1
    // .. .. ==> 0XF8006000[13:7] = 0x00000001U
    // .. ..     ==> MASK : 0x00003F80U    VAL : 0x00000080U
    // .. .. reg_ddrc_dis_rd_bypass = 0x0
    // .. .. ==> 0XF8006000[14:14] = 0x00000000U
    // .. ..     ==> MASK : 0x00004000U    VAL : 0x00000000U
    // .. .. reg_ddrc_dis_act_bypass = 0x0
    // .. .. ==> 0XF8006000[15:15] = 0x00000000U
    // .. ..     ==> MASK : 0x00008000U    VAL : 0x00000000U
    // .. .. reg_ddrc_dis_auto_refresh = 0x0
    // .. .. ==> 0XF8006000[16:16] = 0x00000000U
    // .. ..     ==> MASK : 0x00010000U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006000, 0x0001FFFFU ,0x00000080U),
    // .. .. FINISH: LOCK DDR
    // .. .. reg_ddrc_t_rfc_nom_x32 = 0x82
    // .. .. ==> 0XF8006004[11:0] = 0x00000082U
    // .. ..     ==> MASK : 0x00000FFFU    VAL : 0x00000082U
    // .. .. reg_ddrc_active_ranks = 0x1
    // .. .. ==> 0XF8006004[13:12] = 0x00000001U
    // .. ..     ==> MASK : 0x00003000U    VAL : 0x00001000U
    // .. .. reg_ddrc_addrmap_cs_bit0 = 0x0
    // .. .. ==> 0XF8006004[18:14] = 0x00000000U
    // .. ..     ==> MASK : 0x0007C000U    VAL : 0x00000000U
    // .. .. reg_ddrc_wr_odt_block = 0x1
    // .. .. ==> 0XF8006004[20:19] = 0x00000001U
    // .. ..     ==> MASK : 0x00180000U    VAL : 0x00080000U
    // .. .. reg_ddrc_diff_rank_rd_2cycle_gap = 0x0
    // .. .. ==> 0XF8006004[21:21] = 0x00000000U
    // .. ..     ==> MASK : 0x00200000U    VAL : 0x00000000U
    // .. .. reg_ddrc_addrmap_cs_bit1 = 0x0
    // .. .. ==> 0XF8006004[26:22] = 0x00000000U
    // .. ..     ==> MASK : 0x07C00000U    VAL : 0x00000000U
    // .. .. reg_ddrc_addrmap_open_bank = 0x0
    // .. .. ==> 0XF8006004[27:27] = 0x00000000U
    // .. ..     ==> MASK : 0x08000000U    VAL : 0x00000000U
    // .. .. reg_ddrc_addrmap_4bank_ram = 0x0
    // .. .. ==> 0XF8006004[28:28] = 0x00000000U
    // .. ..     ==> MASK : 0x10000000U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006004, 0x1FFFFFFFU ,0x00081082U),
    // .. .. reg_ddrc_hpr_min_non_critical_x32 = 0xf
    // .. .. ==> 0XF8006008[10:0] = 0x0000000FU
    // .. ..     ==> MASK : 0x000007FFU    VAL : 0x0000000FU
    // .. .. reg_ddrc_hpr_max_starve_x32 = 0xf
    // .. .. ==> 0XF8006008[21:11] = 0x0000000FU
    // .. ..     ==> MASK : 0x003FF800U    VAL : 0x00007800U
    // .. .. reg_ddrc_hpr_xact_run_length = 0xf
    // .. .. ==> 0XF8006008[25:22] = 0x0000000FU
    // .. ..     ==> MASK : 0x03C00000U    VAL : 0x03C00000U
    // .. ..
    EMIT_MASKWRITE(0XF8006008, 0x03FFFFFFU ,0x03C0780FU),
    // .. .. reg_ddrc_lpr_min_non_critical_x32 = 0x1
    // .. .. ==> 0XF800600C[10:0] = 0x00000001U
    // .. ..     ==> MASK : 0x000007FFU    VAL : 0x00000001U
    // .. .. reg_ddrc_lpr_max_starve_x32 = 0x2
    // .. .. ==> 0XF800600C[21:11] = 0x00000002U
    // .. ..     ==> MASK : 0x003FF800U    VAL : 0x00001000U
    // .. .. reg_ddrc_lpr_xact_run_length = 0x8
    // .. .. ==> 0XF800600C[25:22] = 0x00000008U
    // .. ..     ==> MASK : 0x03C00000U    VAL : 0x02000000U
    // .. ..
    EMIT_MASKWRITE(0XF800600C, 0x03FFFFFFU ,0x02001001U),
    // .. .. reg_ddrc_w_min_non_critical_x32 = 0x1
    // .. .. ==> 0XF8006010[10:0] = 0x00000001U
    // .. ..     ==> MASK : 0x000007FFU    VAL : 0x00000001U
    // .. .. reg_ddrc_w_xact_run_length = 0x8
    // .. .. ==> 0XF8006010[14:11] = 0x00000008U
    // .. ..     ==> MASK : 0x00007800U    VAL : 0x00004000U
    // .. .. reg_ddrc_w_max_starve_x32 = 0x2
    // .. .. ==> 0XF8006010[25:15] = 0x00000002U
    // .. ..     ==> MASK : 0x03FF8000U    VAL : 0x00010000U
    // .. ..
    EMIT_MASKWRITE(0XF8006010, 0x03FFFFFFU ,0x00014001U),
    // .. .. reg_ddrc_t_rc = 0x1b
    // .. .. ==> 0XF8006014[5:0] = 0x0000001BU
    // .. ..     ==> MASK : 0x0000003FU    VAL : 0x0000001BU
    // .. .. reg_ddrc_t_rfc_min = 0x56
    // .. .. ==> 0XF8006014[13:6] = 0x00000056U
    // .. ..     ==> MASK : 0x00003FC0U    VAL : 0x00001580U
    // .. .. reg_ddrc_post_selfref_gap_x32 = 0x10
    // .. .. ==> 0XF8006014[20:14] = 0x00000010U
    // .. ..     ==> MASK : 0x001FC000U    VAL : 0x00040000U
    // .. ..
    EMIT_MASKWRITE(0XF8006014, 0x001FFFFFU ,0x0004159BU),
    // .. .. reg_ddrc_wr2pre = 0x13
    // .. .. ==> 0XF8006018[4:0] = 0x00000013U
    // .. ..     ==> MASK : 0x0000001FU    VAL : 0x00000013U
    // .. .. reg_ddrc_powerdown_to_x32 = 0x6
    // .. .. ==> 0XF8006018[9:5] = 0x00000006U
    // .. ..     ==> MASK : 0x000003E0U    VAL : 0x000000C0U
    // .. .. reg_ddrc_t_faw = 0x16
    // .. .. ==> 0XF8006018[15:10] = 0x00000016U
    // .. ..     ==> MASK : 0x0000FC00U    VAL : 0x00005800U
    // .. .. reg_ddrc_t_ras_max = 0x24
    // .. .. ==> 0XF8006018[21:16] = 0x00000024U
    // .. ..     ==> MASK : 0x003F0000U    VAL : 0x00240000U
    // .. .. reg_ddrc_t_ras_min = 0x13
    // .. .. ==> 0XF8006018[26:22] = 0x00000013U
    // .. ..     ==> MASK : 0x07C00000U    VAL : 0x04C00000U
    // .. .. reg_ddrc_t_cke = 0x4
    // .. .. ==> 0XF8006018[31:28] = 0x00000004U
    // .. ..     ==> MASK : 0xF0000000U    VAL : 0x40000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006018, 0xF7FFFFFFU ,0x44E458D3U),
    // .. .. reg_ddrc_write_latency = 0x5
    // .. .. ==> 0XF800601C[4:0] = 0x00000005U
    // .. ..     ==> MASK : 0x0000001FU    VAL : 0x00000005U
    // .. .. reg_ddrc_rd2wr = 0x7
    // .. .. ==> 0XF800601C[9:5] = 0x00000007U
    // .. ..     ==> MASK : 0x000003E0U    VAL : 0x000000E0U
    // .. .. reg_ddrc_wr2rd = 0xf
    // .. .. ==> 0XF800601C[14:10] = 0x0000000FU
    // .. ..     ==> MASK : 0x00007C00U    VAL : 0x00003C00U
    // .. .. reg_ddrc_t_xp = 0x5
    // .. .. ==> 0XF800601C[19:15] = 0x00000005U
    // .. ..     ==> MASK : 0x000F8000U    VAL : 0x00028000U
    // .. .. reg_ddrc_pad_pd = 0x0
    // .. .. ==> 0XF800601C[22:20] = 0x00000000U
    // .. ..     ==> MASK : 0x00700000U    VAL : 0x00000000U
    // .. .. reg_ddrc_rd2pre = 0x5
    // .. .. ==> 0XF800601C[27:23] = 0x00000005U
    // .. ..     ==> MASK : 0x0F800000U    VAL : 0x02800000U
    // .. .. reg_ddrc_t_rcd = 0x7
    // .. .. ==> 0XF800601C[31:28] = 0x00000007U
    // .. ..     ==> MASK : 0xF0000000U    VAL : 0x70000000U
    // .. ..
    EMIT_MASKWRITE(0XF800601C, 0xFFFFFFFFU ,0x7282BCE5U),
    // .. .. reg_ddrc_t_ccd = 0x4
    // .. .. ==> 0XF8006020[4:2] = 0x00000004U
    // .. ..     ==> MASK : 0x0000001CU    VAL : 0x00000010U
    // .. .. reg_ddrc_t_rrd = 0x6
    // .. .. ==> 0XF8006020[7:5] = 0x00000006U
    // .. ..     ==> MASK : 0x000000E0U    VAL : 0x000000C0U
    // .. .. reg_ddrc_refresh_margin = 0x2
    // .. .. ==> 0XF8006020[11:8] = 0x00000002U
    // .. ..     ==> MASK : 0x00000F00U    VAL : 0x00000200U
    // .. .. reg_ddrc_t_rp = 0x7
    // .. .. ==> 0XF8006020[15:12] = 0x00000007U
    // .. ..     ==> MASK : 0x0000F000U    VAL : 0x00007000U
    // .. .. reg_ddrc_refresh_to_x32 = 0x8
    // .. .. ==> 0XF8006020[20:16] = 0x00000008U
    // .. ..     ==> MASK : 0x001F0000U    VAL : 0x00080000U
    // .. .. reg_ddrc_sdram = 0x1
    // .. .. ==> 0XF8006020[21:21] = 0x00000001U
    // .. ..     ==> MASK : 0x00200000U    VAL : 0x00200000U
    // .. .. reg_ddrc_mobile = 0x0
    // .. .. ==> 0XF8006020[22:22] = 0x00000000U
    // .. ..     ==> MASK : 0x00400000U    VAL : 0x00000000U
    // .. .. reg_ddrc_clock_stop_en = 0x0
    // .. .. ==> 0XF8006020[23:23] = 0x00000000U
    // .. ..     ==> MASK : 0x00800000U    VAL : 0x00000000U
    // .. .. reg_ddrc_read_latency = 0x7
    // .. .. ==> 0XF8006020[28:24] = 0x00000007U
    // .. ..     ==> MASK : 0x1F000000U    VAL : 0x07000000U
    // .. .. reg_phy_mode_ddr1_ddr2 = 0x1
    // .. .. ==> 0XF8006020[29:29] = 0x00000001U
    // .. ..     ==> MASK : 0x20000000U    VAL : 0x20000000U
    // .. .. reg_ddrc_dis_pad_pd = 0x0
    // .. .. ==> 0XF8006020[30:30] = 0x00000000U
    // .. ..     ==> MASK : 0x40000000U    VAL : 0x00000000U
    // .. .. reg_ddrc_loopback = 0x0
    // .. .. ==> 0XF8006020[31:31] = 0x00000000U
    // .. ..     ==> MASK : 0x80000000U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006020, 0xFFFFFFFCU ,0x272872D0U),
    // .. .. reg_ddrc_en_2t_timing_mode = 0x0
    // .. .. ==> 0XF8006024[0:0] = 0x00000000U
    // .. ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. .. reg_ddrc_prefer_write = 0x0
    // .. .. ==> 0XF8006024[1:1] = 0x00000000U
    // .. ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. .. reg_ddrc_max_rank_rd = 0xf
    // .. .. ==> 0XF8006024[5:2] = 0x0000000FU
    // .. ..     ==> MASK : 0x0000003CU    VAL : 0x0000003CU
    // .. .. reg_ddrc_mr_wr = 0x0
    // .. .. ==> 0XF8006024[6:6] = 0x00000000U
    // .. ..     ==> MASK : 0x00000040U    VAL : 0x00000000U
    // .. .. reg_ddrc_mr_addr = 0x0
    // .. .. ==> 0XF8006024[8:7] = 0x00000000U
    // .. ..     ==> MASK : 0x00000180U    VAL : 0x00000000U
    // .. .. reg_ddrc_mr_data = 0x0
    // .. .. ==> 0XF8006024[24:9] = 0x00000000U
    // .. ..     ==> MASK : 0x01FFFE00U    VAL : 0x00000000U
    // .. .. ddrc_reg_mr_wr_busy = 0x0
    // .. .. ==> 0XF8006024[25:25] = 0x00000000U
    // .. ..     ==> MASK : 0x02000000U    VAL : 0x00000000U
    // .. .. reg_ddrc_mr_type = 0x0
    // .. .. ==> 0XF8006024[26:26] = 0x00000000U
    // .. ..     ==> MASK : 0x04000000U    VAL : 0x00000000U
    // .. .. reg_ddrc_mr_rdata_valid = 0x0
    // .. .. ==> 0XF8006024[27:27] = 0x00000000U
    // .. ..     ==> MASK : 0x08000000U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006024, 0x0FFFFFFFU ,0x0000003CU),
    // .. .. reg_ddrc_final_wait_x32 = 0x7
    // .. .. ==> 0XF8006028[6:0] = 0x00000007U
    // .. ..     ==> MASK : 0x0000007FU    VAL : 0x00000007U
    // .. .. reg_ddrc_pre_ocd_x32 = 0x0
    // .. .. ==> 0XF8006028[10:7] = 0x00000000U
    // .. ..     ==> MASK : 0x00000780U    VAL : 0x00000000U
    // .. .. reg_ddrc_t_mrd = 0x4
    // .. .. ==> 0XF8006028[13:11] = 0x00000004U
    // .. ..     ==> MASK : 0x00003800U    VAL : 0x00002000U
    // .. ..
    EMIT_MASKWRITE(0XF8006028, 0x00003FFFU ,0x00002007U),
    // .. .. reg_ddrc_emr2 = 0x8
    // .. .. ==> 0XF800602C[15:0] = 0x00000008U
    // .. ..     ==> MASK : 0x0000FFFFU    VAL : 0x00000008U
    // .. .. reg_ddrc_emr3 = 0x0
    // .. .. ==> 0XF800602C[31:16] = 0x00000000U
    // .. ..     ==> MASK : 0xFFFF0000U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF800602C, 0xFFFFFFFFU ,0x00000008U),
    // .. .. reg_ddrc_mr = 0xb30
    // .. .. ==> 0XF8006030[15:0] = 0x00000B30U
    // .. ..     ==> MASK : 0x0000FFFFU    VAL : 0x00000B30U
    // .. .. reg_ddrc_emr = 0x4
    // .. .. ==> 0XF8006030[31:16] = 0x00000004U
    // .. ..     ==> MASK : 0xFFFF0000U    VAL : 0x00040000U
    // .. ..
    EMIT_MASKWRITE(0XF8006030, 0xFFFFFFFFU ,0x00040B30U),
    // .. .. reg_ddrc_burst_rdwr = 0x4
    // .. .. ==> 0XF8006034[3:0] = 0x00000004U
    // .. ..     ==> MASK : 0x0000000FU    VAL : 0x00000004U
    // .. .. reg_ddrc_pre_cke_x1024 = 0x16d
    // .. .. ==> 0XF8006034[13:4] = 0x0000016DU
    // .. ..     ==> MASK : 0x00003FF0U    VAL : 0x000016D0U
    // .. .. reg_ddrc_post_cke_x1024 = 0x1
    // .. .. ==> 0XF8006034[25:16] = 0x00000001U
    // .. ..     ==> MASK : 0x03FF0000U    VAL : 0x00010000U
    // .. .. reg_ddrc_burstchop = 0x0
    // .. .. ==> 0XF8006034[28:28] = 0x00000000U
    // .. ..     ==> MASK : 0x10000000U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006034, 0x13FF3FFFU ,0x000116D4U),
    // .. .. reg_ddrc_force_low_pri_n = 0x0
    // .. .. ==> 0XF8006038[0:0] = 0x00000000U
    // .. ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. .. reg_ddrc_dis_dq = 0x0
    // .. .. ==> 0XF8006038[1:1] = 0x00000000U
    // .. ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. .. reg_phy_debug_mode = 0x0
    // .. .. ==> 0XF8006038[6:6] = 0x00000000U
    // .. ..     ==> MASK : 0x00000040U    VAL : 0x00000000U
    // .. .. reg_phy_wr_level_start = 0x0
    // .. .. ==> 0XF8006038[7:7] = 0x00000000U
    // .. ..     ==> MASK : 0x00000080U    VAL : 0x00000000U
    // .. .. reg_phy_rd_level_start = 0x0
    // .. .. ==> 0XF8006038[8:8] = 0x00000000U
    // .. ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. .. reg_phy_dq0_wait_t = 0x0
    // .. .. ==> 0XF8006038[12:9] = 0x00000000U
    // .. ..     ==> MASK : 0x00001E00U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006038, 0x00001FC3U ,0x00000000U),
    // .. .. reg_ddrc_addrmap_bank_b0 = 0x7
    // .. .. ==> 0XF800603C[3:0] = 0x00000007U
    // .. ..     ==> MASK : 0x0000000FU    VAL : 0x00000007U
    // .. .. reg_ddrc_addrmap_bank_b1 = 0x7
    // .. .. ==> 0XF800603C[7:4] = 0x00000007U
    // .. ..     ==> MASK : 0x000000F0U    VAL : 0x00000070U
    // .. .. reg_ddrc_addrmap_bank_b2 = 0x7
    // .. .. ==> 0XF800603C[11:8] = 0x00000007U
    // .. ..     ==> MASK : 0x00000F00U    VAL : 0x00000700U
    // .. .. reg_ddrc_addrmap_col_b5 = 0x0
    // .. .. ==> 0XF800603C[15:12] = 0x00000000U
    // .. ..     ==> MASK : 0x0000F000U    VAL : 0x00000000U
    // .. .. reg_ddrc_addrmap_col_b6 = 0x0
    // .. .. ==> 0XF800603C[19:16] = 0x00000000U
    // .. ..     ==> MASK : 0x000F0000U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF800603C, 0x000FFFFFU ,0x00000777U),
    // .. .. reg_ddrc_addrmap_col_b2 = 0x0
    // .. .. ==> 0XF8006040[3:0] = 0x00000000U
    // .. ..     ==> MASK : 0x0000000FU    VAL : 0x00000000U
    // .. .. reg_ddrc_addrmap_col_b3 = 0x0
    // .. .. ==> 0XF8006040[7:4] = 0x00000000U
    // .. ..     ==> MASK : 0x000000F0U    VAL : 0x00000000U
    // .. .. reg_ddrc_addrmap_col_b4 = 0x0
    // .. .. ==> 0XF8006040[11:8] = 0x00000000U
    // .. ..     ==> MASK : 0x00000F00U    VAL : 0x00000000U
    // .. .. reg_ddrc_addrmap_col_b7 = 0x0
    // .. .. ==> 0XF8006040[15:12] = 0x00000000U
    // .. ..     ==> MASK : 0x0000F000U    VAL : 0x00000000U
    // .. .. reg_ddrc_addrmap_col_b8 = 0x0
    // .. .. ==> 0XF8006040[19:16] = 0x00000000U
    // .. ..     ==> MASK : 0x000F0000U    VAL : 0x00000000U
    // .. .. reg_ddrc_addrmap_col_b9 = 0xf
    // .. .. ==> 0XF8006040[23:20] = 0x0000000FU
    // .. ..     ==> MASK : 0x00F00000U    VAL : 0x00F00000U
    // .. .. reg_ddrc_addrmap_col_b10 = 0xf
    // .. .. ==> 0XF8006040[27:24] = 0x0000000FU
    // .. ..     ==> MASK : 0x0F000000U    VAL : 0x0F000000U
    // .. .. reg_ddrc_addrmap_col_b11 = 0xf
    // .. .. ==> 0XF8006040[31:28] = 0x0000000FU
    // .. ..     ==> MASK : 0xF0000000U    VAL : 0xF0000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006040, 0xFFFFFFFFU ,0xFFF00000U),
    // .. .. reg_ddrc_addrmap_row_b0 = 0x6
    // .. .. ==> 0XF8006044[3:0] = 0x00000006U
    // .. ..     ==> MASK : 0x0000000FU    VAL : 0x00000006U
    // .. .. reg_ddrc_addrmap_row_b1 = 0x6
    // .. .. ==> 0XF8006044[7:4] = 0x00000006U
    // .. ..     ==> MASK : 0x000000F0U    VAL : 0x00000060U
    // .. .. reg_ddrc_addrmap_row_b2_11 = 0x6
    // .. .. ==> 0XF8006044[11:8] = 0x00000006U
    // .. ..     ==> MASK : 0x00000F00U    VAL : 0x00000600U
    // .. .. reg_ddrc_addrmap_row_b12 = 0x6
    // .. .. ==> 0XF8006044[15:12] = 0x00000006U
    // .. ..     ==> MASK : 0x0000F000U    VAL : 0x00006000U
    // .. .. reg_ddrc_addrmap_row_b13 = 0x6
    // .. .. ==> 0XF8006044[19:16] = 0x00000006U
    // .. ..     ==> MASK : 0x000F0000U    VAL : 0x00060000U
    // .. .. reg_ddrc_addrmap_row_b14 = 0xf
    // .. .. ==> 0XF8006044[23:20] = 0x0000000FU
    // .. ..     ==> MASK : 0x00F00000U    VAL : 0x00F00000U
    // .. .. reg_ddrc_addrmap_row_b15 = 0xf
    // .. .. ==> 0XF8006044[27:24] = 0x0000000FU
    // .. ..     ==> MASK : 0x0F000000U    VAL : 0x0F000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006044, 0x0FFFFFFFU ,0x0FF66666U),
    // .. .. reg_ddrc_rank0_rd_odt = 0x0
    // .. .. ==> 0XF8006048[2:0] = 0x00000000U
    // .. ..     ==> MASK : 0x00000007U    VAL : 0x00000000U
    // .. .. reg_ddrc_rank0_wr_odt = 0x1
    // .. .. ==> 0XF8006048[5:3] = 0x00000001U
    // .. ..     ==> MASK : 0x00000038U    VAL : 0x00000008U
    // .. .. reg_ddrc_rank1_rd_odt = 0x1
    // .. .. ==> 0XF8006048[8:6] = 0x00000001U
    // .. ..     ==> MASK : 0x000001C0U    VAL : 0x00000040U
    // .. .. reg_ddrc_rank1_wr_odt = 0x1
    // .. .. ==> 0XF8006048[11:9] = 0x00000001U
    // .. ..     ==> MASK : 0x00000E00U    VAL : 0x00000200U
    // .. .. reg_phy_rd_local_odt = 0x0
    // .. .. ==> 0XF8006048[13:12] = 0x00000000U
    // .. ..     ==> MASK : 0x00003000U    VAL : 0x00000000U
    // .. .. reg_phy_wr_local_odt = 0x3
    // .. .. ==> 0XF8006048[15:14] = 0x00000003U
    // .. ..     ==> MASK : 0x0000C000U    VAL : 0x0000C000U
    // .. .. reg_phy_idle_local_odt = 0x3
    // .. .. ==> 0XF8006048[17:16] = 0x00000003U
    // .. ..     ==> MASK : 0x00030000U    VAL : 0x00030000U
    // .. .. reg_ddrc_rank2_rd_odt = 0x0
    // .. .. ==> 0XF8006048[20:18] = 0x00000000U
    // .. ..     ==> MASK : 0x001C0000U    VAL : 0x00000000U
    // .. .. reg_ddrc_rank2_wr_odt = 0x0
    // .. .. ==> 0XF8006048[23:21] = 0x00000000U
    // .. ..     ==> MASK : 0x00E00000U    VAL : 0x00000000U
    // .. .. reg_ddrc_rank3_rd_odt = 0x0
    // .. .. ==> 0XF8006048[26:24] = 0x00000000U
    // .. ..     ==> MASK : 0x07000000U    VAL : 0x00000000U
    // .. .. reg_ddrc_rank3_wr_odt = 0x0
    // .. .. ==> 0XF8006048[29:27] = 0x00000000U
    // .. ..     ==> MASK : 0x38000000U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006048, 0x3FFFFFFFU ,0x0003C248U),
    // .. .. reg_phy_rd_cmd_to_data = 0x0
    // .. .. ==> 0XF8006050[3:0] = 0x00000000U
    // .. ..     ==> MASK : 0x0000000FU    VAL : 0x00000000U
    // .. .. reg_phy_wr_cmd_to_data = 0x0
    // .. .. ==> 0XF8006050[7:4] = 0x00000000U
    // .. ..     ==> MASK : 0x000000F0U    VAL : 0x00000000U
    // .. .. reg_phy_rdc_we_to_re_delay = 0x8
    // .. .. ==> 0XF8006050[11:8] = 0x00000008U
    // .. ..     ==> MASK : 0x00000F00U    VAL : 0x00000800U
    // .. .. reg_phy_rdc_fifo_rst_disable = 0x0
    // .. .. ==> 0XF8006050[15:15] = 0x00000000U
    // .. ..     ==> MASK : 0x00008000U    VAL : 0x00000000U
    // .. .. reg_phy_use_fixed_re = 0x1
    // .. .. ==> 0XF8006050[16:16] = 0x00000001U
    // .. ..     ==> MASK : 0x00010000U    VAL : 0x00010000U
    // .. .. reg_phy_rdc_fifo_rst_err_cnt_clr = 0x0
    // .. .. ==> 0XF8006050[17:17] = 0x00000000U
    // .. ..     ==> MASK : 0x00020000U    VAL : 0x00000000U
    // .. .. reg_phy_dis_phy_ctrl_rstn = 0x0
    // .. .. ==> 0XF8006050[18:18] = 0x00000000U
    // .. ..     ==> MASK : 0x00040000U    VAL : 0x00000000U
    // .. .. reg_phy_clk_stall_level = 0x0
    // .. .. ==> 0XF8006050[19:19] = 0x00000000U
    // .. ..     ==> MASK : 0x00080000U    VAL : 0x00000000U
    // .. .. reg_phy_gatelvl_num_of_dq0 = 0x7
    // .. .. ==> 0XF8006050[27:24] = 0x00000007U
    // .. ..     ==> MASK : 0x0F000000U    VAL : 0x07000000U
    // .. .. reg_phy_wrlvl_num_of_dq0 = 0x7
    // .. .. ==> 0XF8006050[31:28] = 0x00000007U
    // .. ..     ==> MASK : 0xF0000000U    VAL : 0x70000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006050, 0xFF0F8FFFU ,0x77010800U),
    // .. .. reg_ddrc_dll_calib_to_min_x1024 = 0x1
    // .. .. ==> 0XF8006058[7:0] = 0x00000001U
    // .. ..     ==> MASK : 0x000000FFU    VAL : 0x00000001U
    // .. .. reg_ddrc_dll_calib_to_max_x1024 = 0x1
    // .. .. ==> 0XF8006058[15:8] = 0x00000001U
    // .. ..     ==> MASK : 0x0000FF00U    VAL : 0x00000100U
    // .. .. reg_ddrc_dis_dll_calib = 0x0
    // .. .. ==> 0XF8006058[16:16] = 0x00000000U
    // .. ..     ==> MASK : 0x00010000U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006058, 0x0001FFFFU ,0x00000101U),
    // .. .. reg_ddrc_rd_odt_delay = 0x3
    // .. .. ==> 0XF800605C[3:0] = 0x00000003U
    // .. ..     ==> MASK : 0x0000000FU    VAL : 0x00000003U
    // .. .. reg_ddrc_wr_odt_delay = 0x0
    // .. .. ==> 0XF800605C[7:4] = 0x00000000U
    // .. ..     ==> MASK : 0x000000F0U    VAL : 0x00000000U
    // .. .. reg_ddrc_rd_odt_hold = 0x0
    // .. .. ==> 0XF800605C[11:8] = 0x00000000U
    // .. ..     ==> MASK : 0x00000F00U    VAL : 0x00000000U
    // .. .. reg_ddrc_wr_odt_hold = 0x5
    // .. .. ==> 0XF800605C[15:12] = 0x00000005U
    // .. ..     ==> MASK : 0x0000F000U    VAL : 0x00005000U
    // .. ..
    EMIT_MASKWRITE(0XF800605C, 0x0000FFFFU ,0x00005003U),
    // .. .. reg_ddrc_pageclose = 0x0
    // .. .. ==> 0XF8006060[0:0] = 0x00000000U
    // .. ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. .. reg_ddrc_lpr_num_entries = 0x1f
    // .. .. ==> 0XF8006060[6:1] = 0x0000001FU
    // .. ..     ==> MASK : 0x0000007EU    VAL : 0x0000003EU
    // .. .. reg_ddrc_auto_pre_en = 0x0
    // .. .. ==> 0XF8006060[7:7] = 0x00000000U
    // .. ..     ==> MASK : 0x00000080U    VAL : 0x00000000U
    // .. .. reg_ddrc_refresh_update_level = 0x0
    // .. .. ==> 0XF8006060[8:8] = 0x00000000U
    // .. ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. .. reg_ddrc_dis_wc = 0x0
    // .. .. ==> 0XF8006060[9:9] = 0x00000000U
    // .. ..     ==> MASK : 0x00000200U    VAL : 0x00000000U
    // .. .. reg_ddrc_dis_collision_page_opt = 0x0
    // .. .. ==> 0XF8006060[10:10] = 0x00000000U
    // .. ..     ==> MASK : 0x00000400U    VAL : 0x00000000U
    // .. .. reg_ddrc_selfref_en = 0x0
    // .. .. ==> 0XF8006060[12:12] = 0x00000000U
    // .. ..     ==> MASK : 0x00001000U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006060, 0x000017FFU ,0x0000003EU),
    // .. .. reg_ddrc_go2critical_hysteresis = 0x0
    // .. .. ==> 0XF8006064[12:5] = 0x00000000U
    // .. ..     ==> MASK : 0x00001FE0U    VAL : 0x00000000U
    // .. .. reg_arb_go2critical_en = 0x1
    // .. .. ==> 0XF8006064[17:17] = 0x00000001U
    // .. ..     ==> MASK : 0x00020000U    VAL : 0x00020000U
    // .. ..
    EMIT_MASKWRITE(0XF8006064, 0x00021FE0U ,0x00020000U),
    // .. .. reg_ddrc_wrlvl_ww = 0x41
    // .. .. ==> 0XF8006068[7:0] = 0x00000041U
    // .. ..     ==> MASK : 0x000000FFU    VAL : 0x00000041U
    // .. .. reg_ddrc_rdlvl_rr = 0x41
    // .. .. ==> 0XF8006068[15:8] = 0x00000041U
    // .. ..     ==> MASK : 0x0000FF00U    VAL : 0x00004100U
    // .. .. reg_ddrc_dfi_t_wlmrd = 0x28
    // .. .. ==> 0XF8006068[25:16] = 0x00000028U
    // .. ..     ==> MASK : 0x03FF0000U    VAL : 0x00280000U
    // .. ..
    EMIT_MASKWRITE(0XF8006068, 0x03FFFFFFU ,0x00284141U),
    // .. .. dfi_t_ctrlupd_interval_min_x1024 = 0x10
    // .. .. ==> 0XF800606C[7:0] = 0x00000010U
    // .. ..     ==> MASK : 0x000000FFU    VAL : 0x00000010U
    // .. .. dfi_t_ctrlupd_interval_max_x1024 = 0x16
    // .. .. ==> 0XF800606C[15:8] = 0x00000016U
    // .. ..     ==> MASK : 0x0000FF00U    VAL : 0x00001600U
    // .. ..
    EMIT_MASKWRITE(0XF800606C, 0x0000FFFFU ,0x00001610U),
    // .. .. reg_ddrc_dfi_t_ctrl_delay = 0x1
    // .. .. ==> 0XF8006078[3:0] = 0x00000001U
    // .. ..     ==> MASK : 0x0000000FU    VAL : 0x00000001U
    // .. .. reg_ddrc_dfi_t_dram_clk_disable = 0x1
    // .. .. ==> 0XF8006078[7:4] = 0x00000001U
    // .. ..     ==> MASK : 0x000000F0U    VAL : 0x00000010U
    // .. .. reg_ddrc_dfi_t_dram_clk_enable = 0x1
    // .. .. ==> 0XF8006078[11:8] = 0x00000001U
    // .. ..     ==> MASK : 0x00000F00U    VAL : 0x00000100U
    // .. .. reg_ddrc_t_cksre = 0x6
    // .. .. ==> 0XF8006078[15:12] = 0x00000006U
    // .. ..     ==> MASK : 0x0000F000U    VAL : 0x00006000U
    // .. .. reg_ddrc_t_cksrx = 0x6
    // .. .. ==> 0XF8006078[19:16] = 0x00000006U
    // .. ..     ==> MASK : 0x000F0000U    VAL : 0x00060000U
    // .. .. reg_ddrc_t_ckesr = 0x4
    // .. .. ==> 0XF8006078[25:20] = 0x00000004U
    // .. ..     ==> MASK : 0x03F00000U    VAL : 0x00400000U
    // .. ..
    EMIT_MASKWRITE(0XF8006078, 0x03FFFFFFU ,0x00466111U),
    // .. .. reg_ddrc_t_ckpde = 0x2
    // .. .. ==> 0XF800607C[3:0] = 0x00000002U
    // .. ..     ==> MASK : 0x0000000FU    VAL : 0x00000002U
    // .. .. reg_ddrc_t_ckpdx = 0x2
    // .. .. ==> 0XF800607C[7:4] = 0x00000002U
    // .. ..     ==> MASK : 0x000000F0U    VAL : 0x00000020U
    // .. .. reg_ddrc_t_ckdpde = 0x2
    // .. .. ==> 0XF800607C[11:8] = 0x00000002U
    // .. ..     ==> MASK : 0x00000F00U    VAL : 0x00000200U
    // .. .. reg_ddrc_t_ckdpdx = 0x2
    // .. .. ==> 0XF800607C[15:12] = 0x00000002U
    // .. ..     ==> MASK : 0x0000F000U    VAL : 0x00002000U
    // .. .. reg_ddrc_t_ckcsx = 0x3
    // .. .. ==> 0XF800607C[19:16] = 0x00000003U
    // .. ..     ==> MASK : 0x000F0000U    VAL : 0x00030000U
    // .. ..
    EMIT_MASKWRITE(0XF800607C, 0x000FFFFFU ,0x00032222U),
    // .. .. refresh_timer0_start_value_x32 = 0x0
    // .. .. ==> 0XF80060A0[11:0] = 0x00000000U
    // .. ..     ==> MASK : 0x00000FFFU    VAL : 0x00000000U
    // .. .. refresh_timer1_start_value_x32 = 0x8
    // .. .. ==> 0XF80060A0[23:12] = 0x00000008U
    // .. ..     ==> MASK : 0x00FFF000U    VAL : 0x00008000U
    // .. ..
    EMIT_MASKWRITE(0XF80060A0, 0x00FFFFFFU ,0x00008000U),
    // .. .. reg_ddrc_dis_auto_zq = 0x0
    // .. .. ==> 0XF80060A4[0:0] = 0x00000000U
    // .. ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. .. reg_ddrc_ddr3 = 0x1
    // .. .. ==> 0XF80060A4[1:1] = 0x00000001U
    // .. ..     ==> MASK : 0x00000002U    VAL : 0x00000002U
    // .. .. reg_ddrc_t_mod = 0x200
    // .. .. ==> 0XF80060A4[11:2] = 0x00000200U
    // .. ..     ==> MASK : 0x00000FFCU    VAL : 0x00000800U
    // .. .. reg_ddrc_t_zq_long_nop = 0x200
    // .. .. ==> 0XF80060A4[21:12] = 0x00000200U
    // .. ..     ==> MASK : 0x003FF000U    VAL : 0x00200000U
    // .. .. reg_ddrc_t_zq_short_nop = 0x40
    // .. .. ==> 0XF80060A4[31:22] = 0x00000040U
    // .. ..     ==> MASK : 0xFFC00000U    VAL : 0x10000000U
    // .. ..
    EMIT_MASKWRITE(0XF80060A4, 0xFFFFFFFFU ,0x10200802U),
    // .. .. t_zq_short_interval_x1024 = 0xcb73
    // .. .. ==> 0XF80060A8[19:0] = 0x0000CB73U
    // .. ..     ==> MASK : 0x000FFFFFU    VAL : 0x0000CB73U
    // .. .. dram_rstn_x1024 = 0x69
    // .. .. ==> 0XF80060A8[27:20] = 0x00000069U
    // .. ..     ==> MASK : 0x0FF00000U    VAL : 0x06900000U
    // .. ..
    EMIT_MASKWRITE(0XF80060A8, 0x0FFFFFFFU ,0x0690CB73U),
    // .. .. deeppowerdown_en = 0x0
    // .. .. ==> 0XF80060AC[0:0] = 0x00000000U
    // .. ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. .. deeppowerdown_to_x1024 = 0xff
    // .. .. ==> 0XF80060AC[8:1] = 0x000000FFU
    // .. ..     ==> MASK : 0x000001FEU    VAL : 0x000001FEU
    // .. ..
    EMIT_MASKWRITE(0XF80060AC, 0x000001FFU ,0x000001FEU),
    // .. .. dfi_wrlvl_max_x1024 = 0xfff
    // .. .. ==> 0XF80060B0[11:0] = 0x00000FFFU
    // .. ..     ==> MASK : 0x00000FFFU    VAL : 0x00000FFFU
    // .. .. dfi_rdlvl_max_x1024 = 0xfff
    // .. .. ==> 0XF80060B0[23:12] = 0x00000FFFU
    // .. ..     ==> MASK : 0x00FFF000U    VAL : 0x00FFF000U
    // .. .. ddrc_reg_twrlvl_max_error = 0x0
    // .. .. ==> 0XF80060B0[24:24] = 0x00000000U
    // .. ..     ==> MASK : 0x01000000U    VAL : 0x00000000U
    // .. .. ddrc_reg_trdlvl_max_error = 0x0
    // .. .. ==> 0XF80060B0[25:25] = 0x00000000U
    // .. ..     ==> MASK : 0x02000000U    VAL : 0x00000000U
    // .. .. reg_ddrc_dfi_wr_level_en = 0x1
    // .. .. ==> 0XF80060B0[26:26] = 0x00000001U
    // .. ..     ==> MASK : 0x04000000U    VAL : 0x04000000U
    // .. .. reg_ddrc_dfi_rd_dqs_gate_level = 0x1
    // .. .. ==> 0XF80060B0[27:27] = 0x00000001U
    // .. ..     ==> MASK : 0x08000000U    VAL : 0x08000000U
    // .. .. reg_ddrc_dfi_rd_data_eye_train = 0x1
    // .. .. ==> 0XF80060B0[28:28] = 0x00000001U
    // .. ..     ==> MASK : 0x10000000U    VAL : 0x10000000U
    // .. ..
    EMIT_MASKWRITE(0XF80060B0, 0x1FFFFFFFU ,0x1CFFFFFFU),
    // .. .. reg_ddrc_2t_delay = 0x0
    // .. .. ==> 0XF80060B4[8:0] = 0x00000000U
    // .. ..     ==> MASK : 0x000001FFU    VAL : 0x00000000U
    // .. .. reg_ddrc_skip_ocd = 0x1
    // .. .. ==> 0XF80060B4[9:9] = 0x00000001U
    // .. ..     ==> MASK : 0x00000200U    VAL : 0x00000200U
    // .. .. reg_ddrc_dis_pre_bypass = 0x0
    // .. .. ==> 0XF80060B4[10:10] = 0x00000000U
    // .. ..     ==> MASK : 0x00000400U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF80060B4, 0x000007FFU ,0x00000200U),
    // .. .. reg_ddrc_dfi_t_rddata_en = 0x6
    // .. .. ==> 0XF80060B8[4:0] = 0x00000006U
    // .. ..     ==> MASK : 0x0000001FU    VAL : 0x00000006U
    // .. .. reg_ddrc_dfi_t_ctrlup_min = 0x3
    // .. .. ==> 0XF80060B8[14:5] = 0x00000003U
    // .. ..     ==> MASK : 0x00007FE0U    VAL : 0x00000060U
    // .. .. reg_ddrc_dfi_t_ctrlup_max = 0x40
    // .. .. ==> 0XF80060B8[24:15] = 0x00000040U
    // .. ..     ==> MASK : 0x01FF8000U    VAL : 0x00200000U
    // .. ..
    EMIT_MASKWRITE(0XF80060B8, 0x01FFFFFFU ,0x00200066U),
    // .. .. Clear_Uncorrectable_DRAM_ECC_error = 0x0
    // .. .. ==> 0XF80060C4[0:0] = 0x00000000U
    // .. ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. .. Clear_Correctable_DRAM_ECC_error = 0x0
    // .. .. ==> 0XF80060C4[1:1] = 0x00000000U
    // .. ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF80060C4, 0x00000003U ,0x00000000U),
    // .. .. CORR_ECC_LOG_VALID = 0x0
    // .. .. ==> 0XF80060C8[0:0] = 0x00000000U
    // .. ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. .. ECC_CORRECTED_BIT_NUM = 0x0
    // .. .. ==> 0XF80060C8[7:1] = 0x00000000U
    // .. ..     ==> MASK : 0x000000FEU    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF80060C8, 0x000000FFU ,0x00000000U),
    // .. .. UNCORR_ECC_LOG_VALID = 0x0
    // .. .. ==> 0XF80060DC[0:0] = 0x00000000U
    // .. ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF80060DC, 0x00000001U ,0x00000000U),
    // .. .. STAT_NUM_CORR_ERR = 0x0
    // .. .. ==> 0XF80060F0[15:8] = 0x00000000U
    // .. ..     ==> MASK : 0x0000FF00U    VAL : 0x00000000U
    // .. .. STAT_NUM_UNCORR_ERR = 0x0
    // .. .. ==> 0XF80060F0[7:0] = 0x00000000U
    // .. ..     ==> MASK : 0x000000FFU    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF80060F0, 0x0000FFFFU ,0x00000000U),
    // .. .. reg_ddrc_ecc_mode = 0x0
    // .. .. ==> 0XF80060F4[2:0] = 0x00000000U
    // .. ..     ==> MASK : 0x00000007U    VAL : 0x00000000U
    // .. .. reg_ddrc_dis_scrub = 0x1
    // .. .. ==> 0XF80060F4[3:3] = 0x00000001U
    // .. ..     ==> MASK : 0x00000008U    VAL : 0x00000008U
    // .. ..
    EMIT_MASKWRITE(0XF80060F4, 0x0000000FU ,0x00000008U),
    // .. .. reg_phy_dif_on = 0x0
    // .. .. ==> 0XF8006114[3:0] = 0x00000000U
    // .. ..     ==> MASK : 0x0000000FU    VAL : 0x00000000U
    // .. .. reg_phy_dif_off = 0x0
    // .. .. ==> 0XF8006114[7:4] = 0x00000000U
    // .. ..     ==> MASK : 0x000000F0U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006114, 0x000000FFU ,0x00000000U),
    // .. .. reg_phy_data_slice_in_use = 0x1
    // .. .. ==> 0XF8006118[0:0] = 0x00000001U
    // .. ..     ==> MASK : 0x00000001U    VAL : 0x00000001U
    // .. .. reg_phy_rdlvl_inc_mode = 0x0
    // .. .. ==> 0XF8006118[1:1] = 0x00000000U
    // .. ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. .. reg_phy_gatelvl_inc_mode = 0x0
    // .. .. ==> 0XF8006118[2:2] = 0x00000000U
    // .. ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. .. reg_phy_wrlvl_inc_mode = 0x0
    // .. .. ==> 0XF8006118[3:3] = 0x00000000U
    // .. ..     ==> MASK : 0x00000008U    VAL : 0x00000000U
    // .. .. reg_phy_board_lpbk_tx = 0x0
    // .. .. ==> 0XF8006118[4:4] = 0x00000000U
    // .. ..     ==> MASK : 0x00000010U    VAL : 0x00000000U
    // .. .. reg_phy_board_lpbk_rx = 0x0
    // .. .. ==> 0XF8006118[5:5] = 0x00000000U
    // .. ..     ==> MASK : 0x00000020U    VAL : 0x00000000U
    // .. .. reg_phy_bist_shift_dq = 0x0
    // .. .. ==> 0XF8006118[14:6] = 0x00000000U
    // .. ..     ==> MASK : 0x00007FC0U    VAL : 0x00000000U
    // .. .. reg_phy_bist_err_clr = 0x0
    // .. .. ==> 0XF8006118[23:15] = 0x00000000U
    // .. ..     ==> MASK : 0x00FF8000U    VAL : 0x00000000U
    // .. .. reg_phy_dq_offset = 0x40
    // .. .. ==> 0XF8006118[30:24] = 0x00000040U
    // .. ..     ==> MASK : 0x7F000000U    VAL : 0x40000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006118, 0x7FFFFFFFU ,0x40000001U),
    // .. .. reg_phy_data_slice_in_use = 0x1
    // .. .. ==> 0XF800611C[0:0] = 0x00000001U
    // .. ..     ==> MASK : 0x00000001U    VAL : 0x00000001U
    // .. .. reg_phy_rdlvl_inc_mode = 0x0
    // .. .. ==> 0XF800611C[1:1] = 0x00000000U
    // .. ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. .. reg_phy_gatelvl_inc_mode = 0x0
    // .. .. ==> 0XF800611C[2:2] = 0x00000000U
    // .. ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. .. reg_phy_wrlvl_inc_mode = 0x0
    // .. .. ==> 0XF800611C[3:3] = 0x00000000U
    // .. ..     ==> MASK : 0x00000008U    VAL : 0x00000000U
    // .. .. reg_phy_board_lpbk_tx = 0x0
    // .. .. ==> 0XF800611C[4:4] = 0x00000000U
    // .. ..     ==> MASK : 0x00000010U    VAL : 0x00000000U
    // .. .. reg_phy_board_lpbk_rx = 0x0
    // .. .. ==> 0XF800611C[5:5] = 0x00000000U
    // .. ..     ==> MASK : 0x00000020U    VAL : 0x00000000U
    // .. .. reg_phy_bist_shift_dq = 0x0
    // .. .. ==> 0XF800611C[14:6] = 0x00000000U
    // .. ..     ==> MASK : 0x00007FC0U    VAL : 0x00000000U
    // .. .. reg_phy_bist_err_clr = 0x0
    // .. .. ==> 0XF800611C[23:15] = 0x00000000U
    // .. ..     ==> MASK : 0x00FF8000U    VAL : 0x00000000U
    // .. .. reg_phy_dq_offset = 0x40
    // .. .. ==> 0XF800611C[30:24] = 0x00000040U
    // .. ..     ==> MASK : 0x7F000000U    VAL : 0x40000000U
    // .. ..
    EMIT_MASKWRITE(0XF800611C, 0x7FFFFFFFU ,0x40000001U),
    // .. .. reg_phy_data_slice_in_use = 0x1
    // .. .. ==> 0XF8006120[0:0] = 0x00000001U
    // .. ..     ==> MASK : 0x00000001U    VAL : 0x00000001U
    // .. .. reg_phy_rdlvl_inc_mode = 0x0
    // .. .. ==> 0XF8006120[1:1] = 0x00000000U
    // .. ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. .. reg_phy_gatelvl_inc_mode = 0x0
    // .. .. ==> 0XF8006120[2:2] = 0x00000000U
    // .. ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. .. reg_phy_wrlvl_inc_mode = 0x0
    // .. .. ==> 0XF8006120[3:3] = 0x00000000U
    // .. ..     ==> MASK : 0x00000008U    VAL : 0x00000000U
    // .. .. reg_phy_board_lpbk_tx = 0x0
    // .. .. ==> 0XF8006120[4:4] = 0x00000000U
    // .. ..     ==> MASK : 0x00000010U    VAL : 0x00000000U
    // .. .. reg_phy_board_lpbk_rx = 0x0
    // .. .. ==> 0XF8006120[5:5] = 0x00000000U
    // .. ..     ==> MASK : 0x00000020U    VAL : 0x00000000U
    // .. .. reg_phy_bist_shift_dq = 0x0
    // .. .. ==> 0XF8006120[14:6] = 0x00000000U
    // .. ..     ==> MASK : 0x00007FC0U    VAL : 0x00000000U
    // .. .. reg_phy_bist_err_clr = 0x0
    // .. .. ==> 0XF8006120[23:15] = 0x00000000U
    // .. ..     ==> MASK : 0x00FF8000U    VAL : 0x00000000U
    // .. .. reg_phy_dq_offset = 0x40
    // .. .. ==> 0XF8006120[30:24] = 0x00000040U
    // .. ..     ==> MASK : 0x7F000000U    VAL : 0x40000000U
    // .. .. reg_phy_data_slice_in_use = 0x1
    // .. .. ==> 0XF8006120[0:0] = 0x00000001U
    // .. ..     ==> MASK : 0x00000001U    VAL : 0x00000001U
    // .. .. reg_phy_rdlvl_inc_mode = 0x0
    // .. .. ==> 0XF8006120[1:1] = 0x00000000U
    // .. ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. .. reg_phy_gatelvl_inc_mode = 0x0
    // .. .. ==> 0XF8006120[2:2] = 0x00000000U
    // .. ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. .. reg_phy_wrlvl_inc_mode = 0x0
    // .. .. ==> 0XF8006120[3:3] = 0x00000000U
    // .. ..     ==> MASK : 0x00000008U    VAL : 0x00000000U
    // .. .. reg_phy_board_lpbk_tx = 0x0
    // .. .. ==> 0XF8006120[4:4] = 0x00000000U
    // .. ..     ==> MASK : 0x00000010U    VAL : 0x00000000U
    // .. .. reg_phy_board_lpbk_rx = 0x0
    // .. .. ==> 0XF8006120[5:5] = 0x00000000U
    // .. ..     ==> MASK : 0x00000020U    VAL : 0x00000000U
    // .. .. reg_phy_bist_shift_dq = 0x0
    // .. .. ==> 0XF8006120[14:6] = 0x00000000U
    // .. ..     ==> MASK : 0x00007FC0U    VAL : 0x00000000U
    // .. .. reg_phy_bist_err_clr = 0x0
    // .. .. ==> 0XF8006120[23:15] = 0x00000000U
    // .. ..     ==> MASK : 0x00FF8000U    VAL : 0x00000000U
    // .. .. reg_phy_dq_offset = 0x40
    // .. .. ==> 0XF8006120[30:24] = 0x00000040U
    // .. ..     ==> MASK : 0x7F000000U    VAL : 0x40000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006120, 0x7FFFFFFFU ,0x40000001U),
    // .. .. reg_phy_data_slice_in_use = 0x1
    // .. .. ==> 0XF8006124[0:0] = 0x00000001U
    // .. ..     ==> MASK : 0x00000001U    VAL : 0x00000001U
    // .. .. reg_phy_rdlvl_inc_mode = 0x0
    // .. .. ==> 0XF8006124[1:1] = 0x00000000U
    // .. ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. .. reg_phy_gatelvl_inc_mode = 0x0
    // .. .. ==> 0XF8006124[2:2] = 0x00000000U
    // .. ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. .. reg_phy_wrlvl_inc_mode = 0x0
    // .. .. ==> 0XF8006124[3:3] = 0x00000000U
    // .. ..     ==> MASK : 0x00000008U    VAL : 0x00000000U
    // .. .. reg_phy_board_lpbk_tx = 0x0
    // .. .. ==> 0XF8006124[4:4] = 0x00000000U
    // .. ..     ==> MASK : 0x00000010U    VAL : 0x00000000U
    // .. .. reg_phy_board_lpbk_rx = 0x0
    // .. .. ==> 0XF8006124[5:5] = 0x00000000U
    // .. ..     ==> MASK : 0x00000020U    VAL : 0x00000000U
    // .. .. reg_phy_bist_shift_dq = 0x0
    // .. .. ==> 0XF8006124[14:6] = 0x00000000U
    // .. ..     ==> MASK : 0x00007FC0U    VAL : 0x00000000U
    // .. .. reg_phy_bist_err_clr = 0x0
    // .. .. ==> 0XF8006124[23:15] = 0x00000000U
    // .. ..     ==> MASK : 0x00FF8000U    VAL : 0x00000000U
    // .. .. reg_phy_dq_offset = 0x40
    // .. .. ==> 0XF8006124[30:24] = 0x00000040U
    // .. ..     ==> MASK : 0x7F000000U    VAL : 0x40000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006124, 0x7FFFFFFFU ,0x40000001U),
    // .. .. reg_phy_wrlvl_init_ratio = 0x0
    // .. .. ==> 0XF800612C[9:0] = 0x00000000U
    // .. ..     ==> MASK : 0x000003FFU    VAL : 0x00000000U
    // .. .. reg_phy_gatelvl_init_ratio = 0xa4
    // .. .. ==> 0XF800612C[19:10] = 0x000000A4U
    // .. ..     ==> MASK : 0x000FFC00U    VAL : 0x00029000U
    // .. ..
    EMIT_MASKWRITE(0XF800612C, 0x000FFFFFU ,0x00029000U),
    // .. .. reg_phy_wrlvl_init_ratio = 0x0
    // .. .. ==> 0XF8006130[9:0] = 0x00000000U
    // .. ..     ==> MASK : 0x000003FFU    VAL : 0x00000000U
    // .. .. reg_phy_gatelvl_init_ratio = 0xa4
    // .. .. ==> 0XF8006130[19:10] = 0x000000A4U
    // .. ..     ==> MASK : 0x000FFC00U    VAL : 0x00029000U
    // .. ..
    EMIT_MASKWRITE(0XF8006130, 0x000FFFFFU ,0x00029000U),
    // .. .. reg_phy_wrlvl_init_ratio = 0x0
    // .. .. ==> 0XF8006134[9:0] = 0x00000000U
    // .. ..     ==> MASK : 0x000003FFU    VAL : 0x00000000U
    // .. .. reg_phy_gatelvl_init_ratio = 0xa4
    // .. .. ==> 0XF8006134[19:10] = 0x000000A4U
    // .. ..     ==> MASK : 0x000FFC00U    VAL : 0x00029000U
    // .. ..
    EMIT_MASKWRITE(0XF8006134, 0x000FFFFFU ,0x00029000U),
    // .. .. reg_phy_wrlvl_init_ratio = 0x0
    // .. .. ==> 0XF8006138[9:0] = 0x00000000U
    // .. ..     ==> MASK : 0x000003FFU    VAL : 0x00000000U
    // .. .. reg_phy_gatelvl_init_ratio = 0xa4
    // .. .. ==> 0XF8006138[19:10] = 0x000000A4U
    // .. ..     ==> MASK : 0x000FFC00U    VAL : 0x00029000U
    // .. ..
    EMIT_MASKWRITE(0XF8006138, 0x000FFFFFU ,0x00029000U),
    // .. .. reg_phy_rd_dqs_slave_ratio = 0x35
    // .. .. ==> 0XF8006140[9:0] = 0x00000035U
    // .. ..     ==> MASK : 0x000003FFU    VAL : 0x00000035U
    // .. .. reg_phy_rd_dqs_slave_force = 0x0
    // .. .. ==> 0XF8006140[10:10] = 0x00000000U
    // .. ..     ==> MASK : 0x00000400U    VAL : 0x00000000U
    // .. .. reg_phy_rd_dqs_slave_delay = 0x0
    // .. .. ==> 0XF8006140[19:11] = 0x00000000U
    // .. ..     ==> MASK : 0x000FF800U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006140, 0x000FFFFFU ,0x00000035U),
    // .. .. reg_phy_rd_dqs_slave_ratio = 0x35
    // .. .. ==> 0XF8006144[9:0] = 0x00000035U
    // .. ..     ==> MASK : 0x000003FFU    VAL : 0x00000035U
    // .. .. reg_phy_rd_dqs_slave_force = 0x0
    // .. .. ==> 0XF8006144[10:10] = 0x00000000U
    // .. ..     ==> MASK : 0x00000400U    VAL : 0x00000000U
    // .. .. reg_phy_rd_dqs_slave_delay = 0x0
    // .. .. ==> 0XF8006144[19:11] = 0x00000000U
    // .. ..     ==> MASK : 0x000FF800U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006144, 0x000FFFFFU ,0x00000035U),
    // .. .. reg_phy_rd_dqs_slave_ratio = 0x35
    // .. .. ==> 0XF8006148[9:0] = 0x00000035U
    // .. ..     ==> MASK : 0x000003FFU    VAL : 0x00000035U
    // .. .. reg_phy_rd_dqs_slave_force = 0x0
    // .. .. ==> 0XF8006148[10:10] = 0x00000000U
    // .. ..     ==> MASK : 0x00000400U    VAL : 0x00000000U
    // .. .. reg_phy_rd_dqs_slave_delay = 0x0
    // .. .. ==> 0XF8006148[19:11] = 0x00000000U
    // .. ..     ==> MASK : 0x000FF800U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006148, 0x000FFFFFU ,0x00000035U),
    // .. .. reg_phy_rd_dqs_slave_ratio = 0x35
    // .. .. ==> 0XF800614C[9:0] = 0x00000035U
    // .. ..     ==> MASK : 0x000003FFU    VAL : 0x00000035U
    // .. .. reg_phy_rd_dqs_slave_force = 0x0
    // .. .. ==> 0XF800614C[10:10] = 0x00000000U
    // .. ..     ==> MASK : 0x00000400U    VAL : 0x00000000U
    // .. .. reg_phy_rd_dqs_slave_delay = 0x0
    // .. .. ==> 0XF800614C[19:11] = 0x00000000U
    // .. ..     ==> MASK : 0x000FF800U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF800614C, 0x000FFFFFU ,0x00000035U),
    // .. .. reg_phy_wr_dqs_slave_ratio = 0x80
    // .. .. ==> 0XF8006154[9:0] = 0x00000080U
    // .. ..     ==> MASK : 0x000003FFU    VAL : 0x00000080U
    // .. .. reg_phy_wr_dqs_slave_force = 0x0
    // .. .. ==> 0XF8006154[10:10] = 0x00000000U
    // .. ..     ==> MASK : 0x00000400U    VAL : 0x00000000U
    // .. .. reg_phy_wr_dqs_slave_delay = 0x0
    // .. .. ==> 0XF8006154[19:11] = 0x00000000U
    // .. ..     ==> MASK : 0x000FF800U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006154, 0x000FFFFFU ,0x00000080U),
    // .. .. reg_phy_wr_dqs_slave_ratio = 0x80
    // .. .. ==> 0XF8006158[9:0] = 0x00000080U
    // .. ..     ==> MASK : 0x000003FFU    VAL : 0x00000080U
    // .. .. reg_phy_wr_dqs_slave_force = 0x0
    // .. .. ==> 0XF8006158[10:10] = 0x00000000U
    // .. ..     ==> MASK : 0x00000400U    VAL : 0x00000000U
    // .. .. reg_phy_wr_dqs_slave_delay = 0x0
    // .. .. ==> 0XF8006158[19:11] = 0x00000000U
    // .. ..     ==> MASK : 0x000FF800U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006158, 0x000FFFFFU ,0x00000080U),
    // .. .. reg_phy_wr_dqs_slave_ratio = 0x80
    // .. .. ==> 0XF800615C[9:0] = 0x00000080U
    // .. ..     ==> MASK : 0x000003FFU    VAL : 0x00000080U
    // .. .. reg_phy_wr_dqs_slave_force = 0x0
    // .. .. ==> 0XF800615C[10:10] = 0x00000000U
    // .. ..     ==> MASK : 0x00000400U    VAL : 0x00000000U
    // .. .. reg_phy_wr_dqs_slave_delay = 0x0
    // .. .. ==> 0XF800615C[19:11] = 0x00000000U
    // .. ..     ==> MASK : 0x000FF800U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF800615C, 0x000FFFFFU ,0x00000080U),
    // .. .. reg_phy_wr_dqs_slave_ratio = 0x80
    // .. .. ==> 0XF8006160[9:0] = 0x00000080U
    // .. ..     ==> MASK : 0x000003FFU    VAL : 0x00000080U
    // .. .. reg_phy_wr_dqs_slave_force = 0x0
    // .. .. ==> 0XF8006160[10:10] = 0x00000000U
    // .. ..     ==> MASK : 0x00000400U    VAL : 0x00000000U
    // .. .. reg_phy_wr_dqs_slave_delay = 0x0
    // .. .. ==> 0XF8006160[19:11] = 0x00000000U
    // .. ..     ==> MASK : 0x000FF800U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006160, 0x000FFFFFU ,0x00000080U),
    // .. .. reg_phy_fifo_we_slave_ratio = 0xf9
    // .. .. ==> 0XF8006168[10:0] = 0x000000F9U
    // .. ..     ==> MASK : 0x000007FFU    VAL : 0x000000F9U
    // .. .. reg_phy_fifo_we_in_force = 0x0
    // .. .. ==> 0XF8006168[11:11] = 0x00000000U
    // .. ..     ==> MASK : 0x00000800U    VAL : 0x00000000U
    // .. .. reg_phy_fifo_we_in_delay = 0x0
    // .. .. ==> 0XF8006168[20:12] = 0x00000000U
    // .. ..     ==> MASK : 0x001FF000U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006168, 0x001FFFFFU ,0x000000F9U),
    // .. .. reg_phy_fifo_we_slave_ratio = 0xf9
    // .. .. ==> 0XF800616C[10:0] = 0x000000F9U
    // .. ..     ==> MASK : 0x000007FFU    VAL : 0x000000F9U
    // .. .. reg_phy_fifo_we_in_force = 0x0
    // .. .. ==> 0XF800616C[11:11] = 0x00000000U
    // .. ..     ==> MASK : 0x00000800U    VAL : 0x00000000U
    // .. .. reg_phy_fifo_we_in_delay = 0x0
    // .. .. ==> 0XF800616C[20:12] = 0x00000000U
    // .. ..     ==> MASK : 0x001FF000U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF800616C, 0x001FFFFFU ,0x000000F9U),
    // .. .. reg_phy_fifo_we_slave_ratio = 0xf9
    // .. .. ==> 0XF8006170[10:0] = 0x000000F9U
    // .. ..     ==> MASK : 0x000007FFU    VAL : 0x000000F9U
    // .. .. reg_phy_fifo_we_in_force = 0x0
    // .. .. ==> 0XF8006170[11:11] = 0x00000000U
    // .. ..     ==> MASK : 0x00000800U    VAL : 0x00000000U
    // .. .. reg_phy_fifo_we_in_delay = 0x0
    // .. .. ==> 0XF8006170[20:12] = 0x00000000U
    // .. ..     ==> MASK : 0x001FF000U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006170, 0x001FFFFFU ,0x000000F9U),
    // .. .. reg_phy_fifo_we_slave_ratio = 0xf9
    // .. .. ==> 0XF8006174[10:0] = 0x000000F9U
    // .. ..     ==> MASK : 0x000007FFU    VAL : 0x000000F9U
    // .. .. reg_phy_fifo_we_in_force = 0x0
    // .. .. ==> 0XF8006174[11:11] = 0x00000000U
    // .. ..     ==> MASK : 0x00000800U    VAL : 0x00000000U
    // .. .. reg_phy_fifo_we_in_delay = 0x0
    // .. .. ==> 0XF8006174[20:12] = 0x00000000U
    // .. ..     ==> MASK : 0x001FF000U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006174, 0x001FFFFFU ,0x000000F9U),
    // .. .. reg_phy_wr_data_slave_ratio = 0xc0
    // .. .. ==> 0XF800617C[9:0] = 0x000000C0U
    // .. ..     ==> MASK : 0x000003FFU    VAL : 0x000000C0U
    // .. .. reg_phy_wr_data_slave_force = 0x0
    // .. .. ==> 0XF800617C[10:10] = 0x00000000U
    // .. ..     ==> MASK : 0x00000400U    VAL : 0x00000000U
    // .. .. reg_phy_wr_data_slave_delay = 0x0
    // .. .. ==> 0XF800617C[19:11] = 0x00000000U
    // .. ..     ==> MASK : 0x000FF800U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF800617C, 0x000FFFFFU ,0x000000C0U),
    // .. .. reg_phy_wr_data_slave_ratio = 0xc0
    // .. .. ==> 0XF8006180[9:0] = 0x000000C0U
    // .. ..     ==> MASK : 0x000003FFU    VAL : 0x000000C0U
    // .. .. reg_phy_wr_data_slave_force = 0x0
    // .. .. ==> 0XF8006180[10:10] = 0x00000000U
    // .. ..     ==> MASK : 0x00000400U    VAL : 0x00000000U
    // .. .. reg_phy_wr_data_slave_delay = 0x0
    // .. .. ==> 0XF8006180[19:11] = 0x00000000U
    // .. ..     ==> MASK : 0x000FF800U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006180, 0x000FFFFFU ,0x000000C0U),
    // .. .. reg_phy_wr_data_slave_ratio = 0xc0
    // .. .. ==> 0XF8006184[9:0] = 0x000000C0U
    // .. ..     ==> MASK : 0x000003FFU    VAL : 0x000000C0U
    // .. .. reg_phy_wr_data_slave_force = 0x0
    // .. .. ==> 0XF8006184[10:10] = 0x00000000U
    // .. ..     ==> MASK : 0x00000400U    VAL : 0x00000000U
    // .. .. reg_phy_wr_data_slave_delay = 0x0
    // .. .. ==> 0XF8006184[19:11] = 0x00000000U
    // .. ..     ==> MASK : 0x000FF800U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006184, 0x000FFFFFU ,0x000000C0U),
    // .. .. reg_phy_wr_data_slave_ratio = 0xc0
    // .. .. ==> 0XF8006188[9:0] = 0x000000C0U
    // .. ..     ==> MASK : 0x000003FFU    VAL : 0x000000C0U
    // .. .. reg_phy_wr_data_slave_force = 0x0
    // .. .. ==> 0XF8006188[10:10] = 0x00000000U
    // .. ..     ==> MASK : 0x00000400U    VAL : 0x00000000U
    // .. .. reg_phy_wr_data_slave_delay = 0x0
    // .. .. ==> 0XF8006188[19:11] = 0x00000000U
    // .. ..     ==> MASK : 0x000FF800U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006188, 0x000FFFFFU ,0x000000C0U),
    // .. .. reg_phy_loopback = 0x0
    // .. .. ==> 0XF8006190[0:0] = 0x00000000U
    // .. ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. .. reg_phy_bl2 = 0x0
    // .. .. ==> 0XF8006190[1:1] = 0x00000000U
    // .. ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. .. reg_phy_at_spd_atpg = 0x0
    // .. .. ==> 0XF8006190[2:2] = 0x00000000U
    // .. ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. .. reg_phy_bist_enable = 0x0
    // .. .. ==> 0XF8006190[3:3] = 0x00000000U
    // .. ..     ==> MASK : 0x00000008U    VAL : 0x00000000U
    // .. .. reg_phy_bist_force_err = 0x0
    // .. .. ==> 0XF8006190[4:4] = 0x00000000U
    // .. ..     ==> MASK : 0x00000010U    VAL : 0x00000000U
    // .. .. reg_phy_bist_mode = 0x0
    // .. .. ==> 0XF8006190[6:5] = 0x00000000U
    // .. ..     ==> MASK : 0x00000060U    VAL : 0x00000000U
    // .. .. reg_phy_invert_clkout = 0x1
    // .. .. ==> 0XF8006190[7:7] = 0x00000001U
    // .. ..     ==> MASK : 0x00000080U    VAL : 0x00000080U
    // .. .. reg_phy_all_dq_mpr_rd_resp = 0x0
    // .. .. ==> 0XF8006190[8:8] = 0x00000000U
    // .. ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. .. reg_phy_sel_logic = 0x0
    // .. .. ==> 0XF8006190[9:9] = 0x00000000U
    // .. ..     ==> MASK : 0x00000200U    VAL : 0x00000000U
    // .. .. reg_phy_ctrl_slave_ratio = 0x100
    // .. .. ==> 0XF8006190[19:10] = 0x00000100U
    // .. ..     ==> MASK : 0x000FFC00U    VAL : 0x00040000U
    // .. .. reg_phy_ctrl_slave_force = 0x0
    // .. .. ==> 0XF8006190[20:20] = 0x00000000U
    // .. ..     ==> MASK : 0x00100000U    VAL : 0x00000000U
    // .. .. reg_phy_ctrl_slave_delay = 0x0
    // .. .. ==> 0XF8006190[27:21] = 0x00000000U
    // .. ..     ==> MASK : 0x0FE00000U    VAL : 0x00000000U
    // .. .. reg_phy_use_rank0_delays = 0x1
    // .. .. ==> 0XF8006190[28:28] = 0x00000001U
    // .. ..     ==> MASK : 0x10000000U    VAL : 0x10000000U
    // .. .. reg_phy_lpddr = 0x0
    // .. .. ==> 0XF8006190[29:29] = 0x00000000U
    // .. ..     ==> MASK : 0x20000000U    VAL : 0x00000000U
    // .. .. reg_phy_cmd_latency = 0x0
    // .. .. ==> 0XF8006190[30:30] = 0x00000000U
    // .. ..     ==> MASK : 0x40000000U    VAL : 0x00000000U
    // .. .. reg_phy_int_lpbk = 0x0
    // .. .. ==> 0XF8006190[31:31] = 0x00000000U
    // .. ..     ==> MASK : 0x80000000U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006190, 0xFFFFFFFFU ,0x10040080U),
    // .. .. reg_phy_wr_rl_delay = 0x2
    // .. .. ==> 0XF8006194[4:0] = 0x00000002U
    // .. ..     ==> MASK : 0x0000001FU    VAL : 0x00000002U
    // .. .. reg_phy_rd_rl_delay = 0x4
    // .. .. ==> 0XF8006194[9:5] = 0x00000004U
    // .. ..     ==> MASK : 0x000003E0U    VAL : 0x00000080U
    // .. .. reg_phy_dll_lock_diff = 0xf
    // .. .. ==> 0XF8006194[13:10] = 0x0000000FU
    // .. ..     ==> MASK : 0x00003C00U    VAL : 0x00003C00U
    // .. .. reg_phy_use_wr_level = 0x1
    // .. .. ==> 0XF8006194[14:14] = 0x00000001U
    // .. ..     ==> MASK : 0x00004000U    VAL : 0x00004000U
    // .. .. reg_phy_use_rd_dqs_gate_level = 0x1
    // .. .. ==> 0XF8006194[15:15] = 0x00000001U
    // .. ..     ==> MASK : 0x00008000U    VAL : 0x00008000U
    // .. .. reg_phy_use_rd_data_eye_level = 0x1
    // .. .. ==> 0XF8006194[16:16] = 0x00000001U
    // .. ..     ==> MASK : 0x00010000U    VAL : 0x00010000U
    // .. .. reg_phy_dis_calib_rst = 0x0
    // .. .. ==> 0XF8006194[17:17] = 0x00000000U
    // .. ..     ==> MASK : 0x00020000U    VAL : 0x00000000U
    // .. .. reg_phy_ctrl_slave_delay = 0x0
    // .. .. ==> 0XF8006194[19:18] = 0x00000000U
    // .. ..     ==> MASK : 0x000C0000U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006194, 0x000FFFFFU ,0x0001FC82U),
    // .. .. reg_arb_page_addr_mask = 0x0
    // .. .. ==> 0XF8006204[31:0] = 0x00000000U
    // .. ..     ==> MASK : 0xFFFFFFFFU    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006204, 0xFFFFFFFFU ,0x00000000U),
    // .. .. reg_arb_pri_wr_portn = 0x3ff
    // .. .. ==> 0XF8006208[9:0] = 0x000003FFU
    // .. ..     ==> MASK : 0x000003FFU    VAL : 0x000003FFU
    // .. .. reg_arb_disable_aging_wr_portn = 0x0
    // .. .. ==> 0XF8006208[16:16] = 0x00000000U
    // .. ..     ==> MASK : 0x00010000U    VAL : 0x00000000U
    // .. .. reg_arb_disable_urgent_wr_portn = 0x0
    // .. .. ==> 0XF8006208[17:17] = 0x00000000U
    // .. ..     ==> MASK : 0x00020000U    VAL : 0x00000000U
    // .. .. reg_arb_dis_page_match_wr_portn = 0x0
    // .. .. ==> 0XF8006208[18:18] = 0x00000000U
    // .. ..     ==> MASK : 0x00040000U    VAL : 0x00000000U
    // .. .. reg_arb_dis_rmw_portn = 0x1
    // .. .. ==> 0XF8006208[19:19] = 0x00000001U
    // .. ..     ==> MASK : 0x00080000U    VAL : 0x00080000U
    // .. ..
    EMIT_MASKWRITE(0XF8006208, 0x000F03FFU ,0x000803FFU),
    // .. .. reg_arb_pri_wr_portn = 0x3ff
    // .. .. ==> 0XF800620C[9:0] = 0x000003FFU
    // .. ..     ==> MASK : 0x000003FFU    VAL : 0x000003FFU
    // .. .. reg_arb_disable_aging_wr_portn = 0x0
    // .. .. ==> 0XF800620C[16:16] = 0x00000000U
    // .. ..     ==> MASK : 0x00010000U    VAL : 0x00000000U
    // .. .. reg_arb_disable_urgent_wr_portn = 0x0
    // .. .. ==> 0XF800620C[17:17] = 0x00000000U
    // .. ..     ==> MASK : 0x00020000U    VAL : 0x00000000U
    // .. .. reg_arb_dis_page_match_wr_portn = 0x0
    // .. .. ==> 0XF800620C[18:18] = 0x00000000U
    // .. ..     ==> MASK : 0x00040000U    VAL : 0x00000000U
    // .. .. reg_arb_dis_rmw_portn = 0x1
    // .. .. ==> 0XF800620C[19:19] = 0x00000001U
    // .. ..     ==> MASK : 0x00080000U    VAL : 0x00080000U
    // .. ..
    EMIT_MASKWRITE(0XF800620C, 0x000F03FFU ,0x000803FFU),
    // .. .. reg_arb_pri_wr_portn = 0x3ff
    // .. .. ==> 0XF8006210[9:0] = 0x000003FFU
    // .. ..     ==> MASK : 0x000003FFU    VAL : 0x000003FFU
    // .. .. reg_arb_disable_aging_wr_portn = 0x0
    // .. .. ==> 0XF8006210[16:16] = 0x00000000U
    // .. ..     ==> MASK : 0x00010000U    VAL : 0x00000000U
    // .. .. reg_arb_disable_urgent_wr_portn = 0x0
    // .. .. ==> 0XF8006210[17:17] = 0x00000000U
    // .. ..     ==> MASK : 0x00020000U    VAL : 0x00000000U
    // .. .. reg_arb_dis_page_match_wr_portn = 0x0
    // .. .. ==> 0XF8006210[18:18] = 0x00000000U
    // .. ..     ==> MASK : 0x00040000U    VAL : 0x00000000U
    // .. .. reg_arb_dis_rmw_portn = 0x1
    // .. .. ==> 0XF8006210[19:19] = 0x00000001U
    // .. ..     ==> MASK : 0x00080000U    VAL : 0x00080000U
    // .. ..
    EMIT_MASKWRITE(0XF8006210, 0x000F03FFU ,0x000803FFU),
    // .. .. reg_arb_pri_wr_portn = 0x3ff
    // .. .. ==> 0XF8006214[9:0] = 0x000003FFU
    // .. ..     ==> MASK : 0x000003FFU    VAL : 0x000003FFU
    // .. .. reg_arb_disable_aging_wr_portn = 0x0
    // .. .. ==> 0XF8006214[16:16] = 0x00000000U
    // .. ..     ==> MASK : 0x00010000U    VAL : 0x00000000U
    // .. .. reg_arb_disable_urgent_wr_portn = 0x0
    // .. .. ==> 0XF8006214[17:17] = 0x00000000U
    // .. ..     ==> MASK : 0x00020000U    VAL : 0x00000000U
    // .. .. reg_arb_dis_page_match_wr_portn = 0x0
    // .. .. ==> 0XF8006214[18:18] = 0x00000000U
    // .. ..     ==> MASK : 0x00040000U    VAL : 0x00000000U
    // .. .. reg_arb_dis_rmw_portn = 0x1
    // .. .. ==> 0XF8006214[19:19] = 0x00000001U
    // .. ..     ==> MASK : 0x00080000U    VAL : 0x00080000U
    // .. ..
    EMIT_MASKWRITE(0XF8006214, 0x000F03FFU ,0x000803FFU),
    // .. .. reg_arb_pri_rd_portn = 0x3ff
    // .. .. ==> 0XF8006218[9:0] = 0x000003FFU
    // .. ..     ==> MASK : 0x000003FFU    VAL : 0x000003FFU
    // .. .. reg_arb_disable_aging_rd_portn = 0x0
    // .. .. ==> 0XF8006218[16:16] = 0x00000000U
    // .. ..     ==> MASK : 0x00010000U    VAL : 0x00000000U
    // .. .. reg_arb_disable_urgent_rd_portn = 0x0
    // .. .. ==> 0XF8006218[17:17] = 0x00000000U
    // .. ..     ==> MASK : 0x00020000U    VAL : 0x00000000U
    // .. .. reg_arb_dis_page_match_rd_portn = 0x0
    // .. .. ==> 0XF8006218[18:18] = 0x00000000U
    // .. ..     ==> MASK : 0x00040000U    VAL : 0x00000000U
    // .. .. reg_arb_set_hpr_rd_portn = 0x0
    // .. .. ==> 0XF8006218[19:19] = 0x00000000U
    // .. ..     ==> MASK : 0x00080000U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006218, 0x000F03FFU ,0x000003FFU),
    // .. .. reg_arb_pri_rd_portn = 0x3ff
    // .. .. ==> 0XF800621C[9:0] = 0x000003FFU
    // .. ..     ==> MASK : 0x000003FFU    VAL : 0x000003FFU
    // .. .. reg_arb_disable_aging_rd_portn = 0x0
    // .. .. ==> 0XF800621C[16:16] = 0x00000000U
    // .. ..     ==> MASK : 0x00010000U    VAL : 0x00000000U
    // .. .. reg_arb_disable_urgent_rd_portn = 0x0
    // .. .. ==> 0XF800621C[17:17] = 0x00000000U
    // .. ..     ==> MASK : 0x00020000U    VAL : 0x00000000U
    // .. .. reg_arb_dis_page_match_rd_portn = 0x0
    // .. .. ==> 0XF800621C[18:18] = 0x00000000U
    // .. ..     ==> MASK : 0x00040000U    VAL : 0x00000000U
    // .. .. reg_arb_set_hpr_rd_portn = 0x0
    // .. .. ==> 0XF800621C[19:19] = 0x00000000U
    // .. ..     ==> MASK : 0x00080000U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF800621C, 0x000F03FFU ,0x000003FFU),
    // .. .. reg_arb_pri_rd_portn = 0x3ff
    // .. .. ==> 0XF8006220[9:0] = 0x000003FFU
    // .. ..     ==> MASK : 0x000003FFU    VAL : 0x000003FFU
    // .. .. reg_arb_disable_aging_rd_portn = 0x0
    // .. .. ==> 0XF8006220[16:16] = 0x00000000U
    // .. ..     ==> MASK : 0x00010000U    VAL : 0x00000000U
    // .. .. reg_arb_disable_urgent_rd_portn = 0x0
    // .. .. ==> 0XF8006220[17:17] = 0x00000000U
    // .. ..     ==> MASK : 0x00020000U    VAL : 0x00000000U
    // .. .. reg_arb_dis_page_match_rd_portn = 0x0
    // .. .. ==> 0XF8006220[18:18] = 0x00000000U
    // .. ..     ==> MASK : 0x00040000U    VAL : 0x00000000U
    // .. .. reg_arb_set_hpr_rd_portn = 0x0
    // .. .. ==> 0XF8006220[19:19] = 0x00000000U
    // .. ..     ==> MASK : 0x00080000U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006220, 0x000F03FFU ,0x000003FFU),
    // .. .. reg_arb_pri_rd_portn = 0x3ff
    // .. .. ==> 0XF8006224[9:0] = 0x000003FFU
    // .. ..     ==> MASK : 0x000003FFU    VAL : 0x000003FFU
    // .. .. reg_arb_disable_aging_rd_portn = 0x0
    // .. .. ==> 0XF8006224[16:16] = 0x00000000U
    // .. ..     ==> MASK : 0x00010000U    VAL : 0x00000000U
    // .. .. reg_arb_disable_urgent_rd_portn = 0x0
    // .. .. ==> 0XF8006224[17:17] = 0x00000000U
    // .. ..     ==> MASK : 0x00020000U    VAL : 0x00000000U
    // .. .. reg_arb_dis_page_match_rd_portn = 0x0
    // .. .. ==> 0XF8006224[18:18] = 0x00000000U
    // .. ..     ==> MASK : 0x00040000U    VAL : 0x00000000U
    // .. .. reg_arb_set_hpr_rd_portn = 0x0
    // .. .. ==> 0XF8006224[19:19] = 0x00000000U
    // .. ..     ==> MASK : 0x00080000U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006224, 0x000F03FFU ,0x000003FFU),
    // .. .. reg_ddrc_lpddr2 = 0x0
    // .. .. ==> 0XF80062A8[0:0] = 0x00000000U
    // .. ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. .. reg_ddrc_per_bank_refresh = 0x0
    // .. .. ==> 0XF80062A8[1:1] = 0x00000000U
    // .. ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. .. reg_ddrc_derate_enable = 0x0
    // .. .. ==> 0XF80062A8[2:2] = 0x00000000U
    // .. ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. .. reg_ddrc_mr4_margin = 0x0
    // .. .. ==> 0XF80062A8[11:4] = 0x00000000U
    // .. ..     ==> MASK : 0x00000FF0U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF80062A8, 0x00000FF7U ,0x00000000U),
    // .. .. reg_ddrc_mr4_read_interval = 0x0
    // .. .. ==> 0XF80062AC[31:0] = 0x00000000U
    // .. ..     ==> MASK : 0xFFFFFFFFU    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF80062AC, 0xFFFFFFFFU ,0x00000000U),
    // .. .. reg_ddrc_min_stable_clock_x1 = 0x5
    // .. .. ==> 0XF80062B0[3:0] = 0x00000005U
    // .. ..     ==> MASK : 0x0000000FU    VAL : 0x00000005U
    // .. .. reg_ddrc_idle_after_reset_x32 = 0x12
    // .. .. ==> 0XF80062B0[11:4] = 0x00000012U
    // .. ..     ==> MASK : 0x00000FF0U    VAL : 0x00000120U
    // .. .. reg_ddrc_t_mrw = 0x5
    // .. .. ==> 0XF80062B0[21:12] = 0x00000005U
    // .. ..     ==> MASK : 0x003FF000U    VAL : 0x00005000U
    // .. ..
    EMIT_MASKWRITE(0XF80062B0, 0x003FFFFFU ,0x00005125U),
    // .. .. reg_ddrc_max_auto_init_x1024 = 0xa8
    // .. .. ==> 0XF80062B4[7:0] = 0x000000A8U
    // .. ..     ==> MASK : 0x000000FFU    VAL : 0x000000A8U
    // .. .. reg_ddrc_dev_zqinit_x32 = 0x12
    // .. .. ==> 0XF80062B4[17:8] = 0x00000012U
    // .. ..     ==> MASK : 0x0003FF00U    VAL : 0x00001200U
    // .. ..
    EMIT_MASKWRITE(0XF80062B4, 0x0003FFFFU ,0x000012A8U),
    // .. .. START: POLL ON DCI STATUS
    // .. .. DONE = 1
    // .. .. ==> 0XF8000B74[13:13] = 0x00000001U
    // .. ..     ==> MASK : 0x00002000U    VAL : 0x00002000U
    // .. ..
    EMIT_MASKPOLL(0XF8000B74, 0x00002000U),
    // .. .. FINISH: POLL ON DCI STATUS
    // .. .. START: UNLOCK DDR
    // .. .. reg_ddrc_soft_rstb = 0x1
    // .. .. ==> 0XF8006000[0:0] = 0x00000001U
    // .. ..     ==> MASK : 0x00000001U    VAL : 0x00000001U
    // .. .. reg_ddrc_powerdown_en = 0x0
    // .. .. ==> 0XF8006000[1:1] = 0x00000000U
    // .. ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. .. reg_ddrc_data_bus_width = 0x0
    // .. .. ==> 0XF8006000[3:2] = 0x00000000U
    // .. ..     ==> MASK : 0x0000000CU    VAL : 0x00000000U
    // .. .. reg_ddrc_burst8_refresh = 0x0
    // .. .. ==> 0XF8006000[6:4] = 0x00000000U
    // .. ..     ==> MASK : 0x00000070U    VAL : 0x00000000U
    // .. .. reg_ddrc_rdwr_idle_gap = 1
    // .. .. ==> 0XF8006000[13:7] = 0x00000001U
    // .. ..     ==> MASK : 0x00003F80U    VAL : 0x00000080U
    // .. .. reg_ddrc_dis_rd_bypass = 0x0
    // .. .. ==> 0XF8006000[14:14] = 0x00000000U
    // .. ..     ==> MASK : 0x00004000U    VAL : 0x00000000U
    // .. .. reg_ddrc_dis_act_bypass = 0x0
    // .. .. ==> 0XF8006000[15:15] = 0x00000000U
    // .. ..     ==> MASK : 0x00008000U    VAL : 0x00000000U
    // .. .. reg_ddrc_dis_auto_refresh = 0x0
    // .. .. ==> 0XF8006000[16:16] = 0x00000000U
    // .. ..     ==> MASK : 0x00010000U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006000, 0x0001FFFFU ,0x00000081U),
    // .. .. FINISH: UNLOCK DDR
    // .. .. START: CHECK DDR STATUS
    // .. .. ddrc_reg_operating_mode = 1
    // .. .. ==> 0XF8006054[2:0] = 0x00000001U
    // .. ..     ==> MASK : 0x00000007U    VAL : 0x00000001U
    // .. ..
    EMIT_MASKPOLL(0XF8006054, 0x00000007U),
    // .. .. FINISH: CHECK DDR STATUS
    // .. FINISH: DDR INITIALIZATION
    // FINISH: top
    //
    EMIT_EXIT(),

    //
};

unsigned long ps7_mio_init_data_2_0[] = {
    // START: top
    // .. START: SLCR SETTINGS
    // .. UNLOCK_KEY = 0XDF0D
    // .. ==> 0XF8000008[15:0] = 0x0000DF0DU
    // ..     ==> MASK : 0x0000FFFFU    VAL : 0x0000DF0DU
    // ..
    EMIT_WRITE(0XF8000008, 0x0000DF0DU),
    // .. FINISH: SLCR SETTINGS
    // .. START: OCM REMAPPING
    // .. FINISH: OCM REMAPPING
    // .. START: DDRIOB SETTINGS
    // .. INP_POWER = 0x0
    // .. ==> 0XF8000B40[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. INP_TYPE = 0x0
    // .. ==> 0XF8000B40[2:1] = 0x00000000U
    // ..     ==> MASK : 0x00000006U    VAL : 0x00000000U
    // .. DCI_UPDATE = 0x0
    // .. ==> 0XF8000B40[3:3] = 0x00000000U
    // ..     ==> MASK : 0x00000008U    VAL : 0x00000000U
    // .. TERM_EN = 0x0
    // .. ==> 0XF8000B40[4:4] = 0x00000000U
    // ..     ==> MASK : 0x00000010U    VAL : 0x00000000U
    // .. DCR_TYPE = 0x0
    // .. ==> 0XF8000B40[6:5] = 0x00000000U
    // ..     ==> MASK : 0x00000060U    VAL : 0x00000000U
    // .. IBUF_DISABLE_MODE = 0x0
    // .. ==> 0XF8000B40[7:7] = 0x00000000U
    // ..     ==> MASK : 0x00000080U    VAL : 0x00000000U
    // .. TERM_DISABLE_MODE = 0x0
    // .. ==> 0XF8000B40[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. OUTPUT_EN = 0x3
    // .. ==> 0XF8000B40[10:9] = 0x00000003U
    // ..     ==> MASK : 0x00000600U    VAL : 0x00000600U
    // .. PULLUP_EN = 0x0
    // .. ==> 0XF8000B40[11:11] = 0x00000000U
    // ..     ==> MASK : 0x00000800U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF8000B40, 0x00000FFFU ,0x00000600U),
    // .. INP_POWER = 0x0
    // .. ==> 0XF8000B44[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. INP_TYPE = 0x0
    // .. ==> 0XF8000B44[2:1] = 0x00000000U
    // ..     ==> MASK : 0x00000006U    VAL : 0x00000000U
    // .. DCI_UPDATE = 0x0
    // .. ==> 0XF8000B44[3:3] = 0x00000000U
    // ..     ==> MASK : 0x00000008U    VAL : 0x00000000U
    // .. TERM_EN = 0x0
    // .. ==> 0XF8000B44[4:4] = 0x00000000U
    // ..     ==> MASK : 0x00000010U    VAL : 0x00000000U
    // .. DCR_TYPE = 0x0
    // .. ==> 0XF8000B44[6:5] = 0x00000000U
    // ..     ==> MASK : 0x00000060U    VAL : 0x00000000U
    // .. IBUF_DISABLE_MODE = 0x0
    // .. ==> 0XF8000B44[7:7] = 0x00000000U
    // ..     ==> MASK : 0x00000080U    VAL : 0x00000000U
    // .. TERM_DISABLE_MODE = 0x0
    // .. ==> 0XF8000B44[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. OUTPUT_EN = 0x3
    // .. ==> 0XF8000B44[10:9] = 0x00000003U
    // ..     ==> MASK : 0x00000600U    VAL : 0x00000600U
    // .. PULLUP_EN = 0x0
    // .. ==> 0XF8000B44[11:11] = 0x00000000U
    // ..     ==> MASK : 0x00000800U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF8000B44, 0x00000FFFU ,0x00000600U),
    // .. INP_POWER = 0x0
    // .. ==> 0XF8000B48[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. INP_TYPE = 0x1
    // .. ==> 0XF8000B48[2:1] = 0x00000001U
    // ..     ==> MASK : 0x00000006U    VAL : 0x00000002U
    // .. DCI_UPDATE = 0x0
    // .. ==> 0XF8000B48[3:3] = 0x00000000U
    // ..     ==> MASK : 0x00000008U    VAL : 0x00000000U
    // .. TERM_EN = 0x1
    // .. ==> 0XF8000B48[4:4] = 0x00000001U
    // ..     ==> MASK : 0x00000010U    VAL : 0x00000010U
    // .. DCR_TYPE = 0x3
    // .. ==> 0XF8000B48[6:5] = 0x00000003U
    // ..     ==> MASK : 0x00000060U    VAL : 0x00000060U
    // .. IBUF_DISABLE_MODE = 0
    // .. ==> 0XF8000B48[7:7] = 0x00000000U
    // ..     ==> MASK : 0x00000080U    VAL : 0x00000000U
    // .. TERM_DISABLE_MODE = 0
    // .. ==> 0XF8000B48[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. OUTPUT_EN = 0x3
    // .. ==> 0XF8000B48[10:9] = 0x00000003U
    // ..     ==> MASK : 0x00000600U    VAL : 0x00000600U
    // .. PULLUP_EN = 0x0
    // .. ==> 0XF8000B48[11:11] = 0x00000000U
    // ..     ==> MASK : 0x00000800U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF8000B48, 0x00000FFFU ,0x00000672U),
    // .. INP_POWER = 0x0
    // .. ==> 0XF8000B4C[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. INP_TYPE = 0x1
    // .. ==> 0XF8000B4C[2:1] = 0x00000001U
    // ..     ==> MASK : 0x00000006U    VAL : 0x00000002U
    // .. DCI_UPDATE = 0x0
    // .. ==> 0XF8000B4C[3:3] = 0x00000000U
    // ..     ==> MASK : 0x00000008U    VAL : 0x00000000U
    // .. TERM_EN = 0x1
    // .. ==> 0XF8000B4C[4:4] = 0x00000001U
    // ..     ==> MASK : 0x00000010U    VAL : 0x00000010U
    // .. DCR_TYPE = 0x3
    // .. ==> 0XF8000B4C[6:5] = 0x00000003U
    // ..     ==> MASK : 0x00000060U    VAL : 0x00000060U
    // .. IBUF_DISABLE_MODE = 0
    // .. ==> 0XF8000B4C[7:7] = 0x00000000U
    // ..     ==> MASK : 0x00000080U    VAL : 0x00000000U
    // .. TERM_DISABLE_MODE = 0
    // .. ==> 0XF8000B4C[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. OUTPUT_EN = 0x3
    // .. ==> 0XF8000B4C[10:9] = 0x00000003U
    // ..     ==> MASK : 0x00000600U    VAL : 0x00000600U
    // .. PULLUP_EN = 0x0
    // .. ==> 0XF8000B4C[11:11] = 0x00000000U
    // ..     ==> MASK : 0x00000800U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF8000B4C, 0x00000FFFU ,0x00000672U),
    // .. INP_POWER = 0x0
    // .. ==> 0XF8000B50[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. INP_TYPE = 0x2
    // .. ==> 0XF8000B50[2:1] = 0x00000002U
    // ..     ==> MASK : 0x00000006U    VAL : 0x00000004U
    // .. DCI_UPDATE = 0x0
    // .. ==> 0XF8000B50[3:3] = 0x00000000U
    // ..     ==> MASK : 0x00000008U    VAL : 0x00000000U
    // .. TERM_EN = 0x1
    // .. ==> 0XF8000B50[4:4] = 0x00000001U
    // ..     ==> MASK : 0x00000010U    VAL : 0x00000010U
    // .. DCR_TYPE = 0x3
    // .. ==> 0XF8000B50[6:5] = 0x00000003U
    // ..     ==> MASK : 0x00000060U    VAL : 0x00000060U
    // .. IBUF_DISABLE_MODE = 0
    // .. ==> 0XF8000B50[7:7] = 0x00000000U
    // ..     ==> MASK : 0x00000080U    VAL : 0x00000000U
    // .. TERM_DISABLE_MODE = 0
    // .. ==> 0XF8000B50[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. OUTPUT_EN = 0x3
    // .. ==> 0XF8000B50[10:9] = 0x00000003U
    // ..     ==> MASK : 0x00000600U    VAL : 0x00000600U
    // .. PULLUP_EN = 0x0
    // .. ==> 0XF8000B50[11:11] = 0x00000000U
    // ..     ==> MASK : 0x00000800U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF8000B50, 0x00000FFFU ,0x00000674U),
    // .. INP_POWER = 0x0
    // .. ==> 0XF8000B54[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. INP_TYPE = 0x2
    // .. ==> 0XF8000B54[2:1] = 0x00000002U
    // ..     ==> MASK : 0x00000006U    VAL : 0x00000004U
    // .. DCI_UPDATE = 0x0
    // .. ==> 0XF8000B54[3:3] = 0x00000000U
    // ..     ==> MASK : 0x00000008U    VAL : 0x00000000U
    // .. TERM_EN = 0x1
    // .. ==> 0XF8000B54[4:4] = 0x00000001U
    // ..     ==> MASK : 0x00000010U    VAL : 0x00000010U
    // .. DCR_TYPE = 0x3
    // .. ==> 0XF8000B54[6:5] = 0x00000003U
    // ..     ==> MASK : 0x00000060U    VAL : 0x00000060U
    // .. IBUF_DISABLE_MODE = 0
    // .. ==> 0XF8000B54[7:7] = 0x00000000U
    // ..     ==> MASK : 0x00000080U    VAL : 0x00000000U
    // .. TERM_DISABLE_MODE = 0
    // .. ==> 0XF8000B54[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. OUTPUT_EN = 0x3
    // .. ==> 0XF8000B54[10:9] = 0x00000003U
    // ..     ==> MASK : 0x00000600U    VAL : 0x00000600U
    // .. PULLUP_EN = 0x0
    // .. ==> 0XF8000B54[11:11] = 0x00000000U
    // ..     ==> MASK : 0x00000800U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF8000B54, 0x00000FFFU ,0x00000674U),
    // .. INP_POWER = 0x0
    // .. ==> 0XF8000B58[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. INP_TYPE = 0x0
    // .. ==> 0XF8000B58[2:1] = 0x00000000U
    // ..     ==> MASK : 0x00000006U    VAL : 0x00000000U
    // .. DCI_UPDATE = 0x0
    // .. ==> 0XF8000B58[3:3] = 0x00000000U
    // ..     ==> MASK : 0x00000008U    VAL : 0x00000000U
    // .. TERM_EN = 0x0
    // .. ==> 0XF8000B58[4:4] = 0x00000000U
    // ..     ==> MASK : 0x00000010U    VAL : 0x00000000U
    // .. DCR_TYPE = 0x0
    // .. ==> 0XF8000B58[6:5] = 0x00000000U
    // ..     ==> MASK : 0x00000060U    VAL : 0x00000000U
    // .. IBUF_DISABLE_MODE = 0x0
    // .. ==> 0XF8000B58[7:7] = 0x00000000U
    // ..     ==> MASK : 0x00000080U    VAL : 0x00000000U
    // .. TERM_DISABLE_MODE = 0x0
    // .. ==> 0XF8000B58[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. OUTPUT_EN = 0x3
    // .. ==> 0XF8000B58[10:9] = 0x00000003U
    // ..     ==> MASK : 0x00000600U    VAL : 0x00000600U
    // .. PULLUP_EN = 0x0
    // .. ==> 0XF8000B58[11:11] = 0x00000000U
    // ..     ==> MASK : 0x00000800U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF8000B58, 0x00000FFFU ,0x00000600U),
    // .. DRIVE_P = 0x1c
    // .. ==> 0XF8000B5C[6:0] = 0x0000001CU
    // ..     ==> MASK : 0x0000007FU    VAL : 0x0000001CU
    // .. DRIVE_N = 0xc
    // .. ==> 0XF8000B5C[13:7] = 0x0000000CU
    // ..     ==> MASK : 0x00003F80U    VAL : 0x00000600U
    // .. SLEW_P = 0x3
    // .. ==> 0XF8000B5C[18:14] = 0x00000003U
    // ..     ==> MASK : 0x0007C000U    VAL : 0x0000C000U
    // .. SLEW_N = 0x3
    // .. ==> 0XF8000B5C[23:19] = 0x00000003U
    // ..     ==> MASK : 0x00F80000U    VAL : 0x00180000U
    // .. GTL = 0x0
    // .. ==> 0XF8000B5C[26:24] = 0x00000000U
    // ..     ==> MASK : 0x07000000U    VAL : 0x00000000U
    // .. RTERM = 0x0
    // .. ==> 0XF8000B5C[31:27] = 0x00000000U
    // ..     ==> MASK : 0xF8000000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF8000B5C, 0xFFFFFFFFU ,0x0018C61CU),
    // .. DRIVE_P = 0x1c
    // .. ==> 0XF8000B60[6:0] = 0x0000001CU
    // ..     ==> MASK : 0x0000007FU    VAL : 0x0000001CU
    // .. DRIVE_N = 0xc
    // .. ==> 0XF8000B60[13:7] = 0x0000000CU
    // ..     ==> MASK : 0x00003F80U    VAL : 0x00000600U
    // .. SLEW_P = 0x6
    // .. ==> 0XF8000B60[18:14] = 0x00000006U
    // ..     ==> MASK : 0x0007C000U    VAL : 0x00018000U
    // .. SLEW_N = 0x1f
    // .. ==> 0XF8000B60[23:19] = 0x0000001FU
    // ..     ==> MASK : 0x00F80000U    VAL : 0x00F80000U
    // .. GTL = 0x0
    // .. ==> 0XF8000B60[26:24] = 0x00000000U
    // ..     ==> MASK : 0x07000000U    VAL : 0x00000000U
    // .. RTERM = 0x0
    // .. ==> 0XF8000B60[31:27] = 0x00000000U
    // ..     ==> MASK : 0xF8000000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF8000B60, 0xFFFFFFFFU ,0x00F9861CU),
    // .. DRIVE_P = 0x1c
    // .. ==> 0XF8000B64[6:0] = 0x0000001CU
    // ..     ==> MASK : 0x0000007FU    VAL : 0x0000001CU
    // .. DRIVE_N = 0xc
    // .. ==> 0XF8000B64[13:7] = 0x0000000CU
    // ..     ==> MASK : 0x00003F80U    VAL : 0x00000600U
    // .. SLEW_P = 0x6
    // .. ==> 0XF8000B64[18:14] = 0x00000006U
    // ..     ==> MASK : 0x0007C000U    VAL : 0x00018000U
    // .. SLEW_N = 0x1f
    // .. ==> 0XF8000B64[23:19] = 0x0000001FU
    // ..     ==> MASK : 0x00F80000U    VAL : 0x00F80000U
    // .. GTL = 0x0
    // .. ==> 0XF8000B64[26:24] = 0x00000000U
    // ..     ==> MASK : 0x07000000U    VAL : 0x00000000U
    // .. RTERM = 0x0
    // .. ==> 0XF8000B64[31:27] = 0x00000000U
    // ..     ==> MASK : 0xF8000000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF8000B64, 0xFFFFFFFFU ,0x00F9861CU),
    // .. DRIVE_P = 0x1c
    // .. ==> 0XF8000B68[6:0] = 0x0000001CU
    // ..     ==> MASK : 0x0000007FU    VAL : 0x0000001CU
    // .. DRIVE_N = 0xc
    // .. ==> 0XF8000B68[13:7] = 0x0000000CU
    // ..     ==> MASK : 0x00003F80U    VAL : 0x00000600U
    // .. SLEW_P = 0x6
    // .. ==> 0XF8000B68[18:14] = 0x00000006U
    // ..     ==> MASK : 0x0007C000U    VAL : 0x00018000U
    // .. SLEW_N = 0x1f
    // .. ==> 0XF8000B68[23:19] = 0x0000001FU
    // ..     ==> MASK : 0x00F80000U    VAL : 0x00F80000U
    // .. GTL = 0x0
    // .. ==> 0XF8000B68[26:24] = 0x00000000U
    // ..     ==> MASK : 0x07000000U    VAL : 0x00000000U
    // .. RTERM = 0x0
    // .. ==> 0XF8000B68[31:27] = 0x00000000U
    // ..     ==> MASK : 0xF8000000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF8000B68, 0xFFFFFFFFU ,0x00F9861CU),
    // .. VREF_INT_EN = 0x0
    // .. ==> 0XF8000B6C[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. VREF_SEL = 0x0
    // .. ==> 0XF8000B6C[4:1] = 0x00000000U
    // ..     ==> MASK : 0x0000001EU    VAL : 0x00000000U
    // .. VREF_EXT_EN = 0x3
    // .. ==> 0XF8000B6C[6:5] = 0x00000003U
    // ..     ==> MASK : 0x00000060U    VAL : 0x00000060U
    // .. VREF_PULLUP_EN = 0x0
    // .. ==> 0XF8000B6C[8:7] = 0x00000000U
    // ..     ==> MASK : 0x00000180U    VAL : 0x00000000U
    // .. REFIO_EN = 0x1
    // .. ==> 0XF8000B6C[9:9] = 0x00000001U
    // ..     ==> MASK : 0x00000200U    VAL : 0x00000200U
    // .. REFIO_TEST = 0x0
    // .. ==> 0XF8000B6C[11:10] = 0x00000000U
    // ..     ==> MASK : 0x00000C00U    VAL : 0x00000000U
    // .. REFIO_PULLUP_EN = 0x0
    // .. ==> 0XF8000B6C[12:12] = 0x00000000U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00000000U
    // .. DRST_B_PULLUP_EN = 0x0
    // .. ==> 0XF8000B6C[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // .. CKE_PULLUP_EN = 0x0
    // .. ==> 0XF8000B6C[14:14] = 0x00000000U
    // ..     ==> MASK : 0x00004000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF8000B6C, 0x00007FFFU ,0x00000260U),
    // .. .. START: ASSERT RESET
    // .. .. RESET = 1
    // .. .. ==> 0XF8000B70[0:0] = 0x00000001U
    // .. ..     ==> MASK : 0x00000001U    VAL : 0x00000001U
    // .. .. VRN_OUT = 0x1
    // .. .. ==> 0XF8000B70[5:5] = 0x00000001U
    // .. ..     ==> MASK : 0x00000020U    VAL : 0x00000020U
    // .. ..
    EMIT_MASKWRITE(0XF8000B70, 0x00000021U ,0x00000021U),
    // .. .. FINISH: ASSERT RESET
    // .. .. START: DEASSERT RESET
    // .. .. RESET = 0
    // .. .. ==> 0XF8000B70[0:0] = 0x00000000U
    // .. ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. .. VRN_OUT = 0x1
    // .. .. ==> 0XF8000B70[5:5] = 0x00000001U
    // .. ..     ==> MASK : 0x00000020U    VAL : 0x00000020U
    // .. ..
    EMIT_MASKWRITE(0XF8000B70, 0x00000021U ,0x00000020U),
    // .. .. FINISH: DEASSERT RESET
    // .. .. RESET = 0x1
    // .. .. ==> 0XF8000B70[0:0] = 0x00000001U
    // .. ..     ==> MASK : 0x00000001U    VAL : 0x00000001U
    // .. .. ENABLE = 0x1
    // .. .. ==> 0XF8000B70[1:1] = 0x00000001U
    // .. ..     ==> MASK : 0x00000002U    VAL : 0x00000002U
    // .. .. VRP_TRI = 0x0
    // .. .. ==> 0XF8000B70[2:2] = 0x00000000U
    // .. ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. .. VRN_TRI = 0x0
    // .. .. ==> 0XF8000B70[3:3] = 0x00000000U
    // .. ..     ==> MASK : 0x00000008U    VAL : 0x00000000U
    // .. .. VRP_OUT = 0x0
    // .. .. ==> 0XF8000B70[4:4] = 0x00000000U
    // .. ..     ==> MASK : 0x00000010U    VAL : 0x00000000U
    // .. .. VRN_OUT = 0x1
    // .. .. ==> 0XF8000B70[5:5] = 0x00000001U
    // .. ..     ==> MASK : 0x00000020U    VAL : 0x00000020U
    // .. .. NREF_OPT1 = 0x0
    // .. .. ==> 0XF8000B70[7:6] = 0x00000000U
    // .. ..     ==> MASK : 0x000000C0U    VAL : 0x00000000U
    // .. .. NREF_OPT2 = 0x0
    // .. .. ==> 0XF8000B70[10:8] = 0x00000000U
    // .. ..     ==> MASK : 0x00000700U    VAL : 0x00000000U
    // .. .. NREF_OPT4 = 0x1
    // .. .. ==> 0XF8000B70[13:11] = 0x00000001U
    // .. ..     ==> MASK : 0x00003800U    VAL : 0x00000800U
    // .. .. PREF_OPT1 = 0x0
    // .. .. ==> 0XF8000B70[16:14] = 0x00000000U
    // .. ..     ==> MASK : 0x0001C000U    VAL : 0x00000000U
    // .. .. PREF_OPT2 = 0x0
    // .. .. ==> 0XF8000B70[19:17] = 0x00000000U
    // .. ..     ==> MASK : 0x000E0000U    VAL : 0x00000000U
    // .. .. UPDATE_CONTROL = 0x0
    // .. .. ==> 0XF8000B70[20:20] = 0x00000000U
    // .. ..     ==> MASK : 0x00100000U    VAL : 0x00000000U
    // .. .. INIT_COMPLETE = 0x0
    // .. .. ==> 0XF8000B70[21:21] = 0x00000000U
    // .. ..     ==> MASK : 0x00200000U    VAL : 0x00000000U
    // .. .. TST_CLK = 0x0
    // .. .. ==> 0XF8000B70[22:22] = 0x00000000U
    // .. ..     ==> MASK : 0x00400000U    VAL : 0x00000000U
    // .. .. TST_HLN = 0x0
    // .. .. ==> 0XF8000B70[23:23] = 0x00000000U
    // .. ..     ==> MASK : 0x00800000U    VAL : 0x00000000U
    // .. .. TST_HLP = 0x0
    // .. .. ==> 0XF8000B70[24:24] = 0x00000000U
    // .. ..     ==> MASK : 0x01000000U    VAL : 0x00000000U
    // .. .. TST_RST = 0x0
    // .. .. ==> 0XF8000B70[25:25] = 0x00000000U
    // .. ..     ==> MASK : 0x02000000U    VAL : 0x00000000U
    // .. .. INT_DCI_EN = 0x0
    // .. .. ==> 0XF8000B70[26:26] = 0x00000000U
    // .. ..     ==> MASK : 0x04000000U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF8000B70, 0x07FFFFFFU ,0x00000823U),
    // .. FINISH: DDRIOB SETTINGS
    // .. START: MIO PROGRAMMING
    // .. TRI_ENABLE = 0
    // .. ==> 0XF8000700[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF8000700[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF8000700[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 2
    // .. ==> 0XF8000700[4:3] = 0x00000002U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000010U
    // .. L3_SEL = 0
    // .. ==> 0XF8000700[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF8000700[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF8000700[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF8000700[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF8000700[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF8000700, 0x00003FFFU ,0x00001610U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF8000704[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF8000704[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF8000704[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF8000704[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF8000704[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF8000704[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF8000704[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF8000704[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF8000704[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF8000704, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF8000708[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF8000708[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF8000708[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 2
    // .. ==> 0XF8000708[4:3] = 0x00000002U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000010U
    // .. L3_SEL = 0
    // .. ==> 0XF8000708[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF8000708[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF8000708[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 0
    // .. ==> 0XF8000708[12:12] = 0x00000000U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00000000U
    // .. DisableRcvr = 0
    // .. ==> 0XF8000708[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF8000708, 0x00003FFFU ,0x00000610U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF800070C[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF800070C[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF800070C[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 2
    // .. ==> 0XF800070C[4:3] = 0x00000002U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000010U
    // .. L3_SEL = 0
    // .. ==> 0XF800070C[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF800070C[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF800070C[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 0
    // .. ==> 0XF800070C[12:12] = 0x00000000U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00000000U
    // .. DisableRcvr = 0
    // .. ==> 0XF800070C[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF800070C, 0x00003FFFU ,0x00000610U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF8000710[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF8000710[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF8000710[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 2
    // .. ==> 0XF8000710[4:3] = 0x00000002U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000010U
    // .. L3_SEL = 0
    // .. ==> 0XF8000710[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF8000710[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF8000710[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 0
    // .. ==> 0XF8000710[12:12] = 0x00000000U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00000000U
    // .. DisableRcvr = 0
    // .. ==> 0XF8000710[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF8000710, 0x00003FFFU ,0x00000610U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF8000714[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF8000714[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF8000714[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 2
    // .. ==> 0XF8000714[4:3] = 0x00000002U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000010U
    // .. L3_SEL = 0
    // .. ==> 0XF8000714[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF8000714[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF8000714[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 0
    // .. ==> 0XF8000714[12:12] = 0x00000000U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00000000U
    // .. DisableRcvr = 0
    // .. ==> 0XF8000714[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF8000714, 0x00003FFFU ,0x00000610U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF8000718[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF8000718[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF8000718[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 2
    // .. ==> 0XF8000718[4:3] = 0x00000002U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000010U
    // .. L3_SEL = 0
    // .. ==> 0XF8000718[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF8000718[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF8000718[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 0
    // .. ==> 0XF8000718[12:12] = 0x00000000U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00000000U
    // .. DisableRcvr = 0
    // .. ==> 0XF8000718[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF8000718, 0x00003FFFU ,0x00000610U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF800071C[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF800071C[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF800071C[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 2
    // .. ==> 0XF800071C[4:3] = 0x00000002U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000010U
    // .. L3_SEL = 0
    // .. ==> 0XF800071C[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF800071C[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF800071C[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 0
    // .. ==> 0XF800071C[12:12] = 0x00000000U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00000000U
    // .. DisableRcvr = 0
    // .. ==> 0XF800071C[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF800071C, 0x00003FFFU ,0x00000610U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF8000720[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF8000720[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF8000720[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 2
    // .. ==> 0XF8000720[4:3] = 0x00000002U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000010U
    // .. L3_SEL = 0
    // .. ==> 0XF8000720[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF8000720[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF8000720[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 0
    // .. ==> 0XF8000720[12:12] = 0x00000000U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00000000U
    // .. DisableRcvr = 0
    // .. ==> 0XF8000720[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF8000720, 0x00003FFFU ,0x00000610U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF8000724[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF8000724[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF8000724[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 2
    // .. ==> 0XF8000724[4:3] = 0x00000002U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000010U
    // .. L3_SEL = 0
    // .. ==> 0XF8000724[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF8000724[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF8000724[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF8000724[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF8000724[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF8000724, 0x00003FFFU ,0x00001610U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF8000728[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF8000728[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF8000728[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 2
    // .. ==> 0XF8000728[4:3] = 0x00000002U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000010U
    // .. L3_SEL = 0
    // .. ==> 0XF8000728[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF8000728[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF8000728[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF8000728[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF8000728[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF8000728, 0x00003FFFU ,0x00001610U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF800072C[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF800072C[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF800072C[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 2
    // .. ==> 0XF800072C[4:3] = 0x00000002U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000010U
    // .. L3_SEL = 0
    // .. ==> 0XF800072C[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF800072C[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF800072C[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF800072C[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF800072C[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF800072C, 0x00003FFFU ,0x00001610U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF8000730[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF8000730[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF8000730[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 2
    // .. ==> 0XF8000730[4:3] = 0x00000002U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000010U
    // .. L3_SEL = 0
    // .. ==> 0XF8000730[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF8000730[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF8000730[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF8000730[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF8000730[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF8000730, 0x00003FFFU ,0x00001610U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF8000734[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF8000734[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF8000734[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 2
    // .. ==> 0XF8000734[4:3] = 0x00000002U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000010U
    // .. L3_SEL = 0
    // .. ==> 0XF8000734[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF8000734[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF8000734[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF8000734[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF8000734[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF8000734, 0x00003FFFU ,0x00001610U),
    // .. TRI_ENABLE = 1
    // .. ==> 0XF8000738[0:0] = 0x00000001U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000001U
    // .. L0_SEL = 0
    // .. ==> 0XF8000738[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF8000738[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 2
    // .. ==> 0XF8000738[4:3] = 0x00000002U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000010U
    // .. L3_SEL = 0
    // .. ==> 0XF8000738[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF8000738[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF8000738[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF8000738[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF8000738[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF8000738, 0x00003FFFU ,0x00001611U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF800073C[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF800073C[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF800073C[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF800073C[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF800073C[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF800073C[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF800073C[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF800073C[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF800073C[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF800073C, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF8000740[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF8000740[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF8000740[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF8000740[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF8000740[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF8000740[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF8000740[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF8000740[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF8000740[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF8000740, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF8000744[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF8000744[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF8000744[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF8000744[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF8000744[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF8000744[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF8000744[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF8000744[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF8000744[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF8000744, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF8000748[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF8000748[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF8000748[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF8000748[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF8000748[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF8000748[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF8000748[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF8000748[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF8000748[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF8000748, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF800074C[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF800074C[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF800074C[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF800074C[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF800074C[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF800074C[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF800074C[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF800074C[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF800074C[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF800074C, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF8000750[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF8000750[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF8000750[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF8000750[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF8000750[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF8000750[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF8000750[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF8000750[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF8000750[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF8000750, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF8000754[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF8000754[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF8000754[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF8000754[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF8000754[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF8000754[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF8000754[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF8000754[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF8000754[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF8000754, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF8000758[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF8000758[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF8000758[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF8000758[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF8000758[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF8000758[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF8000758[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF8000758[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF8000758[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF8000758, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF800075C[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF800075C[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF800075C[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF800075C[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF800075C[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF800075C[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF800075C[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF800075C[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF800075C[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF800075C, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF8000760[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF8000760[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF8000760[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF8000760[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 7
    // .. ==> 0XF8000760[7:5] = 0x00000007U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x000000E0U
    // .. Speed = 0
    // .. ==> 0XF8000760[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF8000760[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF8000760[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF8000760[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF8000760, 0x00003FFFU ,0x000016E0U),
    // .. TRI_ENABLE = 1
    // .. ==> 0XF8000764[0:0] = 0x00000001U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000001U
    // .. L0_SEL = 0
    // .. ==> 0XF8000764[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF8000764[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF8000764[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 7
    // .. ==> 0XF8000764[7:5] = 0x00000007U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x000000E0U
    // .. Speed = 0
    // .. ==> 0XF8000764[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF8000764[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF8000764[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF8000764[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF8000764, 0x00003FFFU ,0x000016E1U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF8000768[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF8000768[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF8000768[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF8000768[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF8000768[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 1
    // .. ==> 0XF8000768[8:8] = 0x00000001U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000100U
    // .. IO_Type = 3
    // .. ==> 0XF8000768[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 0
    // .. ==> 0XF8000768[12:12] = 0x00000000U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00000000U
    // .. DisableRcvr = 0
    // .. ==> 0XF8000768[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF8000768, 0x00003FFFU ,0x00000700U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF800076C[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF800076C[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF800076C[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF800076C[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF800076C[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 1
    // .. ==> 0XF800076C[8:8] = 0x00000001U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000100U
    // .. IO_Type = 3
    // .. ==> 0XF800076C[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 0
    // .. ==> 0XF800076C[12:12] = 0x00000000U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00000000U
    // .. DisableRcvr = 0
    // .. ==> 0XF800076C[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF800076C, 0x00003FFFU ,0x00000700U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF8000770[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF8000770[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF8000770[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF8000770[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF8000770[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF8000770[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF8000770[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF8000770[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF8000770[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF8000770, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF8000774[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF8000774[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF8000774[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF8000774[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF8000774[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF8000774[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF8000774[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF8000774[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF8000774[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF8000774, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF8000778[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF8000778[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF8000778[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF8000778[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF8000778[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF8000778[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF8000778[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF8000778[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF8000778[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF8000778, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF800077C[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF800077C[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF800077C[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF800077C[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF800077C[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF800077C[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF800077C[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF800077C[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF800077C[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF800077C, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF8000780[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF8000780[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF8000780[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF8000780[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF8000780[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF8000780[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF8000780[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF8000780[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF8000780[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF8000780, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF8000784[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF8000784[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF8000784[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF8000784[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF8000784[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF8000784[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF8000784[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF8000784[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF8000784[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF8000784, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF8000788[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF8000788[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF8000788[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF8000788[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF8000788[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF8000788[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF8000788[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF8000788[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF8000788[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF8000788, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF800078C[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF800078C[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF800078C[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF800078C[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF800078C[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF800078C[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF800078C[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF800078C[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF800078C[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF800078C, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF8000790[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF8000790[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF8000790[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF8000790[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF8000790[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF8000790[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF8000790[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF8000790[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF8000790[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF8000790, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF8000794[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF8000794[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF8000794[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF8000794[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF8000794[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF8000794[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF8000794[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF8000794[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF8000794[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF8000794, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF8000798[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF8000798[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF8000798[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF8000798[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF8000798[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF8000798[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF8000798[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF8000798[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF8000798[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF8000798, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF800079C[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF800079C[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF800079C[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF800079C[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF800079C[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF800079C[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF800079C[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF800079C[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF800079C[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF800079C, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF80007A0[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF80007A0[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF80007A0[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF80007A0[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF80007A0[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF80007A0[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF80007A0[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF80007A0[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF80007A0[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF80007A0, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF80007A4[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF80007A4[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF80007A4[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF80007A4[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF80007A4[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF80007A4[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF80007A4[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF80007A4[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF80007A4[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF80007A4, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF80007A8[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF80007A8[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF80007A8[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF80007A8[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF80007A8[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF80007A8[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF80007A8[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF80007A8[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF80007A8[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF80007A8, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF80007AC[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF80007AC[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF80007AC[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF80007AC[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF80007AC[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF80007AC[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF80007AC[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF80007AC[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF80007AC[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF80007AC, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF80007B0[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF80007B0[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF80007B0[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF80007B0[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF80007B0[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF80007B0[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF80007B0[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF80007B0[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF80007B0[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF80007B0, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF80007B4[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF80007B4[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF80007B4[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF80007B4[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF80007B4[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF80007B4[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF80007B4[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF80007B4[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF80007B4[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF80007B4, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF80007B8[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF80007B8[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF80007B8[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF80007B8[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF80007B8[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF80007B8[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF80007B8[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF80007B8[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF80007B8[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF80007B8, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF80007BC[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF80007BC[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF80007BC[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF80007BC[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF80007BC[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF80007BC[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF80007BC[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF80007BC[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF80007BC[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF80007BC, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF80007C0[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF80007C0[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF80007C0[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF80007C0[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF80007C0[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF80007C0[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF80007C0[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF80007C0[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF80007C0[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF80007C0, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF80007C4[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF80007C4[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF80007C4[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF80007C4[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF80007C4[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF80007C4[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF80007C4[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF80007C4[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF80007C4[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF80007C4, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF80007C8[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF80007C8[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF80007C8[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF80007C8[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF80007C8[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF80007C8[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF80007C8[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF80007C8[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF80007C8[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF80007C8, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF80007CC[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF80007CC[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF80007CC[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF80007CC[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF80007CC[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF80007CC[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF80007CC[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF80007CC[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF80007CC[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF80007CC, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF80007D0[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF80007D0[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF80007D0[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF80007D0[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF80007D0[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF80007D0[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF80007D0[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF80007D0[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF80007D0[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF80007D0, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF80007D4[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF80007D4[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF80007D4[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF80007D4[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF80007D4[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF80007D4[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF80007D4[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF80007D4[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF80007D4[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF80007D4, 0x00003FFFU ,0x00001600U),
    // .. FINISH: MIO PROGRAMMING
    // .. START: LOCK IT BACK
    // .. LOCK_KEY = 0X767B
    // .. ==> 0XF8000004[15:0] = 0x0000767BU
    // ..     ==> MASK : 0x0000FFFFU    VAL : 0x0000767BU
    // ..
    //EMIT_WRITE(0XF8000004, 0x0000767BU),
    // .. FINISH: LOCK IT BACK
    // FINISH: top
    //
    EMIT_EXIT(),

    //
};

unsigned long ps7_peripherals_init_data_2_0[] = {
    // START: top
    // .. START: SLCR SETTINGS
    // .. UNLOCK_KEY = 0XDF0D
    // .. ==> 0XF8000008[15:0] = 0x0000DF0DU
    // ..     ==> MASK : 0x0000FFFFU    VAL : 0x0000DF0DU
    // ..
    EMIT_WRITE(0XF8000008, 0x0000DF0DU),
    // .. FINISH: SLCR SETTINGS
    // .. START: DDR TERM/IBUF_DISABLE_MODE SETTINGS
    // .. IBUF_DISABLE_MODE = 0x1
    // .. ==> 0XF8000B48[7:7] = 0x00000001U
    // ..     ==> MASK : 0x00000080U    VAL : 0x00000080U
    // .. TERM_DISABLE_MODE = 0x1
    // .. ==> 0XF8000B48[8:8] = 0x00000001U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000100U
    // ..
    EMIT_MASKWRITE(0XF8000B48, 0x00000180U ,0x00000180U),
    // .. IBUF_DISABLE_MODE = 0x1
    // .. ==> 0XF8000B4C[7:7] = 0x00000001U
    // ..     ==> MASK : 0x00000080U    VAL : 0x00000080U
    // .. TERM_DISABLE_MODE = 0x1
    // .. ==> 0XF8000B4C[8:8] = 0x00000001U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000100U
    // ..
    EMIT_MASKWRITE(0XF8000B4C, 0x00000180U ,0x00000180U),
    // .. IBUF_DISABLE_MODE = 0x1
    // .. ==> 0XF8000B50[7:7] = 0x00000001U
    // ..     ==> MASK : 0x00000080U    VAL : 0x00000080U
    // .. TERM_DISABLE_MODE = 0x1
    // .. ==> 0XF8000B50[8:8] = 0x00000001U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000100U
    // ..
    EMIT_MASKWRITE(0XF8000B50, 0x00000180U ,0x00000180U),
    // .. IBUF_DISABLE_MODE = 0x1
    // .. ==> 0XF8000B54[7:7] = 0x00000001U
    // ..     ==> MASK : 0x00000080U    VAL : 0x00000080U
    // .. TERM_DISABLE_MODE = 0x1
    // .. ==> 0XF8000B54[8:8] = 0x00000001U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000100U
    // ..
    EMIT_MASKWRITE(0XF8000B54, 0x00000180U ,0x00000180U),
    // .. FINISH: DDR TERM/IBUF_DISABLE_MODE SETTINGS
    // .. START: LOCK IT BACK
    // .. LOCK_KEY = 0X767B
    // .. ==> 0XF8000004[15:0] = 0x0000767BU
    // ..     ==> MASK : 0x0000FFFFU    VAL : 0x0000767BU
    // ..
    //EMIT_WRITE(0XF8000004, 0x0000767BU),
    // .. FINISH: LOCK IT BACK
    // .. START: SRAM/NOR SET OPMODE
    // .. FINISH: SRAM/NOR SET OPMODE
    // .. START: UART REGISTERS
    // .. BDIV = 0x6
    // .. ==> 0XE0001034[7:0] = 0x00000006U
    // ..     ==> MASK : 0x000000FFU    VAL : 0x00000006U
    // ..
    EMIT_MASKWRITE(0XE0001034, 0x000000FFU ,0x00000006U),
    // .. CD = 0x7c
    // .. ==> 0XE0001018[15:0] = 0x0000007CU
    // ..     ==> MASK : 0x0000FFFFU    VAL : 0x0000007CU
    // ..
    EMIT_MASKWRITE(0XE0001018, 0x0000FFFFU ,0x0000007CU),
    // .. STPBRK = 0x0
    // .. ==> 0XE0001000[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. STTBRK = 0x0
    // .. ==> 0XE0001000[7:7] = 0x00000000U
    // ..     ==> MASK : 0x00000080U    VAL : 0x00000000U
    // .. RSTTO = 0x0
    // .. ==> 0XE0001000[6:6] = 0x00000000U
    // ..     ==> MASK : 0x00000040U    VAL : 0x00000000U
    // .. TXDIS = 0x0
    // .. ==> 0XE0001000[5:5] = 0x00000000U
    // ..     ==> MASK : 0x00000020U    VAL : 0x00000000U
    // .. TXEN = 0x1
    // .. ==> 0XE0001000[4:4] = 0x00000001U
    // ..     ==> MASK : 0x00000010U    VAL : 0x00000010U
    // .. RXDIS = 0x0
    // .. ==> 0XE0001000[3:3] = 0x00000000U
    // ..     ==> MASK : 0x00000008U    VAL : 0x00000000U
    // .. RXEN = 0x1
    // .. ==> 0XE0001000[2:2] = 0x00000001U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000004U
    // .. TXRES = 0x1
    // .. ==> 0XE0001000[1:1] = 0x00000001U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000002U
    // .. RXRES = 0x1
    // .. ==> 0XE0001000[0:0] = 0x00000001U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000001U
    // ..
    EMIT_MASKWRITE(0XE0001000, 0x000001FFU ,0x00000017U),
    // .. IRMODE = 0x0
    // .. ==> 0XE0001004[11:11] = 0x00000000U
    // ..     ==> MASK : 0x00000800U    VAL : 0x00000000U
    // .. UCLKEN = 0x0
    // .. ==> 0XE0001004[10:10] = 0x00000000U
    // ..     ==> MASK : 0x00000400U    VAL : 0x00000000U
    // .. CHMODE = 0x0
    // .. ==> 0XE0001004[9:8] = 0x00000000U
    // ..     ==> MASK : 0x00000300U    VAL : 0x00000000U
    // .. NBSTOP = 0x0
    // .. ==> 0XE0001004[7:6] = 0x00000000U
    // ..     ==> MASK : 0x000000C0U    VAL : 0x00000000U
    // .. PAR = 0x4
    // .. ==> 0XE0001004[5:3] = 0x00000004U
    // ..     ==> MASK : 0x00000038U    VAL : 0x00000020U
    // .. CHRL = 0x0
    // .. ==> 0XE0001004[2:1] = 0x00000000U
    // ..     ==> MASK : 0x00000006U    VAL : 0x00000000U
    // .. CLKS = 0x0
    // .. ==> 0XE0001004[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XE0001004, 0x00000FFFU ,0x00000020U),
    // .. FINISH: UART REGISTERS
    // .. START: QSPI REGISTERS
    // .. Holdb_dr = 1
    // .. ==> 0XE000D000[19:19] = 0x00000001U
    // ..     ==> MASK : 0x00080000U    VAL : 0x00080000U
    // ..
    EMIT_MASKWRITE(0XE000D000, 0x00080000U ,0x00080000U),
    // .. FINISH: QSPI REGISTERS
    // .. START: PL POWER ON RESET REGISTERS
    // .. PCFG_POR_CNT_4K = 0
    // .. ==> 0XF8007000[29:29] = 0x00000000U
    // ..     ==> MASK : 0x20000000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF8007000, 0x20000000U ,0x00000000U),
    // .. FINISH: PL POWER ON RESET REGISTERS
    // .. START: SMC TIMING CALCULATION REGISTER UPDATE
    // .. .. START: NAND SET CYCLE
    // .. .. Set_t0 = 0x2
    // .. .. ==> 0XE000E014[3:0] = 0x00000002U
    // .. ..     ==> MASK : 0x0000000FU    VAL : 0x00000002U
    // .. .. Set_t1 = 0x2
    // .. .. ==> 0XE000E014[7:4] = 0x00000002U
    // .. ..     ==> MASK : 0x000000F0U    VAL : 0x00000020U
    // .. .. Set_t2 = 0x1
    // .. .. ==> 0XE000E014[10:8] = 0x00000001U
    // .. ..     ==> MASK : 0x00000700U    VAL : 0x00000100U
    // .. .. Set_t3 = 0x1
    // .. .. ==> 0XE000E014[13:11] = 0x00000001U
    // .. ..     ==> MASK : 0x00003800U    VAL : 0x00000800U
    // .. .. Set_t4 = 0x1
    // .. .. ==> 0XE000E014[16:14] = 0x00000001U
    // .. ..     ==> MASK : 0x0001C000U    VAL : 0x00004000U
    // .. .. Set_t5 = 0x1
    // .. .. ==> 0XE000E014[19:17] = 0x00000001U
    // .. ..     ==> MASK : 0x000E0000U    VAL : 0x00020000U
    // .. .. Set_t6 = 0x1
    // .. .. ==> 0XE000E014[23:20] = 0x00000001U
    // .. ..     ==> MASK : 0x00F00000U    VAL : 0x00100000U
    // .. ..
    EMIT_WRITE(0XE000E014, 0x00124922U),
    // .. .. FINISH: NAND SET CYCLE
    // .. .. START: OPMODE
    // .. .. set_mw = 0x0
    // .. .. ==> 0XE000E018[1:0] = 0x00000000U
    // .. ..     ==> MASK : 0x00000003U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XE000E018, 0x00000003U ,0x00000000U),
    // .. .. FINISH: OPMODE
    // .. .. START: DIRECT COMMAND
    // .. .. chip_select = 0x4
    // .. .. ==> 0XE000E010[25:23] = 0x00000004U
    // .. ..     ==> MASK : 0x03800000U    VAL : 0x02000000U
    // .. .. cmd_type = 0x2
    // .. .. ==> 0XE000E010[22:21] = 0x00000002U
    // .. ..     ==> MASK : 0x00600000U    VAL : 0x00400000U
    // .. ..
    EMIT_WRITE(0XE000E010, 0x02400000U),
    // .. .. FINISH: DIRECT COMMAND
    // .. .. START: SRAM/NOR CS0 SET CYCLE
    // .. .. FINISH: SRAM/NOR CS0 SET CYCLE
    // .. .. START: DIRECT COMMAND
    // .. .. FINISH: DIRECT COMMAND
    // .. .. START: NOR CS0 BASE ADDRESS
    // .. .. FINISH: NOR CS0 BASE ADDRESS
    // .. .. START: SRAM/NOR CS1 SET CYCLE
    // .. .. FINISH: SRAM/NOR CS1 SET CYCLE
    // .. .. START: DIRECT COMMAND
    // .. .. FINISH: DIRECT COMMAND
    // .. .. START: NOR CS1 BASE ADDRESS
    // .. .. FINISH: NOR CS1 BASE ADDRESS
    // .. .. START: USB RESET
    // .. .. FINISH: USB RESET
    // .. .. START: ENET RESET
    // .. .. FINISH: ENET RESET
    // .. .. START: I2C RESET
    // .. .. FINISH: I2C RESET
    // .. .. START: NOR CHIP SELECT
    // .. .. .. START: DIR MODE BANK 0
    // .. .. .. FINISH: DIR MODE BANK 0
    // .. .. .. START: MASK_DATA_0_LSW HIGH BANK [15:0]
    // .. .. .. FINISH: MASK_DATA_0_LSW HIGH BANK [15:0]
    // .. .. .. START: OUTPUT ENABLE BANK 0
    // .. .. .. FINISH: OUTPUT ENABLE BANK 0
    // .. .. FINISH: NOR CHIP SELECT
    // .. FINISH: SMC TIMING CALCULATION REGISTER UPDATE
    // FINISH: top
    //
    EMIT_EXIT(),

    //
};

unsigned long ps7_post_config_2_0[] = {
    // START: top
    // .. START: SLCR SETTINGS
    // .. UNLOCK_KEY = 0XDF0D
    // .. ==> 0XF8000008[15:0] = 0x0000DF0DU
    // ..     ==> MASK : 0x0000FFFFU    VAL : 0x0000DF0DU
    // ..
    EMIT_WRITE(0XF8000008, 0x0000DF0DU),
    // .. FINISH: SLCR SETTINGS
    // .. START: ENABLING LEVEL SHIFTER
    // .. USER_INP_ICT_EN_0 = 3
    // .. ==> 0XF8000900[1:0] = 0x00000003U
    // ..     ==> MASK : 0x00000003U    VAL : 0x00000003U
    // .. USER_INP_ICT_EN_1 = 3
    // .. ==> 0XF8000900[3:2] = 0x00000003U
    // ..     ==> MASK : 0x0000000CU    VAL : 0x0000000CU
    // ..
    EMIT_MASKWRITE(0XF8000900, 0x0000000FU ,0x0000000FU),
    // .. FINISH: ENABLING LEVEL SHIFTER
    // .. START: FPGA RESETS TO 0
    // .. reserved_3 = 0
    // .. ==> 0XF8000240[31:25] = 0x00000000U
    // ..     ==> MASK : 0xFE000000U    VAL : 0x00000000U
    // .. FPGA_ACP_RST = 0
    // .. ==> 0XF8000240[24:24] = 0x00000000U
    // ..     ==> MASK : 0x01000000U    VAL : 0x00000000U
    // .. FPGA_AXDS3_RST = 0
    // .. ==> 0XF8000240[23:23] = 0x00000000U
    // ..     ==> MASK : 0x00800000U    VAL : 0x00000000U
    // .. FPGA_AXDS2_RST = 0
    // .. ==> 0XF8000240[22:22] = 0x00000000U
    // ..     ==> MASK : 0x00400000U    VAL : 0x00000000U
    // .. FPGA_AXDS1_RST = 0
    // .. ==> 0XF8000240[21:21] = 0x00000000U
    // ..     ==> MASK : 0x00200000U    VAL : 0x00000000U
    // .. FPGA_AXDS0_RST = 0
    // .. ==> 0XF8000240[20:20] = 0x00000000U
    // ..     ==> MASK : 0x00100000U    VAL : 0x00000000U
    // .. reserved_2 = 0
    // .. ==> 0XF8000240[19:18] = 0x00000000U
    // ..     ==> MASK : 0x000C0000U    VAL : 0x00000000U
    // .. FSSW1_FPGA_RST = 0
    // .. ==> 0XF8000240[17:17] = 0x00000000U
    // ..     ==> MASK : 0x00020000U    VAL : 0x00000000U
    // .. FSSW0_FPGA_RST = 0
    // .. ==> 0XF8000240[16:16] = 0x00000000U
    // ..     ==> MASK : 0x00010000U    VAL : 0x00000000U
    // .. reserved_1 = 0
    // .. ==> 0XF8000240[15:14] = 0x00000000U
    // ..     ==> MASK : 0x0000C000U    VAL : 0x00000000U
    // .. FPGA_FMSW1_RST = 0
    // .. ==> 0XF8000240[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // .. FPGA_FMSW0_RST = 0
    // .. ==> 0XF8000240[12:12] = 0x00000000U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00000000U
    // .. FPGA_DMA3_RST = 0
    // .. ==> 0XF8000240[11:11] = 0x00000000U
    // ..     ==> MASK : 0x00000800U    VAL : 0x00000000U
    // .. FPGA_DMA2_RST = 0
    // .. ==> 0XF8000240[10:10] = 0x00000000U
    // ..     ==> MASK : 0x00000400U    VAL : 0x00000000U
    // .. FPGA_DMA1_RST = 0
    // .. ==> 0XF8000240[9:9] = 0x00000000U
    // ..     ==> MASK : 0x00000200U    VAL : 0x00000000U
    // .. FPGA_DMA0_RST = 0
    // .. ==> 0XF8000240[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. reserved = 0
    // .. ==> 0XF8000240[7:4] = 0x00000000U
    // ..     ==> MASK : 0x000000F0U    VAL : 0x00000000U
    // .. FPGA3_OUT_RST = 0
    // .. ==> 0XF8000240[3:3] = 0x00000000U
    // ..     ==> MASK : 0x00000008U    VAL : 0x00000000U
    // .. FPGA2_OUT_RST = 0
    // .. ==> 0XF8000240[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. FPGA1_OUT_RST = 0
    // .. ==> 0XF8000240[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. FPGA0_OUT_RST = 0
    // .. ==> 0XF8000240[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF8000240, 0xFFFFFFFFU ,0x00000000U),
    // .. FINISH: FPGA RESETS TO 0
    // .. START: AFI REGISTERS
    // .. .. START: AFI0 REGISTERS
    // .. .. FINISH: AFI0 REGISTERS
    // .. .. START: AFI1 REGISTERS
    // .. .. FINISH: AFI1 REGISTERS
    // .. .. START: AFI2 REGISTERS
    // .. .. FINISH: AFI2 REGISTERS
    // .. .. START: AFI3 REGISTERS
    // .. .. FINISH: AFI3 REGISTERS
    // .. FINISH: AFI REGISTERS
    // .. START: LOCK IT BACK
    // .. LOCK_KEY = 0X767B
    // .. ==> 0XF8000004[15:0] = 0x0000767BU
    // ..     ==> MASK : 0x0000FFFFU    VAL : 0x0000767BU
    // ..
    //EMIT_WRITE(0XF8000004, 0x0000767BU),
    // .. FINISH: LOCK IT BACK
    // FINISH: top
    //
    EMIT_EXIT(),

    //
};

unsigned long ps7_debug_2_0[] = {
    // START: top
    // .. START: CROSS TRIGGER CONFIGURATIONS
    // .. .. START: UNLOCKING CTI REGISTERS
    // .. .. KEY = 0XC5ACCE55
    // .. .. ==> 0XF8898FB0[31:0] = 0xC5ACCE55U
    // .. ..     ==> MASK : 0xFFFFFFFFU    VAL : 0xC5ACCE55U
    // .. ..
    EMIT_WRITE(0XF8898FB0, 0xC5ACCE55U),
    // .. .. KEY = 0XC5ACCE55
    // .. .. ==> 0XF8899FB0[31:0] = 0xC5ACCE55U
    // .. ..     ==> MASK : 0xFFFFFFFFU    VAL : 0xC5ACCE55U
    // .. ..
    EMIT_WRITE(0XF8899FB0, 0xC5ACCE55U),
    // .. .. KEY = 0XC5ACCE55
    // .. .. ==> 0XF8809FB0[31:0] = 0xC5ACCE55U
    // .. ..     ==> MASK : 0xFFFFFFFFU    VAL : 0xC5ACCE55U
    // .. ..
    EMIT_WRITE(0XF8809FB0, 0xC5ACCE55U),
    // .. .. FINISH: UNLOCKING CTI REGISTERS
    // .. .. START: ENABLING CTI MODULES AND CHANNELS
    // .. .. FINISH: ENABLING CTI MODULES AND CHANNELS
    // .. .. START: MAPPING CPU0, CPU1 AND FTM EVENTS TO CTM CHANNELS
    // .. .. FINISH: MAPPING CPU0, CPU1 AND FTM EVENTS TO CTM CHANNELS
    // .. FINISH: CROSS TRIGGER CONFIGURATIONS
    // FINISH: top
    //
    EMIT_EXIT(),

    //
};

unsigned long ps7_pll_init_data_1_0[] = {
    // START: top
    // .. START: SLCR SETTINGS
    // .. UNLOCK_KEY = 0XDF0D
    // .. ==> 0XF8000008[15:0] = 0x0000DF0DU
    // ..     ==> MASK : 0x0000FFFFU    VAL : 0x0000DF0DU
    // ..
    EMIT_WRITE(0XF8000008, 0x0000DF0DU),
    // .. FINISH: SLCR SETTINGS
    // .. START: PLL SLCR REGISTERS
    // .. .. START: ARM PLL INIT
    // .. .. PLL_RES = 0x2
    // .. .. ==> 0XF8000110[7:4] = 0x00000002U
    // .. ..     ==> MASK : 0x000000F0U    VAL : 0x00000020U
    // .. .. PLL_CP = 0x2
    // .. .. ==> 0XF8000110[11:8] = 0x00000002U
    // .. ..     ==> MASK : 0x00000F00U    VAL : 0x00000200U
    // .. .. LOCK_CNT = 0xfa
    // .. .. ==> 0XF8000110[21:12] = 0x000000FAU
    // .. ..     ==> MASK : 0x003FF000U    VAL : 0x000FA000U
    // .. ..
    EMIT_MASKWRITE(0XF8000110, 0x003FFFF0U ,0x000FA220U),
    // .. .. .. START: UPDATE FB_DIV
    // .. .. .. PLL_FDIV = 0x28
    // .. .. .. ==> 0XF8000100[18:12] = 0x00000028U
    // .. .. ..     ==> MASK : 0x0007F000U    VAL : 0x00028000U
    // .. .. ..
    EMIT_MASKWRITE(0XF8000100, 0x0007F000U ,0x00028000U),
    // .. .. .. FINISH: UPDATE FB_DIV
    // .. .. .. START: BY PASS PLL
    // .. .. .. PLL_BYPASS_FORCE = 1
    // .. .. .. ==> 0XF8000100[4:4] = 0x00000001U
    // .. .. ..     ==> MASK : 0x00000010U    VAL : 0x00000010U
    // .. .. ..
    EMIT_MASKWRITE(0XF8000100, 0x00000010U ,0x00000010U),
    // .. .. .. FINISH: BY PASS PLL
    // .. .. .. START: ASSERT RESET
    // .. .. .. PLL_RESET = 1
    // .. .. .. ==> 0XF8000100[0:0] = 0x00000001U
    // .. .. ..     ==> MASK : 0x00000001U    VAL : 0x00000001U
    // .. .. ..
    EMIT_MASKWRITE(0XF8000100, 0x00000001U ,0x00000001U),
    // .. .. .. FINISH: ASSERT RESET
    // .. .. .. START: DEASSERT RESET
    // .. .. .. PLL_RESET = 0
    // .. .. .. ==> 0XF8000100[0:0] = 0x00000000U
    // .. .. ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. .. ..
    EMIT_MASKWRITE(0XF8000100, 0x00000001U ,0x00000000U),
    // .. .. .. FINISH: DEASSERT RESET
    // .. .. .. START: CHECK PLL STATUS
    // .. .. .. ARM_PLL_LOCK = 1
    // .. .. .. ==> 0XF800010C[0:0] = 0x00000001U
    // .. .. ..     ==> MASK : 0x00000001U    VAL : 0x00000001U
    // .. .. ..
    EMIT_MASKPOLL(0XF800010C, 0x00000001U),
    // .. .. .. FINISH: CHECK PLL STATUS
    // .. .. .. START: REMOVE PLL BY PASS
    // .. .. .. PLL_BYPASS_FORCE = 0
    // .. .. .. ==> 0XF8000100[4:4] = 0x00000000U
    // .. .. ..     ==> MASK : 0x00000010U    VAL : 0x00000000U
    // .. .. ..
    EMIT_MASKWRITE(0XF8000100, 0x00000010U ,0x00000000U),
    // .. .. .. FINISH: REMOVE PLL BY PASS
    // .. .. .. SRCSEL = 0x0
    // .. .. .. ==> 0XF8000120[5:4] = 0x00000000U
    // .. .. ..     ==> MASK : 0x00000030U    VAL : 0x00000000U
    // .. .. .. DIVISOR = 0x2
    // .. .. .. ==> 0XF8000120[13:8] = 0x00000002U
    // .. .. ..     ==> MASK : 0x00003F00U    VAL : 0x00000200U
    // .. .. .. CPU_6OR4XCLKACT = 0x1
    // .. .. .. ==> 0XF8000120[24:24] = 0x00000001U
    // .. .. ..     ==> MASK : 0x01000000U    VAL : 0x01000000U
    // .. .. .. CPU_3OR2XCLKACT = 0x1
    // .. .. .. ==> 0XF8000120[25:25] = 0x00000001U
    // .. .. ..     ==> MASK : 0x02000000U    VAL : 0x02000000U
    // .. .. .. CPU_2XCLKACT = 0x1
    // .. .. .. ==> 0XF8000120[26:26] = 0x00000001U
    // .. .. ..     ==> MASK : 0x04000000U    VAL : 0x04000000U
    // .. .. .. CPU_1XCLKACT = 0x1
    // .. .. .. ==> 0XF8000120[27:27] = 0x00000001U
    // .. .. ..     ==> MASK : 0x08000000U    VAL : 0x08000000U
    // .. .. .. CPU_PERI_CLKACT = 0x1
    // .. .. .. ==> 0XF8000120[28:28] = 0x00000001U
    // .. .. ..     ==> MASK : 0x10000000U    VAL : 0x10000000U
    // .. .. ..
    ////EMIT_MASKWRITE(0XF8000120, 0x1F003F30U ,0x1F000200U),
    // .. .. FINISH: ARM PLL INIT
    // .. .. START: DDR PLL INIT
    // .. .. PLL_RES = 0x2
    // .. .. ==> 0XF8000114[7:4] = 0x00000002U
    // .. ..     ==> MASK : 0x000000F0U    VAL : 0x00000020U
    // .. .. PLL_CP = 0x2
    // .. .. ==> 0XF8000114[11:8] = 0x00000002U
    // .. ..     ==> MASK : 0x00000F00U    VAL : 0x00000200U
    // .. .. LOCK_CNT = 0x12c
    // .. .. ==> 0XF8000114[21:12] = 0x0000012CU
    // .. ..     ==> MASK : 0x003FF000U    VAL : 0x0012C000U
    // .. ..
    EMIT_MASKWRITE(0XF8000114, 0x003FFFF0U ,0x0012C220U),
    // .. .. .. START: UPDATE FB_DIV
    // .. .. .. PLL_FDIV = 0x20
    // .. .. .. ==> 0XF8000104[18:12] = 0x00000020U
    // .. .. ..     ==> MASK : 0x0007F000U    VAL : 0x00020000U
    // .. .. ..
    EMIT_MASKWRITE(0XF8000104, 0x0007F000U ,0x00020000U),
    // .. .. .. FINISH: UPDATE FB_DIV
    // .. .. .. START: BY PASS PLL
    // .. .. .. PLL_BYPASS_FORCE = 1
    // .. .. .. ==> 0XF8000104[4:4] = 0x00000001U
    // .. .. ..     ==> MASK : 0x00000010U    VAL : 0x00000010U
    // .. .. ..
    EMIT_MASKWRITE(0XF8000104, 0x00000010U ,0x00000010U),
    // .. .. .. FINISH: BY PASS PLL
    // .. .. .. START: ASSERT RESET
    // .. .. .. PLL_RESET = 1
    // .. .. .. ==> 0XF8000104[0:0] = 0x00000001U
    // .. .. ..     ==> MASK : 0x00000001U    VAL : 0x00000001U
    // .. .. ..
    EMIT_MASKWRITE(0XF8000104, 0x00000001U ,0x00000001U),
    // .. .. .. FINISH: ASSERT RESET
    // .. .. .. START: DEASSERT RESET
    // .. .. .. PLL_RESET = 0
    // .. .. .. ==> 0XF8000104[0:0] = 0x00000000U
    // .. .. ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. .. ..
    EMIT_MASKWRITE(0XF8000104, 0x00000001U ,0x00000000U),
    // .. .. .. FINISH: DEASSERT RESET
    // .. .. .. START: CHECK PLL STATUS
    // .. .. .. DDR_PLL_LOCK = 1
    // .. .. .. ==> 0XF800010C[1:1] = 0x00000001U
    // .. .. ..     ==> MASK : 0x00000002U    VAL : 0x00000002U
    // .. .. ..
    EMIT_MASKPOLL(0XF800010C, 0x00000002U),
    // .. .. .. FINISH: CHECK PLL STATUS
    // .. .. .. START: REMOVE PLL BY PASS
    // .. .. .. PLL_BYPASS_FORCE = 0
    // .. .. .. ==> 0XF8000104[4:4] = 0x00000000U
    // .. .. ..     ==> MASK : 0x00000010U    VAL : 0x00000000U
    // .. .. ..
    EMIT_MASKWRITE(0XF8000104, 0x00000010U ,0x00000000U),
    // .. .. .. FINISH: REMOVE PLL BY PASS
    // .. .. .. DDR_3XCLKACT = 0x1
    // .. .. .. ==> 0XF8000124[0:0] = 0x00000001U
    // .. .. ..     ==> MASK : 0x00000001U    VAL : 0x00000001U
    // .. .. .. DDR_2XCLKACT = 0x1
    // .. .. .. ==> 0XF8000124[1:1] = 0x00000001U
    // .. .. ..     ==> MASK : 0x00000002U    VAL : 0x00000002U
    // .. .. .. DDR_3XCLK_DIVISOR = 0x2
    // .. .. .. ==> 0XF8000124[25:20] = 0x00000002U
    // .. .. ..     ==> MASK : 0x03F00000U    VAL : 0x00200000U
    // .. .. .. DDR_2XCLK_DIVISOR = 0x3
    // .. .. .. ==> 0XF8000124[31:26] = 0x00000003U
    // .. .. ..     ==> MASK : 0xFC000000U    VAL : 0x0C000000U
    // .. .. ..
    EMIT_MASKWRITE(0XF8000124, 0xFFF00003U ,0x0C200003U),
    // .. .. FINISH: DDR PLL INIT
    // .. .. START: IO PLL INIT
    // .. .. PLL_RES = 0x4
    // .. .. ==> 0XF8000118[7:4] = 0x00000004U
    // .. ..     ==> MASK : 0x000000F0U    VAL : 0x00000040U
    // .. .. PLL_CP = 0x2
    // .. .. ==> 0XF8000118[11:8] = 0x00000002U
    // .. ..     ==> MASK : 0x00000F00U    VAL : 0x00000200U
    // .. .. LOCK_CNT = 0xfa
    // .. .. ==> 0XF8000118[21:12] = 0x000000FAU
    // .. ..     ==> MASK : 0x003FF000U    VAL : 0x000FA000U
    // .. ..
    EMIT_MASKWRITE(0XF8000118, 0x003FFFF0U ,0x000FA240U),
    // .. .. .. START: UPDATE FB_DIV
    // .. .. .. PLL_FDIV = 0x30
    // .. .. .. ==> 0XF8000108[18:12] = 0x00000030U
    // .. .. ..     ==> MASK : 0x0007F000U    VAL : 0x00030000U
    // .. .. ..
    EMIT_MASKWRITE(0XF8000108, 0x0007F000U ,0x00030000U),
    // .. .. .. FINISH: UPDATE FB_DIV
    // .. .. .. START: BY PASS PLL
    // .. .. .. PLL_BYPASS_FORCE = 1
    // .. .. .. ==> 0XF8000108[4:4] = 0x00000001U
    // .. .. ..     ==> MASK : 0x00000010U    VAL : 0x00000010U
    // .. .. ..
    EMIT_MASKWRITE(0XF8000108, 0x00000010U ,0x00000010U),
    // .. .. .. FINISH: BY PASS PLL
    // .. .. .. START: ASSERT RESET
    // .. .. .. PLL_RESET = 1
    // .. .. .. ==> 0XF8000108[0:0] = 0x00000001U
    // .. .. ..     ==> MASK : 0x00000001U    VAL : 0x00000001U
    // .. .. ..
    EMIT_MASKWRITE(0XF8000108, 0x00000001U ,0x00000001U),
    // .. .. .. FINISH: ASSERT RESET
    // .. .. .. START: DEASSERT RESET
    // .. .. .. PLL_RESET = 0
    // .. .. .. ==> 0XF8000108[0:0] = 0x00000000U
    // .. .. ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. .. ..
    EMIT_MASKWRITE(0XF8000108, 0x00000001U ,0x00000000U),
    // .. .. .. FINISH: DEASSERT RESET
    // .. .. .. START: CHECK PLL STATUS
    // .. .. .. IO_PLL_LOCK = 1
    // .. .. .. ==> 0XF800010C[2:2] = 0x00000001U
    // .. .. ..     ==> MASK : 0x00000004U    VAL : 0x00000004U
    // .. .. ..
    EMIT_MASKPOLL(0XF800010C, 0x00000004U),
    // .. .. .. FINISH: CHECK PLL STATUS
    // .. .. .. START: REMOVE PLL BY PASS
    // .. .. .. PLL_BYPASS_FORCE = 0
    // .. .. .. ==> 0XF8000108[4:4] = 0x00000000U
    // .. .. ..     ==> MASK : 0x00000010U    VAL : 0x00000000U
    // .. .. ..
    EMIT_MASKWRITE(0XF8000108, 0x00000010U ,0x00000000U),
    // .. .. .. FINISH: REMOVE PLL BY PASS
    // .. .. FINISH: IO PLL INIT
    // .. FINISH: PLL SLCR REGISTERS
    // .. START: LOCK IT BACK
    // .. LOCK_KEY = 0X767B
    // .. ==> 0XF8000004[15:0] = 0x0000767BU
    // ..     ==> MASK : 0x0000FFFFU    VAL : 0x0000767BU
    // ..
    //EMIT_WRITE(0XF8000004, 0x0000767BU),
    // .. FINISH: LOCK IT BACK
    // FINISH: top
    //
    EMIT_EXIT(),

    //
};

unsigned long ps7_clock_init_data_1_0[] = {
    // START: top
    // .. START: SLCR SETTINGS
    // .. UNLOCK_KEY = 0XDF0D
    // .. ==> 0XF8000008[15:0] = 0x0000DF0DU
    // ..     ==> MASK : 0x0000FFFFU    VAL : 0x0000DF0DU
    // ..
    EMIT_WRITE(0XF8000008, 0x0000DF0DU),
    // .. FINISH: SLCR SETTINGS
    // .. START: CLOCK CONTROL SLCR REGISTERS
    // .. CLKACT = 0x1
    // .. ==> 0XF8000128[0:0] = 0x00000001U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000001U
    // .. DIVISOR0 = 0xf
    // .. ==> 0XF8000128[13:8] = 0x0000000FU
    // ..     ==> MASK : 0x00003F00U    VAL : 0x00000F00U
    // .. DIVISOR1 = 0x7
    // .. ==> 0XF8000128[25:20] = 0x00000007U
    // ..     ==> MASK : 0x03F00000U    VAL : 0x00700000U
    // ..
    EMIT_MASKWRITE(0XF8000128, 0x03F03F01U ,0x00700F01U),	// DCI_CLK_CTRL
    // .. CLKACT = 0x1
    // .. ==> 0XF8000148[0:0] = 0x00000001U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000001U
    // .. SRCSEL = 0x0
    // .. ==> 0XF8000148[5:4] = 0x00000000U
    // ..     ==> MASK : 0x00000030U    VAL : 0x00000000U
    // .. DIVISOR = 0x10
    // .. ==> 0XF8000148[13:8] = 0x00000010U
    // ..     ==> MASK : 0x00003F00U    VAL : 0x00001000U
    // ..
    EMIT_MASKWRITE(0XF8000148, 0x00003F31U ,0x00001001U),	// SMC_CLK_CTRL
    // .. CLKACT0 = 0x0
    // .. ==> 0XF8000154[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. CLKACT1 = 0x1
    // .. ==> 0XF8000154[1:1] = 0x00000001U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000002U
    // .. SRCSEL = 0x0
    // .. ==> 0XF8000154[5:4] = 0x00000000U
    // ..     ==> MASK : 0x00000030U    VAL : 0x00000000U
    // .. DIVISOR = 0x10
    // .. ==> 0XF8000154[13:8] = 0x00000010U
    // ..     ==> MASK : 0x00003F00U    VAL : 0x00001000U
    // ..
   // EMIT_MASKWRITE(0XF8000154, 0x00003F33U ,0x00001002U),	// UART_CLK_CTRL
    // .. .. START: TRACE CLOCK
    // .. .. FINISH: TRACE CLOCK
    // .. .. CLKACT = 0x1
    // .. .. ==> 0XF8000168[0:0] = 0x00000001U
    // .. ..     ==> MASK : 0x00000001U    VAL : 0x00000001U
    // .. .. SRCSEL = 0x0
    // .. .. ==> 0XF8000168[5:4] = 0x00000000U
    // .. ..     ==> MASK : 0x00000030U    VAL : 0x00000000U
    // .. .. DIVISOR = 0x8
    // .. .. ==> 0XF8000168[13:8] = 0x00000008U
    // .. ..     ==> MASK : 0x00003F00U    VAL : 0x00000800U
    // .. ..
    EMIT_MASKWRITE(0XF8000168, 0x00003F31U ,0x00000801U),
    // .. .. SRCSEL = 0x0
    // .. .. ==> 0XF8000170[5:4] = 0x00000000U
    // .. ..     ==> MASK : 0x00000030U    VAL : 0x00000000U
    // .. .. DIVISOR0 = 0x8
    // .. .. ==> 0XF8000170[13:8] = 0x00000008U
    // .. ..     ==> MASK : 0x00003F00U    VAL : 0x00000800U
    // .. .. DIVISOR1 = 0x4
    // .. .. ==> 0XF8000170[25:20] = 0x00000004U
    // .. ..     ==> MASK : 0x03F00000U    VAL : 0x00400000U
    // .. ..
    //EMIT_MASKWRITE(0XF8000170, 0x03F03F30U ,0x00400800U),	// FPGA0_CLK_CTRL PL Clock 0 Output control
    // .. .. CLK_621_TRUE = 0x1
    // .. .. ==> 0XF80001C4[0:0] = 0x00000001U
    // .. ..     ==> MASK : 0x00000001U    VAL : 0x00000001U
    // .. ..
    //EMIT_MASKWRITE(0XF80001C4, 0x00000001U ,0x00000001U),	// CLK_621_TRUE CPU Clock Ratio Mode select
    // .. .. DMA_CPU_2XCLKACT = 0x1
    // .. .. ==> 0XF800012C[0:0] = 0x00000001U
    // .. ..     ==> MASK : 0x00000001U    VAL : 0x00000001U
    // .. .. USB0_CPU_1XCLKACT = 0x1
    // .. .. ==> 0XF800012C[2:2] = 0x00000001U
    // .. ..     ==> MASK : 0x00000004U    VAL : 0x00000004U
    // .. .. USB1_CPU_1XCLKACT = 0x1
    // .. .. ==> 0XF800012C[3:3] = 0x00000001U
    // .. ..     ==> MASK : 0x00000008U    VAL : 0x00000008U
    // .. .. GEM0_CPU_1XCLKACT = 0x0
    // .. .. ==> 0XF800012C[6:6] = 0x00000000U
    // .. ..     ==> MASK : 0x00000040U    VAL : 0x00000000U
    // .. .. GEM1_CPU_1XCLKACT = 0x0
    // .. .. ==> 0XF800012C[7:7] = 0x00000000U
    // .. ..     ==> MASK : 0x00000080U    VAL : 0x00000000U
    // .. .. SDI0_CPU_1XCLKACT = 0x0
    // .. .. ==> 0XF800012C[10:10] = 0x00000000U
    // .. ..     ==> MASK : 0x00000400U    VAL : 0x00000000U
    // .. .. SDI1_CPU_1XCLKACT = 0x0
    // .. .. ==> 0XF800012C[11:11] = 0x00000000U
    // .. ..     ==> MASK : 0x00000800U    VAL : 0x00000000U
    // .. .. SPI0_CPU_1XCLKACT = 0x0
    // .. .. ==> 0XF800012C[14:14] = 0x00000000U
    // .. ..     ==> MASK : 0x00004000U    VAL : 0x00000000U
    // .. .. SPI1_CPU_1XCLKACT = 0x0
    // .. .. ==> 0XF800012C[15:15] = 0x00000000U
    // .. ..     ==> MASK : 0x00008000U    VAL : 0x00000000U
    // .. .. CAN0_CPU_1XCLKACT = 0x0
    // .. .. ==> 0XF800012C[16:16] = 0x00000000U
    // .. ..     ==> MASK : 0x00010000U    VAL : 0x00000000U
    // .. .. CAN1_CPU_1XCLKACT = 0x0
    // .. .. ==> 0XF800012C[17:17] = 0x00000000U
    // .. ..     ==> MASK : 0x00020000U    VAL : 0x00000000U
    // .. .. I2C0_CPU_1XCLKACT = 0x1
    // .. .. ==> 0XF800012C[18:18] = 0x00000001U
    // .. ..     ==> MASK : 0x00040000U    VAL : 0x00040000U
    // .. .. I2C1_CPU_1XCLKACT = 0x1
    // .. .. ==> 0XF800012C[19:19] = 0x00000001U
    // .. ..     ==> MASK : 0x00080000U    VAL : 0x00080000U
    // .. .. UART0_CPU_1XCLKACT = 0x0
    // .. .. ==> 0XF800012C[20:20] = 0x00000000U
    // .. ..     ==> MASK : 0x00100000U    VAL : 0x00000000U
    // .. .. UART1_CPU_1XCLKACT = 0x1
    // .. .. ==> 0XF800012C[21:21] = 0x00000001U
    // .. ..     ==> MASK : 0x00200000U    VAL : 0x00200000U
    // .. .. GPIO_CPU_1XCLKACT = 0x1
    // .. .. ==> 0XF800012C[22:22] = 0x00000001U
    // .. ..     ==> MASK : 0x00400000U    VAL : 0x00400000U
    // .. .. LQSPI_CPU_1XCLKACT = 0x0
    // .. .. ==> 0XF800012C[23:23] = 0x00000000U
    // .. ..     ==> MASK : 0x00800000U    VAL : 0x00000000U
    // .. .. SMC_CPU_1XCLKACT = 0x1
    // .. .. ==> 0XF800012C[24:24] = 0x00000001U
    // .. ..     ==> MASK : 0x01000000U    VAL : 0x01000000U
    // .. ..
    //EMIT_MASKWRITE(0XF800012C, 0x01FFCCCDU ,0x016C000DU),	// APER_CLK_CTRL AMBA Peripheral Clock Control
    // .. FINISH: CLOCK CONTROL SLCR REGISTERS
    // .. START: THIS SHOULD BE BLANK
    // .. FINISH: THIS SHOULD BE BLANK
    // .. START: LOCK IT BACK
    // .. LOCK_KEY = 0X767B
    // .. ==> 0XF8000004[15:0] = 0x0000767BU
    // ..     ==> MASK : 0x0000FFFFU    VAL : 0x0000767BU
    // ..
    //EMIT_WRITE(0XF8000004, 0x0000767BU),
    // .. FINISH: LOCK IT BACK
    // FINISH: top
    //
    EMIT_EXIT(),

    //
};

unsigned long ps7_ddr_init_data_1_0[] = {
    // START: top
    // .. START: DDR INITIALIZATION
    // .. .. START: LOCK DDR
    // .. .. reg_ddrc_soft_rstb = 0
    // .. .. ==> 0XF8006000[0:0] = 0x00000000U
    // .. ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. .. reg_ddrc_powerdown_en = 0x0
    // .. .. ==> 0XF8006000[1:1] = 0x00000000U
    // .. ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. .. reg_ddrc_data_bus_width = 0x0
    // .. .. ==> 0XF8006000[3:2] = 0x00000000U
    // .. ..     ==> MASK : 0x0000000CU    VAL : 0x00000000U
    // .. .. reg_ddrc_burst8_refresh = 0x0
    // .. .. ==> 0XF8006000[6:4] = 0x00000000U
    // .. ..     ==> MASK : 0x00000070U    VAL : 0x00000000U
    // .. .. reg_ddrc_rdwr_idle_gap = 0x1
    // .. .. ==> 0XF8006000[13:7] = 0x00000001U
    // .. ..     ==> MASK : 0x00003F80U    VAL : 0x00000080U
    // .. .. reg_ddrc_dis_rd_bypass = 0x0
    // .. .. ==> 0XF8006000[14:14] = 0x00000000U
    // .. ..     ==> MASK : 0x00004000U    VAL : 0x00000000U
    // .. .. reg_ddrc_dis_act_bypass = 0x0
    // .. .. ==> 0XF8006000[15:15] = 0x00000000U
    // .. ..     ==> MASK : 0x00008000U    VAL : 0x00000000U
    // .. .. reg_ddrc_dis_auto_refresh = 0x0
    // .. .. ==> 0XF8006000[16:16] = 0x00000000U
    // .. ..     ==> MASK : 0x00010000U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006000, 0x0001FFFFU ,0x00000080U),
    // .. .. FINISH: LOCK DDR
    // .. .. reg_ddrc_t_rfc_nom_x32 = 0x82
    // .. .. ==> 0XF8006004[11:0] = 0x00000082U
    // .. ..     ==> MASK : 0x00000FFFU    VAL : 0x00000082U
    // .. .. reg_ddrc_active_ranks = 0x1
    // .. .. ==> 0XF8006004[13:12] = 0x00000001U
    // .. ..     ==> MASK : 0x00003000U    VAL : 0x00001000U
    // .. .. reg_ddrc_addrmap_cs_bit0 = 0x0
    // .. .. ==> 0XF8006004[18:14] = 0x00000000U
    // .. ..     ==> MASK : 0x0007C000U    VAL : 0x00000000U
    // .. .. reg_ddrc_wr_odt_block = 0x1
    // .. .. ==> 0XF8006004[20:19] = 0x00000001U
    // .. ..     ==> MASK : 0x00180000U    VAL : 0x00080000U
    // .. .. reg_ddrc_diff_rank_rd_2cycle_gap = 0x0
    // .. .. ==> 0XF8006004[21:21] = 0x00000000U
    // .. ..     ==> MASK : 0x00200000U    VAL : 0x00000000U
    // .. .. reg_ddrc_addrmap_cs_bit1 = 0x0
    // .. .. ==> 0XF8006004[26:22] = 0x00000000U
    // .. ..     ==> MASK : 0x07C00000U    VAL : 0x00000000U
    // .. .. reg_ddrc_addrmap_open_bank = 0x0
    // .. .. ==> 0XF8006004[27:27] = 0x00000000U
    // .. ..     ==> MASK : 0x08000000U    VAL : 0x00000000U
    // .. .. reg_ddrc_addrmap_4bank_ram = 0x0
    // .. .. ==> 0XF8006004[28:28] = 0x00000000U
    // .. ..     ==> MASK : 0x10000000U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006004, 0x1FFFFFFFU ,0x00081082U),
    // .. .. reg_ddrc_hpr_min_non_critical_x32 = 0xf
    // .. .. ==> 0XF8006008[10:0] = 0x0000000FU
    // .. ..     ==> MASK : 0x000007FFU    VAL : 0x0000000FU
    // .. .. reg_ddrc_hpr_max_starve_x32 = 0xf
    // .. .. ==> 0XF8006008[21:11] = 0x0000000FU
    // .. ..     ==> MASK : 0x003FF800U    VAL : 0x00007800U
    // .. .. reg_ddrc_hpr_xact_run_length = 0xf
    // .. .. ==> 0XF8006008[25:22] = 0x0000000FU
    // .. ..     ==> MASK : 0x03C00000U    VAL : 0x03C00000U
    // .. ..
    EMIT_MASKWRITE(0XF8006008, 0x03FFFFFFU ,0x03C0780FU),
    // .. .. reg_ddrc_lpr_min_non_critical_x32 = 0x1
    // .. .. ==> 0XF800600C[10:0] = 0x00000001U
    // .. ..     ==> MASK : 0x000007FFU    VAL : 0x00000001U
    // .. .. reg_ddrc_lpr_max_starve_x32 = 0x2
    // .. .. ==> 0XF800600C[21:11] = 0x00000002U
    // .. ..     ==> MASK : 0x003FF800U    VAL : 0x00001000U
    // .. .. reg_ddrc_lpr_xact_run_length = 0x8
    // .. .. ==> 0XF800600C[25:22] = 0x00000008U
    // .. ..     ==> MASK : 0x03C00000U    VAL : 0x02000000U
    // .. ..
    EMIT_MASKWRITE(0XF800600C, 0x03FFFFFFU ,0x02001001U),
    // .. .. reg_ddrc_w_min_non_critical_x32 = 0x1
    // .. .. ==> 0XF8006010[10:0] = 0x00000001U
    // .. ..     ==> MASK : 0x000007FFU    VAL : 0x00000001U
    // .. .. reg_ddrc_w_xact_run_length = 0x8
    // .. .. ==> 0XF8006010[14:11] = 0x00000008U
    // .. ..     ==> MASK : 0x00007800U    VAL : 0x00004000U
    // .. .. reg_ddrc_w_max_starve_x32 = 0x2
    // .. .. ==> 0XF8006010[25:15] = 0x00000002U
    // .. ..     ==> MASK : 0x03FF8000U    VAL : 0x00010000U
    // .. ..
    EMIT_MASKWRITE(0XF8006010, 0x03FFFFFFU ,0x00014001U),
    // .. .. reg_ddrc_t_rc = 0x1b
    // .. .. ==> 0XF8006014[5:0] = 0x0000001BU
    // .. ..     ==> MASK : 0x0000003FU    VAL : 0x0000001BU
    // .. .. reg_ddrc_t_rfc_min = 0x56
    // .. .. ==> 0XF8006014[13:6] = 0x00000056U
    // .. ..     ==> MASK : 0x00003FC0U    VAL : 0x00001580U
    // .. .. reg_ddrc_post_selfref_gap_x32 = 0x10
    // .. .. ==> 0XF8006014[20:14] = 0x00000010U
    // .. ..     ==> MASK : 0x001FC000U    VAL : 0x00040000U
    // .. ..
    EMIT_MASKWRITE(0XF8006014, 0x001FFFFFU ,0x0004159BU),
    // .. .. reg_ddrc_wr2pre = 0x13
    // .. .. ==> 0XF8006018[4:0] = 0x00000013U
    // .. ..     ==> MASK : 0x0000001FU    VAL : 0x00000013U
    // .. .. reg_ddrc_powerdown_to_x32 = 0x6
    // .. .. ==> 0XF8006018[9:5] = 0x00000006U
    // .. ..     ==> MASK : 0x000003E0U    VAL : 0x000000C0U
    // .. .. reg_ddrc_t_faw = 0x16
    // .. .. ==> 0XF8006018[15:10] = 0x00000016U
    // .. ..     ==> MASK : 0x0000FC00U    VAL : 0x00005800U
    // .. .. reg_ddrc_t_ras_max = 0x24
    // .. .. ==> 0XF8006018[21:16] = 0x00000024U
    // .. ..     ==> MASK : 0x003F0000U    VAL : 0x00240000U
    // .. .. reg_ddrc_t_ras_min = 0x13
    // .. .. ==> 0XF8006018[26:22] = 0x00000013U
    // .. ..     ==> MASK : 0x07C00000U    VAL : 0x04C00000U
    // .. .. reg_ddrc_t_cke = 0x4
    // .. .. ==> 0XF8006018[31:28] = 0x00000004U
    // .. ..     ==> MASK : 0xF0000000U    VAL : 0x40000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006018, 0xF7FFFFFFU ,0x44E458D3U),
    // .. .. reg_ddrc_write_latency = 0x5
    // .. .. ==> 0XF800601C[4:0] = 0x00000005U
    // .. ..     ==> MASK : 0x0000001FU    VAL : 0x00000005U
    // .. .. reg_ddrc_rd2wr = 0x7
    // .. .. ==> 0XF800601C[9:5] = 0x00000007U
    // .. ..     ==> MASK : 0x000003E0U    VAL : 0x000000E0U
    // .. .. reg_ddrc_wr2rd = 0xf
    // .. .. ==> 0XF800601C[14:10] = 0x0000000FU
    // .. ..     ==> MASK : 0x00007C00U    VAL : 0x00003C00U
    // .. .. reg_ddrc_t_xp = 0x5
    // .. .. ==> 0XF800601C[19:15] = 0x00000005U
    // .. ..     ==> MASK : 0x000F8000U    VAL : 0x00028000U
    // .. .. reg_ddrc_pad_pd = 0x0
    // .. .. ==> 0XF800601C[22:20] = 0x00000000U
    // .. ..     ==> MASK : 0x00700000U    VAL : 0x00000000U
    // .. .. reg_ddrc_rd2pre = 0x5
    // .. .. ==> 0XF800601C[27:23] = 0x00000005U
    // .. ..     ==> MASK : 0x0F800000U    VAL : 0x02800000U
    // .. .. reg_ddrc_t_rcd = 0x7
    // .. .. ==> 0XF800601C[31:28] = 0x00000007U
    // .. ..     ==> MASK : 0xF0000000U    VAL : 0x70000000U
    // .. ..
    EMIT_MASKWRITE(0XF800601C, 0xFFFFFFFFU ,0x7282BCE5U),
    // .. .. reg_ddrc_t_ccd = 0x4
    // .. .. ==> 0XF8006020[4:2] = 0x00000004U
    // .. ..     ==> MASK : 0x0000001CU    VAL : 0x00000010U
    // .. .. reg_ddrc_t_rrd = 0x6
    // .. .. ==> 0XF8006020[7:5] = 0x00000006U
    // .. ..     ==> MASK : 0x000000E0U    VAL : 0x000000C0U
    // .. .. reg_ddrc_refresh_margin = 0x2
    // .. .. ==> 0XF8006020[11:8] = 0x00000002U
    // .. ..     ==> MASK : 0x00000F00U    VAL : 0x00000200U
    // .. .. reg_ddrc_t_rp = 0x7
    // .. .. ==> 0XF8006020[15:12] = 0x00000007U
    // .. ..     ==> MASK : 0x0000F000U    VAL : 0x00007000U
    // .. .. reg_ddrc_refresh_to_x32 = 0x8
    // .. .. ==> 0XF8006020[20:16] = 0x00000008U
    // .. ..     ==> MASK : 0x001F0000U    VAL : 0x00080000U
    // .. .. reg_ddrc_sdram = 0x1
    // .. .. ==> 0XF8006020[21:21] = 0x00000001U
    // .. ..     ==> MASK : 0x00200000U    VAL : 0x00200000U
    // .. .. reg_ddrc_mobile = 0x0
    // .. .. ==> 0XF8006020[22:22] = 0x00000000U
    // .. ..     ==> MASK : 0x00400000U    VAL : 0x00000000U
    // .. .. reg_ddrc_clock_stop_en = 0x0
    // .. .. ==> 0XF8006020[23:23] = 0x00000000U
    // .. ..     ==> MASK : 0x00800000U    VAL : 0x00000000U
    // .. .. reg_ddrc_read_latency = 0x7
    // .. .. ==> 0XF8006020[28:24] = 0x00000007U
    // .. ..     ==> MASK : 0x1F000000U    VAL : 0x07000000U
    // .. .. reg_phy_mode_ddr1_ddr2 = 0x1
    // .. .. ==> 0XF8006020[29:29] = 0x00000001U
    // .. ..     ==> MASK : 0x20000000U    VAL : 0x20000000U
    // .. .. reg_ddrc_dis_pad_pd = 0x0
    // .. .. ==> 0XF8006020[30:30] = 0x00000000U
    // .. ..     ==> MASK : 0x40000000U    VAL : 0x00000000U
    // .. .. reg_ddrc_loopback = 0x0
    // .. .. ==> 0XF8006020[31:31] = 0x00000000U
    // .. ..     ==> MASK : 0x80000000U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006020, 0xFFFFFFFCU ,0x272872D0U),
    // .. .. reg_ddrc_en_2t_timing_mode = 0x0
    // .. .. ==> 0XF8006024[0:0] = 0x00000000U
    // .. ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. .. reg_ddrc_prefer_write = 0x0
    // .. .. ==> 0XF8006024[1:1] = 0x00000000U
    // .. ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. .. reg_ddrc_max_rank_rd = 0xf
    // .. .. ==> 0XF8006024[5:2] = 0x0000000FU
    // .. ..     ==> MASK : 0x0000003CU    VAL : 0x0000003CU
    // .. .. reg_ddrc_mr_wr = 0x0
    // .. .. ==> 0XF8006024[6:6] = 0x00000000U
    // .. ..     ==> MASK : 0x00000040U    VAL : 0x00000000U
    // .. .. reg_ddrc_mr_addr = 0x0
    // .. .. ==> 0XF8006024[8:7] = 0x00000000U
    // .. ..     ==> MASK : 0x00000180U    VAL : 0x00000000U
    // .. .. reg_ddrc_mr_data = 0x0
    // .. .. ==> 0XF8006024[24:9] = 0x00000000U
    // .. ..     ==> MASK : 0x01FFFE00U    VAL : 0x00000000U
    // .. .. ddrc_reg_mr_wr_busy = 0x0
    // .. .. ==> 0XF8006024[25:25] = 0x00000000U
    // .. ..     ==> MASK : 0x02000000U    VAL : 0x00000000U
    // .. .. reg_ddrc_mr_type = 0x0
    // .. .. ==> 0XF8006024[26:26] = 0x00000000U
    // .. ..     ==> MASK : 0x04000000U    VAL : 0x00000000U
    // .. .. reg_ddrc_mr_rdata_valid = 0x0
    // .. .. ==> 0XF8006024[27:27] = 0x00000000U
    // .. ..     ==> MASK : 0x08000000U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006024, 0x0FFFFFFFU ,0x0000003CU),
    // .. .. reg_ddrc_final_wait_x32 = 0x7
    // .. .. ==> 0XF8006028[6:0] = 0x00000007U
    // .. ..     ==> MASK : 0x0000007FU    VAL : 0x00000007U
    // .. .. reg_ddrc_pre_ocd_x32 = 0x0
    // .. .. ==> 0XF8006028[10:7] = 0x00000000U
    // .. ..     ==> MASK : 0x00000780U    VAL : 0x00000000U
    // .. .. reg_ddrc_t_mrd = 0x4
    // .. .. ==> 0XF8006028[13:11] = 0x00000004U
    // .. ..     ==> MASK : 0x00003800U    VAL : 0x00002000U
    // .. ..
    EMIT_MASKWRITE(0XF8006028, 0x00003FFFU ,0x00002007U),
    // .. .. reg_ddrc_emr2 = 0x8
    // .. .. ==> 0XF800602C[15:0] = 0x00000008U
    // .. ..     ==> MASK : 0x0000FFFFU    VAL : 0x00000008U
    // .. .. reg_ddrc_emr3 = 0x0
    // .. .. ==> 0XF800602C[31:16] = 0x00000000U
    // .. ..     ==> MASK : 0xFFFF0000U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF800602C, 0xFFFFFFFFU ,0x00000008U),
    // .. .. reg_ddrc_mr = 0xb30
    // .. .. ==> 0XF8006030[15:0] = 0x00000B30U
    // .. ..     ==> MASK : 0x0000FFFFU    VAL : 0x00000B30U
    // .. .. reg_ddrc_emr = 0x4
    // .. .. ==> 0XF8006030[31:16] = 0x00000004U
    // .. ..     ==> MASK : 0xFFFF0000U    VAL : 0x00040000U
    // .. ..
    EMIT_MASKWRITE(0XF8006030, 0xFFFFFFFFU ,0x00040B30U),
    // .. .. reg_ddrc_burst_rdwr = 0x4
    // .. .. ==> 0XF8006034[3:0] = 0x00000004U
    // .. ..     ==> MASK : 0x0000000FU    VAL : 0x00000004U
    // .. .. reg_ddrc_pre_cke_x1024 = 0x16d
    // .. .. ==> 0XF8006034[13:4] = 0x0000016DU
    // .. ..     ==> MASK : 0x00003FF0U    VAL : 0x000016D0U
    // .. .. reg_ddrc_post_cke_x1024 = 0x1
    // .. .. ==> 0XF8006034[25:16] = 0x00000001U
    // .. ..     ==> MASK : 0x03FF0000U    VAL : 0x00010000U
    // .. .. reg_ddrc_burstchop = 0x0
    // .. .. ==> 0XF8006034[28:28] = 0x00000000U
    // .. ..     ==> MASK : 0x10000000U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006034, 0x13FF3FFFU ,0x000116D4U),
    // .. .. reg_ddrc_force_low_pri_n = 0x0
    // .. .. ==> 0XF8006038[0:0] = 0x00000000U
    // .. ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. .. reg_ddrc_dis_dq = 0x0
    // .. .. ==> 0XF8006038[1:1] = 0x00000000U
    // .. ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. .. reg_phy_debug_mode = 0x0
    // .. .. ==> 0XF8006038[6:6] = 0x00000000U
    // .. ..     ==> MASK : 0x00000040U    VAL : 0x00000000U
    // .. .. reg_phy_wr_level_start = 0x0
    // .. .. ==> 0XF8006038[7:7] = 0x00000000U
    // .. ..     ==> MASK : 0x00000080U    VAL : 0x00000000U
    // .. .. reg_phy_rd_level_start = 0x0
    // .. .. ==> 0XF8006038[8:8] = 0x00000000U
    // .. ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. .. reg_phy_dq0_wait_t = 0x0
    // .. .. ==> 0XF8006038[12:9] = 0x00000000U
    // .. ..     ==> MASK : 0x00001E00U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006038, 0x00001FC3U ,0x00000000U),
    // .. .. reg_ddrc_addrmap_bank_b0 = 0x7
    // .. .. ==> 0XF800603C[3:0] = 0x00000007U
    // .. ..     ==> MASK : 0x0000000FU    VAL : 0x00000007U
    // .. .. reg_ddrc_addrmap_bank_b1 = 0x7
    // .. .. ==> 0XF800603C[7:4] = 0x00000007U
    // .. ..     ==> MASK : 0x000000F0U    VAL : 0x00000070U
    // .. .. reg_ddrc_addrmap_bank_b2 = 0x7
    // .. .. ==> 0XF800603C[11:8] = 0x00000007U
    // .. ..     ==> MASK : 0x00000F00U    VAL : 0x00000700U
    // .. .. reg_ddrc_addrmap_col_b5 = 0x0
    // .. .. ==> 0XF800603C[15:12] = 0x00000000U
    // .. ..     ==> MASK : 0x0000F000U    VAL : 0x00000000U
    // .. .. reg_ddrc_addrmap_col_b6 = 0x0
    // .. .. ==> 0XF800603C[19:16] = 0x00000000U
    // .. ..     ==> MASK : 0x000F0000U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF800603C, 0x000FFFFFU ,0x00000777U),
    // .. .. reg_ddrc_addrmap_col_b2 = 0x0
    // .. .. ==> 0XF8006040[3:0] = 0x00000000U
    // .. ..     ==> MASK : 0x0000000FU    VAL : 0x00000000U
    // .. .. reg_ddrc_addrmap_col_b3 = 0x0
    // .. .. ==> 0XF8006040[7:4] = 0x00000000U
    // .. ..     ==> MASK : 0x000000F0U    VAL : 0x00000000U
    // .. .. reg_ddrc_addrmap_col_b4 = 0x0
    // .. .. ==> 0XF8006040[11:8] = 0x00000000U
    // .. ..     ==> MASK : 0x00000F00U    VAL : 0x00000000U
    // .. .. reg_ddrc_addrmap_col_b7 = 0x0
    // .. .. ==> 0XF8006040[15:12] = 0x00000000U
    // .. ..     ==> MASK : 0x0000F000U    VAL : 0x00000000U
    // .. .. reg_ddrc_addrmap_col_b8 = 0x0
    // .. .. ==> 0XF8006040[19:16] = 0x00000000U
    // .. ..     ==> MASK : 0x000F0000U    VAL : 0x00000000U
    // .. .. reg_ddrc_addrmap_col_b9 = 0xf
    // .. .. ==> 0XF8006040[23:20] = 0x0000000FU
    // .. ..     ==> MASK : 0x00F00000U    VAL : 0x00F00000U
    // .. .. reg_ddrc_addrmap_col_b10 = 0xf
    // .. .. ==> 0XF8006040[27:24] = 0x0000000FU
    // .. ..     ==> MASK : 0x0F000000U    VAL : 0x0F000000U
    // .. .. reg_ddrc_addrmap_col_b11 = 0xf
    // .. .. ==> 0XF8006040[31:28] = 0x0000000FU
    // .. ..     ==> MASK : 0xF0000000U    VAL : 0xF0000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006040, 0xFFFFFFFFU ,0xFFF00000U),
    // .. .. reg_ddrc_addrmap_row_b0 = 0x6
    // .. .. ==> 0XF8006044[3:0] = 0x00000006U
    // .. ..     ==> MASK : 0x0000000FU    VAL : 0x00000006U
    // .. .. reg_ddrc_addrmap_row_b1 = 0x6
    // .. .. ==> 0XF8006044[7:4] = 0x00000006U
    // .. ..     ==> MASK : 0x000000F0U    VAL : 0x00000060U
    // .. .. reg_ddrc_addrmap_row_b2_11 = 0x6
    // .. .. ==> 0XF8006044[11:8] = 0x00000006U
    // .. ..     ==> MASK : 0x00000F00U    VAL : 0x00000600U
    // .. .. reg_ddrc_addrmap_row_b12 = 0x6
    // .. .. ==> 0XF8006044[15:12] = 0x00000006U
    // .. ..     ==> MASK : 0x0000F000U    VAL : 0x00006000U
    // .. .. reg_ddrc_addrmap_row_b13 = 0x6
    // .. .. ==> 0XF8006044[19:16] = 0x00000006U
    // .. ..     ==> MASK : 0x000F0000U    VAL : 0x00060000U
    // .. .. reg_ddrc_addrmap_row_b14 = 0xf
    // .. .. ==> 0XF8006044[23:20] = 0x0000000FU
    // .. ..     ==> MASK : 0x00F00000U    VAL : 0x00F00000U
    // .. .. reg_ddrc_addrmap_row_b15 = 0xf
    // .. .. ==> 0XF8006044[27:24] = 0x0000000FU
    // .. ..     ==> MASK : 0x0F000000U    VAL : 0x0F000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006044, 0x0FFFFFFFU ,0x0FF66666U),
    // .. .. reg_ddrc_rank0_rd_odt = 0x0
    // .. .. ==> 0XF8006048[2:0] = 0x00000000U
    // .. ..     ==> MASK : 0x00000007U    VAL : 0x00000000U
    // .. .. reg_ddrc_rank0_wr_odt = 0x1
    // .. .. ==> 0XF8006048[5:3] = 0x00000001U
    // .. ..     ==> MASK : 0x00000038U    VAL : 0x00000008U
    // .. .. reg_ddrc_rank1_rd_odt = 0x1
    // .. .. ==> 0XF8006048[8:6] = 0x00000001U
    // .. ..     ==> MASK : 0x000001C0U    VAL : 0x00000040U
    // .. .. reg_ddrc_rank1_wr_odt = 0x1
    // .. .. ==> 0XF8006048[11:9] = 0x00000001U
    // .. ..     ==> MASK : 0x00000E00U    VAL : 0x00000200U
    // .. .. reg_phy_rd_local_odt = 0x0
    // .. .. ==> 0XF8006048[13:12] = 0x00000000U
    // .. ..     ==> MASK : 0x00003000U    VAL : 0x00000000U
    // .. .. reg_phy_wr_local_odt = 0x3
    // .. .. ==> 0XF8006048[15:14] = 0x00000003U
    // .. ..     ==> MASK : 0x0000C000U    VAL : 0x0000C000U
    // .. .. reg_phy_idle_local_odt = 0x3
    // .. .. ==> 0XF8006048[17:16] = 0x00000003U
    // .. ..     ==> MASK : 0x00030000U    VAL : 0x00030000U
    // .. .. reg_ddrc_rank2_rd_odt = 0x0
    // .. .. ==> 0XF8006048[20:18] = 0x00000000U
    // .. ..     ==> MASK : 0x001C0000U    VAL : 0x00000000U
    // .. .. reg_ddrc_rank2_wr_odt = 0x0
    // .. .. ==> 0XF8006048[23:21] = 0x00000000U
    // .. ..     ==> MASK : 0x00E00000U    VAL : 0x00000000U
    // .. .. reg_ddrc_rank3_rd_odt = 0x0
    // .. .. ==> 0XF8006048[26:24] = 0x00000000U
    // .. ..     ==> MASK : 0x07000000U    VAL : 0x00000000U
    // .. .. reg_ddrc_rank3_wr_odt = 0x0
    // .. .. ==> 0XF8006048[29:27] = 0x00000000U
    // .. ..     ==> MASK : 0x38000000U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006048, 0x3FFFFFFFU ,0x0003C248U),
    // .. .. reg_phy_rd_cmd_to_data = 0x0
    // .. .. ==> 0XF8006050[3:0] = 0x00000000U
    // .. ..     ==> MASK : 0x0000000FU    VAL : 0x00000000U
    // .. .. reg_phy_wr_cmd_to_data = 0x0
    // .. .. ==> 0XF8006050[7:4] = 0x00000000U
    // .. ..     ==> MASK : 0x000000F0U    VAL : 0x00000000U
    // .. .. reg_phy_rdc_we_to_re_delay = 0x8
    // .. .. ==> 0XF8006050[11:8] = 0x00000008U
    // .. ..     ==> MASK : 0x00000F00U    VAL : 0x00000800U
    // .. .. reg_phy_rdc_fifo_rst_disable = 0x0
    // .. .. ==> 0XF8006050[15:15] = 0x00000000U
    // .. ..     ==> MASK : 0x00008000U    VAL : 0x00000000U
    // .. .. reg_phy_use_fixed_re = 0x1
    // .. .. ==> 0XF8006050[16:16] = 0x00000001U
    // .. ..     ==> MASK : 0x00010000U    VAL : 0x00010000U
    // .. .. reg_phy_rdc_fifo_rst_err_cnt_clr = 0x0
    // .. .. ==> 0XF8006050[17:17] = 0x00000000U
    // .. ..     ==> MASK : 0x00020000U    VAL : 0x00000000U
    // .. .. reg_phy_dis_phy_ctrl_rstn = 0x0
    // .. .. ==> 0XF8006050[18:18] = 0x00000000U
    // .. ..     ==> MASK : 0x00040000U    VAL : 0x00000000U
    // .. .. reg_phy_clk_stall_level = 0x0
    // .. .. ==> 0XF8006050[19:19] = 0x00000000U
    // .. ..     ==> MASK : 0x00080000U    VAL : 0x00000000U
    // .. .. reg_phy_gatelvl_num_of_dq0 = 0x7
    // .. .. ==> 0XF8006050[27:24] = 0x00000007U
    // .. ..     ==> MASK : 0x0F000000U    VAL : 0x07000000U
    // .. .. reg_phy_wrlvl_num_of_dq0 = 0x7
    // .. .. ==> 0XF8006050[31:28] = 0x00000007U
    // .. ..     ==> MASK : 0xF0000000U    VAL : 0x70000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006050, 0xFF0F8FFFU ,0x77010800U),
    // .. .. reg_ddrc_dll_calib_to_min_x1024 = 0x1
    // .. .. ==> 0XF8006058[7:0] = 0x00000001U
    // .. ..     ==> MASK : 0x000000FFU    VAL : 0x00000001U
    // .. .. reg_ddrc_dll_calib_to_max_x1024 = 0x1
    // .. .. ==> 0XF8006058[15:8] = 0x00000001U
    // .. ..     ==> MASK : 0x0000FF00U    VAL : 0x00000100U
    // .. .. reg_ddrc_dis_dll_calib = 0x0
    // .. .. ==> 0XF8006058[16:16] = 0x00000000U
    // .. ..     ==> MASK : 0x00010000U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006058, 0x0001FFFFU ,0x00000101U),
    // .. .. reg_ddrc_rd_odt_delay = 0x3
    // .. .. ==> 0XF800605C[3:0] = 0x00000003U
    // .. ..     ==> MASK : 0x0000000FU    VAL : 0x00000003U
    // .. .. reg_ddrc_wr_odt_delay = 0x0
    // .. .. ==> 0XF800605C[7:4] = 0x00000000U
    // .. ..     ==> MASK : 0x000000F0U    VAL : 0x00000000U
    // .. .. reg_ddrc_rd_odt_hold = 0x0
    // .. .. ==> 0XF800605C[11:8] = 0x00000000U
    // .. ..     ==> MASK : 0x00000F00U    VAL : 0x00000000U
    // .. .. reg_ddrc_wr_odt_hold = 0x5
    // .. .. ==> 0XF800605C[15:12] = 0x00000005U
    // .. ..     ==> MASK : 0x0000F000U    VAL : 0x00005000U
    // .. ..
    EMIT_MASKWRITE(0XF800605C, 0x0000FFFFU ,0x00005003U),
    // .. .. reg_ddrc_pageclose = 0x0
    // .. .. ==> 0XF8006060[0:0] = 0x00000000U
    // .. ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. .. reg_ddrc_lpr_num_entries = 0x1f
    // .. .. ==> 0XF8006060[6:1] = 0x0000001FU
    // .. ..     ==> MASK : 0x0000007EU    VAL : 0x0000003EU
    // .. .. reg_ddrc_auto_pre_en = 0x0
    // .. .. ==> 0XF8006060[7:7] = 0x00000000U
    // .. ..     ==> MASK : 0x00000080U    VAL : 0x00000000U
    // .. .. reg_ddrc_refresh_update_level = 0x0
    // .. .. ==> 0XF8006060[8:8] = 0x00000000U
    // .. ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. .. reg_ddrc_dis_wc = 0x0
    // .. .. ==> 0XF8006060[9:9] = 0x00000000U
    // .. ..     ==> MASK : 0x00000200U    VAL : 0x00000000U
    // .. .. reg_ddrc_dis_collision_page_opt = 0x0
    // .. .. ==> 0XF8006060[10:10] = 0x00000000U
    // .. ..     ==> MASK : 0x00000400U    VAL : 0x00000000U
    // .. .. reg_ddrc_selfref_en = 0x0
    // .. .. ==> 0XF8006060[12:12] = 0x00000000U
    // .. ..     ==> MASK : 0x00001000U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006060, 0x000017FFU ,0x0000003EU),
    // .. .. reg_ddrc_go2critical_hysteresis = 0x0
    // .. .. ==> 0XF8006064[12:5] = 0x00000000U
    // .. ..     ==> MASK : 0x00001FE0U    VAL : 0x00000000U
    // .. .. reg_arb_go2critical_en = 0x1
    // .. .. ==> 0XF8006064[17:17] = 0x00000001U
    // .. ..     ==> MASK : 0x00020000U    VAL : 0x00020000U
    // .. ..
    EMIT_MASKWRITE(0XF8006064, 0x00021FE0U ,0x00020000U),
    // .. .. reg_ddrc_wrlvl_ww = 0x41
    // .. .. ==> 0XF8006068[7:0] = 0x00000041U
    // .. ..     ==> MASK : 0x000000FFU    VAL : 0x00000041U
    // .. .. reg_ddrc_rdlvl_rr = 0x41
    // .. .. ==> 0XF8006068[15:8] = 0x00000041U
    // .. ..     ==> MASK : 0x0000FF00U    VAL : 0x00004100U
    // .. .. reg_ddrc_dfi_t_wlmrd = 0x28
    // .. .. ==> 0XF8006068[25:16] = 0x00000028U
    // .. ..     ==> MASK : 0x03FF0000U    VAL : 0x00280000U
    // .. ..
    EMIT_MASKWRITE(0XF8006068, 0x03FFFFFFU ,0x00284141U),
    // .. .. dfi_t_ctrlupd_interval_min_x1024 = 0x10
    // .. .. ==> 0XF800606C[7:0] = 0x00000010U
    // .. ..     ==> MASK : 0x000000FFU    VAL : 0x00000010U
    // .. .. dfi_t_ctrlupd_interval_max_x1024 = 0x16
    // .. .. ==> 0XF800606C[15:8] = 0x00000016U
    // .. ..     ==> MASK : 0x0000FF00U    VAL : 0x00001600U
    // .. ..
    EMIT_MASKWRITE(0XF800606C, 0x0000FFFFU ,0x00001610U),
    // .. .. refresh_timer0_start_value_x32 = 0x0
    // .. .. ==> 0XF80060A0[11:0] = 0x00000000U
    // .. ..     ==> MASK : 0x00000FFFU    VAL : 0x00000000U
    // .. .. refresh_timer1_start_value_x32 = 0x8
    // .. .. ==> 0XF80060A0[23:12] = 0x00000008U
    // .. ..     ==> MASK : 0x00FFF000U    VAL : 0x00008000U
    // .. ..
    EMIT_MASKWRITE(0XF80060A0, 0x00FFFFFFU ,0x00008000U),
    // .. .. reg_ddrc_dis_auto_zq = 0x0
    // .. .. ==> 0XF80060A4[0:0] = 0x00000000U
    // .. ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. .. reg_ddrc_ddr3 = 0x1
    // .. .. ==> 0XF80060A4[1:1] = 0x00000001U
    // .. ..     ==> MASK : 0x00000002U    VAL : 0x00000002U
    // .. .. reg_ddrc_t_mod = 0x200
    // .. .. ==> 0XF80060A4[11:2] = 0x00000200U
    // .. ..     ==> MASK : 0x00000FFCU    VAL : 0x00000800U
    // .. .. reg_ddrc_t_zq_long_nop = 0x200
    // .. .. ==> 0XF80060A4[21:12] = 0x00000200U
    // .. ..     ==> MASK : 0x003FF000U    VAL : 0x00200000U
    // .. .. reg_ddrc_t_zq_short_nop = 0x40
    // .. .. ==> 0XF80060A4[31:22] = 0x00000040U
    // .. ..     ==> MASK : 0xFFC00000U    VAL : 0x10000000U
    // .. ..
    EMIT_MASKWRITE(0XF80060A4, 0xFFFFFFFFU ,0x10200802U),
    // .. .. t_zq_short_interval_x1024 = 0xcb73
    // .. .. ==> 0XF80060A8[19:0] = 0x0000CB73U
    // .. ..     ==> MASK : 0x000FFFFFU    VAL : 0x0000CB73U
    // .. .. dram_rstn_x1024 = 0x69
    // .. .. ==> 0XF80060A8[27:20] = 0x00000069U
    // .. ..     ==> MASK : 0x0FF00000U    VAL : 0x06900000U
    // .. ..
    EMIT_MASKWRITE(0XF80060A8, 0x0FFFFFFFU ,0x0690CB73U),
    // .. .. deeppowerdown_en = 0x0
    // .. .. ==> 0XF80060AC[0:0] = 0x00000000U
    // .. ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. .. deeppowerdown_to_x1024 = 0xff
    // .. .. ==> 0XF80060AC[8:1] = 0x000000FFU
    // .. ..     ==> MASK : 0x000001FEU    VAL : 0x000001FEU
    // .. ..
    EMIT_MASKWRITE(0XF80060AC, 0x000001FFU ,0x000001FEU),
    // .. .. dfi_wrlvl_max_x1024 = 0xfff
    // .. .. ==> 0XF80060B0[11:0] = 0x00000FFFU
    // .. ..     ==> MASK : 0x00000FFFU    VAL : 0x00000FFFU
    // .. .. dfi_rdlvl_max_x1024 = 0xfff
    // .. .. ==> 0XF80060B0[23:12] = 0x00000FFFU
    // .. ..     ==> MASK : 0x00FFF000U    VAL : 0x00FFF000U
    // .. .. ddrc_reg_twrlvl_max_error = 0x0
    // .. .. ==> 0XF80060B0[24:24] = 0x00000000U
    // .. ..     ==> MASK : 0x01000000U    VAL : 0x00000000U
    // .. .. ddrc_reg_trdlvl_max_error = 0x0
    // .. .. ==> 0XF80060B0[25:25] = 0x00000000U
    // .. ..     ==> MASK : 0x02000000U    VAL : 0x00000000U
    // .. .. reg_ddrc_dfi_wr_level_en = 0x1
    // .. .. ==> 0XF80060B0[26:26] = 0x00000001U
    // .. ..     ==> MASK : 0x04000000U    VAL : 0x04000000U
    // .. .. reg_ddrc_dfi_rd_dqs_gate_level = 0x1
    // .. .. ==> 0XF80060B0[27:27] = 0x00000001U
    // .. ..     ==> MASK : 0x08000000U    VAL : 0x08000000U
    // .. .. reg_ddrc_dfi_rd_data_eye_train = 0x1
    // .. .. ==> 0XF80060B0[28:28] = 0x00000001U
    // .. ..     ==> MASK : 0x10000000U    VAL : 0x10000000U
    // .. ..
    EMIT_MASKWRITE(0XF80060B0, 0x1FFFFFFFU ,0x1CFFFFFFU),
    // .. .. reg_ddrc_2t_delay = 0x0
    // .. .. ==> 0XF80060B4[8:0] = 0x00000000U
    // .. ..     ==> MASK : 0x000001FFU    VAL : 0x00000000U
    // .. .. reg_ddrc_skip_ocd = 0x1
    // .. .. ==> 0XF80060B4[9:9] = 0x00000001U
    // .. ..     ==> MASK : 0x00000200U    VAL : 0x00000200U
    // .. .. reg_ddrc_dis_pre_bypass = 0x0
    // .. .. ==> 0XF80060B4[10:10] = 0x00000000U
    // .. ..     ==> MASK : 0x00000400U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF80060B4, 0x000007FFU ,0x00000200U),
    // .. .. reg_ddrc_dfi_t_rddata_en = 0x6
    // .. .. ==> 0XF80060B8[4:0] = 0x00000006U
    // .. ..     ==> MASK : 0x0000001FU    VAL : 0x00000006U
    // .. .. reg_ddrc_dfi_t_ctrlup_min = 0x3
    // .. .. ==> 0XF80060B8[14:5] = 0x00000003U
    // .. ..     ==> MASK : 0x00007FE0U    VAL : 0x00000060U
    // .. .. reg_ddrc_dfi_t_ctrlup_max = 0x40
    // .. .. ==> 0XF80060B8[24:15] = 0x00000040U
    // .. ..     ==> MASK : 0x01FF8000U    VAL : 0x00200000U
    // .. ..
    EMIT_MASKWRITE(0XF80060B8, 0x01FFFFFFU ,0x00200066U),
    // .. .. Clear_Uncorrectable_DRAM_ECC_error = 0x0
    // .. .. ==> 0XF80060C4[0:0] = 0x00000000U
    // .. ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. .. Clear_Correctable_DRAM_ECC_error = 0x0
    // .. .. ==> 0XF80060C4[1:1] = 0x00000000U
    // .. ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF80060C4, 0x00000003U ,0x00000000U),
    // .. .. CORR_ECC_LOG_VALID = 0x0
    // .. .. ==> 0XF80060C8[0:0] = 0x00000000U
    // .. ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. .. ECC_CORRECTED_BIT_NUM = 0x0
    // .. .. ==> 0XF80060C8[7:1] = 0x00000000U
    // .. ..     ==> MASK : 0x000000FEU    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF80060C8, 0x000000FFU ,0x00000000U),
    // .. .. UNCORR_ECC_LOG_VALID = 0x0
    // .. .. ==> 0XF80060DC[0:0] = 0x00000000U
    // .. ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF80060DC, 0x00000001U ,0x00000000U),
    // .. .. STAT_NUM_CORR_ERR = 0x0
    // .. .. ==> 0XF80060F0[15:8] = 0x00000000U
    // .. ..     ==> MASK : 0x0000FF00U    VAL : 0x00000000U
    // .. .. STAT_NUM_UNCORR_ERR = 0x0
    // .. .. ==> 0XF80060F0[7:0] = 0x00000000U
    // .. ..     ==> MASK : 0x000000FFU    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF80060F0, 0x0000FFFFU ,0x00000000U),
    // .. .. reg_ddrc_ecc_mode = 0x0
    // .. .. ==> 0XF80060F4[2:0] = 0x00000000U
    // .. ..     ==> MASK : 0x00000007U    VAL : 0x00000000U
    // .. .. reg_ddrc_dis_scrub = 0x1
    // .. .. ==> 0XF80060F4[3:3] = 0x00000001U
    // .. ..     ==> MASK : 0x00000008U    VAL : 0x00000008U
    // .. ..
    EMIT_MASKWRITE(0XF80060F4, 0x0000000FU ,0x00000008U),
    // .. .. reg_phy_dif_on = 0x0
    // .. .. ==> 0XF8006114[3:0] = 0x00000000U
    // .. ..     ==> MASK : 0x0000000FU    VAL : 0x00000000U
    // .. .. reg_phy_dif_off = 0x0
    // .. .. ==> 0XF8006114[7:4] = 0x00000000U
    // .. ..     ==> MASK : 0x000000F0U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006114, 0x000000FFU ,0x00000000U),
    // .. .. reg_phy_data_slice_in_use = 0x1
    // .. .. ==> 0XF8006118[0:0] = 0x00000001U
    // .. ..     ==> MASK : 0x00000001U    VAL : 0x00000001U
    // .. .. reg_phy_rdlvl_inc_mode = 0x0
    // .. .. ==> 0XF8006118[1:1] = 0x00000000U
    // .. ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. .. reg_phy_gatelvl_inc_mode = 0x0
    // .. .. ==> 0XF8006118[2:2] = 0x00000000U
    // .. ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. .. reg_phy_wrlvl_inc_mode = 0x0
    // .. .. ==> 0XF8006118[3:3] = 0x00000000U
    // .. ..     ==> MASK : 0x00000008U    VAL : 0x00000000U
    // .. .. reg_phy_board_lpbk_tx = 0x0
    // .. .. ==> 0XF8006118[4:4] = 0x00000000U
    // .. ..     ==> MASK : 0x00000010U    VAL : 0x00000000U
    // .. .. reg_phy_board_lpbk_rx = 0x0
    // .. .. ==> 0XF8006118[5:5] = 0x00000000U
    // .. ..     ==> MASK : 0x00000020U    VAL : 0x00000000U
    // .. .. reg_phy_bist_shift_dq = 0x0
    // .. .. ==> 0XF8006118[14:6] = 0x00000000U
    // .. ..     ==> MASK : 0x00007FC0U    VAL : 0x00000000U
    // .. .. reg_phy_bist_err_clr = 0x0
    // .. .. ==> 0XF8006118[23:15] = 0x00000000U
    // .. ..     ==> MASK : 0x00FF8000U    VAL : 0x00000000U
    // .. .. reg_phy_dq_offset = 0x40
    // .. .. ==> 0XF8006118[30:24] = 0x00000040U
    // .. ..     ==> MASK : 0x7F000000U    VAL : 0x40000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006118, 0x7FFFFFFFU ,0x40000001U),
    // .. .. reg_phy_data_slice_in_use = 0x1
    // .. .. ==> 0XF800611C[0:0] = 0x00000001U
    // .. ..     ==> MASK : 0x00000001U    VAL : 0x00000001U
    // .. .. reg_phy_rdlvl_inc_mode = 0x0
    // .. .. ==> 0XF800611C[1:1] = 0x00000000U
    // .. ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. .. reg_phy_gatelvl_inc_mode = 0x0
    // .. .. ==> 0XF800611C[2:2] = 0x00000000U
    // .. ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. .. reg_phy_wrlvl_inc_mode = 0x0
    // .. .. ==> 0XF800611C[3:3] = 0x00000000U
    // .. ..     ==> MASK : 0x00000008U    VAL : 0x00000000U
    // .. .. reg_phy_board_lpbk_tx = 0x0
    // .. .. ==> 0XF800611C[4:4] = 0x00000000U
    // .. ..     ==> MASK : 0x00000010U    VAL : 0x00000000U
    // .. .. reg_phy_board_lpbk_rx = 0x0
    // .. .. ==> 0XF800611C[5:5] = 0x00000000U
    // .. ..     ==> MASK : 0x00000020U    VAL : 0x00000000U
    // .. .. reg_phy_bist_shift_dq = 0x0
    // .. .. ==> 0XF800611C[14:6] = 0x00000000U
    // .. ..     ==> MASK : 0x00007FC0U    VAL : 0x00000000U
    // .. .. reg_phy_bist_err_clr = 0x0
    // .. .. ==> 0XF800611C[23:15] = 0x00000000U
    // .. ..     ==> MASK : 0x00FF8000U    VAL : 0x00000000U
    // .. .. reg_phy_dq_offset = 0x40
    // .. .. ==> 0XF800611C[30:24] = 0x00000040U
    // .. ..     ==> MASK : 0x7F000000U    VAL : 0x40000000U
    // .. ..
    EMIT_MASKWRITE(0XF800611C, 0x7FFFFFFFU ,0x40000001U),
    // .. .. reg_phy_data_slice_in_use = 0x1
    // .. .. ==> 0XF8006120[0:0] = 0x00000001U
    // .. ..     ==> MASK : 0x00000001U    VAL : 0x00000001U
    // .. .. reg_phy_rdlvl_inc_mode = 0x0
    // .. .. ==> 0XF8006120[1:1] = 0x00000000U
    // .. ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. .. reg_phy_gatelvl_inc_mode = 0x0
    // .. .. ==> 0XF8006120[2:2] = 0x00000000U
    // .. ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. .. reg_phy_wrlvl_inc_mode = 0x0
    // .. .. ==> 0XF8006120[3:3] = 0x00000000U
    // .. ..     ==> MASK : 0x00000008U    VAL : 0x00000000U
    // .. .. reg_phy_board_lpbk_tx = 0x0
    // .. .. ==> 0XF8006120[4:4] = 0x00000000U
    // .. ..     ==> MASK : 0x00000010U    VAL : 0x00000000U
    // .. .. reg_phy_board_lpbk_rx = 0x0
    // .. .. ==> 0XF8006120[5:5] = 0x00000000U
    // .. ..     ==> MASK : 0x00000020U    VAL : 0x00000000U
    // .. .. reg_phy_bist_shift_dq = 0x0
    // .. .. ==> 0XF8006120[14:6] = 0x00000000U
    // .. ..     ==> MASK : 0x00007FC0U    VAL : 0x00000000U
    // .. .. reg_phy_bist_err_clr = 0x0
    // .. .. ==> 0XF8006120[23:15] = 0x00000000U
    // .. ..     ==> MASK : 0x00FF8000U    VAL : 0x00000000U
    // .. .. reg_phy_dq_offset = 0x40
    // .. .. ==> 0XF8006120[30:24] = 0x00000040U
    // .. ..     ==> MASK : 0x7F000000U    VAL : 0x40000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006120, 0x7FFFFFFFU ,0x40000001U),
    // .. .. reg_phy_data_slice_in_use = 0x1
    // .. .. ==> 0XF8006124[0:0] = 0x00000001U
    // .. ..     ==> MASK : 0x00000001U    VAL : 0x00000001U
    // .. .. reg_phy_rdlvl_inc_mode = 0x0
    // .. .. ==> 0XF8006124[1:1] = 0x00000000U
    // .. ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. .. reg_phy_gatelvl_inc_mode = 0x0
    // .. .. ==> 0XF8006124[2:2] = 0x00000000U
    // .. ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. .. reg_phy_wrlvl_inc_mode = 0x0
    // .. .. ==> 0XF8006124[3:3] = 0x00000000U
    // .. ..     ==> MASK : 0x00000008U    VAL : 0x00000000U
    // .. .. reg_phy_board_lpbk_tx = 0x0
    // .. .. ==> 0XF8006124[4:4] = 0x00000000U
    // .. ..     ==> MASK : 0x00000010U    VAL : 0x00000000U
    // .. .. reg_phy_board_lpbk_rx = 0x0
    // .. .. ==> 0XF8006124[5:5] = 0x00000000U
    // .. ..     ==> MASK : 0x00000020U    VAL : 0x00000000U
    // .. .. reg_phy_bist_shift_dq = 0x0
    // .. .. ==> 0XF8006124[14:6] = 0x00000000U
    // .. ..     ==> MASK : 0x00007FC0U    VAL : 0x00000000U
    // .. .. reg_phy_bist_err_clr = 0x0
    // .. .. ==> 0XF8006124[23:15] = 0x00000000U
    // .. ..     ==> MASK : 0x00FF8000U    VAL : 0x00000000U
    // .. .. reg_phy_dq_offset = 0x40
    // .. .. ==> 0XF8006124[30:24] = 0x00000040U
    // .. ..     ==> MASK : 0x7F000000U    VAL : 0x40000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006124, 0x7FFFFFFFU ,0x40000001U),
    // .. .. reg_phy_wrlvl_init_ratio = 0x0
    // .. .. ==> 0XF800612C[9:0] = 0x00000000U
    // .. ..     ==> MASK : 0x000003FFU    VAL : 0x00000000U
    // .. .. reg_phy_gatelvl_init_ratio = 0xa4
    // .. .. ==> 0XF800612C[19:10] = 0x000000A4U
    // .. ..     ==> MASK : 0x000FFC00U    VAL : 0x00029000U
    // .. ..
    EMIT_MASKWRITE(0XF800612C, 0x000FFFFFU ,0x00029000U),
    // .. .. reg_phy_wrlvl_init_ratio = 0x0
    // .. .. ==> 0XF8006130[9:0] = 0x00000000U
    // .. ..     ==> MASK : 0x000003FFU    VAL : 0x00000000U
    // .. .. reg_phy_gatelvl_init_ratio = 0xa4
    // .. .. ==> 0XF8006130[19:10] = 0x000000A4U
    // .. ..     ==> MASK : 0x000FFC00U    VAL : 0x00029000U
    // .. ..
    EMIT_MASKWRITE(0XF8006130, 0x000FFFFFU ,0x00029000U),
    // .. .. reg_phy_wrlvl_init_ratio = 0x0
    // .. .. ==> 0XF8006134[9:0] = 0x00000000U
    // .. ..     ==> MASK : 0x000003FFU    VAL : 0x00000000U
    // .. .. reg_phy_gatelvl_init_ratio = 0xa4
    // .. .. ==> 0XF8006134[19:10] = 0x000000A4U
    // .. ..     ==> MASK : 0x000FFC00U    VAL : 0x00029000U
    // .. ..
    EMIT_MASKWRITE(0XF8006134, 0x000FFFFFU ,0x00029000U),
    // .. .. reg_phy_wrlvl_init_ratio = 0x0
    // .. .. ==> 0XF8006138[9:0] = 0x00000000U
    // .. ..     ==> MASK : 0x000003FFU    VAL : 0x00000000U
    // .. .. reg_phy_gatelvl_init_ratio = 0xa4
    // .. .. ==> 0XF8006138[19:10] = 0x000000A4U
    // .. ..     ==> MASK : 0x000FFC00U    VAL : 0x00029000U
    // .. ..
    EMIT_MASKWRITE(0XF8006138, 0x000FFFFFU ,0x00029000U),
    // .. .. reg_phy_rd_dqs_slave_ratio = 0x35
    // .. .. ==> 0XF8006140[9:0] = 0x00000035U
    // .. ..     ==> MASK : 0x000003FFU    VAL : 0x00000035U
    // .. .. reg_phy_rd_dqs_slave_force = 0x0
    // .. .. ==> 0XF8006140[10:10] = 0x00000000U
    // .. ..     ==> MASK : 0x00000400U    VAL : 0x00000000U
    // .. .. reg_phy_rd_dqs_slave_delay = 0x0
    // .. .. ==> 0XF8006140[19:11] = 0x00000000U
    // .. ..     ==> MASK : 0x000FF800U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006140, 0x000FFFFFU ,0x00000035U),
    // .. .. reg_phy_rd_dqs_slave_ratio = 0x35
    // .. .. ==> 0XF8006144[9:0] = 0x00000035U
    // .. ..     ==> MASK : 0x000003FFU    VAL : 0x00000035U
    // .. .. reg_phy_rd_dqs_slave_force = 0x0
    // .. .. ==> 0XF8006144[10:10] = 0x00000000U
    // .. ..     ==> MASK : 0x00000400U    VAL : 0x00000000U
    // .. .. reg_phy_rd_dqs_slave_delay = 0x0
    // .. .. ==> 0XF8006144[19:11] = 0x00000000U
    // .. ..     ==> MASK : 0x000FF800U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006144, 0x000FFFFFU ,0x00000035U),
    // .. .. reg_phy_rd_dqs_slave_ratio = 0x35
    // .. .. ==> 0XF8006148[9:0] = 0x00000035U
    // .. ..     ==> MASK : 0x000003FFU    VAL : 0x00000035U
    // .. .. reg_phy_rd_dqs_slave_force = 0x0
    // .. .. ==> 0XF8006148[10:10] = 0x00000000U
    // .. ..     ==> MASK : 0x00000400U    VAL : 0x00000000U
    // .. .. reg_phy_rd_dqs_slave_delay = 0x0
    // .. .. ==> 0XF8006148[19:11] = 0x00000000U
    // .. ..     ==> MASK : 0x000FF800U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006148, 0x000FFFFFU ,0x00000035U),
    // .. .. reg_phy_rd_dqs_slave_ratio = 0x35
    // .. .. ==> 0XF800614C[9:0] = 0x00000035U
    // .. ..     ==> MASK : 0x000003FFU    VAL : 0x00000035U
    // .. .. reg_phy_rd_dqs_slave_force = 0x0
    // .. .. ==> 0XF800614C[10:10] = 0x00000000U
    // .. ..     ==> MASK : 0x00000400U    VAL : 0x00000000U
    // .. .. reg_phy_rd_dqs_slave_delay = 0x0
    // .. .. ==> 0XF800614C[19:11] = 0x00000000U
    // .. ..     ==> MASK : 0x000FF800U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF800614C, 0x000FFFFFU ,0x00000035U),
    // .. .. reg_phy_wr_dqs_slave_ratio = 0x80
    // .. .. ==> 0XF8006154[9:0] = 0x00000080U
    // .. ..     ==> MASK : 0x000003FFU    VAL : 0x00000080U
    // .. .. reg_phy_wr_dqs_slave_force = 0x0
    // .. .. ==> 0XF8006154[10:10] = 0x00000000U
    // .. ..     ==> MASK : 0x00000400U    VAL : 0x00000000U
    // .. .. reg_phy_wr_dqs_slave_delay = 0x0
    // .. .. ==> 0XF8006154[19:11] = 0x00000000U
    // .. ..     ==> MASK : 0x000FF800U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006154, 0x000FFFFFU ,0x00000080U),
    // .. .. reg_phy_wr_dqs_slave_ratio = 0x80
    // .. .. ==> 0XF8006158[9:0] = 0x00000080U
    // .. ..     ==> MASK : 0x000003FFU    VAL : 0x00000080U
    // .. .. reg_phy_wr_dqs_slave_force = 0x0
    // .. .. ==> 0XF8006158[10:10] = 0x00000000U
    // .. ..     ==> MASK : 0x00000400U    VAL : 0x00000000U
    // .. .. reg_phy_wr_dqs_slave_delay = 0x0
    // .. .. ==> 0XF8006158[19:11] = 0x00000000U
    // .. ..     ==> MASK : 0x000FF800U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006158, 0x000FFFFFU ,0x00000080U),
    // .. .. reg_phy_wr_dqs_slave_ratio = 0x80
    // .. .. ==> 0XF800615C[9:0] = 0x00000080U
    // .. ..     ==> MASK : 0x000003FFU    VAL : 0x00000080U
    // .. .. reg_phy_wr_dqs_slave_force = 0x0
    // .. .. ==> 0XF800615C[10:10] = 0x00000000U
    // .. ..     ==> MASK : 0x00000400U    VAL : 0x00000000U
    // .. .. reg_phy_wr_dqs_slave_delay = 0x0
    // .. .. ==> 0XF800615C[19:11] = 0x00000000U
    // .. ..     ==> MASK : 0x000FF800U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF800615C, 0x000FFFFFU ,0x00000080U),
    // .. .. reg_phy_wr_dqs_slave_ratio = 0x80
    // .. .. ==> 0XF8006160[9:0] = 0x00000080U
    // .. ..     ==> MASK : 0x000003FFU    VAL : 0x00000080U
    // .. .. reg_phy_wr_dqs_slave_force = 0x0
    // .. .. ==> 0XF8006160[10:10] = 0x00000000U
    // .. ..     ==> MASK : 0x00000400U    VAL : 0x00000000U
    // .. .. reg_phy_wr_dqs_slave_delay = 0x0
    // .. .. ==> 0XF8006160[19:11] = 0x00000000U
    // .. ..     ==> MASK : 0x000FF800U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006160, 0x000FFFFFU ,0x00000080U),
    // .. .. reg_phy_fifo_we_slave_ratio = 0xf9
    // .. .. ==> 0XF8006168[10:0] = 0x000000F9U
    // .. ..     ==> MASK : 0x000007FFU    VAL : 0x000000F9U
    // .. .. reg_phy_fifo_we_in_force = 0x0
    // .. .. ==> 0XF8006168[11:11] = 0x00000000U
    // .. ..     ==> MASK : 0x00000800U    VAL : 0x00000000U
    // .. .. reg_phy_fifo_we_in_delay = 0x0
    // .. .. ==> 0XF8006168[20:12] = 0x00000000U
    // .. ..     ==> MASK : 0x001FF000U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006168, 0x001FFFFFU ,0x000000F9U),
    // .. .. reg_phy_fifo_we_slave_ratio = 0xf9
    // .. .. ==> 0XF800616C[10:0] = 0x000000F9U
    // .. ..     ==> MASK : 0x000007FFU    VAL : 0x000000F9U
    // .. .. reg_phy_fifo_we_in_force = 0x0
    // .. .. ==> 0XF800616C[11:11] = 0x00000000U
    // .. ..     ==> MASK : 0x00000800U    VAL : 0x00000000U
    // .. .. reg_phy_fifo_we_in_delay = 0x0
    // .. .. ==> 0XF800616C[20:12] = 0x00000000U
    // .. ..     ==> MASK : 0x001FF000U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF800616C, 0x001FFFFFU ,0x000000F9U),
    // .. .. reg_phy_fifo_we_slave_ratio = 0xf9
    // .. .. ==> 0XF8006170[10:0] = 0x000000F9U
    // .. ..     ==> MASK : 0x000007FFU    VAL : 0x000000F9U
    // .. .. reg_phy_fifo_we_in_force = 0x0
    // .. .. ==> 0XF8006170[11:11] = 0x00000000U
    // .. ..     ==> MASK : 0x00000800U    VAL : 0x00000000U
    // .. .. reg_phy_fifo_we_in_delay = 0x0
    // .. .. ==> 0XF8006170[20:12] = 0x00000000U
    // .. ..     ==> MASK : 0x001FF000U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006170, 0x001FFFFFU ,0x000000F9U),
    // .. .. reg_phy_fifo_we_slave_ratio = 0xf9
    // .. .. ==> 0XF8006174[10:0] = 0x000000F9U
    // .. ..     ==> MASK : 0x000007FFU    VAL : 0x000000F9U
    // .. .. reg_phy_fifo_we_in_force = 0x0
    // .. .. ==> 0XF8006174[11:11] = 0x00000000U
    // .. ..     ==> MASK : 0x00000800U    VAL : 0x00000000U
    // .. .. reg_phy_fifo_we_in_delay = 0x0
    // .. .. ==> 0XF8006174[20:12] = 0x00000000U
    // .. ..     ==> MASK : 0x001FF000U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006174, 0x001FFFFFU ,0x000000F9U),
    // .. .. reg_phy_wr_data_slave_ratio = 0xc0
    // .. .. ==> 0XF800617C[9:0] = 0x000000C0U
    // .. ..     ==> MASK : 0x000003FFU    VAL : 0x000000C0U
    // .. .. reg_phy_wr_data_slave_force = 0x0
    // .. .. ==> 0XF800617C[10:10] = 0x00000000U
    // .. ..     ==> MASK : 0x00000400U    VAL : 0x00000000U
    // .. .. reg_phy_wr_data_slave_delay = 0x0
    // .. .. ==> 0XF800617C[19:11] = 0x00000000U
    // .. ..     ==> MASK : 0x000FF800U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF800617C, 0x000FFFFFU ,0x000000C0U),
    // .. .. reg_phy_wr_data_slave_ratio = 0xc0
    // .. .. ==> 0XF8006180[9:0] = 0x000000C0U
    // .. ..     ==> MASK : 0x000003FFU    VAL : 0x000000C0U
    // .. .. reg_phy_wr_data_slave_force = 0x0
    // .. .. ==> 0XF8006180[10:10] = 0x00000000U
    // .. ..     ==> MASK : 0x00000400U    VAL : 0x00000000U
    // .. .. reg_phy_wr_data_slave_delay = 0x0
    // .. .. ==> 0XF8006180[19:11] = 0x00000000U
    // .. ..     ==> MASK : 0x000FF800U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006180, 0x000FFFFFU ,0x000000C0U),
    // .. .. reg_phy_wr_data_slave_ratio = 0xc0
    // .. .. ==> 0XF8006184[9:0] = 0x000000C0U
    // .. ..     ==> MASK : 0x000003FFU    VAL : 0x000000C0U
    // .. .. reg_phy_wr_data_slave_force = 0x0
    // .. .. ==> 0XF8006184[10:10] = 0x00000000U
    // .. ..     ==> MASK : 0x00000400U    VAL : 0x00000000U
    // .. .. reg_phy_wr_data_slave_delay = 0x0
    // .. .. ==> 0XF8006184[19:11] = 0x00000000U
    // .. ..     ==> MASK : 0x000FF800U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006184, 0x000FFFFFU ,0x000000C0U),
    // .. .. reg_phy_wr_data_slave_ratio = 0xc0
    // .. .. ==> 0XF8006188[9:0] = 0x000000C0U
    // .. ..     ==> MASK : 0x000003FFU    VAL : 0x000000C0U
    // .. .. reg_phy_wr_data_slave_force = 0x0
    // .. .. ==> 0XF8006188[10:10] = 0x00000000U
    // .. ..     ==> MASK : 0x00000400U    VAL : 0x00000000U
    // .. .. reg_phy_wr_data_slave_delay = 0x0
    // .. .. ==> 0XF8006188[19:11] = 0x00000000U
    // .. ..     ==> MASK : 0x000FF800U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006188, 0x000FFFFFU ,0x000000C0U),
    // .. .. reg_phy_loopback = 0x0
    // .. .. ==> 0XF8006190[0:0] = 0x00000000U
    // .. ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. .. reg_phy_bl2 = 0x0
    // .. .. ==> 0XF8006190[1:1] = 0x00000000U
    // .. ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. .. reg_phy_at_spd_atpg = 0x0
    // .. .. ==> 0XF8006190[2:2] = 0x00000000U
    // .. ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. .. reg_phy_bist_enable = 0x0
    // .. .. ==> 0XF8006190[3:3] = 0x00000000U
    // .. ..     ==> MASK : 0x00000008U    VAL : 0x00000000U
    // .. .. reg_phy_bist_force_err = 0x0
    // .. .. ==> 0XF8006190[4:4] = 0x00000000U
    // .. ..     ==> MASK : 0x00000010U    VAL : 0x00000000U
    // .. .. reg_phy_bist_mode = 0x0
    // .. .. ==> 0XF8006190[6:5] = 0x00000000U
    // .. ..     ==> MASK : 0x00000060U    VAL : 0x00000000U
    // .. .. reg_phy_invert_clkout = 0x1
    // .. .. ==> 0XF8006190[7:7] = 0x00000001U
    // .. ..     ==> MASK : 0x00000080U    VAL : 0x00000080U
    // .. .. reg_phy_all_dq_mpr_rd_resp = 0x0
    // .. .. ==> 0XF8006190[8:8] = 0x00000000U
    // .. ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. .. reg_phy_sel_logic = 0x0
    // .. .. ==> 0XF8006190[9:9] = 0x00000000U
    // .. ..     ==> MASK : 0x00000200U    VAL : 0x00000000U
    // .. .. reg_phy_ctrl_slave_ratio = 0x100
    // .. .. ==> 0XF8006190[19:10] = 0x00000100U
    // .. ..     ==> MASK : 0x000FFC00U    VAL : 0x00040000U
    // .. .. reg_phy_ctrl_slave_force = 0x0
    // .. .. ==> 0XF8006190[20:20] = 0x00000000U
    // .. ..     ==> MASK : 0x00100000U    VAL : 0x00000000U
    // .. .. reg_phy_ctrl_slave_delay = 0x0
    // .. .. ==> 0XF8006190[27:21] = 0x00000000U
    // .. ..     ==> MASK : 0x0FE00000U    VAL : 0x00000000U
    // .. .. reg_phy_use_rank0_delays = 0x1
    // .. .. ==> 0XF8006190[28:28] = 0x00000001U
    // .. ..     ==> MASK : 0x10000000U    VAL : 0x10000000U
    // .. .. reg_phy_lpddr = 0x0
    // .. .. ==> 0XF8006190[29:29] = 0x00000000U
    // .. ..     ==> MASK : 0x20000000U    VAL : 0x00000000U
    // .. .. reg_phy_cmd_latency = 0x0
    // .. .. ==> 0XF8006190[30:30] = 0x00000000U
    // .. ..     ==> MASK : 0x40000000U    VAL : 0x00000000U
    // .. .. reg_phy_int_lpbk = 0x0
    // .. .. ==> 0XF8006190[31:31] = 0x00000000U
    // .. ..     ==> MASK : 0x80000000U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006190, 0xFFFFFFFFU ,0x10040080U),
    // .. .. reg_phy_wr_rl_delay = 0x2
    // .. .. ==> 0XF8006194[4:0] = 0x00000002U
    // .. ..     ==> MASK : 0x0000001FU    VAL : 0x00000002U
    // .. .. reg_phy_rd_rl_delay = 0x4
    // .. .. ==> 0XF8006194[9:5] = 0x00000004U
    // .. ..     ==> MASK : 0x000003E0U    VAL : 0x00000080U
    // .. .. reg_phy_dll_lock_diff = 0xf
    // .. .. ==> 0XF8006194[13:10] = 0x0000000FU
    // .. ..     ==> MASK : 0x00003C00U    VAL : 0x00003C00U
    // .. .. reg_phy_use_wr_level = 0x1
    // .. .. ==> 0XF8006194[14:14] = 0x00000001U
    // .. ..     ==> MASK : 0x00004000U    VAL : 0x00004000U
    // .. .. reg_phy_use_rd_dqs_gate_level = 0x1
    // .. .. ==> 0XF8006194[15:15] = 0x00000001U
    // .. ..     ==> MASK : 0x00008000U    VAL : 0x00008000U
    // .. .. reg_phy_use_rd_data_eye_level = 0x1
    // .. .. ==> 0XF8006194[16:16] = 0x00000001U
    // .. ..     ==> MASK : 0x00010000U    VAL : 0x00010000U
    // .. .. reg_phy_dis_calib_rst = 0x0
    // .. .. ==> 0XF8006194[17:17] = 0x00000000U
    // .. ..     ==> MASK : 0x00020000U    VAL : 0x00000000U
    // .. .. reg_phy_ctrl_slave_delay = 0x0
    // .. .. ==> 0XF8006194[19:18] = 0x00000000U
    // .. ..     ==> MASK : 0x000C0000U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006194, 0x000FFFFFU ,0x0001FC82U),
    // .. .. reg_arb_page_addr_mask = 0x0
    // .. .. ==> 0XF8006204[31:0] = 0x00000000U
    // .. ..     ==> MASK : 0xFFFFFFFFU    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006204, 0xFFFFFFFFU ,0x00000000U),
    // .. .. reg_arb_pri_wr_portn = 0x3ff
    // .. .. ==> 0XF8006208[9:0] = 0x000003FFU
    // .. ..     ==> MASK : 0x000003FFU    VAL : 0x000003FFU
    // .. .. reg_arb_disable_aging_wr_portn = 0x0
    // .. .. ==> 0XF8006208[16:16] = 0x00000000U
    // .. ..     ==> MASK : 0x00010000U    VAL : 0x00000000U
    // .. .. reg_arb_disable_urgent_wr_portn = 0x0
    // .. .. ==> 0XF8006208[17:17] = 0x00000000U
    // .. ..     ==> MASK : 0x00020000U    VAL : 0x00000000U
    // .. .. reg_arb_dis_page_match_wr_portn = 0x0
    // .. .. ==> 0XF8006208[18:18] = 0x00000000U
    // .. ..     ==> MASK : 0x00040000U    VAL : 0x00000000U
    // .. .. reg_arb_dis_rmw_portn = 0x1
    // .. .. ==> 0XF8006208[19:19] = 0x00000001U
    // .. ..     ==> MASK : 0x00080000U    VAL : 0x00080000U
    // .. ..
    EMIT_MASKWRITE(0XF8006208, 0x000F03FFU ,0x000803FFU),
    // .. .. reg_arb_pri_wr_portn = 0x3ff
    // .. .. ==> 0XF800620C[9:0] = 0x000003FFU
    // .. ..     ==> MASK : 0x000003FFU    VAL : 0x000003FFU
    // .. .. reg_arb_disable_aging_wr_portn = 0x0
    // .. .. ==> 0XF800620C[16:16] = 0x00000000U
    // .. ..     ==> MASK : 0x00010000U    VAL : 0x00000000U
    // .. .. reg_arb_disable_urgent_wr_portn = 0x0
    // .. .. ==> 0XF800620C[17:17] = 0x00000000U
    // .. ..     ==> MASK : 0x00020000U    VAL : 0x00000000U
    // .. .. reg_arb_dis_page_match_wr_portn = 0x0
    // .. .. ==> 0XF800620C[18:18] = 0x00000000U
    // .. ..     ==> MASK : 0x00040000U    VAL : 0x00000000U
    // .. .. reg_arb_dis_rmw_portn = 0x1
    // .. .. ==> 0XF800620C[19:19] = 0x00000001U
    // .. ..     ==> MASK : 0x00080000U    VAL : 0x00080000U
    // .. ..
    EMIT_MASKWRITE(0XF800620C, 0x000F03FFU ,0x000803FFU),
    // .. .. reg_arb_pri_wr_portn = 0x3ff
    // .. .. ==> 0XF8006210[9:0] = 0x000003FFU
    // .. ..     ==> MASK : 0x000003FFU    VAL : 0x000003FFU
    // .. .. reg_arb_disable_aging_wr_portn = 0x0
    // .. .. ==> 0XF8006210[16:16] = 0x00000000U
    // .. ..     ==> MASK : 0x00010000U    VAL : 0x00000000U
    // .. .. reg_arb_disable_urgent_wr_portn = 0x0
    // .. .. ==> 0XF8006210[17:17] = 0x00000000U
    // .. ..     ==> MASK : 0x00020000U    VAL : 0x00000000U
    // .. .. reg_arb_dis_page_match_wr_portn = 0x0
    // .. .. ==> 0XF8006210[18:18] = 0x00000000U
    // .. ..     ==> MASK : 0x00040000U    VAL : 0x00000000U
    // .. .. reg_arb_dis_rmw_portn = 0x1
    // .. .. ==> 0XF8006210[19:19] = 0x00000001U
    // .. ..     ==> MASK : 0x00080000U    VAL : 0x00080000U
    // .. ..
    EMIT_MASKWRITE(0XF8006210, 0x000F03FFU ,0x000803FFU),
    // .. .. reg_arb_pri_wr_portn = 0x3ff
    // .. .. ==> 0XF8006214[9:0] = 0x000003FFU
    // .. ..     ==> MASK : 0x000003FFU    VAL : 0x000003FFU
    // .. .. reg_arb_disable_aging_wr_portn = 0x0
    // .. .. ==> 0XF8006214[16:16] = 0x00000000U
    // .. ..     ==> MASK : 0x00010000U    VAL : 0x00000000U
    // .. .. reg_arb_disable_urgent_wr_portn = 0x0
    // .. .. ==> 0XF8006214[17:17] = 0x00000000U
    // .. ..     ==> MASK : 0x00020000U    VAL : 0x00000000U
    // .. .. reg_arb_dis_page_match_wr_portn = 0x0
    // .. .. ==> 0XF8006214[18:18] = 0x00000000U
    // .. ..     ==> MASK : 0x00040000U    VAL : 0x00000000U
    // .. .. reg_arb_dis_rmw_portn = 0x1
    // .. .. ==> 0XF8006214[19:19] = 0x00000001U
    // .. ..     ==> MASK : 0x00080000U    VAL : 0x00080000U
    // .. ..
    EMIT_MASKWRITE(0XF8006214, 0x000F03FFU ,0x000803FFU),
    // .. .. reg_arb_pri_rd_portn = 0x3ff
    // .. .. ==> 0XF8006218[9:0] = 0x000003FFU
    // .. ..     ==> MASK : 0x000003FFU    VAL : 0x000003FFU
    // .. .. reg_arb_disable_aging_rd_portn = 0x0
    // .. .. ==> 0XF8006218[16:16] = 0x00000000U
    // .. ..     ==> MASK : 0x00010000U    VAL : 0x00000000U
    // .. .. reg_arb_disable_urgent_rd_portn = 0x0
    // .. .. ==> 0XF8006218[17:17] = 0x00000000U
    // .. ..     ==> MASK : 0x00020000U    VAL : 0x00000000U
    // .. .. reg_arb_dis_page_match_rd_portn = 0x0
    // .. .. ==> 0XF8006218[18:18] = 0x00000000U
    // .. ..     ==> MASK : 0x00040000U    VAL : 0x00000000U
    // .. .. reg_arb_set_hpr_rd_portn = 0x0
    // .. .. ==> 0XF8006218[19:19] = 0x00000000U
    // .. ..     ==> MASK : 0x00080000U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006218, 0x000F03FFU ,0x000003FFU),
    // .. .. reg_arb_pri_rd_portn = 0x3ff
    // .. .. ==> 0XF800621C[9:0] = 0x000003FFU
    // .. ..     ==> MASK : 0x000003FFU    VAL : 0x000003FFU
    // .. .. reg_arb_disable_aging_rd_portn = 0x0
    // .. .. ==> 0XF800621C[16:16] = 0x00000000U
    // .. ..     ==> MASK : 0x00010000U    VAL : 0x00000000U
    // .. .. reg_arb_disable_urgent_rd_portn = 0x0
    // .. .. ==> 0XF800621C[17:17] = 0x00000000U
    // .. ..     ==> MASK : 0x00020000U    VAL : 0x00000000U
    // .. .. reg_arb_dis_page_match_rd_portn = 0x0
    // .. .. ==> 0XF800621C[18:18] = 0x00000000U
    // .. ..     ==> MASK : 0x00040000U    VAL : 0x00000000U
    // .. .. reg_arb_set_hpr_rd_portn = 0x0
    // .. .. ==> 0XF800621C[19:19] = 0x00000000U
    // .. ..     ==> MASK : 0x00080000U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF800621C, 0x000F03FFU ,0x000003FFU),
    // .. .. reg_arb_pri_rd_portn = 0x3ff
    // .. .. ==> 0XF8006220[9:0] = 0x000003FFU
    // .. ..     ==> MASK : 0x000003FFU    VAL : 0x000003FFU
    // .. .. reg_arb_disable_aging_rd_portn = 0x0
    // .. .. ==> 0XF8006220[16:16] = 0x00000000U
    // .. ..     ==> MASK : 0x00010000U    VAL : 0x00000000U
    // .. .. reg_arb_disable_urgent_rd_portn = 0x0
    // .. .. ==> 0XF8006220[17:17] = 0x00000000U
    // .. ..     ==> MASK : 0x00020000U    VAL : 0x00000000U
    // .. .. reg_arb_dis_page_match_rd_portn = 0x0
    // .. .. ==> 0XF8006220[18:18] = 0x00000000U
    // .. ..     ==> MASK : 0x00040000U    VAL : 0x00000000U
    // .. .. reg_arb_set_hpr_rd_portn = 0x0
    // .. .. ==> 0XF8006220[19:19] = 0x00000000U
    // .. ..     ==> MASK : 0x00080000U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006220, 0x000F03FFU ,0x000003FFU),
    // .. .. reg_arb_pri_rd_portn = 0x3ff
    // .. .. ==> 0XF8006224[9:0] = 0x000003FFU
    // .. ..     ==> MASK : 0x000003FFU    VAL : 0x000003FFU
    // .. .. reg_arb_disable_aging_rd_portn = 0x0
    // .. .. ==> 0XF8006224[16:16] = 0x00000000U
    // .. ..     ==> MASK : 0x00010000U    VAL : 0x00000000U
    // .. .. reg_arb_disable_urgent_rd_portn = 0x0
    // .. .. ==> 0XF8006224[17:17] = 0x00000000U
    // .. ..     ==> MASK : 0x00020000U    VAL : 0x00000000U
    // .. .. reg_arb_dis_page_match_rd_portn = 0x0
    // .. .. ==> 0XF8006224[18:18] = 0x00000000U
    // .. ..     ==> MASK : 0x00040000U    VAL : 0x00000000U
    // .. .. reg_arb_set_hpr_rd_portn = 0x0
    // .. .. ==> 0XF8006224[19:19] = 0x00000000U
    // .. ..     ==> MASK : 0x00080000U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006224, 0x000F03FFU ,0x000003FFU),
    // .. .. reg_ddrc_lpddr2 = 0x0
    // .. .. ==> 0XF80062A8[0:0] = 0x00000000U
    // .. ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. .. reg_ddrc_per_bank_refresh = 0x0
    // .. .. ==> 0XF80062A8[1:1] = 0x00000000U
    // .. ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. .. reg_ddrc_derate_enable = 0x0
    // .. .. ==> 0XF80062A8[2:2] = 0x00000000U
    // .. ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. .. reg_ddrc_mr4_margin = 0x0
    // .. .. ==> 0XF80062A8[11:4] = 0x00000000U
    // .. ..     ==> MASK : 0x00000FF0U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF80062A8, 0x00000FF7U ,0x00000000U),
    // .. .. reg_ddrc_mr4_read_interval = 0x0
    // .. .. ==> 0XF80062AC[31:0] = 0x00000000U
    // .. ..     ==> MASK : 0xFFFFFFFFU    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF80062AC, 0xFFFFFFFFU ,0x00000000U),
    // .. .. reg_ddrc_min_stable_clock_x1 = 0x5
    // .. .. ==> 0XF80062B0[3:0] = 0x00000005U
    // .. ..     ==> MASK : 0x0000000FU    VAL : 0x00000005U
    // .. .. reg_ddrc_idle_after_reset_x32 = 0x12
    // .. .. ==> 0XF80062B0[11:4] = 0x00000012U
    // .. ..     ==> MASK : 0x00000FF0U    VAL : 0x00000120U
    // .. .. reg_ddrc_t_mrw = 0x5
    // .. .. ==> 0XF80062B0[21:12] = 0x00000005U
    // .. ..     ==> MASK : 0x003FF000U    VAL : 0x00005000U
    // .. ..
    EMIT_MASKWRITE(0XF80062B0, 0x003FFFFFU ,0x00005125U),
    // .. .. reg_ddrc_max_auto_init_x1024 = 0xa8
    // .. .. ==> 0XF80062B4[7:0] = 0x000000A8U
    // .. ..     ==> MASK : 0x000000FFU    VAL : 0x000000A8U
    // .. .. reg_ddrc_dev_zqinit_x32 = 0x12
    // .. .. ==> 0XF80062B4[17:8] = 0x00000012U
    // .. ..     ==> MASK : 0x0003FF00U    VAL : 0x00001200U
    // .. ..
    EMIT_MASKWRITE(0XF80062B4, 0x0003FFFFU ,0x000012A8U),
    // .. .. START: POLL ON DCI STATUS
    // .. .. DONE = 1
    // .. .. ==> 0XF8000B74[13:13] = 0x00000001U
    // .. ..     ==> MASK : 0x00002000U    VAL : 0x00002000U
    // .. ..
    EMIT_MASKPOLL(0XF8000B74, 0x00002000U),
    // .. .. FINISH: POLL ON DCI STATUS
    // .. .. START: UNLOCK DDR
    // .. .. reg_ddrc_soft_rstb = 0x1
    // .. .. ==> 0XF8006000[0:0] = 0x00000001U
    // .. ..     ==> MASK : 0x00000001U    VAL : 0x00000001U
    // .. .. reg_ddrc_powerdown_en = 0x0
    // .. .. ==> 0XF8006000[1:1] = 0x00000000U
    // .. ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. .. reg_ddrc_data_bus_width = 0x0
    // .. .. ==> 0XF8006000[3:2] = 0x00000000U
    // .. ..     ==> MASK : 0x0000000CU    VAL : 0x00000000U
    // .. .. reg_ddrc_burst8_refresh = 0x0
    // .. .. ==> 0XF8006000[6:4] = 0x00000000U
    // .. ..     ==> MASK : 0x00000070U    VAL : 0x00000000U
    // .. .. reg_ddrc_rdwr_idle_gap = 1
    // .. .. ==> 0XF8006000[13:7] = 0x00000001U
    // .. ..     ==> MASK : 0x00003F80U    VAL : 0x00000080U
    // .. .. reg_ddrc_dis_rd_bypass = 0x0
    // .. .. ==> 0XF8006000[14:14] = 0x00000000U
    // .. ..     ==> MASK : 0x00004000U    VAL : 0x00000000U
    // .. .. reg_ddrc_dis_act_bypass = 0x0
    // .. .. ==> 0XF8006000[15:15] = 0x00000000U
    // .. ..     ==> MASK : 0x00008000U    VAL : 0x00000000U
    // .. .. reg_ddrc_dis_auto_refresh = 0x0
    // .. .. ==> 0XF8006000[16:16] = 0x00000000U
    // .. ..     ==> MASK : 0x00010000U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF8006000, 0x0001FFFFU ,0x00000081U),
    // .. .. FINISH: UNLOCK DDR
    // .. .. START: CHECK DDR STATUS
    // .. .. ddrc_reg_operating_mode = 1
    // .. .. ==> 0XF8006054[2:0] = 0x00000001U
    // .. ..     ==> MASK : 0x00000007U    VAL : 0x00000001U
    // .. ..
    EMIT_MASKPOLL(0XF8006054, 0x00000007U),
    // .. .. FINISH: CHECK DDR STATUS
    // .. FINISH: DDR INITIALIZATION
    // FINISH: top
    //
    EMIT_EXIT(),

    //
};

unsigned long ps7_mio_init_data_1_0[] = {
    // START: top
    // .. START: SLCR SETTINGS
    // .. UNLOCK_KEY = 0XDF0D
    // .. ==> 0XF8000008[15:0] = 0x0000DF0DU
    // ..     ==> MASK : 0x0000FFFFU    VAL : 0x0000DF0DU
    // ..
    EMIT_WRITE(0XF8000008, 0x0000DF0DU),
    // .. FINISH: SLCR SETTINGS
    // .. START: OCM REMAPPING
    // .. FINISH: OCM REMAPPING
    // .. START: DDRIOB SETTINGS
    // .. INP_POWER = 0x0
    // .. ==> 0XF8000B40[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. INP_TYPE = 0x0
    // .. ==> 0XF8000B40[2:1] = 0x00000000U
    // ..     ==> MASK : 0x00000006U    VAL : 0x00000000U
    // .. DCI_UPDATE = 0x0
    // .. ==> 0XF8000B40[3:3] = 0x00000000U
    // ..     ==> MASK : 0x00000008U    VAL : 0x00000000U
    // .. TERM_EN = 0x0
    // .. ==> 0XF8000B40[4:4] = 0x00000000U
    // ..     ==> MASK : 0x00000010U    VAL : 0x00000000U
    // .. DCR_TYPE = 0x0
    // .. ==> 0XF8000B40[6:5] = 0x00000000U
    // ..     ==> MASK : 0x00000060U    VAL : 0x00000000U
    // .. IBUF_DISABLE_MODE = 0x0
    // .. ==> 0XF8000B40[7:7] = 0x00000000U
    // ..     ==> MASK : 0x00000080U    VAL : 0x00000000U
    // .. TERM_DISABLE_MODE = 0x0
    // .. ==> 0XF8000B40[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. OUTPUT_EN = 0x3
    // .. ==> 0XF8000B40[10:9] = 0x00000003U
    // ..     ==> MASK : 0x00000600U    VAL : 0x00000600U
    // .. PULLUP_EN = 0x0
    // .. ==> 0XF8000B40[11:11] = 0x00000000U
    // ..     ==> MASK : 0x00000800U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF8000B40, 0x00000FFFU ,0x00000600U),
    // .. INP_POWER = 0x0
    // .. ==> 0XF8000B44[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. INP_TYPE = 0x0
    // .. ==> 0XF8000B44[2:1] = 0x00000000U
    // ..     ==> MASK : 0x00000006U    VAL : 0x00000000U
    // .. DCI_UPDATE = 0x0
    // .. ==> 0XF8000B44[3:3] = 0x00000000U
    // ..     ==> MASK : 0x00000008U    VAL : 0x00000000U
    // .. TERM_EN = 0x0
    // .. ==> 0XF8000B44[4:4] = 0x00000000U
    // ..     ==> MASK : 0x00000010U    VAL : 0x00000000U
    // .. DCR_TYPE = 0x0
    // .. ==> 0XF8000B44[6:5] = 0x00000000U
    // ..     ==> MASK : 0x00000060U    VAL : 0x00000000U
    // .. IBUF_DISABLE_MODE = 0x0
    // .. ==> 0XF8000B44[7:7] = 0x00000000U
    // ..     ==> MASK : 0x00000080U    VAL : 0x00000000U
    // .. TERM_DISABLE_MODE = 0x0
    // .. ==> 0XF8000B44[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. OUTPUT_EN = 0x3
    // .. ==> 0XF8000B44[10:9] = 0x00000003U
    // ..     ==> MASK : 0x00000600U    VAL : 0x00000600U
    // .. PULLUP_EN = 0x0
    // .. ==> 0XF8000B44[11:11] = 0x00000000U
    // ..     ==> MASK : 0x00000800U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF8000B44, 0x00000FFFU ,0x00000600U),
    // .. INP_POWER = 0x0
    // .. ==> 0XF8000B48[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. INP_TYPE = 0x1
    // .. ==> 0XF8000B48[2:1] = 0x00000001U
    // ..     ==> MASK : 0x00000006U    VAL : 0x00000002U
    // .. DCI_UPDATE = 0x0
    // .. ==> 0XF8000B48[3:3] = 0x00000000U
    // ..     ==> MASK : 0x00000008U    VAL : 0x00000000U
    // .. TERM_EN = 0x1
    // .. ==> 0XF8000B48[4:4] = 0x00000001U
    // ..     ==> MASK : 0x00000010U    VAL : 0x00000010U
    // .. DCR_TYPE = 0x3
    // .. ==> 0XF8000B48[6:5] = 0x00000003U
    // ..     ==> MASK : 0x00000060U    VAL : 0x00000060U
    // .. IBUF_DISABLE_MODE = 0
    // .. ==> 0XF8000B48[7:7] = 0x00000000U
    // ..     ==> MASK : 0x00000080U    VAL : 0x00000000U
    // .. TERM_DISABLE_MODE = 0
    // .. ==> 0XF8000B48[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. OUTPUT_EN = 0x3
    // .. ==> 0XF8000B48[10:9] = 0x00000003U
    // ..     ==> MASK : 0x00000600U    VAL : 0x00000600U
    // .. PULLUP_EN = 0x0
    // .. ==> 0XF8000B48[11:11] = 0x00000000U
    // ..     ==> MASK : 0x00000800U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF8000B48, 0x00000FFFU ,0x00000672U),
    // .. INP_POWER = 0x0
    // .. ==> 0XF8000B4C[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. INP_TYPE = 0x1
    // .. ==> 0XF8000B4C[2:1] = 0x00000001U
    // ..     ==> MASK : 0x00000006U    VAL : 0x00000002U
    // .. DCI_UPDATE = 0x0
    // .. ==> 0XF8000B4C[3:3] = 0x00000000U
    // ..     ==> MASK : 0x00000008U    VAL : 0x00000000U
    // .. TERM_EN = 0x1
    // .. ==> 0XF8000B4C[4:4] = 0x00000001U
    // ..     ==> MASK : 0x00000010U    VAL : 0x00000010U
    // .. DCR_TYPE = 0x3
    // .. ==> 0XF8000B4C[6:5] = 0x00000003U
    // ..     ==> MASK : 0x00000060U    VAL : 0x00000060U
    // .. IBUF_DISABLE_MODE = 0
    // .. ==> 0XF8000B4C[7:7] = 0x00000000U
    // ..     ==> MASK : 0x00000080U    VAL : 0x00000000U
    // .. TERM_DISABLE_MODE = 0
    // .. ==> 0XF8000B4C[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. OUTPUT_EN = 0x3
    // .. ==> 0XF8000B4C[10:9] = 0x00000003U
    // ..     ==> MASK : 0x00000600U    VAL : 0x00000600U
    // .. PULLUP_EN = 0x0
    // .. ==> 0XF8000B4C[11:11] = 0x00000000U
    // ..     ==> MASK : 0x00000800U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF8000B4C, 0x00000FFFU ,0x00000672U),
    // .. INP_POWER = 0x0
    // .. ==> 0XF8000B50[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. INP_TYPE = 0x2
    // .. ==> 0XF8000B50[2:1] = 0x00000002U
    // ..     ==> MASK : 0x00000006U    VAL : 0x00000004U
    // .. DCI_UPDATE = 0x0
    // .. ==> 0XF8000B50[3:3] = 0x00000000U
    // ..     ==> MASK : 0x00000008U    VAL : 0x00000000U
    // .. TERM_EN = 0x1
    // .. ==> 0XF8000B50[4:4] = 0x00000001U
    // ..     ==> MASK : 0x00000010U    VAL : 0x00000010U
    // .. DCR_TYPE = 0x3
    // .. ==> 0XF8000B50[6:5] = 0x00000003U
    // ..     ==> MASK : 0x00000060U    VAL : 0x00000060U
    // .. IBUF_DISABLE_MODE = 0
    // .. ==> 0XF8000B50[7:7] = 0x00000000U
    // ..     ==> MASK : 0x00000080U    VAL : 0x00000000U
    // .. TERM_DISABLE_MODE = 0
    // .. ==> 0XF8000B50[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. OUTPUT_EN = 0x3
    // .. ==> 0XF8000B50[10:9] = 0x00000003U
    // ..     ==> MASK : 0x00000600U    VAL : 0x00000600U
    // .. PULLUP_EN = 0x0
    // .. ==> 0XF8000B50[11:11] = 0x00000000U
    // ..     ==> MASK : 0x00000800U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF8000B50, 0x00000FFFU ,0x00000674U),
    // .. INP_POWER = 0x0
    // .. ==> 0XF8000B54[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. INP_TYPE = 0x2
    // .. ==> 0XF8000B54[2:1] = 0x00000002U
    // ..     ==> MASK : 0x00000006U    VAL : 0x00000004U
    // .. DCI_UPDATE = 0x0
    // .. ==> 0XF8000B54[3:3] = 0x00000000U
    // ..     ==> MASK : 0x00000008U    VAL : 0x00000000U
    // .. TERM_EN = 0x1
    // .. ==> 0XF8000B54[4:4] = 0x00000001U
    // ..     ==> MASK : 0x00000010U    VAL : 0x00000010U
    // .. DCR_TYPE = 0x3
    // .. ==> 0XF8000B54[6:5] = 0x00000003U
    // ..     ==> MASK : 0x00000060U    VAL : 0x00000060U
    // .. IBUF_DISABLE_MODE = 0
    // .. ==> 0XF8000B54[7:7] = 0x00000000U
    // ..     ==> MASK : 0x00000080U    VAL : 0x00000000U
    // .. TERM_DISABLE_MODE = 0
    // .. ==> 0XF8000B54[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. OUTPUT_EN = 0x3
    // .. ==> 0XF8000B54[10:9] = 0x00000003U
    // ..     ==> MASK : 0x00000600U    VAL : 0x00000600U
    // .. PULLUP_EN = 0x0
    // .. ==> 0XF8000B54[11:11] = 0x00000000U
    // ..     ==> MASK : 0x00000800U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF8000B54, 0x00000FFFU ,0x00000674U),
    // .. INP_POWER = 0x0
    // .. ==> 0XF8000B58[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. INP_TYPE = 0x0
    // .. ==> 0XF8000B58[2:1] = 0x00000000U
    // ..     ==> MASK : 0x00000006U    VAL : 0x00000000U
    // .. DCI_UPDATE = 0x0
    // .. ==> 0XF8000B58[3:3] = 0x00000000U
    // ..     ==> MASK : 0x00000008U    VAL : 0x00000000U
    // .. TERM_EN = 0x0
    // .. ==> 0XF8000B58[4:4] = 0x00000000U
    // ..     ==> MASK : 0x00000010U    VAL : 0x00000000U
    // .. DCR_TYPE = 0x0
    // .. ==> 0XF8000B58[6:5] = 0x00000000U
    // ..     ==> MASK : 0x00000060U    VAL : 0x00000000U
    // .. IBUF_DISABLE_MODE = 0x0
    // .. ==> 0XF8000B58[7:7] = 0x00000000U
    // ..     ==> MASK : 0x00000080U    VAL : 0x00000000U
    // .. TERM_DISABLE_MODE = 0x0
    // .. ==> 0XF8000B58[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. OUTPUT_EN = 0x3
    // .. ==> 0XF8000B58[10:9] = 0x00000003U
    // ..     ==> MASK : 0x00000600U    VAL : 0x00000600U
    // .. PULLUP_EN = 0x0
    // .. ==> 0XF8000B58[11:11] = 0x00000000U
    // ..     ==> MASK : 0x00000800U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF8000B58, 0x00000FFFU ,0x00000600U),
    // .. DRIVE_P = 0x1c
    // .. ==> 0XF8000B5C[6:0] = 0x0000001CU
    // ..     ==> MASK : 0x0000007FU    VAL : 0x0000001CU
    // .. DRIVE_N = 0xc
    // .. ==> 0XF8000B5C[13:7] = 0x0000000CU
    // ..     ==> MASK : 0x00003F80U    VAL : 0x00000600U
    // .. SLEW_P = 0x3
    // .. ==> 0XF8000B5C[18:14] = 0x00000003U
    // ..     ==> MASK : 0x0007C000U    VAL : 0x0000C000U
    // .. SLEW_N = 0x3
    // .. ==> 0XF8000B5C[23:19] = 0x00000003U
    // ..     ==> MASK : 0x00F80000U    VAL : 0x00180000U
    // .. GTL = 0x0
    // .. ==> 0XF8000B5C[26:24] = 0x00000000U
    // ..     ==> MASK : 0x07000000U    VAL : 0x00000000U
    // .. RTERM = 0x0
    // .. ==> 0XF8000B5C[31:27] = 0x00000000U
    // ..     ==> MASK : 0xF8000000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF8000B5C, 0xFFFFFFFFU ,0x0018C61CU),
    // .. DRIVE_P = 0x1c
    // .. ==> 0XF8000B60[6:0] = 0x0000001CU
    // ..     ==> MASK : 0x0000007FU    VAL : 0x0000001CU
    // .. DRIVE_N = 0xc
    // .. ==> 0XF8000B60[13:7] = 0x0000000CU
    // ..     ==> MASK : 0x00003F80U    VAL : 0x00000600U
    // .. SLEW_P = 0x6
    // .. ==> 0XF8000B60[18:14] = 0x00000006U
    // ..     ==> MASK : 0x0007C000U    VAL : 0x00018000U
    // .. SLEW_N = 0x1f
    // .. ==> 0XF8000B60[23:19] = 0x0000001FU
    // ..     ==> MASK : 0x00F80000U    VAL : 0x00F80000U
    // .. GTL = 0x0
    // .. ==> 0XF8000B60[26:24] = 0x00000000U
    // ..     ==> MASK : 0x07000000U    VAL : 0x00000000U
    // .. RTERM = 0x0
    // .. ==> 0XF8000B60[31:27] = 0x00000000U
    // ..     ==> MASK : 0xF8000000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF8000B60, 0xFFFFFFFFU ,0x00F9861CU),
    // .. DRIVE_P = 0x1c
    // .. ==> 0XF8000B64[6:0] = 0x0000001CU
    // ..     ==> MASK : 0x0000007FU    VAL : 0x0000001CU
    // .. DRIVE_N = 0xc
    // .. ==> 0XF8000B64[13:7] = 0x0000000CU
    // ..     ==> MASK : 0x00003F80U    VAL : 0x00000600U
    // .. SLEW_P = 0x6
    // .. ==> 0XF8000B64[18:14] = 0x00000006U
    // ..     ==> MASK : 0x0007C000U    VAL : 0x00018000U
    // .. SLEW_N = 0x1f
    // .. ==> 0XF8000B64[23:19] = 0x0000001FU
    // ..     ==> MASK : 0x00F80000U    VAL : 0x00F80000U
    // .. GTL = 0x0
    // .. ==> 0XF8000B64[26:24] = 0x00000000U
    // ..     ==> MASK : 0x07000000U    VAL : 0x00000000U
    // .. RTERM = 0x0
    // .. ==> 0XF8000B64[31:27] = 0x00000000U
    // ..     ==> MASK : 0xF8000000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF8000B64, 0xFFFFFFFFU ,0x00F9861CU),
    // .. DRIVE_P = 0x1c
    // .. ==> 0XF8000B68[6:0] = 0x0000001CU
    // ..     ==> MASK : 0x0000007FU    VAL : 0x0000001CU
    // .. DRIVE_N = 0xc
    // .. ==> 0XF8000B68[13:7] = 0x0000000CU
    // ..     ==> MASK : 0x00003F80U    VAL : 0x00000600U
    // .. SLEW_P = 0x6
    // .. ==> 0XF8000B68[18:14] = 0x00000006U
    // ..     ==> MASK : 0x0007C000U    VAL : 0x00018000U
    // .. SLEW_N = 0x1f
    // .. ==> 0XF8000B68[23:19] = 0x0000001FU
    // ..     ==> MASK : 0x00F80000U    VAL : 0x00F80000U
    // .. GTL = 0x0
    // .. ==> 0XF8000B68[26:24] = 0x00000000U
    // ..     ==> MASK : 0x07000000U    VAL : 0x00000000U
    // .. RTERM = 0x0
    // .. ==> 0XF8000B68[31:27] = 0x00000000U
    // ..     ==> MASK : 0xF8000000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF8000B68, 0xFFFFFFFFU ,0x00F9861CU),
    // .. VREF_INT_EN = 0x0
    // .. ==> 0XF8000B6C[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. VREF_SEL = 0x0
    // .. ==> 0XF8000B6C[4:1] = 0x00000000U
    // ..     ==> MASK : 0x0000001EU    VAL : 0x00000000U
    // .. VREF_EXT_EN = 0x3
    // .. ==> 0XF8000B6C[6:5] = 0x00000003U
    // ..     ==> MASK : 0x00000060U    VAL : 0x00000060U
    // .. VREF_PULLUP_EN = 0x0
    // .. ==> 0XF8000B6C[8:7] = 0x00000000U
    // ..     ==> MASK : 0x00000180U    VAL : 0x00000000U
    // .. REFIO_EN = 0x1
    // .. ==> 0XF8000B6C[9:9] = 0x00000001U
    // ..     ==> MASK : 0x00000200U    VAL : 0x00000200U
    // .. REFIO_PULLUP_EN = 0x0
    // .. ==> 0XF8000B6C[12:12] = 0x00000000U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00000000U
    // .. DRST_B_PULLUP_EN = 0x0
    // .. ==> 0XF8000B6C[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // .. CKE_PULLUP_EN = 0x0
    // .. ==> 0XF8000B6C[14:14] = 0x00000000U
    // ..     ==> MASK : 0x00004000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF8000B6C, 0x000073FFU ,0x00000260U),
    // .. .. START: ASSERT RESET
    // .. .. RESET = 1
    // .. .. ==> 0XF8000B70[0:0] = 0x00000001U
    // .. ..     ==> MASK : 0x00000001U    VAL : 0x00000001U
    // .. .. VRN_OUT = 0x1
    // .. .. ==> 0XF8000B70[5:5] = 0x00000001U
    // .. ..     ==> MASK : 0x00000020U    VAL : 0x00000020U
    // .. ..
    EMIT_MASKWRITE(0XF8000B70, 0x00000021U ,0x00000021U),
    // .. .. FINISH: ASSERT RESET
    // .. .. START: DEASSERT RESET
    // .. .. RESET = 0
    // .. .. ==> 0XF8000B70[0:0] = 0x00000000U
    // .. ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. .. VRN_OUT = 0x1
    // .. .. ==> 0XF8000B70[5:5] = 0x00000001U
    // .. ..     ==> MASK : 0x00000020U    VAL : 0x00000020U
    // .. ..
    EMIT_MASKWRITE(0XF8000B70, 0x00000021U ,0x00000020U),
    // .. .. FINISH: DEASSERT RESET
    // .. .. RESET = 0x1
    // .. .. ==> 0XF8000B70[0:0] = 0x00000001U
    // .. ..     ==> MASK : 0x00000001U    VAL : 0x00000001U
    // .. .. ENABLE = 0x1
    // .. .. ==> 0XF8000B70[1:1] = 0x00000001U
    // .. ..     ==> MASK : 0x00000002U    VAL : 0x00000002U
    // .. .. VRP_TRI = 0x0
    // .. .. ==> 0XF8000B70[2:2] = 0x00000000U
    // .. ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. .. VRN_TRI = 0x0
    // .. .. ==> 0XF8000B70[3:3] = 0x00000000U
    // .. ..     ==> MASK : 0x00000008U    VAL : 0x00000000U
    // .. .. VRP_OUT = 0x0
    // .. .. ==> 0XF8000B70[4:4] = 0x00000000U
    // .. ..     ==> MASK : 0x00000010U    VAL : 0x00000000U
    // .. .. VRN_OUT = 0x1
    // .. .. ==> 0XF8000B70[5:5] = 0x00000001U
    // .. ..     ==> MASK : 0x00000020U    VAL : 0x00000020U
    // .. .. NREF_OPT1 = 0x0
    // .. .. ==> 0XF8000B70[7:6] = 0x00000000U
    // .. ..     ==> MASK : 0x000000C0U    VAL : 0x00000000U
    // .. .. NREF_OPT2 = 0x0
    // .. .. ==> 0XF8000B70[10:8] = 0x00000000U
    // .. ..     ==> MASK : 0x00000700U    VAL : 0x00000000U
    // .. .. NREF_OPT4 = 0x1
    // .. .. ==> 0XF8000B70[13:11] = 0x00000001U
    // .. ..     ==> MASK : 0x00003800U    VAL : 0x00000800U
    // .. .. PREF_OPT1 = 0x0
    // .. .. ==> 0XF8000B70[16:14] = 0x00000000U
    // .. ..     ==> MASK : 0x0001C000U    VAL : 0x00000000U
    // .. .. PREF_OPT2 = 0x0
    // .. .. ==> 0XF8000B70[19:17] = 0x00000000U
    // .. ..     ==> MASK : 0x000E0000U    VAL : 0x00000000U
    // .. .. UPDATE_CONTROL = 0x0
    // .. .. ==> 0XF8000B70[20:20] = 0x00000000U
    // .. ..     ==> MASK : 0x00100000U    VAL : 0x00000000U
    // .. .. INIT_COMPLETE = 0x0
    // .. .. ==> 0XF8000B70[21:21] = 0x00000000U
    // .. ..     ==> MASK : 0x00200000U    VAL : 0x00000000U
    // .. .. TST_CLK = 0x0
    // .. .. ==> 0XF8000B70[22:22] = 0x00000000U
    // .. ..     ==> MASK : 0x00400000U    VAL : 0x00000000U
    // .. .. TST_HLN = 0x0
    // .. .. ==> 0XF8000B70[23:23] = 0x00000000U
    // .. ..     ==> MASK : 0x00800000U    VAL : 0x00000000U
    // .. .. TST_HLP = 0x0
    // .. .. ==> 0XF8000B70[24:24] = 0x00000000U
    // .. ..     ==> MASK : 0x01000000U    VAL : 0x00000000U
    // .. .. TST_RST = 0x0
    // .. .. ==> 0XF8000B70[25:25] = 0x00000000U
    // .. ..     ==> MASK : 0x02000000U    VAL : 0x00000000U
    // .. .. INT_DCI_EN = 0x0
    // .. .. ==> 0XF8000B70[26:26] = 0x00000000U
    // .. ..     ==> MASK : 0x04000000U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XF8000B70, 0x07FFFFFFU ,0x00000823U),
    // .. FINISH: DDRIOB SETTINGS
    // .. START: MIO PROGRAMMING
    // .. TRI_ENABLE = 0
    // .. ==> 0XF8000700[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF8000700[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF8000700[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 2
    // .. ==> 0XF8000700[4:3] = 0x00000002U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000010U
    // .. L3_SEL = 0
    // .. ==> 0XF8000700[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF8000700[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF8000700[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF8000700[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF8000700[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF8000700, 0x00003FFFU ,0x00001610U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF8000704[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF8000704[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF8000704[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF8000704[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF8000704[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF8000704[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF8000704[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF8000704[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF8000704[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF8000704, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF8000708[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF8000708[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF8000708[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 2
    // .. ==> 0XF8000708[4:3] = 0x00000002U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000010U
    // .. L3_SEL = 0
    // .. ==> 0XF8000708[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF8000708[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF8000708[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 0
    // .. ==> 0XF8000708[12:12] = 0x00000000U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00000000U
    // .. DisableRcvr = 0
    // .. ==> 0XF8000708[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF8000708, 0x00003FFFU ,0x00000610U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF800070C[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF800070C[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF800070C[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 2
    // .. ==> 0XF800070C[4:3] = 0x00000002U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000010U
    // .. L3_SEL = 0
    // .. ==> 0XF800070C[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF800070C[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF800070C[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 0
    // .. ==> 0XF800070C[12:12] = 0x00000000U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00000000U
    // .. DisableRcvr = 0
    // .. ==> 0XF800070C[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF800070C, 0x00003FFFU ,0x00000610U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF8000710[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF8000710[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF8000710[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 2
    // .. ==> 0XF8000710[4:3] = 0x00000002U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000010U
    // .. L3_SEL = 0
    // .. ==> 0XF8000710[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF8000710[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF8000710[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 0
    // .. ==> 0XF8000710[12:12] = 0x00000000U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00000000U
    // .. DisableRcvr = 0
    // .. ==> 0XF8000710[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF8000710, 0x00003FFFU ,0x00000610U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF8000714[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF8000714[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF8000714[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 2
    // .. ==> 0XF8000714[4:3] = 0x00000002U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000010U
    // .. L3_SEL = 0
    // .. ==> 0XF8000714[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF8000714[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF8000714[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 0
    // .. ==> 0XF8000714[12:12] = 0x00000000U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00000000U
    // .. DisableRcvr = 0
    // .. ==> 0XF8000714[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF8000714, 0x00003FFFU ,0x00000610U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF8000718[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF8000718[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF8000718[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 2
    // .. ==> 0XF8000718[4:3] = 0x00000002U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000010U
    // .. L3_SEL = 0
    // .. ==> 0XF8000718[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF8000718[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF8000718[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 0
    // .. ==> 0XF8000718[12:12] = 0x00000000U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00000000U
    // .. DisableRcvr = 0
    // .. ==> 0XF8000718[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF8000718, 0x00003FFFU ,0x00000610U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF800071C[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF800071C[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF800071C[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 2
    // .. ==> 0XF800071C[4:3] = 0x00000002U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000010U
    // .. L3_SEL = 0
    // .. ==> 0XF800071C[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF800071C[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF800071C[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 0
    // .. ==> 0XF800071C[12:12] = 0x00000000U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00000000U
    // .. DisableRcvr = 0
    // .. ==> 0XF800071C[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF800071C, 0x00003FFFU ,0x00000610U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF8000720[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF8000720[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF8000720[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 2
    // .. ==> 0XF8000720[4:3] = 0x00000002U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000010U
    // .. L3_SEL = 0
    // .. ==> 0XF8000720[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF8000720[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF8000720[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 0
    // .. ==> 0XF8000720[12:12] = 0x00000000U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00000000U
    // .. DisableRcvr = 0
    // .. ==> 0XF8000720[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF8000720, 0x00003FFFU ,0x00000610U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF8000724[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF8000724[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF8000724[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 2
    // .. ==> 0XF8000724[4:3] = 0x00000002U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000010U
    // .. L3_SEL = 0
    // .. ==> 0XF8000724[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF8000724[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF8000724[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF8000724[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF8000724[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF8000724, 0x00003FFFU ,0x00001610U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF8000728[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF8000728[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF8000728[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 2
    // .. ==> 0XF8000728[4:3] = 0x00000002U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000010U
    // .. L3_SEL = 0
    // .. ==> 0XF8000728[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF8000728[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF8000728[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF8000728[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF8000728[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF8000728, 0x00003FFFU ,0x00001610U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF800072C[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF800072C[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF800072C[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 2
    // .. ==> 0XF800072C[4:3] = 0x00000002U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000010U
    // .. L3_SEL = 0
    // .. ==> 0XF800072C[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF800072C[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF800072C[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF800072C[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF800072C[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF800072C, 0x00003FFFU ,0x00001610U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF8000730[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF8000730[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF8000730[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 2
    // .. ==> 0XF8000730[4:3] = 0x00000002U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000010U
    // .. L3_SEL = 0
    // .. ==> 0XF8000730[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF8000730[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF8000730[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF8000730[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF8000730[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF8000730, 0x00003FFFU ,0x00001610U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF8000734[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF8000734[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF8000734[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 2
    // .. ==> 0XF8000734[4:3] = 0x00000002U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000010U
    // .. L3_SEL = 0
    // .. ==> 0XF8000734[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF8000734[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF8000734[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF8000734[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF8000734[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF8000734, 0x00003FFFU ,0x00001610U),
    // .. TRI_ENABLE = 1
    // .. ==> 0XF8000738[0:0] = 0x00000001U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000001U
    // .. L0_SEL = 0
    // .. ==> 0XF8000738[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF8000738[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 2
    // .. ==> 0XF8000738[4:3] = 0x00000002U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000010U
    // .. L3_SEL = 0
    // .. ==> 0XF8000738[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF8000738[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF8000738[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF8000738[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF8000738[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF8000738, 0x00003FFFU ,0x00001611U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF800073C[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF800073C[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF800073C[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF800073C[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF800073C[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF800073C[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF800073C[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF800073C[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF800073C[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF800073C, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF8000740[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF8000740[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF8000740[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF8000740[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF8000740[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF8000740[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF8000740[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF8000740[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF8000740[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF8000740, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF8000744[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF8000744[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF8000744[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF8000744[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF8000744[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF8000744[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF8000744[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF8000744[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF8000744[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF8000744, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF8000748[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF8000748[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF8000748[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF8000748[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF8000748[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF8000748[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF8000748[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF8000748[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF8000748[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF8000748, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF800074C[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF800074C[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF800074C[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF800074C[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF800074C[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF800074C[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF800074C[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF800074C[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF800074C[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF800074C, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF8000750[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF8000750[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF8000750[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF8000750[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF8000750[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF8000750[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF8000750[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF8000750[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF8000750[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF8000750, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF8000754[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF8000754[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF8000754[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF8000754[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF8000754[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF8000754[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF8000754[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF8000754[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF8000754[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF8000754, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF8000758[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF8000758[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF8000758[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF8000758[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF8000758[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF8000758[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF8000758[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF8000758[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF8000758[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF8000758, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF800075C[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF800075C[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF800075C[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF800075C[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF800075C[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF800075C[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF800075C[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF800075C[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF800075C[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF800075C, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF8000760[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF8000760[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF8000760[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF8000760[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 7
    // .. ==> 0XF8000760[7:5] = 0x00000007U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x000000E0U
    // .. Speed = 0
    // .. ==> 0XF8000760[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF8000760[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF8000760[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF8000760[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF8000760, 0x00003FFFU ,0x000016E0U),
    // .. TRI_ENABLE = 1
    // .. ==> 0XF8000764[0:0] = 0x00000001U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000001U
    // .. L0_SEL = 0
    // .. ==> 0XF8000764[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF8000764[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF8000764[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 7
    // .. ==> 0XF8000764[7:5] = 0x00000007U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x000000E0U
    // .. Speed = 0
    // .. ==> 0XF8000764[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF8000764[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF8000764[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF8000764[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF8000764, 0x00003FFFU ,0x000016E1U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF8000768[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF8000768[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF8000768[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF8000768[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF8000768[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 1
    // .. ==> 0XF8000768[8:8] = 0x00000001U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000100U
    // .. IO_Type = 3
    // .. ==> 0XF8000768[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 0
    // .. ==> 0XF8000768[12:12] = 0x00000000U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00000000U
    // .. DisableRcvr = 0
    // .. ==> 0XF8000768[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF8000768, 0x00003FFFU ,0x00000700U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF800076C[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF800076C[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF800076C[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF800076C[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF800076C[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 1
    // .. ==> 0XF800076C[8:8] = 0x00000001U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000100U
    // .. IO_Type = 3
    // .. ==> 0XF800076C[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 0
    // .. ==> 0XF800076C[12:12] = 0x00000000U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00000000U
    // .. DisableRcvr = 0
    // .. ==> 0XF800076C[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF800076C, 0x00003FFFU ,0x00000700U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF8000770[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF8000770[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF8000770[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF8000770[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF8000770[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF8000770[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF8000770[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF8000770[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF8000770[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF8000770, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF8000774[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF8000774[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF8000774[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF8000774[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF8000774[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF8000774[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF8000774[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF8000774[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF8000774[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF8000774, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF8000778[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF8000778[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF8000778[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF8000778[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF8000778[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF8000778[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF8000778[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF8000778[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF8000778[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF8000778, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF800077C[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF800077C[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF800077C[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF800077C[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF800077C[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF800077C[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF800077C[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF800077C[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF800077C[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF800077C, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF8000780[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF8000780[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF8000780[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF8000780[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF8000780[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF8000780[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF8000780[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF8000780[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF8000780[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF8000780, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF8000784[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF8000784[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF8000784[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF8000784[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF8000784[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF8000784[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF8000784[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF8000784[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF8000784[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF8000784, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF8000788[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF8000788[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF8000788[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF8000788[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF8000788[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF8000788[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF8000788[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF8000788[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF8000788[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF8000788, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF800078C[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF800078C[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF800078C[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF800078C[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF800078C[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF800078C[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF800078C[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF800078C[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF800078C[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF800078C, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF8000790[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF8000790[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF8000790[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF8000790[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF8000790[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF8000790[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF8000790[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF8000790[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF8000790[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF8000790, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF8000794[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF8000794[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF8000794[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF8000794[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF8000794[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF8000794[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF8000794[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF8000794[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF8000794[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF8000794, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF8000798[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF8000798[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF8000798[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF8000798[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF8000798[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF8000798[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF8000798[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF8000798[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF8000798[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF8000798, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF800079C[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF800079C[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF800079C[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF800079C[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF800079C[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF800079C[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF800079C[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF800079C[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF800079C[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF800079C, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF80007A0[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF80007A0[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF80007A0[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF80007A0[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF80007A0[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF80007A0[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF80007A0[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF80007A0[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF80007A0[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF80007A0, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF80007A4[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF80007A4[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF80007A4[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF80007A4[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF80007A4[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF80007A4[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF80007A4[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF80007A4[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF80007A4[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF80007A4, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF80007A8[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF80007A8[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF80007A8[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF80007A8[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF80007A8[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF80007A8[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF80007A8[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF80007A8[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF80007A8[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF80007A8, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF80007AC[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF80007AC[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF80007AC[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF80007AC[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF80007AC[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF80007AC[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF80007AC[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF80007AC[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF80007AC[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF80007AC, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF80007B0[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF80007B0[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF80007B0[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF80007B0[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF80007B0[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF80007B0[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF80007B0[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF80007B0[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF80007B0[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF80007B0, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF80007B4[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF80007B4[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF80007B4[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF80007B4[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF80007B4[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF80007B4[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF80007B4[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF80007B4[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF80007B4[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF80007B4, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF80007B8[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF80007B8[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF80007B8[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF80007B8[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF80007B8[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF80007B8[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF80007B8[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF80007B8[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF80007B8[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF80007B8, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF80007BC[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF80007BC[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF80007BC[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF80007BC[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF80007BC[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF80007BC[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF80007BC[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF80007BC[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF80007BC[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF80007BC, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF80007C0[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF80007C0[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF80007C0[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF80007C0[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF80007C0[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF80007C0[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF80007C0[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF80007C0[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF80007C0[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF80007C0, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF80007C4[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF80007C4[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF80007C4[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF80007C4[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF80007C4[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF80007C4[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF80007C4[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF80007C4[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF80007C4[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF80007C4, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF80007C8[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF80007C8[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF80007C8[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF80007C8[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF80007C8[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF80007C8[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF80007C8[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF80007C8[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF80007C8[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF80007C8, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF80007CC[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF80007CC[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF80007CC[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF80007CC[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF80007CC[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF80007CC[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF80007CC[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF80007CC[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF80007CC[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF80007CC, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF80007D0[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF80007D0[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF80007D0[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF80007D0[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF80007D0[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF80007D0[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF80007D0[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF80007D0[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF80007D0[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF80007D0, 0x00003FFFU ,0x00001600U),
    // .. TRI_ENABLE = 0
    // .. ==> 0XF80007D4[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // .. L0_SEL = 0
    // .. ==> 0XF80007D4[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. L1_SEL = 0
    // .. ==> 0XF80007D4[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. L2_SEL = 0
    // .. ==> 0XF80007D4[4:3] = 0x00000000U
    // ..     ==> MASK : 0x00000018U    VAL : 0x00000000U
    // .. L3_SEL = 0
    // .. ==> 0XF80007D4[7:5] = 0x00000000U
    // ..     ==> MASK : 0x000000E0U    VAL : 0x00000000U
    // .. Speed = 0
    // .. ==> 0XF80007D4[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. IO_Type = 3
    // .. ==> 0XF80007D4[11:9] = 0x00000003U
    // ..     ==> MASK : 0x00000E00U    VAL : 0x00000600U
    // .. PULLUP = 1
    // .. ==> 0XF80007D4[12:12] = 0x00000001U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00001000U
    // .. DisableRcvr = 0
    // .. ==> 0XF80007D4[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF80007D4, 0x00003FFFU ,0x00001600U),
    // .. FINISH: MIO PROGRAMMING
    // .. START: LOCK IT BACK
    // .. LOCK_KEY = 0X767B
    // .. ==> 0XF8000004[15:0] = 0x0000767BU
    // ..     ==> MASK : 0x0000FFFFU    VAL : 0x0000767BU
    // ..
    //EMIT_WRITE(0XF8000004, 0x0000767BU),
    // .. FINISH: LOCK IT BACK
    // FINISH: top
    //
    EMIT_EXIT(),

    //
};

unsigned long ps7_peripherals_init_data_1_0[] = {
    // START: top
    // .. START: SLCR SETTINGS
    // .. UNLOCK_KEY = 0XDF0D
    // .. ==> 0XF8000008[15:0] = 0x0000DF0DU
    // ..     ==> MASK : 0x0000FFFFU    VAL : 0x0000DF0DU
    // ..
    EMIT_WRITE(0XF8000008, 0x0000DF0DU),
    // .. FINISH: SLCR SETTINGS
    // .. START: DDR TERM/IBUF_DISABLE_MODE SETTINGS
    // .. IBUF_DISABLE_MODE = 0x1
    // .. ==> 0XF8000B48[7:7] = 0x00000001U
    // ..     ==> MASK : 0x00000080U    VAL : 0x00000080U
    // .. TERM_DISABLE_MODE = 0x1
    // .. ==> 0XF8000B48[8:8] = 0x00000001U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000100U
    // ..
    EMIT_MASKWRITE(0XF8000B48, 0x00000180U ,0x00000180U),
    // .. IBUF_DISABLE_MODE = 0x1
    // .. ==> 0XF8000B4C[7:7] = 0x00000001U
    // ..     ==> MASK : 0x00000080U    VAL : 0x00000080U
    // .. TERM_DISABLE_MODE = 0x1
    // .. ==> 0XF8000B4C[8:8] = 0x00000001U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000100U
    // ..
    EMIT_MASKWRITE(0XF8000B4C, 0x00000180U ,0x00000180U),
    // .. IBUF_DISABLE_MODE = 0x1
    // .. ==> 0XF8000B50[7:7] = 0x00000001U
    // ..     ==> MASK : 0x00000080U    VAL : 0x00000080U
    // .. TERM_DISABLE_MODE = 0x1
    // .. ==> 0XF8000B50[8:8] = 0x00000001U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000100U
    // ..
    EMIT_MASKWRITE(0XF8000B50, 0x00000180U ,0x00000180U),
    // .. IBUF_DISABLE_MODE = 0x1
    // .. ==> 0XF8000B54[7:7] = 0x00000001U
    // ..     ==> MASK : 0x00000080U    VAL : 0x00000080U
    // .. TERM_DISABLE_MODE = 0x1
    // .. ==> 0XF8000B54[8:8] = 0x00000001U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000100U
    // ..
    EMIT_MASKWRITE(0XF8000B54, 0x00000180U ,0x00000180U),
    // .. FINISH: DDR TERM/IBUF_DISABLE_MODE SETTINGS
    // .. START: LOCK IT BACK
    // .. LOCK_KEY = 0X767B
    // .. ==> 0XF8000004[15:0] = 0x0000767BU
    // ..     ==> MASK : 0x0000FFFFU    VAL : 0x0000767BU
    // ..
    //EMIT_WRITE(0XF8000004, 0x0000767BU),
    // .. FINISH: LOCK IT BACK
    // .. START: SRAM/NOR SET OPMODE
    // .. FINISH: SRAM/NOR SET OPMODE
    // .. START: UART REGISTERS
    // .. BDIV = 0x6
    // .. ==> 0XE0001034[7:0] = 0x00000006U
    // ..     ==> MASK : 0x000000FFU    VAL : 0x00000006U
    // ..
    EMIT_MASKWRITE(0XE0001034, 0x000000FFU ,0x00000006U),
    // .. CD = 0x7c
    // .. ==> 0XE0001018[15:0] = 0x0000007CU
    // ..     ==> MASK : 0x0000FFFFU    VAL : 0x0000007CU
    // ..
    EMIT_MASKWRITE(0XE0001018, 0x0000FFFFU ,0x0000007CU),
    // .. STPBRK = 0x0
    // .. ==> 0XE0001000[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. STTBRK = 0x0
    // .. ==> 0XE0001000[7:7] = 0x00000000U
    // ..     ==> MASK : 0x00000080U    VAL : 0x00000000U
    // .. RSTTO = 0x0
    // .. ==> 0XE0001000[6:6] = 0x00000000U
    // ..     ==> MASK : 0x00000040U    VAL : 0x00000000U
    // .. TXDIS = 0x0
    // .. ==> 0XE0001000[5:5] = 0x00000000U
    // ..     ==> MASK : 0x00000020U    VAL : 0x00000000U
    // .. TXEN = 0x1
    // .. ==> 0XE0001000[4:4] = 0x00000001U
    // ..     ==> MASK : 0x00000010U    VAL : 0x00000010U
    // .. RXDIS = 0x0
    // .. ==> 0XE0001000[3:3] = 0x00000000U
    // ..     ==> MASK : 0x00000008U    VAL : 0x00000000U
    // .. RXEN = 0x1
    // .. ==> 0XE0001000[2:2] = 0x00000001U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000004U
    // .. TXRES = 0x1
    // .. ==> 0XE0001000[1:1] = 0x00000001U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000002U
    // .. RXRES = 0x1
    // .. ==> 0XE0001000[0:0] = 0x00000001U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000001U
    // ..
    EMIT_MASKWRITE(0XE0001000, 0x000001FFU ,0x00000017U),
    // .. IRMODE = 0x0
    // .. ==> 0XE0001004[11:11] = 0x00000000U
    // ..     ==> MASK : 0x00000800U    VAL : 0x00000000U
    // .. UCLKEN = 0x0
    // .. ==> 0XE0001004[10:10] = 0x00000000U
    // ..     ==> MASK : 0x00000400U    VAL : 0x00000000U
    // .. CHMODE = 0x0
    // .. ==> 0XE0001004[9:8] = 0x00000000U
    // ..     ==> MASK : 0x00000300U    VAL : 0x00000000U
    // .. NBSTOP = 0x0
    // .. ==> 0XE0001004[7:6] = 0x00000000U
    // ..     ==> MASK : 0x000000C0U    VAL : 0x00000000U
    // .. PAR = 0x4
    // .. ==> 0XE0001004[5:3] = 0x00000004U
    // ..     ==> MASK : 0x00000038U    VAL : 0x00000020U
    // .. CHRL = 0x0
    // .. ==> 0XE0001004[2:1] = 0x00000000U
    // ..     ==> MASK : 0x00000006U    VAL : 0x00000000U
    // .. CLKS = 0x0
    // .. ==> 0XE0001004[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XE0001004, 0x00000FFFU ,0x00000020U),
    // .. FINISH: UART REGISTERS
    // .. START: QSPI REGISTERS
    // .. Holdb_dr = 1
    // .. ==> 0XE000D000[19:19] = 0x00000001U
    // ..     ==> MASK : 0x00080000U    VAL : 0x00080000U
    // ..
    EMIT_MASKWRITE(0XE000D000, 0x00080000U ,0x00080000U),
    // .. FINISH: QSPI REGISTERS
    // .. START: PL POWER ON RESET REGISTERS
    // .. PCFG_POR_CNT_4K = 0
    // .. ==> 0XF8007000[29:29] = 0x00000000U
    // ..     ==> MASK : 0x20000000U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF8007000, 0x20000000U ,0x00000000U),
    // .. FINISH: PL POWER ON RESET REGISTERS
    // .. START: SMC TIMING CALCULATION REGISTER UPDATE
    // .. .. START: NAND SET CYCLE
    // .. .. Set_t0 = 0x2
    // .. .. ==> 0XE000E014[3:0] = 0x00000002U
    // .. ..     ==> MASK : 0x0000000FU    VAL : 0x00000002U
    // .. .. Set_t1 = 0x2
    // .. .. ==> 0XE000E014[7:4] = 0x00000002U
    // .. ..     ==> MASK : 0x000000F0U    VAL : 0x00000020U
    // .. .. Set_t2 = 0x1
    // .. .. ==> 0XE000E014[10:8] = 0x00000001U
    // .. ..     ==> MASK : 0x00000700U    VAL : 0x00000100U
    // .. .. Set_t3 = 0x1
    // .. .. ==> 0XE000E014[13:11] = 0x00000001U
    // .. ..     ==> MASK : 0x00003800U    VAL : 0x00000800U
    // .. .. Set_t4 = 0x1
    // .. .. ==> 0XE000E014[16:14] = 0x00000001U
    // .. ..     ==> MASK : 0x0001C000U    VAL : 0x00004000U
    // .. .. Set_t5 = 0x1
    // .. .. ==> 0XE000E014[19:17] = 0x00000001U
    // .. ..     ==> MASK : 0x000E0000U    VAL : 0x00020000U
    // .. .. Set_t6 = 0x1
    // .. .. ==> 0XE000E014[23:20] = 0x00000001U
    // .. ..     ==> MASK : 0x00F00000U    VAL : 0x00100000U
    // .. ..
    EMIT_WRITE(0XE000E014, 0x00124922U),
    // .. .. FINISH: NAND SET CYCLE
    // .. .. START: OPMODE
    // .. .. set_mw = 0x0
    // .. .. ==> 0XE000E018[1:0] = 0x00000000U
    // .. ..     ==> MASK : 0x00000003U    VAL : 0x00000000U
    // .. ..
    EMIT_MASKWRITE(0XE000E018, 0x00000003U ,0x00000000U),
    // .. .. FINISH: OPMODE
    // .. .. START: DIRECT COMMAND
    // .. .. chip_select = 0x4
    // .. .. ==> 0XE000E010[25:23] = 0x00000004U
    // .. ..     ==> MASK : 0x03800000U    VAL : 0x02000000U
    // .. .. cmd_type = 0x2
    // .. .. ==> 0XE000E010[22:21] = 0x00000002U
    // .. ..     ==> MASK : 0x00600000U    VAL : 0x00400000U
    // .. ..
    EMIT_WRITE(0XE000E010, 0x02400000U),
    // .. .. FINISH: DIRECT COMMAND
    // .. .. START: SRAM/NOR CS0 SET CYCLE
    // .. .. FINISH: SRAM/NOR CS0 SET CYCLE
    // .. .. START: DIRECT COMMAND
    // .. .. FINISH: DIRECT COMMAND
    // .. .. START: NOR CS0 BASE ADDRESS
    // .. .. FINISH: NOR CS0 BASE ADDRESS
    // .. .. START: SRAM/NOR CS1 SET CYCLE
    // .. .. FINISH: SRAM/NOR CS1 SET CYCLE
    // .. .. START: DIRECT COMMAND
    // .. .. FINISH: DIRECT COMMAND
    // .. .. START: NOR CS1 BASE ADDRESS
    // .. .. FINISH: NOR CS1 BASE ADDRESS
    // .. .. START: USB RESET
    // .. .. FINISH: USB RESET
    // .. .. START: ENET RESET
    // .. .. FINISH: ENET RESET
    // .. .. START: I2C RESET
    // .. .. FINISH: I2C RESET
    // .. .. START: NOR CHIP SELECT
    // .. .. .. START: DIR MODE BANK 0
    // .. .. .. FINISH: DIR MODE BANK 0
    // .. .. .. START: MASK_DATA_0_LSW HIGH BANK [15:0]
    // .. .. .. FINISH: MASK_DATA_0_LSW HIGH BANK [15:0]
    // .. .. .. START: OUTPUT ENABLE BANK 0
    // .. .. .. FINISH: OUTPUT ENABLE BANK 0
    // .. .. FINISH: NOR CHIP SELECT
    // .. FINISH: SMC TIMING CALCULATION REGISTER UPDATE
    // FINISH: top
    //
    EMIT_EXIT(),

    //
};

unsigned long ps7_post_config_1_0[] = {
    // START: top
    // .. START: SLCR SETTINGS
    // .. UNLOCK_KEY = 0XDF0D
    // .. ==> 0XF8000008[15:0] = 0x0000DF0DU
    // ..     ==> MASK : 0x0000FFFFU    VAL : 0x0000DF0DU
    // ..
    EMIT_WRITE(0XF8000008, 0x0000DF0DU),
    // .. FINISH: SLCR SETTINGS
    // .. START: ENABLING LEVEL SHIFTER
    // .. USER_INP_ICT_EN_0 = 3
    // .. ==> 0XF8000900[1:0] = 0x00000003U
    // ..     ==> MASK : 0x00000003U    VAL : 0x00000003U
    // .. USER_INP_ICT_EN_1 = 3
    // .. ==> 0XF8000900[3:2] = 0x00000003U
    // ..     ==> MASK : 0x0000000CU    VAL : 0x0000000CU
    // ..
    EMIT_MASKWRITE(0XF8000900, 0x0000000FU ,0x0000000FU),
    // .. FINISH: ENABLING LEVEL SHIFTER
    // .. START: FPGA RESETS TO 0
    // .. reserved_3 = 0
    // .. ==> 0XF8000240[31:25] = 0x00000000U
    // ..     ==> MASK : 0xFE000000U    VAL : 0x00000000U
    // .. FPGA_ACP_RST = 0
    // .. ==> 0XF8000240[24:24] = 0x00000000U
    // ..     ==> MASK : 0x01000000U    VAL : 0x00000000U
    // .. FPGA_AXDS3_RST = 0
    // .. ==> 0XF8000240[23:23] = 0x00000000U
    // ..     ==> MASK : 0x00800000U    VAL : 0x00000000U
    // .. FPGA_AXDS2_RST = 0
    // .. ==> 0XF8000240[22:22] = 0x00000000U
    // ..     ==> MASK : 0x00400000U    VAL : 0x00000000U
    // .. FPGA_AXDS1_RST = 0
    // .. ==> 0XF8000240[21:21] = 0x00000000U
    // ..     ==> MASK : 0x00200000U    VAL : 0x00000000U
    // .. FPGA_AXDS0_RST = 0
    // .. ==> 0XF8000240[20:20] = 0x00000000U
    // ..     ==> MASK : 0x00100000U    VAL : 0x00000000U
    // .. reserved_2 = 0
    // .. ==> 0XF8000240[19:18] = 0x00000000U
    // ..     ==> MASK : 0x000C0000U    VAL : 0x00000000U
    // .. FSSW1_FPGA_RST = 0
    // .. ==> 0XF8000240[17:17] = 0x00000000U
    // ..     ==> MASK : 0x00020000U    VAL : 0x00000000U
    // .. FSSW0_FPGA_RST = 0
    // .. ==> 0XF8000240[16:16] = 0x00000000U
    // ..     ==> MASK : 0x00010000U    VAL : 0x00000000U
    // .. reserved_1 = 0
    // .. ==> 0XF8000240[15:14] = 0x00000000U
    // ..     ==> MASK : 0x0000C000U    VAL : 0x00000000U
    // .. FPGA_FMSW1_RST = 0
    // .. ==> 0XF8000240[13:13] = 0x00000000U
    // ..     ==> MASK : 0x00002000U    VAL : 0x00000000U
    // .. FPGA_FMSW0_RST = 0
    // .. ==> 0XF8000240[12:12] = 0x00000000U
    // ..     ==> MASK : 0x00001000U    VAL : 0x00000000U
    // .. FPGA_DMA3_RST = 0
    // .. ==> 0XF8000240[11:11] = 0x00000000U
    // ..     ==> MASK : 0x00000800U    VAL : 0x00000000U
    // .. FPGA_DMA2_RST = 0
    // .. ==> 0XF8000240[10:10] = 0x00000000U
    // ..     ==> MASK : 0x00000400U    VAL : 0x00000000U
    // .. FPGA_DMA1_RST = 0
    // .. ==> 0XF8000240[9:9] = 0x00000000U
    // ..     ==> MASK : 0x00000200U    VAL : 0x00000000U
    // .. FPGA_DMA0_RST = 0
    // .. ==> 0XF8000240[8:8] = 0x00000000U
    // ..     ==> MASK : 0x00000100U    VAL : 0x00000000U
    // .. reserved = 0
    // .. ==> 0XF8000240[7:4] = 0x00000000U
    // ..     ==> MASK : 0x000000F0U    VAL : 0x00000000U
    // .. FPGA3_OUT_RST = 0
    // .. ==> 0XF8000240[3:3] = 0x00000000U
    // ..     ==> MASK : 0x00000008U    VAL : 0x00000000U
    // .. FPGA2_OUT_RST = 0
    // .. ==> 0XF8000240[2:2] = 0x00000000U
    // ..     ==> MASK : 0x00000004U    VAL : 0x00000000U
    // .. FPGA1_OUT_RST = 0
    // .. ==> 0XF8000240[1:1] = 0x00000000U
    // ..     ==> MASK : 0x00000002U    VAL : 0x00000000U
    // .. FPGA0_OUT_RST = 0
    // .. ==> 0XF8000240[0:0] = 0x00000000U
    // ..     ==> MASK : 0x00000001U    VAL : 0x00000000U
    // ..
    EMIT_MASKWRITE(0XF8000240, 0xFFFFFFFFU ,0x00000000U),
    // .. FINISH: FPGA RESETS TO 0
    // .. START: AFI REGISTERS
    // .. .. START: AFI0 REGISTERS
    // .. .. FINISH: AFI0 REGISTERS
    // .. .. START: AFI1 REGISTERS
    // .. .. FINISH: AFI1 REGISTERS
    // .. .. START: AFI2 REGISTERS
    // .. .. FINISH: AFI2 REGISTERS
    // .. .. START: AFI3 REGISTERS
    // .. .. FINISH: AFI3 REGISTERS
    // .. FINISH: AFI REGISTERS
    // .. START: LOCK IT BACK
    // .. LOCK_KEY = 0X767B
    // .. ==> 0XF8000004[15:0] = 0x0000767BU
    // ..     ==> MASK : 0x0000FFFFU    VAL : 0x0000767BU
    // ..
    //EMIT_WRITE(0XF8000004, 0x0000767BU),
    // .. FINISH: LOCK IT BACK
    // FINISH: top
    //
    EMIT_EXIT(),

    //
};

unsigned long ps7_debug_1_0[] = {
    // START: top
    // .. START: CROSS TRIGGER CONFIGURATIONS
    // .. .. START: UNLOCKING CTI REGISTERS
    // .. .. KEY = 0XC5ACCE55
    // .. .. ==> 0XF8898FB0[31:0] = 0xC5ACCE55U
    // .. ..     ==> MASK : 0xFFFFFFFFU    VAL : 0xC5ACCE55U
    // .. ..
    EMIT_WRITE(0XF8898FB0, 0xC5ACCE55U),
    // .. .. KEY = 0XC5ACCE55
    // .. .. ==> 0XF8899FB0[31:0] = 0xC5ACCE55U
    // .. ..     ==> MASK : 0xFFFFFFFFU    VAL : 0xC5ACCE55U
    // .. ..
    EMIT_WRITE(0XF8899FB0, 0xC5ACCE55U),
    // .. .. KEY = 0XC5ACCE55
    // .. .. ==> 0XF8809FB0[31:0] = 0xC5ACCE55U
    // .. ..     ==> MASK : 0xFFFFFFFFU    VAL : 0xC5ACCE55U
    // .. ..
    EMIT_WRITE(0XF8809FB0, 0xC5ACCE55U),
    // .. .. FINISH: UNLOCKING CTI REGISTERS
    // .. .. START: ENABLING CTI MODULES AND CHANNELS
    // .. .. FINISH: ENABLING CTI MODULES AND CHANNELS
    // .. .. START: MAPPING CPU0, CPU1 AND FTM EVENTS TO CTM CHANNELS
    // .. .. FINISH: MAPPING CPU0, CPU1 AND FTM EVENTS TO CTM CHANNELS
    // .. FINISH: CROSS TRIGGER CONFIGURATIONS
    // FINISH: top
    //
    EMIT_EXIT(),

    //
};
#endif

#endif

static int ps7_init(void)
{
	int ret;

	SCLR->SLCR_UNLOCK = 0x0000DF0DU;

	ret = ps7_config(ps7_mio_init_data_3_0);
	if (ret != PS7_INIT_SUCCESS)
		return ret;

	ret = ps7_config(ps7_clock_init_data_3_0);
	if (ret != PS7_INIT_SUCCESS)
		return ret;

	ret = ps7_config(ps7_ddr_init_data_3_0);
	if (ret != PS7_INIT_SUCCESS)
		return ret;

	ret = ps7_config(ps7_peripherals_init_data_3_0);
	if (ret != PS7_INIT_SUCCESS)
		return ret;

	SCLR->SLCR_UNLOCK = 0x0000DF0DU;
	return PS7_INIT_SUCCESS;
}

// NT5CC128M16IP-DI BGA DDR3 NT5CC128M16IP DI
void FLASHMEMINITFUNC arm_hardware_sdram_initialize(void)
{
	PRINTF("arm_hardware_sdram_initialize start\n");
	ps7_init();
	PRINTF("arm_hardware_sdram_initialize done\n");
}
#endif

#endif /* WITHSDRAMHW */
