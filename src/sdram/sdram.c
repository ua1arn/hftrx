/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#include "hardware.h"

#if WITHSDRAMHW

#include <stdint.h>
#include <string.h>

#include "formats.h"	// for debug prints
#include "clocks.h"
#include "gpio.h"


void sdram_test_pattern(uintptr_t addr, uint_fast16_t buffer_size, uint_fast16_t pattern)
{
	for (uint32_t i = 0; i < buffer_size; i++)
		*(volatile uint16_t*) (addr + 2 * i) = pattern;

	volatile uint16_t r;
	for (uint32_t i = 0; i < buffer_size; i++)
	{
		r = *(volatile uint16_t*) (addr + 2 * i);
		if(r !=  pattern)
			PRINTF("ERROR! %p - fill pattern: %04X, read: %04X\n", (void *) (addr + 2 * i),  (unsigned) pattern, (unsigned) r);
	}

}

void sdram_test_increment(uintptr_t addr, uint_fast16_t buffer_size, uint_fast16_t seed)
{
	for (uint32_t i = 0; i < buffer_size; i++)
		*(volatile uint16_t*) (addr + 2 * i) = seed + i;

	volatile uint16_t r;
	for (uint32_t i = 0; i < buffer_size; i++)
	{
		r = *(volatile uint16_t*) (addr + 2 * i);
		if(r !=  seed + i)
			PRINTF("ERROR! %p - fill increment: %04X, read: %04X\n", (void *) (addr + 2 * i),  (unsigned) (seed + i), (unsigned) r);
	}

}

void sdram_test_random(uintptr_t addr, uint_fast16_t buffer_size)
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
			PRINTF("ERROR! %p - fill random: %04X, read: %04X\n", (void *) (addr + 2 * i), (unsigned) aTxBuffer[i], (unsigned) r);
	}

}

#if CPUSTYLE_STM32F7XX

#include "sdram.h"

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
  while (FMC_GetFlagStatus(FMC_Bank2_SDRAM, FMC_FLAG_Busy) != RESET)
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
  while (FMC_GetFlagStatus(FMC_Bank2_SDRAM, FMC_FLAG_Busy) != RESET)
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
  while (FMC_GetFlagStatus(FMC_Bank2_SDRAM, FMC_FLAG_Busy) != RESET)
  {
  }
  /* Send the  first command */
  FMC_SDRAMCmdConfig(&FMC_SDRAMCommandStructure);

  /* Wait until the SDRAM controller is ready */
  while (FMC_GetFlagStatus(FMC_Bank2_SDRAM, FMC_FLAG_Busy) != RESET)
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
  while (FMC_GetFlagStatus(FMC_Bank2_SDRAM, FMC_FLAG_Busy) != RESET)
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
  while (FMC_GetFlagStatus(FMC_Bank2_SDRAM, FMC_FLAG_Busy) != RESET)
  {
  }
#elif defined CTLSTYLE_V3D	/* Плата STM32F746G-DISCO с процессором STM32F746NGH6	*/
	FMC_SDRAMCommandStructure.FMC_CommandMode = FMC_Command_Mode_CLK_Enabled;
	FMC_SDRAMCommandStructure.FMC_CommandTarget = FMC_Command_Target_bank1;
	FMC_SDRAMCommandStructure.FMC_AutoRefreshNumber = 1;
	FMC_SDRAMCommandStructure.FMC_ModeRegisterDefinition = 0;

	while (FMC_GetFlagStatus(FMC_Bank1_SDRAM, FMC_FLAG_Busy) != RESET)
	{
	}

	FMC_SDRAMCmdConfig(&FMC_SDRAMCommandStructure);
	__Delay(10);

	FMC_SDRAMCommandStructure.FMC_CommandMode = FMC_Command_Mode_PALL;
	FMC_SDRAMCommandStructure.FMC_CommandTarget = FMC_Command_Target_bank1;
	FMC_SDRAMCommandStructure.FMC_AutoRefreshNumber = 1;
	FMC_SDRAMCommandStructure.FMC_ModeRegisterDefinition = 0;

	while (FMC_GetFlagStatus(FMC_Bank1_SDRAM, FMC_FLAG_Busy) != RESET)
	{
	}

	FMC_SDRAMCmdConfig(&FMC_SDRAMCommandStructure);

	FMC_SDRAMCommandStructure.FMC_CommandMode = FMC_Command_Mode_AutoRefresh;
	FMC_SDRAMCommandStructure.FMC_CommandTarget = FMC_Command_Target_bank1;
	FMC_SDRAMCommandStructure.FMC_AutoRefreshNumber = 8;
	FMC_SDRAMCommandStructure.FMC_ModeRegisterDefinition = 0;

	while (FMC_GetFlagStatus(FMC_Bank1_SDRAM, FMC_FLAG_Busy) != RESET)
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

	while (FMC_GetFlagStatus(FMC_Bank1_SDRAM, FMC_FLAG_Busy) != RESET)
	{
	}

	FMC_SDRAMCmdConfig(&FMC_SDRAMCommandStructure);

	FMC_SetRefreshCount(1292);

	while (FMC_GetFlagStatus(FMC_Bank1_SDRAM, FMC_FLAG_Busy) != RESET)
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

#elif CPUSTYLE_STM32H7XX

void arm_hardware_sdram_initialize(void)
{
	ASSERT(0);
}

#elif CPUSTYLE_STM32MP1

#elif CPUSTYLE_XC7Z || CPUSTYLE_XCZU

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

#if WITHPS7BOARD_MYC_Y7Z020

	// XC7Z020-2CLG400C, dual bank MT41K256M16HA-125IT

	#include "./myc_y7z020/ps7_init_c.h"

#elif WITHPS7BOARD_ZINC20

	// XC7Z020-1CLG484C, dual bank MT41K256M16HA-125IT

	#include "./zinc20/ps7_init_c.h"

#elif WITHPS7BOARD_EBAZ4205

	// XC7Z010-1CLG400C, single bank MT41K256M16HA-125IT
	#include "./ebaz4205/ps7_init_c.h"


#elif WITHPS7BOARD_ANTMINER

	#include "./antminer/ps7_init_c.h"
	// XC7Z010-1CLG400C, dual bank MT41K256M16HA-125IT

#else

	#warning WITHPS7BOARD_xxx not defined

#endif


// NT5CC128M16IP-DI BGA DDR3 NT5CC128M16IP DI
void FLASHMEMINITFUNC arm_hardware_sdram_initialize(void)
{
	PRINTF("arm_hardware_sdram_initialize start\n");
	VERIFY(PS7_INIT_SUCCESS == ps7_init());
	PRINTF("arm_hardware_sdram_initialize done\n");
}

#elif CPUSTYLE_A64

void FLASHMEMINITFUNC arm_hardware_sdram_initialize(void)
{
	PRINTF("arm_hardware_sdram_initialize start\n");

//	dram_clock_init();
//	init_dram_controller();

	sys_dram_init();
//	TP();
//
//	printhex(0x40000000, (void *) 0x40000000, 256);
//	memset((void *) 0x40000000, 0xFF, 128);
//	memset((void *) 0x40000080, 0x00, 128);
//	printhex(0x40000000, (void *) 0x40000000, 256);
//	memset((void *) 0x70000000, 0xFF, 128);
//	memset((void *) 0x70000080, 0x00, 128);
//	printhex(0x70000000, (void *) 0x70000000, 256);

	PRINTF("arm_hardware_sdram_initialize done\n");

}


#elif CPUSTYLE_T113 || CPUSTYLE_F133

#include "spi.h"

/**
 *
	Hi,

	this is the extracted version of the Allwinner D1/D1s/R528/T113-s DRAM
	"driver", to be included into mainline U-Boot at some point. With this
	on top of my previous T113-s3 support series[1], I can boot my MangoPi MQ-R
	without the help of awboot.
	The DRAM init code is based on awboot's version, though has been heavily
	reworked. To show what has been done, I pushed a history branch [2], which
	takes a verbatim copy of awboot's mctl_hal.c, then converts this over the
	course of about 80 patches into the version posted here. The series there
	contains an awboot/U-Boot compat layer, so the file can be used in both
	repositories. This compat layer is dropped here, but can be put back by
	reverting the top patch of [2].

	I was wondering if people could have a look at this version here, to give
	early feedback. I will (re-)post this as part of a proper R528/T113-s
	support series, but first need to sort out some minor issues and address
	Samuel's comments on the previous version.

	If you wonder, the (working!) Kconfig DRAM variables for the T113-s3 are:
	CONFIG_DRAM_CLK=792
	CONFIG_DRAM_ZQ=8092667
	CONFIG_DRAM_SUNXI_ODT_EN=0
	CONFIG_DRAM_SUNXI_TPR0=0x004a2195
	CONFIG_DRAM_SUNXI_TPR11=0x340000
	CONFIG_DRAM_SUNXI_TPR12=0x46
	CONFIG_DRAM_SUNXI_TPR13=0x34000100

	For the D1 with DDR3 chips (most boards?), it should be those values:
	CONFIG_DRAM_CLK=792
	CONFIG_DRAM_SUNXI_ODT_EN=1
	CONFIG_DRAM_SUNXI_TPR0=0x004a2195
	CONFIG_DRAM_SUNXI_TPR11=0x870000
	CONFIG_DRAM_SUNXI_TPR12=0x24
	CONFIG_DRAM_SUNXI_TPR13=0x34050100

	According to the dump of some MangoPi MQ-1 firmware, the D1s should work with:
	CONFIG_SUNXI_DRAM_DDR2=y
	CONFIG_DRAM_CLK=528
	CONFIG_DRAM_ZQ=8092665
	CONFIG_DRAM_SUNXI_ODT_EN=0
	CONFIG_DRAM_SUNXI_TPR0=0x00471992
	CONFIG_DRAM_SUNXI_TPR11=0x30010
	CONFIG_DRAM_SUNXI_TPR12=0x35
	CONFIG_DRAM_SUNXI_TPR13=0x34000000

	Many thanks!
	Andre

	[1] https://lore.kernel.org/u-boot/20221206004549.29015-1-andre.przywara@arm.com/
	[2] https://github.com/apritzel/u-boot/commits/d1_dram_history

 *
 */
#if CPUSTYLE_T113
static struct dram_para_t ddrp3 =
{
	.dram_clk = 792,
	.dram_type = 3,
	.dram_zq = 8092667,
	.dram_odt_en = 0x00,
	.dram_para1 = 0x000010d2,
	.dram_para2 = 0x0000,
	.dram_mr0 = 0x1c70,
	.dram_mr1 = 0x042,
	.dram_mr2 = 0x18,
	.dram_mr3 = 0x0,
	.dram_tpr0 = 0x004a2195,
	.dram_tpr1 = 0x02423190,
	.dram_tpr2 = 0x0008B061,
	.dram_tpr3 = 0xB4787896,
	.dram_tpr4 = 0x0,
	.dram_tpr5 = 0x48484848,
	.dram_tpr6 = 0x00000048,
	.dram_tpr7 = 0x1620121e,
	.dram_tpr8 = 0x0,
	.dram_tpr9 = 0x0,
	.dram_tpr10 = 0x0,
	.dram_tpr11 = 0x00340000,
	.dram_tpr12 = 0x00000046,
	.dram_tpr13 = 0x34000100,
};

int sys_dram_init(void)
{
	set_pll_cpux_axi(PLL_CPU_N);
	return init_DRAM(0, & ddrp3) != 0;
}

#elif CPUSTYLE_F133

/*
 *
CONFIG_SUNXI_DRAM_DDR2=y
CONFIG_DRAM_CLK=528
CONFIG_DRAM_ZQ=8092665
CONFIG_DRAM_SUNXI_ODT_EN=0
CONFIG_DRAM_SUNXI_TPR0=0x00471992
CONFIG_DRAM_SUNXI_TPR11=0x30010
CONFIG_DRAM_SUNXI_TPR12=0x35
CONFIG_DRAM_SUNXI_TPR13=0x34000000

 */
static struct dram_para_t ddrp2 = {
	.dram_clk = 528,
	.dram_type = 2,
	.dram_zq = 8092665,
	.dram_odt_en = 0x00,
	.dram_para1 = 0x000000d2,
	.dram_para2 = 0x00000000,
	.dram_mr0 = 0x00000e73,
	.dram_mr1 = 0x02,
	.dram_mr2 = 0x0,
	.dram_mr3 = 0x0,
	.dram_tpr0 = 0x00471992,
	.dram_tpr1 = 0x0131a10c,
	.dram_tpr2 = 0x00057041,
	.dram_tpr3 = 0xb4787896,
	.dram_tpr4 = 0x0,
	.dram_tpr5 = 0x48484848,
	.dram_tpr6 = 0x48,
	.dram_tpr7 = 0x1621121e,
	.dram_tpr8 = 0x0,
	.dram_tpr9 = 0x0,
	.dram_tpr10 = 0x00000000,
	.dram_tpr11 = 0x00030010,
	.dram_tpr12 = 0x00000035,
	.dram_tpr13 = 0x34000000,
};

int sys_dram_init(void)
{
	set_pll_riscv_axi(PLL_CPU_N);
	return init_DRAM(0, & ddrp2) != 0;
}

#endif /* CPUSTYLE_T113, CPUSTYLE_F133 */

void FLASHMEMINITFUNC arm_hardware_sdram_initialize(void)
{
	PRINTF("arm_hardware_sdram_initialize start\n");
#if 0 && CPUSTYLE_T113
	const uintptr_t ddr3init_base = 0x00028000;
	/* вызывается до разрешения MMU */
	bootloader_readimage(0x00040000, (void *) ddr3init_base, 0x8000);
	memcpy((void *) (ddr3init_base + 0x0038), & ddrp, sizeof ddrp);
	((void(*)(void))(ddr3init_base))();
	set_pll_cpux_axi(PLL_CPU_N);
	#if WITHDEBUG && 1
		//HARDWARE_DEBUG_INITIALIZE();
		HARDWARE_DEBUG_SET_SPEED(DEBUGSPEED);
	#endif /* WITHDEBUG */

#elif 0 && CPUSTYLE_F133
	const uintptr_t ddr3init_base = 0x00020000;
	/* вызывается до разрешения MMU */
	TP();
	bootloader_readimage(0x00040000, (void *) ddr3init_base, 20 * 1024);
	printhex(ddr3init_base, (void *) ddr3init_base, 256);
	TP();
	memcpy((void *) (ddr3init_base + 0x0018), & ddrp2, sizeof ddrp2);
	TP();
	((void(*)(void))(ddr3init_base))();
	TP();
	set_pll_riscv_axi(PLL_CPU_N);
	#if WITHDEBUG && 1
		//HARDWARE_DEBUG_INITIALIZE();
		HARDWARE_DEBUG_SET_SPEED(DEBUGSPEED);
	#endif /* WITHDEBUG */

#else
	PRINTF("default: allwnrt113_get_pll_ddr_freq()=%" PRIuFAST64 "\n", allwnrt113_get_pll_ddr_freq());
	PRINTF("default: allwnrt113_get_dram_freq()=%" PRIuFAST32 "\n", allwnrt113_get_dram_freq());
	if (sys_dram_init() == 0)
	{
		PRINTF("No external memory");
#ifdef BOARD_BLINK_INITIALIZE
		BOARD_BLINK_INITIALIZE();
		for (;;)
		{
			BOARD_BLINK_SETSTATE(1);
			local_delay_ms(100);
			BOARD_BLINK_SETSTATE(0);
			local_delay_ms(100);
		}
#endif
		for (;;)
			;
	}
	PRINTF("settings: allwnrt113_get_pll_ddr_freq()=%" PRIuFAST64 "\n", allwnrt113_get_pll_ddr_freq());
	PRINTF("settings: allwnrt113_get_dram_freq()=%" PRIuFAST32 "\n", allwnrt113_get_dram_freq());

#endif
	PRINTF("arm_hardware_sdram_initialize done\n");
	//local_delay_ms(1000);
}

#endif /* CPUSTYLE_T113 || CPUSTYLE_F133 */

#endif /* WITHSDRAMHW */
