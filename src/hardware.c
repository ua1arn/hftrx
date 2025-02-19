/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#include "hardware.h"	/* зависящие от процессора функции работы с портами */
#include "keyboard.h"

#include <string.h>
#include <math.h>

#include "board.h"
#include "audio.h"
#include "formats.h"	// for debug prints
#include "gpio.h"
#include "spi.h"
#include "encoder.h"
#include "clocks.h"

#if 0
#define DBGC(c) do { \
	while ((UART0->UART_USR & (1u << 1)) == 0) \
		; \
	UART0->UART_RBR_THR_DLL = (c); \
} while (0)
#endif

#if WITHRTOS
#include "FreeRTOS.h"
//#include "task.h"
#endif /* WITHRTOS */

#if CPUSTYLE_XC7Z && ! LINUX_SUBSYSTEM

extern uint8_t bd_space[];

#include "xc7z_inc.h"
static XGpioPs xc7z_gpio;
#if ! WITHISBOOTLOADER
static XAdcPs xc7z_xadc;
#endif /* ! WITHISBOOTLOADER */

void xc7z_hardware_initialize(void)
{
	int Status;

	SCLR->SLCR_UNLOCK = 0x0000DF0DU;
	SCLR->APER_CLK_CTRL |= (1uL << 22);	// APER_CLK_CTRL.GPIO_CPU_1XCLKACT
	// GPIO PS init
	XGpioPs_Config * gpiocfg = XGpioPs_LookupConfig(XPAR_XGPIOPS_0_DEVICE_ID);
	Status = XGpioPs_CfgInitialize(& xc7z_gpio, gpiocfg, gpiocfg->BaseAddr);
	if (Status != XST_SUCCESS)
	{
		PRINTF("PS GPIO init error\n");
		ASSERT(0);
	}

#if WITHCPUFANPWM
	{
		const float FS = powf(2, 32);
		uint32_t fan_pwm_period = 25000 * FS / REFERENCE_FREQ;
		AX_PWM_mWriteReg(XPAR_CPU_FAN_PWM_0_S00_AXI_BASEADDR, AX_PWM_AXI_SLV_REG0_OFFSET, fan_pwm_period);

		uint32_t fan_pwm_duty = FS * (1.0f - 0.7f) - 1;
		AX_PWM_mWriteReg(XPAR_CPU_FAN_PWM_0_S00_AXI_BASEADDR, AX_PWM_AXI_SLV_REG1_OFFSET, fan_pwm_duty);
	}
#endif /* WITHCPUFANPWM */

#if WITHPS7BOARD_MYC_Y7Z020
	// пока так
	xc7z_gpio_output(TARGET_RFADC_SHDN_EMIO);
	xc7z_writepin(TARGET_RFADC_SHDN_EMIO, 0);

	xc7z_gpio_output(TARGET_RFADC_DITH_EMIO);
	xc7z_writepin(TARGET_RFADC_DITH_EMIO, 0);
#endif /* WITHPS7BOARD_MYC_Y7Z020 */

#if (WITHTHERMOLEVEL || WITHTHERMOLEVEL2)

	HARDWARE_DCDC_SETDIV(0);

	XAdcPs_Config * xadccfg = XAdcPs_LookupConfig(XPAR_XADCPS_0_DEVICE_ID);
	XAdcPs_CfgInitialize(& xc7z_xadc, xadccfg, xadccfg->BaseAddress);

	Status = XAdcPs_SelfTest(& xc7z_xadc);
	if (Status != XST_SUCCESS)
	{
		PRINTF("XADC init error\n");
		ASSERT(0);
	}

	XAdcPs_SetSequencerMode(& xc7z_xadc, XADCPS_SEQ_MODE_SAFE);
#endif /* (WITHTHERMOLEVEL || WITHTHERMOLEVEL2) */
}

void xcz_dcdc_sync(uint32_t freq)
{
#if WITHDCDCFREQCTL
	uint32_t fs = 1200000uL;
	float pwm_duty = 0.5;

	if (freq >= 3000000uL && freq < 5000000uL)
		fs = 450000uL;
	else if (freq >= 5000000 && freq < 8000000uL)
		fs = 830000uL;

	uint32_t dcdc_pwm_period = fs * pow(2, 32) / REFERENCE_FREQ;
	AX_PWM_mWriteReg(XPAR_DCDC_PWM_1_S00_AXI_BASEADDR, AX_PWM_AXI_SLV_REG0_OFFSET, dcdc_pwm_period);

	float32_t dcdc_pwm_duty = powf(2, 32) * (1.0 - pwm_duty) - 1.0;
	AX_PWM_mWriteReg(XPAR_DCDC_PWM_1_S00_AXI_BASEADDR, AX_PWM_AXI_SLV_REG1_OFFSET, dcdc_pwm_duty);
#endif /* WITHDCDCFREQCTL */
}

float xc7z_get_cpu_temperature(void)
{
#if WITHTHERMOLEVEL
	uint32_t TempRawData = XAdcPs_GetAdcData(& xc7z_xadc, XADCPS_CH_TEMP);
	return XAdcPs_RawToTemperature(TempRawData);
#else
	return 0;
#endif /* WITHTHERMOLEVEL */
}


uint_fast8_t xc7z_readpin(uint8_t pin)
{
	ASSERT(xc7z_gpio.IsReady == XIL_COMPONENT_IS_READY);
	ASSERT(pin < xc7z_gpio.MaxPinNum);

	uint_fast8_t Bank;
	uint_fast8_t PinNumber;

	GPIO_BANK_DEFINE(pin, Bank, PinNumber);

	uint8_t val = (XGpioPs_ReadReg(xc7z_gpio.GpioConfig.BaseAddr,
			((uint32_t)(Bank) * XGPIOPS_DATA_BANK_OFFSET) +
			XGPIOPS_DATA_RO_OFFSET) >> (uint32_t)PinNumber) & (uint32_t)1;

	return val;
}

void xc7z_writepin(uint8_t pin, uint8_t val)
{
	ASSERT(xc7z_gpio.IsReady == XIL_COMPONENT_IS_READY);
	ASSERT(pin < xc7z_gpio.MaxPinNum);

	uint_fast8_t Bank;
	uint_fast8_t PinNumber;
	uint32_t RegOffset;
	uint32_t DataVar = val;
	uint32_t Value;

	GPIO_BANK_DEFINE(pin, Bank, PinNumber);

	SCLR->SLCR_UNLOCK = 0x0000DF0DuL;
	GPIO_BANK_SET_OUTPUTS(Bank, 1uL << PinNumber, !! val << PinNumber);
	return;
}

void xc7z_gpio_input(uint8_t pin)
{
	SCLR->SLCR_UNLOCK = 0x0000DF0DuL;
	if (pin < ZYNQ_MIO_CNT)
	{
		MIO_SET_MODE(pin, 0x00001600uL); /* initial value - with pull-up, TRI_ENABLE=0, then 3-state is controlled by the gpio.OEN_x register. */ \
	}

	ASSERT(xc7z_gpio.IsReady == XIL_COMPONENT_IS_READY);
	ASSERT(pin < xc7z_gpio.MaxPinNum);

	uint_fast8_t Bank;
	uint_fast8_t PinNumber;

	GPIO_BANK_DEFINE(pin, Bank, PinNumber);

	uint32_t DirModeReg = XGpioPs_ReadReg(xc7z_gpio.GpioConfig.BaseAddr,
			((uint32_t)(Bank) * XGPIOPS_REG_MASK_OFFSET) +
			XGPIOPS_DIRM_OFFSET);

	DirModeReg &= ~ ((uint32_t)1 << (uint32_t)PinNumber);

	XGpioPs_WriteReg(xc7z_gpio.GpioConfig.BaseAddr,
			((uint32_t)(Bank) * XGPIOPS_REG_MASK_OFFSET) +
			XGPIOPS_DIRM_OFFSET, DirModeReg);
}

void xc7z_gpio_output(uint8_t pin)
{
	SCLR->SLCR_UNLOCK = 0x0000DF0DuL;
	if (pin < ZYNQ_MIO_CNT)
	{
		MIO_SET_MODE(pin, 0x00001600uL); /* initial value - with pull-up, TRI_ENABLE=0, then 3-state is controlled by the gpio.OEN_x register. */ \
	}

	ASSERT(xc7z_gpio.IsReady == XIL_COMPONENT_IS_READY);
	ASSERT(pin < xc7z_gpio.MaxPinNum);

	uint_fast8_t Bank;
	uint_fast8_t PinNumber;

	GPIO_BANK_DEFINE(pin, Bank, PinNumber);

	// XGPIOPS_DIRM_OFFSET = 0x00000204U
	uint32_t DirModeReg = XGpioPs_ReadReg(xc7z_gpio.GpioConfig.BaseAddr,
			((uint32_t)(Bank) * XGPIOPS_REG_MASK_OFFSET) +
			XGPIOPS_DIRM_OFFSET);
	DirModeReg |= ((uint32_t)1 << PinNumber);
	XGpioPs_WriteReg(xc7z_gpio.GpioConfig.BaseAddr,
			((uint32_t)(Bank) * XGPIOPS_REG_MASK_OFFSET) +
			XGPIOPS_DIRM_OFFSET, DirModeReg);

	// XGPIOPS_OUTEN_OFFSET = 0x0208
	uint32_t OpEnableReg = XGpioPs_ReadReg(xc7z_gpio.GpioConfig.BaseAddr,
			((uint32_t)(Bank) * XGPIOPS_REG_MASK_OFFSET) +
			XGPIOPS_OUTEN_OFFSET);
	OpEnableReg |= ((uint32_t)1 << PinNumber);
	XGpioPs_WriteReg(xc7z_gpio.GpioConfig.BaseAddr,
			((uint32_t)(Bank) * XGPIOPS_REG_MASK_OFFSET) +
			XGPIOPS_OUTEN_OFFSET, OpEnableReg);
}

#endif /* CPUSTYLE_XC7Z && ! LINUX_SUBSYSTEM */

static IRQLSPINLOCK_t tickerslock = IRQLSPINLOCK_INIT;
static IRQLSPINLOCK_t adcdoneslock = IRQLSPINLOCK_INIT;
static VLIST_ENTRY tickers;
static VLIST_ENTRY adcdones;

void ticker_initialize_ext(ticker_t * p, unsigned nticks, void (* cb)(void *), void * ctx, enum ticker_mode mode)
{
	p->period = nticks;
	p->ticks = 0;
	p->cb = cb;
	p->ctx = ctx;
	p->mode = mode;
}

void ticker_initialize(ticker_t * p, unsigned nticks, void (* cb)(void *), void * ctx)
{
	ticker_initialize_ext(p, nticks, cb, ctx, TICKERMD_PERIODIC);
}

static void ticker_trampoline(void * ctx)
{
	board_dpc_call((dpcobj_t *) ctx, board_dpc_coreid());	// Запрос отложенного выполнения USER-MODE функции
}

void ticker_initialize_user(ticker_t * p, unsigned nticks, dpcobj_t * dpc)
{
	ticker_initialize(p, nticks, ticker_trampoline, dpc);
}

void ticker_add(ticker_t * p)
{
	IRQL_t oldIrql;

	IRQLSPIN_LOCK(& tickerslock, & oldIrql, TICKER_IRQL);
	ASSERT(tickers.Blink != NULL && tickers.Flink != NULL);
	InsertHeadVList(& tickers, & p->item);
	IRQLSPIN_UNLOCK(& tickerslock, oldIrql);
}

void ticker_remove(ticker_t * p)
{
	IRQL_t oldIrql;

	IRQLSPIN_LOCK(& tickerslock, & oldIrql, TICKER_IRQL);
	RemoveEntryVList(& p->item);
	IRQLSPIN_UNLOCK(& tickerslock, oldIrql);
}

/* начало интервала в случае TICKERMD_MANUAL */
void ticker_start(ticker_t * p)
{
	IRQL_t oldIrql;

	IRQLSPIN_LOCK(& tickerslock, & oldIrql, TICKER_IRQL);
	switch (p->mode)
	{
	case TICKERMD_MANUAL:
		break;
	default:
		break;
	}
	IRQLSPIN_UNLOCK(& tickerslock, oldIrql);
}

/* изменение периода запущенного тикера */
void ticker_setperiod(ticker_t * p, unsigned nticks)
{
	IRQL_t oldIrql;

	IRQLSPIN_LOCK(& tickerslock, & oldIrql, TICKER_IRQL);
	if (p->period < nticks)
	{
		p->period = nticks;
	}
	else if (p->period > nticks)
	{
		p->period = nticks;
		p->ticks = 0;
	}
	IRQLSPIN_UNLOCK(& tickerslock, oldIrql);
}

// Вызывается из обработчика системного таймера
static void tickers_event(void)
{
	IRQL_t oldIrql;

	IRQLSPIN_LOCK(& tickerslock, & oldIrql, TICKER_IRQL);
	PVLIST_ENTRY t;
	ASSERT(tickers.Blink != NULL && tickers.Flink != NULL);
	for (t = tickers.Blink; t != & tickers;)
	{
		ASSERT(t != NULL);
		PVLIST_ENTRY tnext = t->Blink;	/* текущий элемент может быть удалён из списка */
		ticker_t * const p = CONTAINING_RECORD(t, ticker_t, item);
	
		switch (p->mode)
		{
		case TICKERMD_MANUAL:
			break;
		case TICKERMD_PERIODIC:
			break;
		default:
			break;
		}
		if (++ p->ticks >= p->period)
		{
			p->ticks = 0;
			if (p->cb != NULL)
				(p->cb)(p->ctx);
			switch (p->mode)
			{
			case TICKERMD_MANUAL:
				//RemoveEntryVList(t);
				break;
			default:
				break;
			}
		}
		t = tnext;
	}
	IRQLSPIN_UNLOCK(& tickerslock, oldIrql);
}

static void adcstart_event(void * ctx)
{
#if WITHCPUADCHW
	hardware_adc_startonescan();	// хотя бы один вход (s-метр) есть.
#endif /* WITHCPUADCHW */
}

static void adcdones_event(void * ctx);

void tickers_initialize(void)
{
	IRQLSPINLOCK_INITIALIZE(& tickerslock);
	InitializeListHead(& tickers);

	IRQLSPINLOCK_INITIALIZE(& adcdoneslock);
	InitializeListHead(& adcdones);

#if WITHCPUADCHW

	static ticker_t adcticker;
	ticker_initialize(& adcticker, SEQNTICKS(KBD_TICKS_PERIOD), adcstart_event, NULL);
	ticker_add(& adcticker);

#else /* WITHCPUADCHW */

	static ticker_t adcticker;
	ticker_initialize(& adcticker, SEQNTICKS(KBD_TICKS_PERIOD), adcdones_event, NULL);
	ticker_add(& adcticker);

#endif /* WITHCPUADCHW */
}

void tickers_deinitialize(void)
{
	IRQL_t oldIrql;

	IRQLSPIN_LOCK(& tickerslock, & oldIrql, TICKER_IRQL);
	RemoveEntryVList(& tickers);
	IRQLSPIN_UNLOCK(& tickerslock, oldIrql);
}

// регистрируются обработчики конца преобразования АЦП
void adcdone_initialize(adcdone_t * p, void (* cb)(void *), void * ctx)
{
	p->cb = cb;
	p->ctx = ctx;
}

// регистрируется обработчик конца преобразования АЦП
void adcdone_add(adcdone_t * p)
{
	IRQL_t oldIrql;

	IRQLSPIN_LOCK(& adcdoneslock, & oldIrql, TICKER_IRQL);
	InsertHeadVList(& adcdones, & p->item);
	IRQLSPIN_UNLOCK(& adcdoneslock, oldIrql);
}

// Call on KBD_TICKS_PERIOD
static void adcdones_event(void * ctx)
{
	IRQL_t oldIrql;

	IRQLSPIN_LOCK(& adcdoneslock, & oldIrql, TICKER_IRQL);
	PVLIST_ENTRY t;
	for (t = adcdones.Blink; t != & adcdones; t = t->Blink)
	{
		ASSERT(t != NULL);
		adcdone_t * const p = CONTAINING_RECORD(t, adcdone_t, item);

		if (p->cb != NULL)
			(p->cb)(p->ctx);
	}
	IRQLSPIN_UNLOCK(& adcdoneslock, oldIrql);
}

static volatile uint32_t sys_now_counter;

#if 1//! WITHLWIP

uint32_t sys_now(void)
{
	return sys_now_counter;
}

#endif /* WITHLWIP */

uint32_t board_millis(void)
{
	return sys_now_counter;
}

//#include "hal_time_ms.h"
uint32_t hal_time_ms(void){
	return sys_now_counter;
}

/* Машинно-независимый обработчик прерываний. */
// Функции с побочным эффектом - отсчитывание времени.
// При возможности вызываются столько раз, сколько произошло таймерных прерываний.
RAMFUNC void spool_systimerbundle1(void)
{
	//beacon_255();
#ifdef USE_HAL_DRIVER
	HAL_IncTick();
#endif /* USE_HAL_DRIVER */

	sys_now_counter += (1000 / TICKS_FREQUENCY);

	tickers_event();
}

/* Машинно-независимый обработчик прерываний. */
// Функции с побочным эффектом редиспетчеризации.
// Если пропущены прерывания, компенсировать дополнительными вызовами нет смысла.
RAMFUNC void spool_systimerbundle2(void)
{
}

#if WITHCPUADCHW
/* 
	Машинно-независимый обработчик прерываний.
	Вызывается с периодом 1/TIMETICKS по окончании получения данных всех каналов АЦП,
	перечисленных в таблице adcinputs.
*/

RAMFUNC void spool_adcdonebundle(void)
{
	adcdones_event(NULL);
}
#endif /* WITHCPUADCHW */

//static volatile uint_fast8_t hardware_reqshutdown;
/* возвращаем запрос на выключение - от компаратора питания */
uint_fast8_t 
hardware_getshutdown(void)
{
#if CPUSTYLE_STM32F1XX || CPUSTYLE_STM32F30X || CPUSTYLE_STM32L0XX
	//return hardware_reqshutdown;
	return (PWR->CR & PWR_CR_PVDE) && (PWR->CSR & PWR_CSR_PVDO);

#else
	return 0;
#endif
}

void 
hardware_encoders_initialize(void)
{
#if WITHENCODER
	ENCODER_INITIALIZE();
#endif /* WITHENCODER */
}

/* Чтение состояния выходов валкодера #1 - в два младших бита */
/* Состояние фазы A - в бите с весом 2, фазы B - в бите с весом 1 */

uint_fast8_t 
hardware_get_encoder_bits(void)
{
#if WITHENCODER && defined (ENCODER_BITS_GET)
	return ENCODER_BITS_GET();
#elif WITHENCODER && defined (ENCODER_BITS) && defined (ENCODER_SHIFT)
	return (ENCODER_INPUT_PORT & ENCODER_BITS) >> ENCODER_SHIFT;	// Биты валкодера #1
#elif WITHENCODER && defined (ENCODER_BITS)
	const portholder_t v = ENCODER_INPUT_PORT;
	return ((v & ENCODER_BITA) != 0) * GETENCBIT_A + ((v & ENCODER_BITB) != 0) * GETENCBIT_B;	// Биты идут не подряд
#else /* WITHENCODER */
	return 0;
#endif /* WITHENCODER */
}

/* Чтение состояния выходов валкодера #2 - в два младших бита */
/* Состояние фазы A - в бите с весом 2, фазы B - в бите с весом 1 */

uint_fast8_t
hardware_get_encoder2_bits(void)
{
#if WITHENCODER2 && defined (ENCODER2_BITS_GET)
	return ENCODER2_BITS_GET();
#elif WITHENCODER2 && ENCODER2_BITS && defined (ENCODER2_SHIFT)
	return (ENCODER2_INPUT_PORT & ENCODER2_BITS) >> ENCODER2_SHIFT;	// Биты валкодера #2
#elif WITHENCODER2 && ENCODER2_BITS
	const portholder_t v = ENCODER2_INPUT_PORT;
	return ((v & ENCODER2_BITA) != 0) * GETENCBIT_A + ((v & ENCODER2_BITB) != 0);	// Биты идут не подряд
#elif WITHENCODER2 && (CPUSTYLE_XC7Z || CPUSTYLE_RK356X)
	return (gpio_readpin(ENCODER2_BITA) != 0) * GETENCBIT_A + (gpio_readpin(ENCODER2_BITB) != 0) * GETENCBIT_B;
#else /* WITHENCODER2 */
	return 0;
#endif /* WITHENCODER2 */
}

/* Чтение состояния выходов валкодера #2 - в два младших бита */
/* Состояние фазы A - в бите с весом 2, фазы B - в бите с весом 1 */

uint_fast8_t
hardware_get_encoder_sub_bits(void)
{
#if WITHENCODER_SUB && defined (ENCODER_SUB_BITS_GET)
	return ENCODER_SUB_BITS_GET();
#else /* WITHENCODER_SUB */
	return 0;
#endif /* WITHENCODER_SUB */
}

/* Чтение состояния выходов валкодера #3 - в два младших бита */
/* Состояние фазы A - в бите с весом 2, фазы B - в бите с весом 1 */

uint_fast8_t
hardware_get_encoder3_bits(void)
{
#if WITHENCODER_1F && defined (ENC1F_BITS_GET)
	return ENC1F_BITS_GET();
#else /* WITHENCODER_1F */
	return 0;
#endif /* WITHENCODER_1F */
}

/* Чтение состояния выходов валкодера #4 - в два младших бита */
/* Состояние фазы A - в бите с весом 2, фазы B - в бите с весом 1 */

uint_fast8_t 
hardware_get_encoder4_bits(void)
{
#if WITHENCODER_2F && defined (ENC2F_BITS_GET)
	return ENC2F_BITS_GET();
#else /* WITHENCODER_2F */
	return 0;
#endif /* WITHENCODER_2F */
}

/* Чтение состояния выходов валкодера #5 - в два младших бита */
/* Состояние фазы A - в бите с весом 2, фазы B - в бите с весом 1 */

uint_fast8_t
hardware_get_encoder5_bits(void)
{
#if WITHENCODER_3F && defined (ENC3F_BITS_GET)
	return ENC3F_BITS_GET();
#else /* WITHENCODER_3F */
	return 0;
#endif /* WITHENCODER_3F */
}

/* Чтение состояния выходов валкодера #6 - в два младших бита */
/* Состояние фазы A - в бите с весом 2, фазы B - в бите с весом 1 */

uint_fast8_t
hardware_get_encoder6_bits(void)
{
#if WITHENCODER_4F && defined (ENC4F_BITS_GET)
	return ENC4F_BITS_GET();
#else /* WITHENCODER_4F */
	return 0;
#endif /* WITHENCODER_4F */
}

// ADC intgerface functions

#if WITHCPUADCHW


// Проверка что индекс входа АЦП относится ко встроенной периферии процессора
uint_fast8_t
isadchw(uint_fast8_t adci)
{
	return adci < BOARD_ADCX0BASE;
}

//#define ADCINPUTS_COUNT (board_get_adcinputs())

static uint_fast8_t adc_input;

#if CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

void RAMFUNC_NONILINE ADC_Handler(void)
{
	//const unsigned long sr = ADC->ADC_ISR;	// ADC_IER_DRDY, ADC_ISR_DRDY
	(void) ADC->ADC_ISR;	// ADC_IER_DRDY, ADC_ISR_DRDY

	// ATSAM3Sxx считывает только 10 или 12 бит
	// Read the 8 most significant bits
	// of the AD conversion result
	board_adc_store_data(board_get_adcch(adc_input), ADC->ADC_LCDR & ADC_LCDR_LDATA_Msk);	// на этом цикле используем результат
	// Select next ADC input
	for (;;)
	{
		if (++ adc_input >= board_get_adcinputs())
		{
			spool_adcdonebundle();
			break;
		}
		else
		{
			const uint_fast8_t adci = board_get_adcch(adc_input);
			if (isadchw(adci))
			{
				// Select next ADC input (only one)
				const portholder_t mask = ADC_CHER_CH0 << adci;
				ADC->ADC_CHER = mask; /* enable ADC */
				ADC->ADC_CHDR = ~ mask; /* disable ADC */
				ADC->ADC_CR = ADC_CR_START;	// Start the AD conversion
				break;
			}
		}
	}
}
	
#elif CPUSTYLE_AT91SAM7S

RAMFUNC_NONILINE void AT91F_ADC_IRQHandler(void)
{
	(void) AT91C_BASE_ADC->ADC_SR;
	// Read the 8 most significant bits
	// of the AD conversion result
	board_adc_store_data(board_get_adcch(adc_input), AT91C_BASE_ADC->ADC_LCDR & AT91C_ADC_LDATA);	// на этом цикле используем результат
	// Select next ADC input
	for (;;)
	{
		if (++ adc_input >= board_get_adcinputs())
		{
			spool_adcdonebundle();
			break;
		}
		else
		{
			// Select next ADC input (only one)
			const uint_fast8_t adci = board_get_adcch(adc_input);
			if (isadchw(adci))
			{
				const portholder_t mask = AT91C_ADC_CH0 << adci;
				AT91C_BASE_ADC->ADC_CHDR = ~ mask; /* disable ADC inputs */
				AT91C_BASE_ADC->ADC_CHER = mask; /* enable ADC */
				AT91C_BASE_ADC->ADC_CR = AT91C_ADC_START;	// Start the AD conversion
				break;
			}
		}
	}
}

#elif CPUSTYLE_ATMEGA
	///////adc
	// получение кода выбора входа
	static uint_fast8_t hardware_atmega_admux(uint_fast8_t ch)
	{
		enum { ATMEGA_ADC_VREF_TYPE = ((0UL << REFS1) | (1UL << REFS0))	}; // AVCC used as reference volage
		#if HARDWARE_ADCBITS == 8
			return ch | ATMEGA_ADC_VREF_TYPE | (1UL << ADLAR);
		#else
			return ch | ATMEGA_ADC_VREF_TYPE;
		#endif
	}

	ISR(ADC_vect)
	{
		#if HARDWARE_ADCBITS == 8
			// Read the 8 most significant bits
			// of the AD conversion result
			board_adc_store_data(board_get_adcch(adc_input), ADCH);
		#else
			// Read the AD conversion result
			board_adc_store_data(board_get_adcch(adc_input), ADCW);
		#endif 
		// Select next ADC input
		for (;;)
		{
			if (++ adc_input >= board_get_adcinputs())
			{
				spool_adcdonebundle();
				break;
			}
			else
			{
				// Select next ADC input (only one)
				const uint_fast8_t adci = board_get_adcch(adc_input);
				if (isadchw(adci))
				{
					ADMUX = hardware_atmega_admux(adci);
					ADCSRA |= (1U << ADSC);			// Start the AD conversion
					break;
				}
			}
		}
	}
#elif CPUSTYLE_ATXMEGAXXXA4

	#warning TODO: write atxmega code for ADC interrupt handler


	// adc
	ISR(ADCA_CH0_vect)
	{
			// на этом цикле используем результат
		#if HARDWARE_ADCBITS == 8
			// Select next ADC input
			// Read the 8 most significant bits
			// of the AD conversion result
			board_adc_store_data(board_get_adcch(adc_input), ADCA.CH0.RESH);
		#else
			// Read the AD conversion result
			board_adc_store_data(board_get_adcch(adc_input), ADCA.CH0.RESH * 256 + ADCA.CH0.RESL);
		#endif 
		// Select next ADC input
		for (;;)
		{
			if (++ adc_input >= board_get_adcinputs())
			{
				spool_adcdonebundle();
				break;
			}
			else
			{
				// Select next ADC input (only one)
				const uint_fast8_t adci = board_get_adcch(adc_input);
				if (isadchw(adci))
				{
					ADCA.CH0.MUXCTRL = adci;
					ADCA.CH0.CTRL |= (1U << ADC_CH_START_bp);			// Start the AD conversion
					break;
				}
			}
		}
	}


#elif CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1

// For SM32H7XXX: ADC_IRQn is a same vector as ADC1_2_IRQn (decimal 18)


const adcinmap_t * getadcmap(uint_fast8_t adci)
{
	static const adcinmap_t adcinmaps [] =
	{
#if CPUSTYLE_STM32H7XX
		{	16,	ADC1,	ADC12_COMMON,	15,	},	// @0:	PA0	ADC1_INP16 (PA0_C ADC12_INP0)
		{	17,	ADC1,	ADC12_COMMON,	15,	},	// @1:	PA1	ADC1_INP17 (PA1_C ADC12_INP1)
		{	14,	ADC1,	ADC12_COMMON,	15,	},	// @2:	PA2	ADC12_INP14
		{	15,	ADC1,	ADC12_COMMON,	15,	},	// @3:	PA3	ADC12_INP15
		{	18,	ADC1,	ADC12_COMMON,	15,	},	// @4:	PA4	ADC12_INP18
		{	19,	ADC1,	ADC12_COMMON,	15,	},	// @5:	PA5	ADC12_INP19
		{	3,	ADC1,	ADC12_COMMON,	15,	},	// @6:	PA6	ADC12_INP3
		{	7,	ADC1,	ADC12_COMMON,	15,	},	// @7:	PA7	ADC12_INP7
		{	9,	ADC1,	ADC12_COMMON,	15,	},	// @8:	PB0	ADC12_INP9
		{	5,	ADC1,	ADC12_COMMON,	15,	},	// @9:	PB1	ADC12_INP5
		{	10,	ADC1,	ADC12_COMMON,	15,	},	// @10:	PC0	ADC123_INP10
		{	11,	ADC1,	ADC12_COMMON,	15,	},	// @11:	PC1	ADC123_INP11
		{	12,	ADC1,	ADC12_COMMON,	15,	},	// @12:	PC2	ADC123_INP12 (PC2_C ADC3_INP0)
		{	13,	ADC1,	ADC12_COMMON,	15,	},	// @13:	PC3	ADC12_INP13 (PC3_C ADC3_INP1)
		{	4,	ADC1,	ADC12_COMMON,	15,	},	// @14:	PC4	ADC12_INP4
		{	8,	ADC1,	ADC12_COMMON,	15,	},	// @15:	PC5	ADC12_INP8
		{	18,	ADC3,	ADC3_COMMON,	90,	},	// @16:	Temperature sensor (VSENSE) - 9.0 uS required
		{	19,	ADC3,	ADC3_COMMON,	43,	},	// @17:	Reference voltage (VREFINT) - 4.3 uS required
#elif CPUSTYLE_STM32MP1
		//	On ADC1, fast channels are PA6, PA7, PB0, PB1, PC4, PC5, PF11, PF12.
		//	On ADC2, fast channels are PA6, PA7, PB0, PB1, PC4, PC5, PF13, PF14.
		{	16,	ADC1,	ADC12_COMMON,	15,	},	// @0:	PA0		ADC1_INP16
		{	17,	ADC1,	ADC12_COMMON,	15,	},	// @1:	PA1		ADC1_INP17
		{	14,	ADC1,	ADC12_COMMON,	15,	},	// @2:	PA2		ADC1_INP14
		{	15,	ADC1,	ADC12_COMMON,	15,	},	// @3:	PA3		ADC1_INP15
		{	18,	ADC1,	ADC12_COMMON,	15,	},	// @4:	PA4		ADC1_INP18, ADC2_INP18
		{	19,	ADC1,	ADC12_COMMON,	15,	},	// @5:	PA5		ADC1_INP19, ADC2_INP19
		{	3,	ADC1,	ADC12_COMMON,	15,	},	// @6:	PA6		ADC1_INP3
		{	7,	ADC1,	ADC12_COMMON,	15,	},	// @7:	PA7		ADC1_INP7
		{	9,	ADC1,	ADC12_COMMON,	15,	},	// @8:	PB0		ADC1_INP9, ADC2_INP9
		{	5,	ADC1,	ADC12_COMMON,	15,	},	// @9:	PB1		ADC1_INP5, ADC2_INP5
		{	10,	ADC1,	ADC12_COMMON,	15,	},	// @10:	PC0		ADC1_INP10, ADC2_INP10
		{	11,	ADC1,	ADC12_COMMON,	15,	},	// @11:	PC1		ADC1_INP11, ADC2_INP11
		{	12,	ADC1,	ADC12_COMMON,	15,	},	// @12:	PC2		ADC1_INP12
		{	13,	ADC1,	ADC12_COMMON,	15,	},	// @13:	PC3		ADC1_INP13
		{	4,	ADC1,	ADC12_COMMON,	15,	},	// @14:	PC4		ADC1_INP4, ADC2_INP4
		{	8,	ADC1,	ADC12_COMMON,	15,	},	// @15:	PC5		ADC1_INP8, ADC2_INP8
		{	2, 	ADC1, 	ADC12_COMMON, 	15,	},	// @16: PF11	ADC1_INP2
		{	6, 	ADC1, 	ADC12_COMMON, 	15,	},	// @17: PF12	ADC1_INP6, ADC1_INN2
		{	2, 	ADC2, 	ADC12_COMMON, 	15,	},	// @18: PF13	ADC2_INP2
		{	6, 	ADC2, 	ADC12_COMMON, 	15,	},	// @19: PF14	ADC2_INP6, ADC2_INN2
		//{	18,	ADC3,	ADC3_COMMON,	90,	},	// @20:	Temperature sensor (VSENSE) - 9.0 uS required
		//{	19,	ADC3,	ADC3_COMMON,	43,	},	// @21:	Reference voltage (VREFINT) - 4.3 uS required
#endif /* CPUSTYLE_STM32H7XX, CPUSTYLE_STM32MP1 */
	};

	ASSERT(adci < (sizeof adcinmaps / sizeof adcinmaps [0]));
	return & adcinmaps [adci];
}

static void 
ADCs_IRQHandler(ADC_TypeDef * p)
{
	ASSERT(adc_input < board_get_adcinputs());
	const adcinmap_t * const adcmap = getadcmap(board_get_adcch(adc_input));
	ADC_TypeDef * const adc = adcmap->adc;
	ASSERT(adc == p);
	ASSERT((adc->ISR & ADC_ISR_OVR) == 0);
	ASSERT((adc->CR & (ADC_CR_JADSTART | ADC_CR_ADSTART)) == 0);
	if ((adc->ISR & ADC_ISR_EOS) != 0)
	{
		adc->ISR = ADC_ISR_EOS;		// EOS (end of regular sequence) flag
		board_adc_store_data(board_get_adcch(adc_input), (adc->DR & ADC_DR_RDATA) >> ADC_DR_RDATA_Pos);	// на этом цикле используем результат
		// Select next ADC input
		for (;;)
		{
			if (++ adc_input >= board_get_adcinputs())
			{
				spool_adcdonebundle();
				break;
			}
			else
			{
				// Select next ADC input (only one)
				const uint_fast8_t adci = board_get_adcch(adc_input);
				if (isadchw(adci))
				{
					// Установить следующий вход (блок ADC может измениться)
					const adcinmap_t * const adcmap = getadcmap(adci);
					ADC_TypeDef * const adc = adcmap->adc;

					ASSERT((adc->CR & (ADC_CR_JADSTART | ADC_CR_ADSTART)) == 0);
					adc->SQR1 = (adc->SQR1 & ~ (ADC_SQR1_L | ADC_SQR1_SQ1)) |
						0 * ADC_SQR1_L_0 |	// Выбираем преобразование с одного канала. Сканирования нет.
						adcmap->ch * ADC_SQR1_SQ1_0 |
						0;
					adc->CR |= ADC_CR_ADSTART;	// Запуск преобразования
					break;
				}
			}
		}
	}
	else
	{
		ASSERT(0);
	}
}

#if CPUSTYLE_STM32H7XX

	void
	ADC_IRQHandler(void)
	{
		ADCs_IRQHandler(ADC1);
	}

	void
	ADC3_IRQHandler(void)
	{
		ADCs_IRQHandler(ADC3);
	}

#elif CPUSTYLE_STM32MP1

	void
	ADC1_IRQHandler(void)
	{
		ADCs_IRQHandler(ADC1);
	}

	void
	ADC2_IRQHandler(void)
	{
		ADCs_IRQHandler(ADC2);
	}

#endif

#elif CPUSTYLE_STM32F1XX || CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX

static void
adcs_stm32f4xx_irq_handler(void)
{
	ASSERT(adc_input < board_get_adcinputs());
	//const unsigned long sr = ADC1->SR;
	ADC1->SR = 0;		// Сбрасываем все запросы прерывания.
	board_adc_store_data(board_get_adcch(adc_input), ADC1->DR & ADC_DR_DATA);	// на этом цикле используем результат
	// Select next ADC input
	for (;;)
	{
		if (++ adc_input >= board_get_adcinputs())
		{
			spool_adcdonebundle();
			break;
		}
		else
		{
			// Select next ADC input (only one)
			const uint_fast8_t adci = board_get_adcch(adc_input);
			if (isadchw(adci))
			{
				ADC1->SQR3 = (ADC1->SQR3 & ~ ADC_SQR3_SQ1) | (ADC_SQR3_SQ1_0 * adci);
				#if CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX
				ADC1->CR2 |= ADC_CR2_SWSTART;	// !!!!
				#endif
				break;
			}
		}
	}
}

void
ADC_IRQHandler(void)
{
	adcs_stm32f4xx_irq_handler();
}

void
ADC1_2_IRQHandler(void)
{
	adcs_stm32f4xx_irq_handler();
}



#elif CPUSTYLE_STM32F0XX
	#if STM32F0XX_MD
		void 
		ADC1_COMP_IRQHandler(void)
		{
			ASSERT(adc_input < board_get_adcinputs());
			board_adc_store_data(board_get_adcch(adc_input), ADC1->DR & ADC_DR_DATA);	// используем результат
			ADC1->ISR = ADC_ISR_EOC;
			ADC1->CHSELR = 1UL <<  board_get_adcch(adc_input);
			// Select next ADC input
			for (;;)
			{
				if (++ adc_input >= board_get_adcinputs())
				{
					spool_adcdonebundle();
					break;
				}
				else
				{
					// Select next ADC input (only one)
					const uint_fast8_t adci = board_get_adcch(adc_input);
					if (isadchw(adci))
					{
						ADC1->CHSELR = 1UL <<  board_get_adcch(adc_input);
						ADC1->CR = ADC_CR_ADSTART;	// ADC Start of Regular conversion
						break;
					}
				}
			}
		}
	#else /* STM32F0XX_MD */
		void 
		ADC1_IRQHandler(void)
		{
			ASSERT(adc_input < board_get_adcinputs());
			board_adc_store_data(board_get_adcch(adc_input), ADC1->DR & ADC_DR_DATA);	// используем результат
			ADC1->ISR = ADC_ISR_EOC;
			ADC1->CHSELR = 1UL <<  board_get_adcch(adc_input);
			// Select next ADC input
			for (;;)
			{
				if (++ adc_input >= board_get_adcinputs())
				{
					spool_adcdonebundle();
					break;
				}
				else
				{
					// Select next ADC input (only one)
					const uint_fast8_t adci = board_get_adcch(adc_input);
					if (isadchw(adci))
					{
						ADC1->CHSELR = 1UL << adci;
						ADC1->CR = ADC_CR_ADSTART;	// ADC Start of Regular conversion
						break;
					}
				}
			}
		}
	#endif /* STM32F0XX_MD */

#elif CPUSTYLE_STM32L0XX

	void 
	ADC1_COMP_IRQHandler(void)
	{
		ASSERT(adc_input < board_get_adcinputs());
		board_adc_store_data(board_get_adcch(adc_input), ADC1->DR & ADC_DR_DATA);	// используем результат
		ADC1->ISR = ADC_ISR_EOC;
		ADC1->CHSELR |= 1UL <<  board_get_adcch(adc_input);
		// Select next ADC input
		for (;;)
		{
			if (++ adc_input >= board_get_adcinputs())
			{
				spool_adcdonebundle();
				break;
			}
			else
			{
				// Select next ADC input (only one)
				const uint_fast8_t adci = board_get_adcch(adc_input);
				if (isadchw(adci))
				{
					ADC1->CHSELR |= 1UL << adci;
					ADC1->CR = ADC_CR_ADSTART;	// ADC Start of Regular conversion
					break;
				}
			}
		}
	}

#elif CPUSTYLE_STM32F30X

void 
ADC1_2_IRQHandler(void)
{
	ASSERT(adc_input < board_get_adcinputs());
	//const unsigned long sr = ADC1->SR;
	ADC1->ISR = 0;		// Сбрасываем все запросы прерывания.
	board_adc_store_data(board_get_adcch(adc_input), ADC1->DR & ADC_DR_RDATA);	// на этом цикле используем результат
	// Select next ADC input
	for (;;)
	{
		if (++ adc_input >= board_get_adcinputs())
		{
			spool_adcdonebundle();
			break;
		}
		else
		{
			// Select next ADC input (only one)
			const uint_fast8_t adci = board_get_adcch(adc_input);
			if (isadchw(adci))
			{
				ADC1->SQR1 = (ADC1->SQR1 & ~ ADC_SQR1_SQ1) | (ADC_SQR1_SQ1_0 * adci); 
				ADC1->CR |= ADC_CR_ADSTART;	// ADC Start of Regular conversion
				break;
			}
		}
	}
}

#elif CPUSTYLE_R7S721

void RAMFUNC_NONILINE
r7s721_adi_irq_handler(void)
{

	//dbg_putchar('.');
	//dbg_putchar('0' + adc_input);
	//dbg_putchar('0' + board_get_adcch(adc_input));

	//dbg_putchar(' ');
	/*
	static const volatile uint16_t * const adcports [HARDWARE_ADCINPUTS] =
	{
		& ADC.ADDRA,
		& ADC.ADDRB,
		& ADC.ADDRC,
		& ADC.ADDRD,
		& ADC.ADDRE,
		& ADC.ADDRF,
		& ADC.ADDRG,
		& ADC.ADDRH,
	};
	*/
	ASSERT(adc_input < board_get_adcinputs());
	//const unsigned long sr = ADC1->SR;
	////ADC1->ISR = 0;		// Сбрасываем все запросы прерывания.
	board_adc_store_data(board_get_adcch(adc_input), (& ADC.ADDRA) [board_get_adcch(adc_input)] >> 4);	// на этом цикле используем результат
	// Select next ADC input
	for (;;)
	{
		if (++ adc_input >= board_get_adcinputs())
		{
			ADC.ADCSR &= ~ ADC_SR_ADF;	// ADF: A/D end flag - Cleared by reading ADF while ADF = 1, then writing 0 to ADF
			// Это был последний вход
			spool_adcdonebundle();
			break;
		}
		else
		{
			// Select next ADC input (only one)
			const uint_fast8_t adci = board_get_adcch(adc_input);
			if (isadchw(adci))
			{
				ADC.ADCSR = (ADC.ADCSR & ~ (ADC_SR_ADF | ADC_SR_CH)) | 
					(adci << ADC_SR_CH_SHIFT) |	// канал для преобразования
					1 * ADC_SR_ADST |	// ADST: Single mode: A/D conversion starts
					0;
				break;
			}
		}
	}
}

#elif CPUSTYLE_T113 || CPUSTYLE_F133
	// ADC IRQ handler
	//#warning Unhandled CPUSTYLE_T113
#elif CPUSTYLE_A64
	// ADC IRQ handler

#elif CPUSTYLE_T507
	// ADC IRQ handler

#else
	// ADC IRQ handler
#endif


// хотя бы один вход (s-метр) есть.
void
hardware_adc_startonescan(void)
{
	//ASSERT((adc_input == 0) || (adc_input == board_get_adcinputs()));	// проверяем, успело ли отработать ранее запущенное преобразование
	if ((adc_input != 0) && (adc_input < board_get_adcinputs()))
		return;	// не успели
	// Ищем первый АЦП из встроеных в процессор
	for (adc_input = 0; adc_input < board_get_adcinputs(); ++ adc_input)
	{
		const uint_fast8_t adci = board_get_adcch(adc_input);
		if (isadchw(adci))
			break;
	}
	if (adc_input >= board_get_adcinputs())
		return;

#if CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	// Select next ADC input (only one)
	const portholder_t mask = ADC_CHER_CH0 << board_get_adcch(adc_input);
	ADC->ADC_CHER = mask; /* enable ADC */
	ADC->ADC_CHDR = ~ mask; /* disable ADC */
	ADC->ADC_CR = ADC_CR_START;	// Start the AD conversion

#elif CPUSTYLE_AT91SAM7S

	// Select next ADC input (only one)
	const portholder_t mask = AT91C_ADC_CH0 << board_get_adcch(adc_input);
	AT91C_BASE_ADC->ADC_CHDR = ~ mask; /* disable ADC inputs */
	AT91C_BASE_ADC->ADC_CHER = mask; /* enable ADC */
	AT91C_BASE_ADC->ADC_CR = AT91C_ADC_START;	// Start the AD conversion

#elif CPUSTYLE_ATMEGA

	ADMUX = hardware_atmega_admux(board_get_adcch(adc_input));
	// Start the AD conversion
	ADCSRA |= (1U << ADSC);

#elif CPUSTYLE_ATXMEGAXXXA4

	#warning TODO: write atxmega code - ADC start
	ADCA.CH0.MUXCTRL = board_get_adcch(adc_input);
	// Start the AD conversion
	ADCA.CH0.CTRL |= (1U << ADC_CH_START_bp);			// Start the AD conversion

#elif CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1
	// Установить следующий вход (блок ADC может измениться)
	const adcinmap_t * const adcmap = getadcmap(board_get_adcch(adc_input));
	ADC_TypeDef * const adc = adcmap->adc;

	if ((adc->CR & ADC_CR_ADEN) == 0)
		return;
	if ((adc->CR & ADC_CR_ADSTART) != 0)
		return;	// еще не закончилось ранее запущеное преобразование

	ASSERT((adc->CR & ADC_CR_JADSTART) == 0);
	ASSERT((adc->CR & ADC_CR_ADSTART) == 0);
	ASSERT((adc->CR & (ADC_CR_JADSTART | ADC_CR_ADSTART)) == 0);

	adc->SQR1 = (adc->SQR1 & ~ (ADC_SQR1_L | ADC_SQR1_SQ1)) |
		0 * ADC_SQR1_L_0 |	//Выбираем преобразование с одного канала. Сканирования нет.
		adcmap->ch * ADC_SQR1_SQ1_0 |
		0;
	adc->CR |= ADC_CR_ADSTART;	// Запуск преобразования

#elif CPUSTYLE_STM32F1XX || CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX
	//#warning TODO: to be implemented for CPUSTYLE_STM32F1XX

	// Установить следующий вход
	ADC1->SQR3 = (ADC1->SQR3 & ~ ADC_SQR3_SQ1) | (ADC_SQR3_SQ1_0 * board_get_adcch(adc_input));
	#if CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX
		ADC1->CR2 |= ADC_CR2_SWSTART;	// !!!!
	#endif

#elif CPUSTYLE_STM32F30X
	#warning TODO: Add code for STM32F30X support

	// Установить следующий вход
	ADC1->SQR1 = (ADC1->SQR1 & ~ ADC_SQR1_SQ1) | (ADC_SQR1_SQ1_0 * board_get_adcch(adc_input));
	ADC1->CR |= ADC_CR_ADSTART;	// ADC Start of Regular conversion

#elif CPUSTYLE_STM32L0XX

	ADC1->CHSELR = 1UL <<  board_get_adcch(adc_input);
	ADC1->CR = ADC_CR_ADSTART;	// ADC Start of Regular conversion

#elif CPUSTYLE_R7S721
	//#warning TODO: Add code for R7S721 ADC support
	// 27.4.1 Single Mode
	// Установить следующий вход
	ADC.ADCSR = (ADC.ADCSR & ~ (ADC_SR_CH)) |
		(board_get_adcch(adc_input) << ADC_SR_CH_SHIFT) |	// канал для преобразования
		1 * ADC_SR_ADST |	// ADST: Single mode: A/D conversion starts
		0;

#elif CPUSTYLE_STM32F0XX
	#warning: #warning Must be implemented for this CPU

#elif CPUSTYLE_T113 || CPUSTYLE_F133

	//#warning Unhandled CPUSTYLE_T113
#elif CPUSTYLE_A64

#else

	//#warning Undefined CPUSTYLE_XXX

#endif
}

#endif /* WITHCPUADCHW */


uint32_t hardware_get_random(void)
{
#if CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX

	#if defined(STM32F446xx)
		#warning RNG not exist
		return 0;
	#elif defined(STM32F401xC)
		#warning RNG not exist
		return 0;
	#else
		if ((RCC->AHB2ENR & RCC_AHB2ENR_RNGEN) == 0)
		{
			RCC->AHB2ENR |= RCC_AHB2ENR_RNGEN;	/* RNG clock enable */
			(void) RCC->AHB2ENR;
			RNG->CR |= RNG_CR_RNGEN;
		}

		while ((RNG->SR & RNG_SR_DRDY) == 0)
			;
		return RNG->DR;
	#endif

#elif CPUSTYLE_STM32L0XX && defined (RCC_AHBENR_RNGEN)

	if ((RCC->AHBENR & RCC_AHBENR_RNGEN) == 0)
	{
		RCC->AHBENR |= RCC_AHBENR_RNGEN;	/* RNG clock enable */
		(void) RCC->AHBENR;
		RNG->CR |= RNG_CR_RNGEN;
	}

	while ((RNG->SR & RNG_SR_DRDY) == 0)
		;
	return RNG->DR;

#elif CPUSTYLE_T507 || CPUSTYLE_H616 || CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64 || CPUSTYLE_H3 || CPUSTYLE_A133

	return cpu_getdebugticks();

#else

	//#warning RNG not exist - hardware_get_random not work
	return 0;

#endif


}


#if CPUSTYLE_ARM || CPUSTYLE_RISCV

// количество циклов на микросекунду
static unsigned long
local_delay_uscycles(unsigned timeUS, unsigned cpufreq_MHz)
{
#if CPUSTYLE_AT91SAM7S
	#warning TODO: calibrate constant	 looks like CPUSTYLE_STM32MP1
	const unsigned long top = timeUS * 175uL / cpufreq_MHz;
	//const unsigned long top = 55 * cpufreq_MHz * timeUS / 1000;
#elif CPUSTYLE_ATSAM3S
	#warning TODO: calibrate constant looks like CPUSTYLE_STM32MP1
	const unsigned long top = timeUS * 270uL / cpufreq_MHz;
	//const unsigned long top = 55 * cpufreq_MHz * timeUS / 1000;
#elif CPUSTYLE_ATSAM4S
	#warning TODO: calibrate constant looks like CPUSTYLE_STM32MP1
	const unsigned long top = timeUS * 270uL / cpufreq_MHz;
	//const unsigned long top = 55 * cpufreq_MHz * timeUS / 1000;
#elif CPUSTYLE_STM32F0XX
	#warning TODO: calibrate constant looks like CPUSTYLE_STM32MP1
	const unsigned long top = timeUS * 190uL / cpufreq_MHz;
	//const unsigned long top = 55 * cpufreq_MHz * timeUS / 1000;
#elif CPUSTYLE_RP20XX
	const unsigned long top = timeUS * 1480uL / cpufreq_MHz;
#elif CPUSTYLE_STM32L0XX
	#warning TODO: calibrate constant looks like CPUSTYLE_STM32MP1
	const unsigned long top = timeUS * 20uL / cpufreq_MHz;
	//const unsigned long top = 55 * cpufreq_MHz * timeUS / 1000;
#elif CPUSTYLE_STM32F1XX
	#warning TODO: calibrate constant looks like CPUSTYLE_STM32MP1
	const unsigned long top = timeUS * 345uL / cpufreq_MHz;
	//const unsigned long top = 55 * cpufreq_MHz * timeUS / 1000;
#elif CPUSTYLE_STM32F30X
	#warning TODO: calibrate constant looks like CPUSTYLE_STM32MP1
	const unsigned long top = timeUS * 430uL / cpufreq_MHz;
	//const unsigned long top = 55 * cpufreq_MHz * timeUS / 1000;
#elif CPUSTYLE_STM32F4XX
	#warning TODO: calibrate constant looks like CPUSTYLE_STM32MP1
	const unsigned long top = timeUS * 3800uL / cpufreq_MHz;
#elif CPUSTYLE_STM32F7XX
	#warning TODO: calibrate constant looks like CPUSTYLE_STM32MP1
	const unsigned long top = 55uL * cpufreq_MHz * timeUS / 1000;
#elif CPUSTYLE_STM32H7XX
	#warning TODO: calibrate constant looks like CPUSTYLE_STM32MP1
	const unsigned long top = 77uL * cpufreq_MHz * timeUS / 1000;
#elif CPUSTYLE_R7S721
	const unsigned long top = 105uL * cpufreq_MHz * timeUS / 1000;
#elif CPUSTYLE_XC7Z
	const unsigned long top = 125uL * cpufreq_MHz * timeUS / 1000;
#elif CPUSTYLE_RK356X
	const unsigned long top = 125uL * cpufreq_MHz * timeUS / 1000;
#elif CPUSTYLE_STM32MP1
	// калибровано для 800 МГц Cortex-A7 процессора
	const unsigned long top = 120uL * cpufreq_MHz * timeUS / 1000;
#elif CPUSTYLE_H3
	// калибровано для 800 МГц Cortex-A7 процессора
	const unsigned long top = 120uL * cpufreq_MHz * timeUS / 1000;
#elif CPUSTYLE_CA53
	// калибровано для Cortex-A53 процессора
	const unsigned long top = 145uL * cpufreq_MHz * timeUS / 1000;
#elif CPUSTYLE_T113
	// калибровано для 1200 МГц Cortex-A7 процессора
	const unsigned long top = 120uL * cpufreq_MHz * timeUS / 1000;
#elif CPUSTYLE_V3S
	// калибровано для 1200 МГц Cortex-A7 процессора
	const unsigned long top = 120uL * cpufreq_MHz * timeUS / 1000;
#elif CPUSTYLE_F133
	// калибровано для 1200 МГц RISC-V C906 процессора
	const unsigned long top = 165uL * cpufreq_MHz * timeUS / 1000;
#elif CPUSTYLE_VM14
	// калибровано для 1200 МГц процессора
	const unsigned long top = 165uL * cpufreq_MHz * timeUS / 1000;
#else
	#error TODO: calibrate constant looks like CPUSTYLE_STM32MP1
	const unsigned long top = 55uL * cpufreq_MHz * timeUS / 1000;
#endif
	return top;
}

static unsigned cpufreqMHz = 10;
// Атрибут RAMFUNC_NONILINE убран, так как функция
// используется в инициализации SDRAM на процессорах STM32F746.
// TODO: перекалибровать для FLASH контроллеров.
void /* RAMFUNC_NONILINE */ local_delay_us(int timeUS)
{
	if (timeUS == 0)
		return;
#if 0 //LINUX_SUBSYSTEM
	usleep(timeUS);
#else
	// Частота процессора приволится к мегагерцам.
	const unsigned long top = local_delay_uscycles(timeUS, cpufreqMHz);
	//
	volatile unsigned long t;
	for (t = 0; t < top; ++ t)
	{
	}
#endif /* LINUX_SUBSYSTEM */
}
// exactly as required
//
void local_delay_ms(int timeMS)
{
#if 0 //LINUX_SUBSYSTEM
	usleep(timeMS * 1000);
#else
	if (timeMS == 0)
		return;
	// Частота процессора приволится к мегагерцам.
	const unsigned long top = local_delay_uscycles(1000, cpufreqMHz);
	int n;
	for (n = 0; n < timeMS; ++ n)
	{
		volatile unsigned long t;
		for (t = 0; t < top; ++ t)
		{
		}
	}
#endif /* LINUX_SUBSYSTEM */
}

// задержка до того как включили MMU и cache */
void local_delay_ms_nocache(int timeMS)
{
	int t = timeMS / 25;
	if (t == 0)
		t = 1;
	local_delay_ms(t);
}

void local_delay_initialize(void)
{
	cpufreqMHz = CPU_FREQ / 1000000;
}

#endif /* */


#if CPUSTYLE_STM32H7XX

// MPU initialize
static void lowlevel_stm32h7xx_mpu_initialize(void)
{
	/* Disables the MPU */
	MPU->CTRL = (MPU->CTRL & ~ (MPU_CTRL_ENABLE_Msk)) |
		0 * MPU_CTRL_ENABLE_Msk |
		0;


 
#define INNER_NORMAL_WB_RWA_TYPE(x)   (( 0x04 << MPU_RASR_TEX_Pos ) | ( DISABLE  << MPU_RASR_C_Pos ) | ( ENABLE  << MPU_RASR_B_Pos )  | ( x << MPU_RASR_S_Pos )) 
#define INNER_NORMAL_WB_NWA_TYPE(x)   (( 0x04 << MPU_RASR_TEX_Pos ) | ( ENABLE  << MPU_RASR_C_Pos )  | ( ENABLE  << MPU_RASR_B_Pos )  | ( x << MPU_RASR_S_Pos )) 
#define STRONGLY_ORDERED_SHAREABLE_TYPE      (( 0x00 << MPU_RASR_TEX_Pos ) | ( DISABLE << MPU_RASR_C_Pos ) | ( DISABLE << MPU_RASR_B_Pos ))     // DO not care // 
#define SHAREABLE_DEVICE_TYPE                (( 0x00 << MPU_RASR_TEX_Pos ) | ( DISABLE << MPU_RASR_C_Pos ) | ( ENABLE  << MPU_RASR_B_Pos ))     // DO not care // 
 

	// SRAM
	/* Set the Region base address and region number */
	MPU->RBAR = D1_AXISRAM_BASE | MPU_RBAR_VALID_Msk | 0x00;
    MPU->RASR = 
		(0x00 << MPU_RASR_XN_Pos)   |	// DisableExec
		(0x03 << MPU_RASR_AP_Pos)   |	// AccessPermission
		(0x04 << MPU_RASR_TEX_Pos)  |	// TypeExtField
		(0x01 << MPU_RASR_C_Pos)    |	// IsCacheable
	 	(0x01 << MPU_RASR_B_Pos)    |	// IsBufferable
		(0x00 << MPU_RASR_S_Pos)    |	// IsShareable
		(0x00 << MPU_RASR_SRD_Pos)  |	// SubRegionDisable (8 bits mask)
		(0x12 << MPU_RASR_SIZE_Pos) |	// Size 512 kB
		(0x01 << MPU_RASR_ENABLE_Pos) |	// Enable
		0;
	// ITCM
	/* Set the Region base address and region number */
	MPU->RBAR = D1_ITCMRAM_BASE | MPU_RBAR_VALID_Msk | 0x01;
    MPU->RASR = 
		(0x00 << MPU_RASR_XN_Pos)   |	// DisableExec
		(0x03 << MPU_RASR_AP_Pos)   |	// AccessPermission
		(0x04 << MPU_RASR_TEX_Pos)  |	// TypeExtField
		(0x01 << MPU_RASR_C_Pos)    |	// IsCacheable
	 	(0x01 << MPU_RASR_B_Pos)    |	// IsBufferable
		(0x00 << MPU_RASR_S_Pos)    |	// IsShareable
		(0x00 << MPU_RASR_SRD_Pos)  |	// SubRegionDisable (8 bits mask)
		(0x0F << MPU_RASR_SIZE_Pos) |	// Size 64 kB
		(0x01 << MPU_RASR_ENABLE_Pos) |	// Enable
		0;
	// DTCM
	/* Set the Region base address and region number */
	MPU->RBAR = D1_DTCMRAM_BASE | MPU_RBAR_VALID_Msk | 0x02;
    MPU->RASR = 
		(0x00 << MPU_RASR_XN_Pos)   |	// DisableExec
		(0x03 << MPU_RASR_AP_Pos)   |	// AccessPermission
		(0x04 << MPU_RASR_TEX_Pos)  |	// TypeExtField
		(0x01 << MPU_RASR_C_Pos)    |	// IsCacheable
	 	(0x01 << MPU_RASR_B_Pos)    |	// IsBufferable
		(0x00 << MPU_RASR_S_Pos)    |	// IsShareable
		(0x00 << MPU_RASR_SRD_Pos)  |	// SubRegionDisable (8 bits mask)
		(0x10 << MPU_RASR_SIZE_Pos) |	// Size 128 kB
		(0x01 << MPU_RASR_ENABLE_Pos) |	// Enable
		0;
	// FLASH
	/* Set the Region base address and region number */
	MPU->RBAR = D1_AXIFLASH_BASE | MPU_RBAR_VALID_Msk | 0x03;
    MPU->RASR = 
		(0x00 << MPU_RASR_XN_Pos)   |	// DisableExec
		(0x03 << MPU_RASR_AP_Pos)   |	// AccessPermission
		(0x04 << MPU_RASR_TEX_Pos)  |	// TypeExtField
		(0x01 << MPU_RASR_C_Pos)    |	// IsCacheable
	 	(0x01 << MPU_RASR_B_Pos)    |	// IsBufferable
		(0x00 << MPU_RASR_S_Pos)    |	// IsShareable
		(0x00 << MPU_RASR_SRD_Pos)  |	// SubRegionDisable (8 bits mask)
		(0x14 << MPU_RASR_SIZE_Pos) |	// Size 2 MB
		(0x01 << MPU_RASR_ENABLE_Pos) |	// Enable
		0;
	// DEVICE
	/* Set the Region base address and region number */
	MPU->RBAR = PERIPH_BASE | MPU_RBAR_VALID_Msk | 0x04;
    MPU->RASR = 
		(0x00 << MPU_RASR_XN_Pos)   |	// DisableExec
		(0x03 << MPU_RASR_AP_Pos)   |	// AccessPermission
		(0x04 << MPU_RASR_TEX_Pos)  |	// TypeExtField
		(0x00 << MPU_RASR_C_Pos)    |	// IsCacheable
	 	(0x00 << MPU_RASR_B_Pos)    |	// IsBufferable
		(0x00 << MPU_RASR_S_Pos)    |	// IsShareable
		(0x00 << MPU_RASR_SRD_Pos)  |	// SubRegionDisable (8 bits mask)
		(0x1B << MPU_RASR_SIZE_Pos) |	// Size 256 MB
		(0x01 << MPU_RASR_ENABLE_Pos) |	// Enable
		0;
	/* Enables the MPU */
	MPU->CTRL = (MPU->CTRL & ~ (MPU_CTRL_ENABLE_Msk)) |
		1 * MPU_CTRL_ENABLE_Msk |
		0;
}

#endif /* CPUSTYLE_STM32H7XX */

#if (__CORTEX_A != 0) && (! defined(__aarch64__))

/** \brief  Enable Floating Point Unit

  Critical section, called from undef handler, so systick is disabled
 */
__STATIC_INLINE void __FPU_Enable_fixed(void)
{
  __ASM volatile(
    //Permit access to VFP/NEON, registers by modifying CPACR
    "        MRC     p15,0,R1,c1,c0,2  \n"
	"        MOVW    R2, #:lower16:0x00F00000   \n"
	"        MOVT    R2, #:upper16:0x00F00000   \n"
	"        ORR     R1,R1,R2 \n"
	"        MCR     p15,0,R1,c1,c0,2  \n"

    //Ensure that subsequent instructions occur in the context of VFP/NEON access permitted
    "        ISB                       \n"

    //Enable VFP/NEON
    "        VMRS    R1,FPEXC          \n"
	"        MOVW    R2, #:lower16:0x40000000   \n"
	"        MOVT    R2, #:upper16:0x40000000   \n"
	"        ORR     R1,R1,R2 \n"
    "        VMSR    FPEXC,R1          \n"

    //Initialise VFP/NEON registers to 0
    "        MOV     R2,#0             \n"

    //Initialise D16 registers to 0
    "        VMOV    D0, R2,R2         \n"
    "        VMOV    D1, R2,R2         \n"
    "        VMOV    D2, R2,R2         \n"
    "        VMOV    D3, R2,R2         \n"
    "        VMOV    D4, R2,R2         \n"
    "        VMOV    D5, R2,R2         \n"
    "        VMOV    D6, R2,R2         \n"
    "        VMOV    D7, R2,R2         \n"
    "        VMOV    D8, R2,R2         \n"
    "        VMOV    D9, R2,R2         \n"
    "        VMOV    D10,R2,R2         \n"
    "        VMOV    D11,R2,R2         \n"
    "        VMOV    D12,R2,R2         \n"
    "        VMOV    D13,R2,R2         \n"
    "        VMOV    D14,R2,R2         \n"
    "        VMOV    D15,R2,R2         \n"

#if (defined(__ARM_NEON) && (__ARM_NEON == 1))
    //Initialise D32 registers to 0
    "        VMOV    D16,R2,R2         \n"
    "        VMOV    D17,R2,R2         \n"
    "        VMOV    D18,R2,R2         \n"
    "        VMOV    D19,R2,R2         \n"
    "        VMOV    D20,R2,R2         \n"
    "        VMOV    D21,R2,R2         \n"
    "        VMOV    D22,R2,R2         \n"
    "        VMOV    D23,R2,R2         \n"
    "        VMOV    D24,R2,R2         \n"
    "        VMOV    D25,R2,R2         \n"
    "        VMOV    D26,R2,R2         \n"
    "        VMOV    D27,R2,R2         \n"
    "        VMOV    D28,R2,R2         \n"
    "        VMOV    D29,R2,R2         \n"
    "        VMOV    D30,R2,R2         \n"
    "        VMOV    D31,R2,R2         \n"
#endif

    //Initialise FPSCR to a known state
    "        VMRS    R1,FPSCR          \n"
	"        MOVW    R2, #:lower16:0x00086060   \n"
	"        MOVT    R2, #:upper16:0x00086060   \n"
	"        AND     R1,R1,R2          \n" //Mask off all bits that do not have to be preserved. Non-preserved bits can/should be zero.
    "        VMSR    FPSCR,R1            "
    : : : "cc", "r1", "r2"
  );
}

//	MRC p15, 0, <Rt>, c6, c0, 2 ; Read IFAR into Rt
//	MCR p15, 0, <Rt>, c6, c0, 2 ; Write Rt to IFAR

/** \brief  Get IFAR
\return		Instruction Fault Address register value
*/
uint32_t __get_IFAR(void)
{
	uint32_t result;
	__get_CP(15, 0, result, 6, 0, 2);
	return result;
}

//	MRC p15, 0, <Rt>, c6, c0, 0 ; Read DFAR into Rt
//	MCR p15, 0, <Rt>, c6, c0, 0 ; Write Rt to DFAR

/** \brief  Get DFAR
\return		Data Fault Address register value
*/
uint32_t __get_DFAR(void)
{
	uint32_t result;
	__get_CP(15, 0, result, 6, 0, 0);
	return result;
}
#endif

#if (__CORTEX_A != 0)

void Undef_Handler(void)
{
	const volatile uint32_t marker = 0xDEADBEEF;

	PRINTF("UndefHandler trapped[%p]\n", Undef_Handler);
	PRINTF("CPUID=%d\n", (int) (__get_MPIDR() & 0x03));
	unsigned i;
	for (i = 0; i < 8; ++ i)
	{
		PRINTF("marker[%2d]=%08X\n", i, (unsigned) (& marker) [i]);
	}
	for (;;)
		;
}

void SWI_Handler(void)
{
	const volatile uint32_t marker = 0xDEADBEEF;
	dbg_puts_impl_P("SWIHandler trapped.\n");
	PRINTF("CPUID=%d\n", (int) (__get_MPIDR() & 0x03));
	unsigned i;
	for (i = 0; i < 8; ++ i)
	{
		PRINTF("marker [%2d] = %08X\n", i, (unsigned) (& marker) [i]);
	}
//	for (;;)
//		;
}

// Prefetch Abort
void PAbort_Handler(void)
{
	const volatile uint32_t marker = 0xDEADBEEF;
	dbg_puts_impl_P(PSTR("PAbort_Handler trapped.\n"));
	dbg_puts_impl_P((__get_MPIDR() & 0x03) ? PSTR("CPUID=1\n") : PSTR("CPUID=0\n"));
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Warray-bounds"
	PRINTF(PSTR("DFSR=%08X, IFAR=%08X, pc=%08X, sp~%08x __get_MPIDR()=%08X\n"), (unsigned) __get_DFSR(), (unsigned) __get_IFAR(), (unsigned) (& marker) [2], (unsigned) (uintptr_t) & marker, (unsigned) __get_MPIDR());
#pragma GCC diagnostic pop
	const int WnR = (__get_DFSR() & (1uL << 11)) != 0;
	const int Status = (__get_DFSR() & (0x0FuL << 0));
	/*
		1. 0b000001 alignment fault
		2. 0b000100 instruction cache maintenance fault
		3. 0bx01100 1st level translation, synchronous external abort
		4. 0bx01110 2nd level translation, synchronous external abort
		5. 0b000101 translation fault, section
		6. 0b000111 translation fault, page
		7. 0b000011 access flag fault, section
		8. 0b000110 access flag fault, page
		9. 0b001001 domain fault, section
		10. 0b001011 domain fault, page
		11. 0b001101 permission fault, section
		12. 0b001111 permission fault, page
		13. 0bx01000 synchronous external abort, nontranslation
		14. 0bx10110 asynchronous external abort
		15. 0b000010 debug event.
	*/
	PRINTF(PSTR(" WnR=%d, Status=%02X\n"), (int) WnR, (unsigned) Status);
	switch (Status)
	{
	case 0x01: PRINTF(PSTR("alignment fault\n")); break;
	case 0x04: PRINTF(PSTR("instruction cache maintenance fault\n")); break;
	case 0x0C: PRINTF(PSTR("1st level translation, synchronous external abort\n")); break;
	case 0x0E: PRINTF(PSTR("2nd level translation, synchronous external abort\n")); break;
	case 0x05: PRINTF(PSTR("translation fault, section\n")); break;
	case 0x07: PRINTF(PSTR("translation fault, page\n")); break;
	case 0x03: PRINTF(PSTR("access flag fault, section\n")); break;
	case 0x06: PRINTF(PSTR("access flag fault, page\n")); break;
	case 0x09: PRINTF(PSTR("domain fault, section\n")); break;
	case 0x0B: PRINTF(PSTR("domain fault, page\n")); break;
	case 0x0D: PRINTF(PSTR("permission fault, section\n")); break;
	case 0x0F: PRINTF(PSTR("permission fault, page\n")); break;
	case 0x08: PRINTF(PSTR("synchronous external abort, nontranslation\n")); break;
	case 0x16: PRINTF(PSTR("asynchronous external abort\n")); break;
	case 0x02: PRINTF(PSTR("debug event.\n")); break;
	default: PRINTF(PSTR("undefined Status=%02X\n"), Status); break;
	}
//	unsigned i;
//	for (i = 0; i < 8; ++ i)
//	{
//		PRINTF("marker [%2d] = %08X\n", i, (unsigned) (& marker) [i]);
//	}
	for (;;)
	{
#if defined (BOARD_BLINK_SETSTATE)
		BOARD_BLINK_SETSTATE(1);
		local_delay_ms(250);
		BOARD_BLINK_SETSTATE(0);
		local_delay_ms(250);
#endif /* defined (BOARD_BLINK_SETSTATE) */
	}
}

// Data Abort.
void DAbort_Handler(void)
{
	const volatile uint32_t marker = 0xDEADBEEF;
	dbg_puts_impl_P(PSTR("DAbort_Handler trapped. CPUID="));
	dbg_putchar('0' + (__get_MPIDR() & 0x03));
	dbg_putchar('\n');
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Warray-bounds"
	PRINTF(PSTR("DFSR=%08X, DFAR=%08X, pc=%08X, sp~%08X\n"), (unsigned) __get_DFSR(), (unsigned) __get_DFAR(), (unsigned) (& marker) [2], (unsigned) (uintptr_t) & marker);
#pragma GCC diagnostic pop
	const unsigned WnR = (__get_DFSR() & (UINT32_C(1) << 11)) != 0;
	const unsigned Status = (__get_DFSR() & (UINT32_C(0x0F) << 0));
	/*
		1. 0b000001 alignment fault
		2. 0b000100 instruction cache maintenance fault
		3. 0bx01100 1st level translation, synchronous external abort
		4. 0bx01110 2nd level translation, synchronous external abort
		5. 0b000101 translation fault, section
		6. 0b000111 translation fault, page
		7. 0b000011 access flag fault, section
		8. 0b000110 access flag fault, page
		9. 0b001001 domain fault, section
		10. 0b001011 domain fault, page
		11. 0b001101 permission fault, section
		12. 0b001111 permission fault, page
		13. 0bx01000 synchronous external abort, nontranslation
		14. 0bx10110 asynchronous external abort
		15. 0b000010 debug event.
	*/
	PRINTF(PSTR(" WnR=%d, Status=%02X\n"), (int) WnR, (unsigned) Status);
	switch (Status)
	{
	case 0x01: PRINTF(PSTR("alignment fault\n")); break;
	case 0x04: PRINTF(PSTR("instruction cache maintenance fault\n")); break;
	case 0x0C: PRINTF(PSTR("1st level translation, synchronous external abort\n")); break;
	case 0x0E: PRINTF(PSTR("2nd level translation, synchronous external abort\n")); break;
	case 0x05: PRINTF(PSTR("translation fault, section\n")); break;
	case 0x07: PRINTF(PSTR("translation fault, page\n")); break;
	case 0x03: PRINTF(PSTR("access flag fault, section\n")); break;
	case 0x06: PRINTF(PSTR("access flag fault, page\n")); break;
	case 0x09: PRINTF(PSTR("domain fault, section\n")); break;
	case 0x0B: PRINTF(PSTR("domain fault, page\n")); break;
	case 0x0D: PRINTF(PSTR("permission fault, section\n")); break;
	case 0x0F: PRINTF(PSTR("permission fault, page\n")); break;
	case 0x08: PRINTF(PSTR("synchronous external abort, nontranslation\n")); break;
	case 0x16: PRINTF(PSTR("asynchronous external abort\n")); break;
	case 0x02: PRINTF(PSTR("debug event.\n")); break;
	default: PRINTF(PSTR("undefined Status=%02X\n"), Status); break;
	}
	unsigned i;
	for (i = 0; i < 8; ++ i)
	{
		PRINTF("marker [%2d] = %08X\n", i, (unsigned) (& marker) [i]);
	}
	for (;;)
	{
#if defined (BOARD_BLINK_SETSTATE)
		BOARD_BLINK_SETSTATE(1);
		local_delay_ms(1250);
		BOARD_BLINK_SETSTATE(0);
		local_delay_ms(1250);
#endif /* defined (BOARD_BLINK_SETSTATE) */
	}
}

void FIQ_Handler(void)
{
	dbg_puts_impl_P(PSTR("FIQ_Handler trapped. CPUID="));
	dbg_putchar('0' + (__get_MPIDR() & 0x03));
	dbg_putchar('\n');
	for (;;)
		;
}

void Hyp_Handler(void)
{
	dbg_puts_impl_P(PSTR("Hyp_Handler trapped. CPUID="));
	dbg_putchar('0' + (__get_MPIDR() & 0x03));
	dbg_putchar('\n');
	for (;;)
		;
}

#endif /* (__CORTEX_A != 0) */

#if CPUSTYLE_ARM_CM7

// Сейчас в эту память будем читать по DMA
// Убрать копию этой области из кэша
// Используется только в startup
void dcache_invalidate(uintptr_t base, int_fast32_t dsize)
{
	//ASSERT((base % 32) == 0);		// при работе с BACKUP SRAM невыровненно
	SCB_InvalidateDCache_by_Addr((void *) base, dsize);	// DCIMVAC register used.
}

// Сейчас эта память будет записываться по DMA куда-то
// Записать содержимое кэша данных в память
void dcache_clean(uintptr_t base, int_fast32_t dsize)
{
	//ASSERT((base % 32) == 0);		// при работе с BACKUP SRAM невыровненно
	SCB_CleanDCache_by_Addr((void *) base, dsize);	// DCCMVAC register used.
}

// Записать содержимое кэша данных в память
// применяется после начальной инициализации среды выполнния
void dcache_clean_all(void)
{
	SCB_CleanDCache();	// DCCMVAC register used.
}

// Сейчас эта память будет записываться по DMA куда-то. Потом содержимое не требуется
// Записать содержимое кэша данных в память
// Убрать копию этой области из кэша
void dcache_clean_invalidate(uintptr_t base, int_fast32_t dsize)
{
	//ASSERT((base % 32) == 0);		// при работе с BACKUP SRAM невыровненно
	SCB_CleanInvalidateDCache_by_Addr((void *) base, dsize);	// DCCIMVAC register used.
}


int_fast32_t dcache_rowsize(void)
{
	return DCACHEROWSIZE;
}


int_fast32_t icache_rowsize(void)
{
	return ICACHEROWSIZE;
}

#elif ((__CORTEX_A != 0) || CPUSTYLE_ARM9)

//	MVA
//	For more information about the possible meaning when the table shows that an MVA is required
// 	see Terms used in describing the maintenance operations on page B2-1272.
// 	When the data is stated to be an MVA, it does not have to be cache line aligned.

void L1_CleanDCache_by_Addr(void * addr, int32_t op_size)
{
	if (op_size > 0)
	{
		//int32_t op_size = dsize + (((uintptr_t) addr) & (DCACHEROWSIZE - 1U));
		uintptr_t op_mva = (uintptr_t) addr;
		__DSB();
		do
		{
			__set_DCCMVAC(op_mva);	// Clean data cache line by address.
			op_mva += DCACHEROWSIZE;
			op_size -= DCACHEROWSIZE;
		} while (op_size > 0);
		__DMB();     // ensure the ordering of data cache maintenance operations and their effects
	}
}

void L1_CleanInvalidateDCache_by_Addr(void * addr, int32_t op_size)
{
	if (op_size > 0)
	{
		//int32_t op_size = dsize + (((uintptr_t) addr) & (DCACHEROWSIZE - 1U));
		uintptr_t op_mva = (uintptr_t) addr;
		__DSB();
		do
		{
			__set_DCCIMVAC(op_mva);	// Clean and Invalidate data cache by address.
			op_mva += DCACHEROWSIZE;
			op_size -= DCACHEROWSIZE;
		} while (op_size > 0);
		__DMB();     // ensure the ordering of data cache maintenance operations and their effects
	}
}

void L1_InvalidateDCache_by_Addr(void * addr, int32_t op_size)
{
	if (op_size > 0)
	{
		//int32_t op_size = dsize + (((uintptr_t) addr) & (DCACHEROWSIZE - 1U));
		uintptr_t op_mva = (uintptr_t) addr;
		do
		{
			__set_DCIMVAC(op_mva);	// Invalidate data cache line by address.
			op_mva += DCACHEROWSIZE;
			op_size -= DCACHEROWSIZE;
		} while (op_size > 0);
		// Cache Invalidate operation is not follow by memory-writes
	}
}

#if (! defined(__aarch64__))
/** \brief  Get CTR
\return		Cache Type Register value
*/
uint32_t __get_CTR(void)
{
	uint32_t result;
	__get_CP(15, 0, result, 0, 0, 1);
	return result;
}
#endif

int_fast32_t dcache_rowsize(void)
{
	const uint32_t v = __get_CTR();
	const uint32_t DminLine = (v >> 16) & 0x0F;	// Log2 of the number of words in the smallest cache line of all the data caches and unified caches that are controlled by the processor
	return 4 << DminLine;
}

int_fast32_t icache_rowsize(void)
{
	const uint32_t v = __get_CTR();
	const uint32_t IminLine = (v >> 0) & 0x0F;	// Log2 of the number of words in the smallest cache line of all the instruction caches and unified caches that are controlled by the processor
	return 4 << IminLine;
}

#if (__L2C_PRESENT == 1)

void L2_CleanDCache_by_Addr(void *__restrict addr, int32_t dsize)
{
	if (dsize > 0)
	{
		int32_t op_size = dsize + (((uintptr_t) addr) & (DCACHEROWSIZE - 1U));
		uintptr_t op_addr = (uintptr_t) addr /* & ~ (uintptr_t) (DCACHEROWSIZE - 1U) */;
		do
		{
			// Clean cache by physical address
			L2C_310->CLEAN_LINE_PA = op_addr;	// Atomic operation. These operations stall the slave ports until they are complete.
			op_addr += DCACHEROWSIZE;
			op_size -= DCACHEROWSIZE;
		} while (op_size > 0);
	}
}

void L2_CleanInvalidateDCache_by_Addr(void *__restrict addr, int32_t dsize)
{
	if (dsize > 0)
	{
		int32_t op_size = dsize + (((uintptr_t) addr) & (DCACHEROWSIZE - 1U));
		uintptr_t op_addr = (uintptr_t) addr /* & ~ (uintptr_t) (DCACHEROWSIZE - 1U) */;
		do
		{
			// Clean and Invalidate cache by physical address
			L2C_310->CLEAN_INV_LINE_PA = op_addr;	// Atomic operation. These operations stall the slave ports until they are complete.
			op_addr += DCACHEROWSIZE;
			op_size -= DCACHEROWSIZE;
		} while (op_size > 0);
	}
}

void L2_InvalidateDCache_by_Addr(void *__restrict addr, int32_t dsize)
{
	if (dsize > 0)
	{
		int32_t op_size = dsize + (((uintptr_t) addr) & (DCACHEROWSIZE - 1U));
		uintptr_t op_addr = (uintptr_t) addr /* & ~ (uintptr_t) (DCACHEROWSIZE - 1U) */;
		do
		{
			// Invalidate cache by physical address
			L2C_310->INV_LINE_PA = op_addr;	// Atomic operation. These operations stall the slave ports until they are complete.
			op_addr += DCACHEROWSIZE;
			op_size -= DCACHEROWSIZE;
		} while (op_size > 0);
	}
}
#endif /* (__L2C_PRESENT == 1) */

// Записать содержимое кэша данных в память
// применяется после начальной инициализации среды выполнния
void dcache_clean_all(void)
{
	L1C_CleanInvalidateDCacheAll();
#if (__L2C_PRESENT == 1)
	L2C_CleanInvAllByWay();
#endif
}

// Сейчас в эту память будем читать по DMA
void dcache_invalidate(uintptr_t addr, int_fast32_t dsize)
{
	L1_InvalidateDCache_by_Addr((void *) addr, dsize);
#if (__L2C_PRESENT == 1)
	L2_InvalidateDCache_by_Addr((void *) addr, dsize);
#endif /* (__L2C_PRESENT == 1) */
}

// Сейчас эта память будет записываться по DMA куда-то
void dcache_clean(uintptr_t addr, int_fast32_t dsize)
{
	L1_CleanDCache_by_Addr((void *) addr, dsize);
#if (__L2C_PRESENT == 1)
	L2_CleanDCache_by_Addr((void *) addr, dsize);
#endif /* (__L2C_PRESENT == 1) */
}

// Сейчас эта память будет записываться по DMA куда-то. Потом содержимое не требуется
void dcache_clean_invalidate(uintptr_t addr, int_fast32_t dsize)
{
	L1_CleanInvalidateDCache_by_Addr((void *) addr, dsize);
#if (__L2C_PRESENT == 1)
	L2_CleanInvalidateDCache_by_Addr((void *) addr, dsize);
#endif /* (__L2C_PRESENT == 1) */
}

#elif CPUSTYLE_F133

// C906 core specific cache operations


//	cache.c/iva means three instructions:
//	 - dcache.cva %0  : writeback     by virtual address cacheline
//	 - dcache.iva %0  : invalid       by virtual address cacheline
//	 - dcache.civa %0 : writeback+inv by virtual address cacheline

//static inline void local_flush_icache_all(void)
//{
//	asm volatile ("fence.i" ::: "memory");
//}


//      __ASM volatile(".4byte 0x0245000b\n":::"memory"); /* dcache.cva a0 */
//      __ASM volatile(".4byte 0x0285000b\n":::"memory"); /* dcache.cpa a0 */
//      __ASM volatile(".4byte 0x0265000b\n":::"memory"); /* dcache.iva a0 */
//      __ASM volatile(".4byte 0x02a5000b\n":::"memory"); /* dcache.ipa a0 */
//      __ASM volatile(".4byte 0x0275000b\n":::"memory"); /* dcache.civa a0 */
//      __ASM volatile(".4byte 0x02b5000b\n":::"memory"); /* dcache.cipa a0 */
//      __ASM volatile(".4byte 0x0010000b\n":::"memory"); /* dcache.call */
//

// Сейчас в эту память будем читать по DMA
void dcache_invalidate(uintptr_t base, int_fast32_t dsize)
{
	if (dsize > 0)
	{
		//base &= ~ (uintptr_t) (DCACHEROWSIZE - 1);
		for(; dsize > 0; dsize -= DCACHEROWSIZE, base += DCACHEROWSIZE)
		{
			__ASM volatile(
					"\t" "mv a0,%0\n"
					//"\t" ".4byte 0x0265000b\n" /* dcache.iva a0 */
					"\t" ".4byte 0x02a5000b\n" /* dcache.ipa a0 */
					:: "r"(base):"a0");
		}
		__ASM volatile(".4byte 0x01b0000b\n":::"memory");		/* sync.is */
	}
}

// Сейчас эта память будет записываться по DMA куда-то
void dcache_clean(uintptr_t base, int_fast32_t dsize)
{
	if (dsize > 0)
	{
		//base &= ~ (uintptr_t) (DCACHEROWSIZE - 1);
		for(; dsize > 0; dsize -= DCACHEROWSIZE, base += DCACHEROWSIZE)
		{
			__ASM volatile(
					"\t" "mv a0,%0\n"
					//"\t" ".4byte 0x0245000b\n" /* dcache.cva a0 */
					"\t" ".4byte 0x0285000b\n" /* dcache.cpa a0 */
					:: "r"(base):"a0");
		}
		__ASM volatile(".4byte 0x01b0000b\n":::"memory");		/* sync.is */
	}
}

// Сейчас эта память будет записываться по DMA куда-то. Потом содержимое не требуется
void dcache_clean_invalidate(uintptr_t base, int_fast32_t dsize)
{
	if (dsize > 0)
	{
		//base &= ~ (uintptr_t) (DCACHEROWSIZE - 1);
		for(; dsize > 0; dsize -= DCACHEROWSIZE, base += DCACHEROWSIZE)
		{
			__ASM volatile(
					"\t" "mv a0,%0\n"
					//"\t" ".4byte 0x0275000b\n" /* dcache.civa a0 */
					"\t" ".4byte 0x02b5000b\n" /* dcache.cipa a0 */
					:: "r"(base):"a0");
		}
		__ASM volatile(".4byte 0x01b0000b\n":::"memory");		/* sync.is */
	}
}

// Записать содержимое кэша данных в память
// применяется после начальной инициализации среды выполнния
void dcache_clean_all(void)
{
	__ASM volatile(".4byte 0x0010000b\n":::"memory"); /* dcache.call */
}


int_fast32_t dcache_rowsize(void)
{
	return DCACHEROWSIZE;
}


int_fast32_t icache_rowsize(void)
{
	return ICACHEROWSIZE;
}

#else

// Заглушки
// Сейчас в эту память будем читать по DMA
void dcache_invalidate(uintptr_t base, int_fast32_t dsize)
{
}

// Сейчас эта память будет записываться по DMA куда-то
void dcache_clean(uintptr_t base, int_fast32_t dsize)
{
}

// Записать содержимое кэша данных в память
// применяется после начальной инициализации среды выполнния
void dcache_clean_all(void)
{
}

// Сейчас эта память будет записываться по DMA куда-то. Потом содержимое не требуется
void dcache_clean_invalidate(uintptr_t base, int_fast32_t dsize)
{
}


int_fast32_t dcache_rowsize(void)
{
	return DCACHEROWSIZE;
}


int_fast32_t icache_rowsize(void)
{
	return ICACHEROWSIZE;
}

#endif /* CPUSTYLE_ARM_CM7 */

// получение частоты, с которой инкрементируется счетчик
uint_fast32_t cpu_getdebugticksfreq(void)
{
	return CPU_FREQ;
}

// получение из аппаратного счетчика монотонно увеличивающегося кода
// see SystemInit() in hardware.c
uint_fast32_t cpu_getdebugticks(void)
{
#if CPUSTYLE_ARM_CM3 || CPUSTYLE_ARM_CM4 || CPUSTYLE_ARM_CM7
	return DWT->CYCCNT;	// use TIMESTAMP_GET();
#elif ((__CORTEX_A != 0) || CPUSTYLE_ARM9) && (! defined(__aarch64__))
	{
		uint32_t result;
		// Read CCNT Register
		//	MRC p15, 0, <Rt>, c9, c13, 0 : Read PMCCNTR into Rt
		//	MCR p15, 0, <Rt>, c9, c13, 0 : Write Rt to PMCCNTR
		//asm volatile ("MRC p15, 0, %0, c9, c13, 0\t\n": "=r"(value));  
		__get_CP(15, 0, result, 9, 13, 0);
		return(result);
	}

#elif defined(__riscv)

	uint64_t v = csr_read_mcycle();
	return v;

#else
	//#warning Wromg CPUSTYLE_xxx - cpu_getdebugticks not work
	return 0;
#endif
}


#if (__CORTEX_A != 0) || CPUSTYLE_ARM9 || CPUSTYLE_RISCV


#if defined (__aarch64__)
// 13.3 Memory attributes

// Also see TCR_EL3 parameter
#define CACHEATTR_NOCACHE 0x00		// Non-cacheable
#define CACHEATTR_WB_WA_CACHE 0x01	// Write-Back Write-Allocate Cacheable
//#define CACHEATTR_WT_NWA_CACHE 0x02	// Write-Through Cacheable
//#define CACHEATTR_WB_NWA_CACHE 0x03	// Write-Back no Write-Allocate Cacheable

static const uint32_t aarch64_pageattr =
			0x01 * (UINT32_C(1) << 10) |	// AF
			0x00 * (UINT32_C(1) << 5) |		// NS
			0x01 * (UINT32_C(1) << 6) |		// AP[1:0]
			0x03 * (UINT32_C(1) << 8) |		// SH[1:0]
			0;

//	//PRINTF("aarch64_pageattr=%08X\n", (unsigned) aarch64_pageattr);
//	//ASSERT(0x00000740 == aarch64_pageattr);
//	//uintptr_t ttb_base1_addr = (uintptr_t) ttb_base1 & ~ UINT64_C(0x3FFFFFFF);
//	// 0x740 - BLOCK_1GB
//	// 0x74C - BLOCK_2MB

	enum aarch64_attrindex
	{
		AARCH64_ATTR_CACHED = 0,
		AARCH64_ATTR_NCACHED,
		AARCH64_ATTR_DEVICE

	};

	static const uint32_t pageAttrDEVICE =
			aarch64_pageattr |
			AARCH64_ATTR_DEVICE * (UINT32_C(1) << 2) |
			0
			;
	static const uint32_t pageAttrRAM =
			aarch64_pageattr |
			AARCH64_ATTR_CACHED * (UINT32_C(1) << 2) |
			0
			;
	static const uint32_t pageAttrNCRAM =
			aarch64_pageattr |
			AARCH64_ATTR_NCACHED * (UINT32_C(1) << 2) |
			0
			;
//	PRINTF("pageAttrNCRAM=%08X\n", pageAttrNCRAM);
//	PRINTF("pageAttrRAM=%08X\n", pageAttrRAM);
//	PRINTF("pageAttrDEVICE=%08X\n", pageAttrDEVICE);
//	pageAttrNCRAM=00000744
//	pageAttrRAM=00000740
//	pageAttrDEVICE=00000748

#undef TTB_PARA_NCACHED
#undef TTB_PARA_CACHED
#undef TTB_PARA_DEVICE
#undef TTB_PARA_NO_ACCESS

#define	TTB_PARA_NCACHED(ro, xn)	(aarch64_pageattr | pageAttrNCRAM | 0x01)
#define	TTB_PARA_CACHED(ro, xn) 	(aarch64_pageattr | pageAttrRAM | 0x01)
#define	TTB_PARA_DEVICE 			(aarch64_pageattr | pageAttrDEVICE | 0x01)
#define	TTB_PARA_NO_ACCESS 			0

#elif (__CORTEX_A != 0)

// Short-descriptor format memory region attributes, without TEX remap
// When using the Short-descriptor translation table formats, TEX remap is disabled when SCTLR.TRE is set to 0.

// For TRE - see
// B4.1.127 PRRR, Primary Region Remap Register, VMSA

#define APRWval 		0x03	/* Full access */
#define APROval 		0x06	/* All write accesses generate Permission faults */
#define DOMAINval		0x0F
#define SECTIONval		0x02	/* 0b10, Section or Supersection, PXN  */

/* Table B3-10 TEX, C, and B encodings when TRE == 0 */

/* Outer and Inner Write-Back, Write-Allocate */
// Cacheable memory attributes, without TEX remap
// DDI0406C_d_armv7ar_arm.pdf
// Table B3-11 Inner and Outer cache attribute encoding

#define MKATTR_TEXval(cacheattr) (0x04u | ((cacheattr) & 0x03u))
#define MKATTR_Cval(cacheattr) (!! ((cacheattr) & 0x02u))
#define MKATTR_Bval(cacheattr) (!! ((cacheattr) & 0x01u))

// Also see __set_TTBR0 parameter
#define CACHEATTR_NOCACHE 0x00		// Non-cacheable
#define CACHEATTR_WB_WA_CACHE 0x01	// Write-Back, Write-Allocate
//#define CACHEATTR_WT_NWA_CACHE 0x02	// Write-Through, no Write-Allocate
//#define CACHEATTR_WB_NWA_CACHE 0x03	// Write-Back, no Write-Allocate

/* атрибуты для разных областей памяти (при TEX[2]=1 способе задания) */
#define RAM_ATTRS CACHEATTR_WB_WA_CACHE
//#define RAM_ATTRS CACHEATTR_WB_NWA_CACHE
#define DEVICE_ATTRS CACHEATTR_NOCACHE
#define NCRAM_ATTRS CACHEATTR_NOCACHE

#define TEXval_RAM		MKATTR_TEXval(RAM_ATTRS)	// Define the Outer cache attribute
#define Cval_RAM		MKATTR_Cval(RAM_ATTRS)		// Define the Inner cache attribute
#define Bval_RAM		MKATTR_Bval(RAM_ATTRS)		// Define the Inner cache attribute

#if WITHSMPSYSTEM
	#define SHAREDval_RAM 1		// required for ldrex.. and strex.. functionality
#else /* WITHSMPSYSTEM */
	#define SHAREDval_RAM 0		// If non-zero, Renesas Cortex-A9 hung by buffers
#endif /* WITHSMPSYSTEM */

#define TEXval_NCRAM	MKATTR_TEXval(NCRAM_ATTRS)	// Define the Outer cache attribute
#define Cval_NCRAM		MKATTR_Cval(NCRAM_ATTRS)	// Define the Inner cache attribute
#define Bval_NCRAM		MKATTR_Bval(NCRAM_ATTRS)	// Define the Inner cache attribute

#if WITHSMPSYSTEM
	#define SHAREDval_NCRAM 1		// required for ldrex.. and strex.. functionality
#else /* WITHSMPSYSTEM */
	#define SHAREDval_NCRAM 0		// If non-zero, Renesas Cortex-A9 hung by buffers
#endif /* WITHSMPSYSTEM */

#if 1
	/* Shareable Device */
	#define TEXval_DEVICE       0x00
	#define Cval_DEVICE         0
	#define Bval_DEVICE         1
	#define SHAREDval_DEVICE 	0
#else
	/* Shareable Device */
	#define TEXval_DEVICE	MKATTR_TEXval(DEVICE_ATTRS)	// Define the Outer cache attribute
	#define Cval_DEVICE		MKATTR_Cval(DEVICE_ATTRS)	// Define the Inner cache attribute
	#define Bval_DEVICE		MKATTR_Bval(DEVICE_ATTRS)	// Define the Inner cache attribute

	#if WITHSMPSYSTEM
		#define SHAREDval_DEVICE 1		// required for ldrex.. and strex.. functionality
	#else /* WITHSMPSYSTEM */
		#define SHAREDval_DEVICE 0		// If non-zero, Renesas Cortex-A9 hung by buffers
	#endif /* WITHSMPSYSTEM */
#endif

// See B3.5.2 in DDI0406C_C_arm_architecture_reference_manual.pdf

#define	TTB_PARA(TEXv, Bv, Cv, DOMAINv, SHAREDv, APv, XNv) ( \
		(SECTIONval) * (UINT32_C(1) << 0) |	/* 0b10, Section or Supersection, PXN */ \
		!! (Bv) * (UINT32_C(1) << 2) |	/* B */ \
		!! (Cv) * (UINT32_C(1) << 3) |	/* C */ \
		!! (XNv) * (UINT32_C(1) << 4) |	/* XN The Execute-never bit. */ \
		(DOMAINv) * (UINT32_C(1) << 5) |	/* DOMAIN */ \
		0 * (UINT32_C(1) << 9) |	/* implementation defined */ \
		(((APv) >> 0) & 0x03) * (UINT32_C(1) << 10) |	/* AP [1..0] */ \
		((TEXv) & 0x07) * (UINT32_C(1) << 12) |	/* TEX */ \
		(((APv) >> 2) & 0x01) * (UINT32_C(1) << 15) |	/* AP[2] */ \
		!! (SHAREDv) * (UINT32_C(1) << 16) |	/* S */ \
		0 * (UINT32_C(1) << 17) |	/* nG */ \
		0 * (UINT32_C(1) << 18) |	/* 0 */ \
		0 * (UINT32_C(1) << 19) |	/* NS */ \
		0 \
	)

#define	TTB_PARA_NCACHED(ro, xn)	 TTB_PARA(TEXval_NCRAM, Bval_NCRAM, Cval_NCRAM, DOMAINval, SHAREDval_NCRAM, (ro) ? APROval : APRWval, (xn) != 0)
#define	TTB_PARA_CACHED(ro, xn) TTB_PARA(TEXval_RAM, Bval_RAM, Cval_RAM, DOMAINval, SHAREDval_RAM, (ro) ? APROval : APRWval, (xn) != 0)
#define	TTB_PARA_DEVICE 		TTB_PARA(TEXval_DEVICE, Bval_DEVICE, Cval_DEVICE, DOMAINval, SHAREDval_DEVICE, APRWval, 1 /* XN=1 */)
#define	TTB_PARA_NO_ACCESS 		0

#elif CPUSTYLE_RISCV

// See Table 4.2: Encoding of PTE Type field.

#define	TTB_PARA_CACHED(ro, xn) ((0x00u << 1) | 0x01)
#define	TTB_PARA_NCACHED(ro, xn) ((0x00u << 1) | 0x01)
#define	TTB_PARA_DEVICE 		((0x00u << 1) | 0x01)
#define	TTB_PARA_NO_ACCESS 		0

#endif /* __CORTEX_A */

#if defined(__aarch64__)

	/* TTB должна размещаться в памяти, не инициализируемой перед запуском системы */
	// Last x4 - for 34 bit address (16 GB address space)
	// Check TCR_EL3 setup
	// pages of 2 MB
	static RAMFRAMEBUFF __ALIGNED(4 * 1024) volatile uint64_t level2_pagetable [512 * 4 * 4];	// ttb0_base must be a 4KB-aligned address.
	static RAMFRAMEBUFF __ALIGNED(4 * 1024) volatile uint64_t ttb0_base [ARRAY_SIZE(level2_pagetable) / 512];	// ttb0_base must be a 4KB-aligned address.

#elif CPUSTYLE_RISCV

	static RAMFRAMEBUFF __ALIGNED(4 * 1024) volatile uint64_t level2_pagetable [512 * 4];	// Used as PPN in SATP register
	static RAMFRAMEBUFF __ALIGNED(4 * 1024) volatile uint64_t ttb0_base [512];	// Used as PPN in SATP register

	// https://lupyuen.codeberg.page/articles/mmu.html#appendix-flush-the-mmu-cache-for-t-head-c906
	// https://github.com/apache/nuttx/blob/4d63921f0a28aeee89b3a2ae861aaa83d731d28d/arch/risc-v/src/common/riscv_mmu.h#L220
	static inline void mmu_write_satp(uintptr_t reg)
	{
	  __ASM volatile
	    (
	      "csrw satp, %0\n"
	      "sfence.vma x0, x0\n"
	      "fence rw, rw\n"
	      //"fence.i\n"
	      :
	      : "rK" (reg)
	      : "memory"
	    );

	  /* Flush the MMU Cache if needed (T-Head C906) */

//	  if (mmu_flush_cache != NULL)
//	    {
//	      mmu_flush_cache(reg);
//	    }
	}


	// https://lupyuen.codeberg.page/articles/mmu.html#appendix-flush-the-mmu-cache-for-t-head-c906

	// Flush the MMU Cache for T-Head C906.  Called by mmu_write_satp() after
	// updating the MMU SATP Register, when swapping MMU Page Tables.
	// This operation executes RISC-V Instructions that are specific to
	// T-Head C906.
	void mmu_flush_cache(void) {
	  __ASM volatile (
	    // DCACHE.IALL: Invalidate all Page Table Entries in the D-Cache
	    ".4byte 0x0020000b\n"

	    // SYNC.S: Ensure that all Cache Operations are completed
	    ".4byte 0x0190000b\n"
	  );
	}

#else /* defined(__aarch64__) */

	/* TTB должна размещаться в памяти, не инициализируемой перед запуском системы */
	static RAMFRAMEBUFF __ALIGNED(16 * 1024) volatile uint32_t ttb0_base [4096];
#endif /* defined(__aarch64__) */

/*
 * https://community.st.com/s/question/0D73W000000UagD/what-a-type-of-mmu-memory-regions-recommended-for-regions-with-peripheralsstronglyordered-or-device?s1oid=00Db0000000YtG6&s1nid=0DB0X000000DYbd&emkind=chatterCommentNotification&s1uid=0050X000007vtUt&emtm=1599464922440&fromEmail=1&s1ext=0&t=1599470826880
 *
 *
 * PatrickF (ST Employee)

2 hours ago
Hello,

"Device" should be used for all peripherals to get best performance.
There is no rationale to use "Strongly-Ordered" with Cortex-A7
(explanation should be found by deep analysis/understanding of various ARM documents).
 *
 *
*/

static uintptr_t
ttb_1MB_accessbits(uintptr_t a, int ro, int xn)
{
	const uint32_t addrbase = a & ~ (uintptr_t) UINT32_C(0x0FFFFF);

#if CPUSTYLE_R7S721020

	// Все сравнения должны быть не точнее 1 MB

	if (a == 0x00000000)
		return addrbase | TTB_PARA_NO_ACCESS;		// NULL pointers access trap

	if (a >= 0x18000000 && a < 0x20000000)			// FIXME: QSPI memory mapped should be R/O, but...
		return addrbase | TTB_PARA_CACHED(ro || 0, 0);

	if (a >= 0x00000000 && a < 0x00A00000)			// up to 10 MB
		return addrbase | TTB_PARA_CACHED(ro, 0);
	if (a >= 0x20000000 && a < 0x20A00000)			// up to 10 MB
		return addrbase | TTB_PARA_CACHED(ro, 0);

	return addrbase | TTB_PARA_DEVICE;

#elif CPUSTYLE_STM32MP1

	extern uint32_t __RAMNC_BASE;
	extern uint32_t __RAMNC_TOP;
	const uintptr_t __ramnc_base = (uintptr_t) & __RAMNC_BASE;
	const uintptr_t __ramnc_top = (uintptr_t) & __RAMNC_TOP;
	if (a >= __ramnc_base && a < __ramnc_top)			// non-cached DRAM
		return addrbase | TTB_PARA_NCACHED(ro, 1 || xn);

	// Все сравнения должны быть не точнее 1 MB
	if (a >= 0x20000000 && a < 0x30000000)			// SYSRAM
		return addrbase | TTB_PARA_CACHED(ro, 0);
	// 1 GB DDR RAM memory size allowed
	if (a >= 0xC0000000)							// DDR memory
		return addrbase | TTB_PARA_CACHED(ro, 0);

	return addrbase | TTB_PARA_DEVICE;
	return addrbase | TTB_PARA_NO_ACCESS;

#elif CPUSTYLE_XC7Z

	// Все сравнения должны быть не точнее 1 MB
#if WITHLWIP
	if (a == (uintptr_t) bd_space)
		return addrbase | TTB_PARA_NCACHED(ro, 0);
#endif /* WITHLWIP */

	if (a >= 0x00000000 && a < 0x00100000)			//  OCM (On Chip Memory), DDR3_SCU
		return addrbase | TTB_PARA_CACHED(ro, 0);

	if (a >= 0x00100000 && a < 0x40000000)			//  DDR3 - 255 MB
		return addrbase | TTB_PARA_CACHED(ro, 0);

	if (a >= 0xE1000000 && a < 0xE6000000)			//  SMC (Static Memory Controller)
		return addrbase | TTB_PARA_CACHED(ro, 0);

	if (a >= 0x40000000 && a < 0xFC000000)	// PL, peripherials
		return addrbase | TTB_PARA_DEVICE;

	if (a >= 0xFC000000 && a < 0xFE000000)			//  Quad-SPI linear address for linear mode
		return addrbase | TTB_PARA_CACHED(ro || 0, 0);

	if (a >= 0xFFF00000)			// OCM (On Chip Memory) is mapped high
		return addrbase | TTB_PARA_CACHED(ro, 0);

	return addrbase | TTB_PARA_DEVICE;

#elif CPUSTYLE_T113

	// Все сравнения должны быть не точнее 1 MB

	extern uint32_t __RAMNC_BASE;
	extern uint32_t __RAMNC_TOP;
	const uintptr_t __ramnc_base = (uintptr_t) & __RAMNC_BASE;
	const uintptr_t __ramnc_top = (uintptr_t) & __RAMNC_TOP;
	if (a >= __ramnc_base && a < __ramnc_top)			// non-cached DRAM
		return addrbase | TTB_PARA_NCACHED(ro, 1 || xn);

	if (a < 0x00400000)
		return addrbase | TTB_PARA_CACHED(ro, 0);

	if (a >= 0x40000000)			//  DDR3 - 2 GB
		return addrbase | TTB_PARA_CACHED(ro, 0);
//	if (a >= 0x000020000 && a < 0x000038000)			//  SYSRAM - 64 kB
//		return addrbase | TTB_PARA_CACHED(ro, 0);

	return addrbase | TTB_PARA_DEVICE;

#elif CPUSTYLE_F133

	// Все сравнения должны быть не точнее 2 MB

	extern uint32_t __RAMNC_BASE;
	extern uint32_t __RAMNC_TOP;
	const uintptr_t __ramnc_base = (uintptr_t) & __RAMNC_BASE;
	const uintptr_t __ramnc_top = (uintptr_t) & __RAMNC_TOP;
	if (a >= __ramnc_base && a < __ramnc_top)			// non-cached DRAM
		return addrbase | TTB_PARA_NCACHED(ro, 1 || xn);

	if (a < 0x00400000)
		return addrbase | TTB_PARA_CACHED(ro, 0);

	if (a >= 0x40000000)			//  DDR3 - 2 GB
		return addrbase | TTB_PARA_CACHED(ro, 0);
//	if (a >= 0x000020000 && a < 0x000038000)			//  SYSRAM - 64 kB
//		return addrbase | TTB_PARA_CACHED(ro, 0);

	return addrbase | TTB_PARA_DEVICE;

#elif CPUSTYLE_V3S

	// Все сравнения должны быть не точнее 1 MB

	extern uint32_t __RAMNC_BASE;
	extern uint32_t __RAMNC_TOP;
	const uintptr_t __ramnc_base = (uintptr_t) & __RAMNC_BASE;
	const uintptr_t __ramnc_top = (uintptr_t) & __RAMNC_TOP;
	if (a >= __ramnc_base && a < __ramnc_top)			// non-cached DRAM
		return addrbase | TTB_PARA_NCACHED(ro, 1 || xn);

	if (a < 0x00400000)
		return addrbase | TTB_PARA_CACHED(ro, 0);

	if (a >= 0x40000000)			//  DDR3 - 2 GB
		return addrbase | TTB_PARA_CACHED(ro, 0);
//	if (a >= 0x000020000 && a < 0x000038000)			//  SYSRAM - 64 kB
//		return addrbase | TTB_PARA_CACHED(ro, 0);

	return addrbase | TTB_PARA_DEVICE;

#elif CPUSTYLE_H3

	// Все сравнения должны быть не точнее 1 MB

	extern uint32_t __RAMNC_BASE;
	extern uint32_t __RAMNC_TOP;
	const uintptr_t __ramnc_base = (uintptr_t) & __RAMNC_BASE;
	const uintptr_t __ramnc_top = (uintptr_t) & __RAMNC_TOP;
	if (a >= __ramnc_base && a < __ramnc_top)			// non-cached DRAM
		return addrbase | TTB_PARA_NCACHED(ro, 1 || xn);

	if (a < 0x01000000)
		return addrbase | TTB_PARA_CACHED(ro, 0);	// SRAM A1, SRAM A2, SRAM C

	if (a >= 0xC0000000)
		return addrbase | TTB_PARA_CACHED(ro, 0);	// N-BROM, S-BROM

	if (a >= 0x40000000)			//  DDR3 - 2 GB
		return addrbase | TTB_PARA_CACHED(ro, 0);

	return addrbase | TTB_PARA_DEVICE;

#elif CPUSTYLE_A64

	// Все сравнения должны быть не точнее 1 MB

	extern uint32_t __RAMNC_BASE;
	extern uint32_t __RAMNC_TOP;
	const uintptr_t __ramnc_base = (uintptr_t) & __RAMNC_BASE;
	const uintptr_t __ramnc_top = (uintptr_t) & __RAMNC_TOP;
	if (a >= __ramnc_base && a < __ramnc_top)			// non-cached DRAM
		return addrbase | TTB_PARA_NCACHED(ro, 1 || xn);

	if (a < 0x01000000)
		return addrbase | TTB_PARA_CACHED(ro, 0);

	if (a >= 0x40000000)			//  DDR3 - 2 GB
		return addrbase | TTB_PARA_CACHED(ro, 0);

	return addrbase | TTB_PARA_DEVICE;

#elif CPUSTYLE_T507 || CPUSTYLE_H616

	// Все сравнения должны быть не точнее 1 MB

	extern uint32_t __RAMNC_BASE;
	extern uint32_t __RAMNC_TOP;
	const uintptr_t __ramnc_base = (uintptr_t) & __RAMNC_BASE;
	const uintptr_t __ramnc_top = (uintptr_t) & __RAMNC_TOP;
	if (a >= __ramnc_base && a < __ramnc_top)			// non-cached DRAM
		return addrbase | TTB_PARA_NCACHED(ro, 1 || xn);

	if (a < 0x01000000)			// BROM, SYSRAM A1, SRAM C
		return addrbase | TTB_PARA_CACHED(ro, 0);
	// 1 GB DDR RAM memory size allowed
	if (a >= 0x40000000)			//  DRAM - 2 GB
		return addrbase | TTB_PARA_CACHED(ro, 0);

	return addrbase | TTB_PARA_DEVICE;

#elif CPUSTYLE_A133 || CPUSTYLE_R818

	// Все сравнения должны быть не точнее 1 MB

	extern uint32_t __RAMNC_BASE;
	extern uint32_t __RAMNC_TOP;
	const uintptr_t __ramnc_base = (uintptr_t) & __RAMNC_BASE;
	const uintptr_t __ramnc_top = (uintptr_t) & __RAMNC_TOP;
	if (a >= __ramnc_base && a < __ramnc_top)			// non-cached DRAM
		return addrbase | TTB_PARA_NCACHED(ro, 1 || xn);

	if (a < 0x01000000)			// BROM, SYSRAM A1, SRAM C
		return addrbase | TTB_PARA_CACHED(ro, 0);
	// 1 GB DDR RAM memory size allowed
	if (a >= 0x40000000)			//  DRAM - 2 GB
		return addrbase | TTB_PARA_CACHED(ro, 0);

	return addrbase | TTB_PARA_DEVICE;

#elif CPUSTYLE_VM14

	// 1892ВМ14Я ELVEES multicore.ru
	// Все сравнения должны быть не точнее 1 MB

	if (a >= 0x20000000 && a < 0x20100000)			//  SRAM - 64K
		return addrbase | TTB_PARA_CACHED(ro, 0);

	if (a >= 0x40000000 && a < 0xC0000000)			//  DDR - 2 GB
		return addrbase | TTB_PARA_CACHED(ro, 0);

	return addrbase | TTB_PARA_DEVICE;

#else

	// Все сравнения должны быть не точнее 1 MB

	#warning ttb_1MB_accessbits: Unhandled CPUSTYLE_xxxx

	return addrbase | TTB_PARA_DEVICE;

#endif
}

#if defined (__aarch64__)

static void
ttb_level2_2MB_initialize(uintptr_t (* accessbits)(uintptr_t a, int ro, int xn), uintptr_t textstart, uint_fast32_t textsize)
{
	unsigned i;
	const uint_fast32_t pagesize = (UINT32_C(1) << 21);

	for (i = 0; i < ARRAY_SIZE(level2_pagetable); ++ i)
	{
		const uintptr_t address = (uintptr_t) i << 21;
		level2_pagetable [i] =  accessbits(address, 0, 0);
	}
	/* Установить R/O атрибуты для указанной области */
	while (textsize >= pagesize)
	{
		level2_pagetable [textstart / pagesize] =  accessbits(textstart, 0 * 1, 0);
		textsize -= pagesize;
		textstart += pagesize;
	}
}
#endif

static void
ttb_level0_1MB_initialize(uintptr_t (* accessbits)(uintptr_t a, int ro, int xn), uintptr_t textstart, uint_fast32_t textsize)
{
	unsigned i;
	const uint_fast32_t pagesize = (1uL << 20);

	for (i = 0; i <  ARRAY_SIZE(ttb0_base); ++ i)
	{
		const uintptr_t address = (uintptr_t) i << 20;
		ttb0_base [i] =  accessbits(address, 0, 0);
	}
	/* Установить R/O атрибуты для указанной области */
	while (textsize >= pagesize)
	{
		ttb0_base [textstart / pagesize] =  accessbits(textstart, 0 * 1, 0);
		textsize -= pagesize;
		textstart += pagesize;
	}
}

static void
sysinit_mmu_tables(void)
{
	//PRINTF("sysinit_mmu_tables\n");

#if (__CORTEX_A != 0) || CPUSTYLE_ARM9
	// MMU iniitialize

#if 1 && (__CORTEX_A == 9U) && WITHSMPSYSTEM && defined (SCU_CONTROL_BASE)
	{
		// SCU inut
		// SCU Control Register
		((volatile uint32_t *) SCU_CONTROL_BASE) [0] &= ~ 0x01;
//
//
//		// Filtering Start Address Register
//		((volatile uint32_t *) SCU_CONTROL_BASE) [0x10] = (((volatile uint32_t *) SCU_CONTROL_BASE) [0x10] & ~ (0xFFFuL << 20)) |
//				(0x001uL << 20) |
//				0;
//		TP();
//		// Filtering End Address Register
//		((volatile uint32_t *) SCU_CONTROL_BASE) [0x11] = (((volatile uint32_t *) SCU_CONTROL_BASE) [0x11] & ~ (0xFFFuL << 20)) |
//				(0xFFEuL << 20) |
//				0;

		((volatile uint32_t *) SCU_CONTROL_BASE) [0x3] = 0;		// SCU Invalidate All Registers in Secure State
		((volatile uint32_t *) SCU_CONTROL_BASE) [0] |= 0x01;	// SCU Control Register
	}
#endif /* 1 && (__CORTEX_A == 9U) && WITHSMPSYSTEM && defined (SCU_CONTROL_BASE) */


#if defined (__aarch64__)
	// MMU iniitialize

	unsigned i;
	for (i = 0; i < ARRAY_SIZE(ttb0_base); ++ i)
	{
		ttb0_base [i] = (uintptr_t) (level2_pagetable + 512 * i) | 0x03;
	}

	ttb_level2_2MB_initialize(ttb_1MB_accessbits, 0, 0);

#elif WITHISBOOTLOADER || CPUSTYLE_R7S721

	// MMU iniitialize
	ttb_level0_1MB_initialize(ttb_1MB_accessbits, 0, 0);

#elif CPUSTYLE_STM32MP1
	extern uint32_t __data_start__;
	// MMU iniitialize
	ttb_level0_1MB_initialize(ttb_1MB_accessbits, 0xC0000000, (uintptr_t) & __data_start__ - 0xC0000000);

#else
	// MMU iniitialize
	ttb_level0_1MB_initialize(ttb_1MB_accessbits, 0, 0);

#endif


#elif CPUSTYLE_RISCV
	#warning To be implemented
	// RISC-V MMU initialize


	//ttb_level0_1MB_initialize(ttb_1MB_accessbits, 0, 0);

	// XuanTie-Openc906 SYSMAP

	// The C906 is fully compatible with the RV64GC instruction set and supports the standard M/S/U privilege program model.
	// The C906 includes a standard 8-16 region PMP and Sv39 MMU, which is fully compatible with RISC-V Linux.
	// The C906 includes standard CLINT and PLIC interrupt controllers, RV compatible HPM.
	// ? 0xEFFFF000
	// See https://github.com/sophgo/cvi_alios_open/blob/aca2daa48266cd96b142f83bad4e33a6f13d6a24/components/csi/csi2/include/core/core_rv64.h
	// Strong Order, Cacheable, Bufferable, Shareable, Security

	// Bit 63 - Strong order
	// Bit 62 - Cacheable
	// Bit 61 - Buffer
	// Bit 0 - Valid
	#define RAM_ATTRS 		((UINT64_C(0) << 63) | (UINT64_C(1) << 62) | (UINT64_C(1) << 61) | (UINT64_C(0x0E) << 0) | 1)	// Cacheable memory
	#define NCRAM_ATTRS 	((UINT64_C(0) << 63) | (UINT64_C(0) << 62) | (UINT64_C(0) << 61) | (UINT64_C(0x0E) << 0) | 1)	// Non-cacheable memory
	#define DEVICE_ATTRS 	((UINT64_C(1) << 63) | (UINT64_C(0) << 62) | (UINT64_C(0) << 61) | (UINT64_C(0x0E) << 0) | 1)	// Non-bufferable device
	#define TABLE_ATTRS		((UINT64_C(0) << 63) | 1) // Pointer to next level of page table

	// When the page table size is set to 4 KB, 2 MB, or 1 GB, the page table is indexed by 3, 2, or 1 times, respectively.
	uintptr_t address = 0;
	uintptr_t addrstep = UINT64_C(1) << 21;	// 2 MB
	unsigned i;
	for (i = 0; i < ARRAY_SIZE(level2_pagetable); ++ i)
	{
		level2_pagetable [i] =
				//((address >> 12) & 0x1FF) * (UINT64_C(1) << 10) |	// 9 bits PPN [0], 4 KB granulation
				((address >> 21) & 0x1FF) * (UINT64_C(1) << 19) |	// 9 bits PPN [1]
				//((address >> 36) & 0x7FF) * (UINT64_C(1) << 28) |	// 11 bits PPN [2]
				RAM_ATTRS |
				0;
		address += addrstep;
	}
	// Pointe to 1 GB pages
	for (i = 0; i < ARRAY_SIZE(ttb0_base); ++ i)
	{
		//uintptr_t address = (uintptr_t) (level2_pagetable + 512 * i) | 0x03;
		uintptr_t address = 0 * (UINT64_C(1) << 30) * i;
		ttb0_base [i] =
			((address >> 12) & 0x1FF) * (UINT64_C(1) << 10) |	// 9 bits PPN [0], 4 KB granulation
			//((address >> 24) & 0x1FF) * (UINT64_C(1) << 19) |	// 9 bits PPN [1]
			//((address >> 36) & 0x7FF) * (UINT64_C(1) << 28) |	// 11 bits PPN [2]
			RAM_ATTRS |
			0;
	}

	//ttb_level2_2MB_initialize(ttb_1MB_accessbits, 0, 0);
#if 0


	#define FULLADFSZ 32	// Not __riscv_xlen
	if (0)
	{
		const unsigned SYSMAP_ASH = 12;	// 40-28

		extern uint32_t __RAMNC_BASE;
		extern uint32_t __RAMNC_TOP;
		const uintptr_t __ramnc_base = (uintptr_t) & __RAMNC_BASE;
		const uintptr_t __ramnc_top = (uintptr_t) & __RAMNC_TOP;

		// See SYSMAP_BASE_ADDR, SYSMAP_FLAG

		// The smallest address of address space 0 is 0x0
		SYSMAP->PARAM [0].ADDR = (0x40000000 >> SYSMAP_ASH);	// The largest address (noninclusive) of address space
		SYSMAP->PARAM [0].ATTR = DEVICE_ATTRS;

		SYSMAP->PARAM [1].ADDR = (__ramnc_base >> SYSMAP_ASH);	// The largest address (noninclusive) of address space
		SYSMAP->PARAM [1].ATTR = RAM_ATTRS;

		SYSMAP->PARAM [2].ADDR = (__ramnc_top >> SYSMAP_ASH);	// The largest address (noninclusive) of address space
		SYSMAP->PARAM [2].ATTR = NCRAM_ATTRS;

		SYSMAP->PARAM [3].ADDR = (0xC0000000 >> SYSMAP_ASH);	// The largest address (noninclusive) of address space
		SYSMAP->PARAM [3].ATTR = RAM_ATTRS;

		// DRAM space ends at 0xC0000000
		SYSMAP->PARAM [4].ADDR = (0xC1000000 >> SYSMAP_ASH);	// The largest address (noninclusive) of address space
		SYSMAP->PARAM [4].ATTR = DEVICE_ATTRS;

		SYSMAP->PARAM [5].ADDR = (0xC2000000 >> SYSMAP_ASH);	// The largest address (noninclusive) of address space
		SYSMAP->PARAM [5].ATTR = DEVICE_ATTRS;

		SYSMAP->PARAM [6].ADDR = (0xC3000000 >> SYSMAP_ASH);	// The largest address (noninclusive) of address space
		SYSMAP->PARAM [6].ATTR = DEVICE_ATTRS;

		SYSMAP->PARAM [7].ADDR = (0xFFFFFFFFFF >> SYSMAP_ASH);	// The largest address (noninclusive) of address space
		SYSMAP->PARAM [7].ATTR = DEVICE_ATTRS;

		{
			unsigned i;
			for (i = 0; i < ARRAY_SIZE(SYSMAP->PARAM); ++ i)
			{
				const uint_fast32_t attr = SYSMAP->PARAM [i].ATTR;
				PRINTF("2 SYSMAP zone%u: base=%010lX SO=%u, C=%u. B=%u\n",
						i,
						(unsigned long) (((uintptr_t) SYSMAP->PARAM [i].ADDR) << SYSMAP_ASH),
						(attr >> 4) & 0x01,
						(attr >> 3) & 0x01,
						(attr >> 2) & 0x01
						);
			}
		}
	}

#endif

#endif

	//PRINTF("sysinit_mmu_tables done.\n");
}

/* Загрузка TTBR, инвалидация кеш памяти и включение MMU */
static void
sysinit_ttbr_initialize(void)
{
#if defined(__aarch64__)

	ASSERT(((uintptr_t) ttb0_base & 0x0FFF) == 0); // 4 KB

	//__set_TTBR0_EL1((uintptr_t) ttb0_base);
	//__set_TTBR0_EL2((uintptr_t) ttb0_base);
	__set_TTBR0_EL3((uintptr_t) ttb0_base);

	// DDI0500J_cortex_a53_r0p4_trm.pdf
	// 4.3.53 Translation Control Register, EL3
	const uint_fast32_t IRGN_attr = CACHEATTR_WB_WA_CACHE;	// Normal memory, Inner Write-Back Write-Allocate Cacheable.
	const uint_fast32_t RGN_attr = CACHEATTR_WB_WA_CACHE;	// Normal memory, Outer Write-Back Write-Allocate Cacheable.
	const unsigned aspacebits = 21 + __log2_up(ARRAY_SIZE(level2_pagetable));	// pages of 2 MB
	uint_fast32_t tcrv =
			0x00 * (UINT32_C(1) << 14) | 	// TG0 TTBR0_EL3 granule size 0b00 4 KB
			0x03 * (UINT32_C(1) << 12) |	// 0x03 - Inner shareable
			RGN_attr * (UINT32_C(1) << 10) |	// Outer cacheability attribute
			IRGN_attr * (UINT32_C(1) << 8) |	// Inner cacheability attribute
			(64 - aspacebits) * (UINT32_C(1) << 0) |		// n=0..63. T0SZ=2^(64-n): n=28: 64GB, n=30: 16GB, n=32: 4GB, n=43: 2MB
			0;
	__set_TCR_EL3(tcrv);

	const uint_fast32_t mairv =
			0xFF * (UINT32_C(1) << (AARCH64_ATTR_CACHED * 8)) |		// Normal Memory, Inner/Outer Write-back non-transient
			0x44 * (UINT32_C(1) << (AARCH64_ATTR_NCACHED * 8)) |	// Normal memory, Inner/Outer Non-Cacheable
			0x00 * (UINT32_C(1) << (AARCH64_ATTR_DEVICE * 8)) | 	// Device-nGnRnE memory
			0;
	// Program the domain access register
	//__set_DACR32_EL2(0xFFFFFFFF); 	// domain 15: access are not checked
	__set_MAIR_EL3(mairv);

	__ISB();

	MMU_InvalidateTLB();

	// Обеспечиваем нормальную обработку RESEТ
	L1C_InvalidateDCacheAll();
	L1C_InvalidateICacheAll();
	L1C_InvalidateBTAC();
#if (__L2C_PRESENT == 1)
	L2C_InvAllByWay();
#endif

#if 0
	PRINTF("aspacebits=%u\n", aspacebits);
	//PRINTF("log2=%u\n", __log2_up(ARRAY_SIZE(level2_pagetable)));
	PRINTF("__get_TCR_EL3=%016" PRIX32 "\n", __get_TCR_EL3());
	PRINTF("__get_MAIR_EL3=%016" PRIX64 "\n", __get_MAIR_EL3());
	uint64_t mair = __get_MAIR_EL3();
	PRINTF("a7=%02X a6=%02X a5=%02X a4=%02X a3=%02X a2=%02X a1=%02X a0=%02X\n",
			(unsigned) (mair >> 56) & 0xFF,
			(unsigned) (mair >> 48) & 0xFF,
			(unsigned) (mair >> 40) & 0xFF,
			(unsigned) (mair >> 32) & 0xFF,
			(unsigned) (mair >> 24) & 0xFF,
			(unsigned) (mair >> 16) & 0xFF,
			(unsigned) (mair >> 8) & 0xFF,
			(unsigned) (mair >> 0) & 0xFF
			);
#endif
	MMU_Enable();

#elif (__CORTEX_A != 0)

	ASSERT(((uintptr_t) ttb0_base & 0x3FFF) == 0);

	//CP15_writeTTBCR(0);
	   /* Set location of level 1 page table
	    ; 31:14 - Translation table base addr (31:14-TTBCR.N, TTBCR.N is 0 out of reset)
	    ; 13:7  - 0x0
	    ; 6     - IRGN[0] 0x1  (Inner WB WA)
	    ; 5     - NOS     0x0  (Non-shared)
	    ; 4:3   - RGN     0x01 (Outer WB WA)
	    ; 2     - IMP     0x0  (Implementation Defined)
	    ; 1     - S       0x0  (Non-shared)
	    ; 0     - IRGN[1] 0x0  (Inner WB WA) */

	// B4.1.154 TTBR0, Translation Table Base Register 0, VMSA
#if WITHSMPSYSTEM
	// TTBR0
	const uint_fast32_t IRGN_attr = CACHEATTR_WB_WA_CACHE;	// Normal memory, Inner Write-Back Write-Allocate Cacheable.
	const uint_fast32_t RGN_attr = CACHEATTR_WB_WA_CACHE;	// Normal memory, Outer Write-Back Write-Allocate Cacheable.
	__set_TTBR0(
			(uintptr_t) ttb0_base |
			((uint_fast32_t) !! (IRGN_attr & 0x01) << 6) |	// IRGN[0]
			((uint_fast32_t) !! (IRGN_attr & 0x02) << 0) |	// IRGN[1]
			(RGN_attr << 3) |	// RGN
			!1*(UINT32_C(1) << 5) |	// NOS - Not Outer Shareable bit - TEST for RAMNC
			1*(UINT32_C(1) << 1) |	// S - Shareable bit. Indicates the Shareable attribute for the memory associated with the translation table
			0);
#else /* WITHSMPSYSTEM */
	// TTBR0
	__set_TTBR0(
			(uintptr_t) ttb0_base |
			//(!! (IRGN_attr & 0x02) << 6) | (!! (IRGN_attr & 0x01) << 0) |
			(UINT32_C(1) << 3) |	// RGN
			0*(UINT32_C(1) << 5) |	// NOS
			0*(UINT32_C(1) << 1) |	// S
			0);
#endif /* WITHSMPSYSTEM */
	//CP15_writeTTB1((unsigned int) ttb0_base | 0x48);	// TTBR1
	  __ISB();

	// Program the domain access register
	__set_DACR(0xFFFFFFFF); // domain 15: access are not checked

	MMU_InvalidateTLB();

	// Обеспечиваем нормальную обработку RESEТ
	L1C_InvalidateDCacheAll();
	L1C_InvalidateICacheAll();
	L1C_InvalidateBTAC();
#if (__L2C_PRESENT == 1)
	L2C_InvAllByWay();
#endif

	MMU_Enable();

#elif CPUSTYLE_RISCV

	// See https://github.com/sophgo/cvi_alios_open/blob/aca2daa48266cd96b142f83bad4e33a6f13d6a24/components/csi/csi2/include/core/core_rv64.h
	// Strong Order, Cacheable, Bufferable, Shareable, Security

	#define CSR_SATP_MODE_PHYS   0
	#define CSR_SATP_MODE_SV32   1
	#define CSR_SATP_MODE_SV39   8
	#define CSR_SATP_MODE_SV48   9
	#define CSR_SATP_MODE_SV57   10

	ASSERT(((uintptr_t) ttb0_base & 0x0FFF) == 0);
	mmu_flush_cache();

	// 5.2.1.1 MMU address translation register (SATP)
	// When Mode is 0, the MMU is disabled. C906 supports only the MMU disabled and Sv39 modes
	const uint_fast64_t satp =
			CSR_SATP_MODE_PHYS * (UINT64_C(1) << 60) | // MODE
			//CSR_SATP_MODE_SV39 * (UINT64_C(1) << 60) | // MODE
			0x00 * (UINT64_C(1) << 44) | // ASID
			(((uintptr_t) ttb0_base >> 12) & UINT64_C(0x0FFFFFFF)) * (UINT64_C(1) << 0) |	// PPN - 28 bit
			0;
	csr_write_satp(satp);
	PRINTF("csr_read_satp()=%016" PRIX64 "\n", csr_read_satp());

//	mmu_write_satp(satp);
//	mmu_flush_cache();
//	PRINTF("csr_read_satp()=%016" PRIX64 "\n", csr_read_satp());

	// MAEE in MXSTATUS
	//
	//
	//	/*
	//	(15) When MM is 1, unaligned access is supported, and the hardware handles unaligned access
	//	(16) When UCME is 1, user mode can execute extended cache operation instructions
	//	(17) When CLINTEE is 1, CLINT-initiated superuser software interrupts and timer interrupts can be responded to
	//	(21) When the MAEE is 1, the address attribute bit is extended in the PTE of the MMU, and the user can configure the address attribute of the page
	//	(22) When the THEADISAE is 1, the C906 extended instruction set can be used
	//	*/
	//	csr_set(CSR_MXSTATUS, 0x638000);
//	csr_set_bits_mxstatus(
//			1 * (UINT32_C(1) << 21) |
//			0
//			);

//	//#warning Implement for RISC-C
//	// 4.1.11 Supervisor Page-Table Base Register (sptbr)
//	csr_write_sptbr((uintptr_t) ttb0_base >> 10);

	// https://people.eecs.berkeley.edu/~krste/papers/riscv-priv-spec-1.7.pdf
	// 3.1.6 Virtualization Management Field in mstatus Register
	// Table 3.3: Encoding of virtualization management field VM[4:0]

//	{
//		uint_xlen_t v = csr_read_mstatus();
//		v &= ~ ((uint_xlen_t) 0x1F) << 24;	// VM[4:0]
//		v |= ((uint_xlen_t) 0x08) << 24;	// Set Page-based 32-bit virtual addressing.
//		//csr_write_mstatus(v);
//	}


	// 15.1.2 M-mode exception configuration register group
	// https://riscv.org/wp-content/uploads/2019/08/riscv-privileged-20190608-1.pdf


#endif
}

#elif defined (__CORTEX_M)

static void
sysinit_ttbr_initialize(void)
{
}

#endif /* CPUSTYLE_R7S721 */

// PLL and caches iniitialize
static void
sysinit_fpu_initialize(void)
{
#if CPUSTYLE_ARM_CM3 || CPUSTYLE_ARM_CM4 || CPUSTYLE_ARM_CM7
	#if (__FPU_PRESENT == 1) && (__FPU_USED == 1)

		/* FPU enable on Cortex M4F */
		SCB->CPACR |= ((3UL << 10 * 2) | (3UL << 11 * 2));  /* set CP10 and CP11 Full Access */

		#if 0
			/* Lazy stacking enabled, automatic state saving enabled is a default state */
			/* http://infocenter.arm.com/help/topic/com.arm.doc.dai0298a/DAI0298A_cortex_m4f_lazy_stacking_and_context_switching.pdf */
			__set_FPSCR(			/* Floating-Point Context Control Register */
				(__get_FPSCR() & ~ (FPU_FPCCR_LSPEN_Msk)) | /* disable Lazy stacking feature */
				FPU_FPCCR_ASPEN_Msk |
				0);
		#endif

	#endif
	#ifdef UNALIGNED_SUPPORT_DISABLE
		SCB->CCR |= SCB_CCR_UNALIGN_TRP_Msk;
	#endif

#elif defined(__aarch64__) && ! LINUX_SUBSYSTEM

	// FPU
	//__FPU_Enable_fixed();
	__set_SCTLR_EL3(__get_SCTLR_EL3() & ~ SCTLR_EL3_SA_Msk & ~ SCTLR_EL3_A_Msk);	// Disable stack alignment check. The possible values are
	__set_SCTLR_EL3(__get_SCTLR_EL3() | SCTLR_EL3_NAA_Msk);	// Unaligned accesses by the specified instructions do not generate an Alignment fault.

	__set_SCTLR_EL2(__get_SCTLR_EL2() | SCTLR_EL3_NAA_Msk);	// Unaligned accesses by the specified instructions do not generate an Alignment fault.
	__set_SCTLR_EL2(__get_SCTLR_EL2() & ~ SCTLR_EL3_A_Msk);	// Disable stack alignment check. The possible values are
	//__builtin_aarch64_set_fpcr( __builtin_aarch64_get_fpcr() & 0x00086060u);

	__set_CPACR_EL1(__get_CPACR_EL1() | 0x03 * (UINT64_C(1) << 20));	// FPEN 0x03 - 0b11 No instructions are trapped.
	__set_SCTLR_EL1(__get_SCTLR_EL1() | 0x01 * (UINT64_C(1) << 14));	// DZE - Enables access to the DC ZVA instruction at EL0. The possible values ar
	__set_SCTLR_EL1(__get_SCTLR_EL1() | SCTLR_EL3_NAA_Msk);	// Unaligned accesses by the specified instructions do not generate an Alignment fault.
	__set_SCTLR_EL1(__get_SCTLR_EL1() & ~ SCTLR_EL3_A_Msk);	// Disable stack alignment check. The possible values are

	L1C_DisableCaches();
	L1C_DisableBTAC();
	MMU_Disable();

#elif (__CORTEX_A != 0)

	// FPU
	__FPU_Enable_fixed();
	L1C_DisableCaches();
	L1C_DisableBTAC();
	MMU_Disable();

#elif CPUSTYLE_RISCV

	// 15.1.7.1 M-mode extension status register (MXSTATUS)
	//	(22) When the THEADISAE is 1, the C906 extended instruction set can be used
	//	(18) When MHRD is 1, hardware writeback is not performed after the TLB is missing.
	csr_set_bits_mxstatus(
			1 * (UINT32_C(1) << 22) |
			//1 * (UINT32_C(1) << 18) |
			0
			);

	/* disable interrupts*/
	csr_clr_bits_mie(MIE_MSI_BIT_MASK | MIE_MTI_BIT_MASK | MIE_MEI_BIT_MASK);	// MSI MTI MEI
	csr_clr_bits_mstatus(MSTATUS_MIE_BIT_MASK); // Disable interrupts routing
	// See:
	// https://people.eecs.berkeley.edu/~krste/papers/riscv-priv-spec-1.7.pdf
	// 3.1.8 Extension Context Status in mstatus Register

	csr_set_bits_mstatus(0x00006000);	/* MSTATUS_FS = 0x00006000 = Dirty */
 	csr_write_fcsr(0);             		/* initialize rounding mode, undefined at reset */
	//__FPU_Enable();


#endif /*  */

#if ! WITHISBOOTLOADER_DDR
#if defined(__GIC_PRESENT) && (__GIC_PRESENT == 1U)

	GIC_Enable();

#endif
	InitializeIrql(IRQL_USER);	// nested interrupts support
#endif
}

static void
sysinit_sdram_initialize(void)
{
#if WITHSDRAMHW && WITHISBOOTLOADER
	arm_hardware_sdram_initialize();
#endif /* WITHSDRAMHW && WITHISBOOTLOADER */
#if CPUSTYLE_T113 && ! WITHISBOOTLOADER
	// На 0x00028000:
	// При 0 видим память DSP
	// При 1 видим память что была при загрузке
	// 0: DSP 128K Local SRAM Remap for DSP_SYS
	// 1: DSP 128K Local SRAM Remap for System Boot
	// After system boots up, this bit must be set to 0 before using DSP
	//PRINTF("SYS_CFG->DSP_BOOT_RAMMAP_REG=%08" PRIX32 "\n", SYS_CFG->DSP_BOOT_RAMMAP_REG);
	SYS_CFG->DSP_BOOT_RAMMAP_REG = 0;
	//PRINTF("SYS_CFG->DSP_BOOT_RAMMAP_REG=%08" PRIX32 "\n", SYS_CFG->DSP_BOOT_RAMMAP_REG);
#endif /* CPUSTYLE_T113 && ! WITHISBOOTLOADER */
#if CPUSTYLE_F133
	/* Все 160 килобайт на 0x00020000 доступны процссору */
	SYS_CFG->DSP_BOOT_RAMMAP_REG = 1;
#endif /* CPUSTYLE_F133 */
}

static void
sysinit_debug_initialize(void)
{
#if WITHDEBUG && ! LINUX_SUBSYSTEM
	HARDWARE_DEBUG_INITIALIZE();
	HARDWARE_DEBUG_SET_SPEED(DEBUGSPEED);
#endif /* WITHDEBUG */
#if CPUSTYLE_STM32MP1

	//RCC->DBGCFGR |= RCC_DBGCFGR_TRACECKEN_Msk;
	RCC->DBGCFGR |= RCC_DBGCFGR_DBGCKEN_Msk;

	RCC->DBGCFGR = (RCC->DBGCFGR & ~ (RCC_DBGCFGR_TRACEDIV_Msk)) |
			RCC_DBGCFGR_TRACEDIV_2;

	RCC->DBGCFGR |= RCC_DBGCFGR_DBGRST_Msk;
	RCC->DBGCFGR &= ~ RCC_DBGCFGR_DBGRST_Msk;

	//printhex32(DBGMCU_BASE, DBGMCU, sizeof * DBGMCU);
	DBGMCU->CR = 0;
	DBGMCU->APB4FZ1 = 0;	/*!< Debug MCU APB4FZ1 freeze register CPU1 */
	DBGMCU->APB4FZ2 = 0;	/*!< Debug MCU APB4FZ2 freeze register CPU2 */
	DBGMCU->APB1FZ1 = 0;	/*!< Debug MCU APB1FZ1 freeze register CPU1 */
	DBGMCU->APB1FZ2 = 0;	/*!< Debug MCU APB1FZ2 freeze register CPU2 */
	DBGMCU->APB2FZ1 = 0;	/*!< Debug MCU APB2FZ1 freeze register CPU1 */
	DBGMCU->APB2FZ2 = 0;	/*!< Debug MCU APB2FZ2 freeze register CPU2 */
	DBGMCU->APB3FZ1 = 0;	/*!< Debug MCU APB3FZ1 freeze register CPU1 */
	DBGMCU->APB3FZ2 = 0;	/*!< Debug MCU APB3FZ2 freeze register CPU2 */
	DBGMCU->APB5FZ1 = 0;	/*!< Debug MCU APB5FZ1 freeze register CPU1 */
	DBGMCU->APB5FZ2 = 0;	/*!< Debug MCU APB5FZ2 freeze register CPU2 */

	/*
	 * Sleep bit only affects the Cortex®-M4, the Cortex®-A7 debug clock continues to run even when the core clocks are stopped (PxSTOP mode).
	 */

	DBGMCU->CR =
		DBGMCU_CR_DBG_SLEEP_Msk |
		DBGMCU_CR_DBG_STOP_Msk |
		DBGMCU_CR_DBG_STANDBY_Msk |
		0;


	//printhex32(DBGMCU_BASE, DBGMCU, sizeof * DBGMCU);

	//RCC->DBGCFGR |= RCC_DBGCFGR_DBGRST_Msk;

#endif /* CPUSTYLE_STM32MP1 */
}

static void
sysinit_perfmeter_initialize(void)
{
#if __CORTEX_M == 3U || __CORTEX_M == 4U || __CORTEX_M == 7U

	#if WITHDEBUG && __CORTEX_M == 7U
		// Поддержка для функций диагностики быстродействия BEGINx_STAMP/ENDx_STAMP - audio.c
		CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
		DWT->LAR = 0xC5ACCE55;	// Key value for unlock
		DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
		DWT->LAR = 0x00000000;	// Key value for lock
	#endif /* WITHDEBUG && __CORTEX_M == 7U */

#endif /* __CORTEX_M == 3U || __CORTEX_M == 4U || __CORTEX_M == 7U */

#if ((__CORTEX_A != 0) || CPUSTYLE_ARM9) && (! defined(__aarch64__))

	#if WITHDEBUG || 1
//	{
//		uint32_t value;
//		__get_CP(15, 0, value, 9, 12, 0);	// Read PMNC
//		PRINTF("PMNC=0x%" PRIX32 "\n", value);
//		PRINTF("counters=%" PRIu32 "\n", (value >> 11) & 0x1F);
//	}
	{
		// Поддержка для функций диагностики быстродействия BEGINx_STAMP/ENDx_STAMP - audio.c
		// From https://stackoverflow.com/questions/3247373/how-to-measure-program-execution-time-in-arm-cortex-a8-processor
		    /* enable user-mode access to the performance counter*/
		// User Enable Register (USEREN)
		//asm ("MCR p15, 0, %0, C9, C14, 0\n\t" :: "r"(1));
		__set_CP(15, 0, 0x00000001, 9, 14, 0);

		/* disable counter overflow interrupts (just in case)*/
		// Interrupt Enable Clear Register (INTENC)
		//asm ("MCR p15, 0, %0, C9, C14, 2\n\t" :: "r"(0x8000000f));
		__set_CP(15, 0, 0x80000000, 9, 14, 2);

		enum { do_reset = 0, enable_divider = 0 };
		// in general enable all counters (including cycle counter)
		uint32_t value = (UINT32_C(1) << 0);	// ENABLE bit

		// peform reset:
		if (do_reset)
		{
			value |= (UINT32_C(1) << 1);     // reset all counters to zero.
			value |= (UINT32_C(1) << 2);     // reset cycle counter to zero.
		}

		if (enable_divider)
			value |= (UINT32_C(1) << 3);     // enable "by 64" divider for CCNT. Clock Divider, bit [3]

		value |= (UINT32_C(1) << 4);		// Export Enable, bit [4]

		// program the performance-counter control-register PMNC:
		//asm volatile ("MCR p15, 0, %0, c9, c12, 0\t\n" :: "r"(value));
		__set_CP(15, 0, value, 9, 12, 0);

		// enable all counters: Count Enable Set Register (CNTENS)
		//asm volatile ("MCR p15, 0, %0, c9, c12, 1\t\n" :: "r"(0x8000000f));
		__set_CP(15, 0, 0x80000000, 9, 12, 1);

		// clear overflows: Overflow Flag Status Register (FLAG)
		//asm volatile ("MCR p15, 0, %0, c9, c12, 3\t\n" :: "r"(0x8000000f));
		__set_CP(15, 0, 0x80000000, 9, 12, 3);
	}
	#endif /* WITHDEBUG */

#endif /* ((__CORTEX_A != 0) || CPUSTYLE_ARM9) && (! defined(__aarch64__)) */
}

static void
sysinit_vbar_initialize(void)
{
#if defined(__aarch64__) && ! LINUX_SUBSYSTEM

#if WITHRTOS
	extern unsigned long __Vectors64_rtos;
	const uintptr_t vbase = (uintptr_t) & __Vectors64_rtos;
#else /* WITHRTOS */
	extern unsigned long __Vectors64;
	const uintptr_t vbase = (uintptr_t) & __Vectors64;
#endif /* WITHRTOS */

	ASSERT((vbase & 0x3FF) == 0);

	//__set_VBAR_EL1(vbase);	 // Set Vector Base Address Register (Bits 10..0 of address should be zero)
	//__set_VBAR_EL2(vbase);	 // Set Vector Base Address Register (Bits 10..0 of address should be zero)
	__set_VBAR_EL3(vbase);	 // Set Vector Base Address Register (Bits 10..0 of address should be zero)

	__set_SCR_EL3(__get_SCR_EL3() | (UINT32_C(1) << 1));	// Physical IRQ while executing at all exception levels are taken in EL3

#elif (__CORTEX_A != 0) || CPUSTYLE_ARM9
#if WITHRTOS
	extern unsigned long __Vectors_rtos;
	const uintptr_t vbase = (uintptr_t) & __Vectors_rtos;
#else /* WITHRTOS */
	extern unsigned long __Vectors;
	const uintptr_t vbase = (uintptr_t) & __Vectors;
#endif /* WITHRTOS */

	ASSERT((vbase & 0x01F) == 0);
	__set_VBAR(vbase);	 // Set Vector Base Address Register (bits 4..0 should be zero)

	__set_SCTLR(__get_SCTLR() & ~ SCTLR_V_Msk);	// v=0 - use VBAR as vectors address
	__set_SCTLR(__get_SCTLR() & ~ SCTLR_A_Msk);	// 0 = Strict alignment fault checking disabled. This is the reset value.

#endif /* (__CORTEX_A != 0) */

#if CPUSTYLE_RISCV

	// http://five-embeddev.com/riscv-isa-manual/latest/machine.html#machine-trap-vector-base-address-register-mtvec
	// 3.1.7 Machine Trap-Vector Base-Address Register (mtvec)
	// https://five-embeddev.com/baremetal/vectored_interrupts/

	//TP();
//	PRINTF("MSIP0: %08X\n", * (volatile uint32_t *) RISCV_MSIP0);
//	* (volatile uint32_t *) RISCV_MSIP0 |= 0x01;
//	PRINTF("MSIP0: %08X\n", * (volatile uint32_t *) RISCV_MSIP0);
	extern uint32_t __Vectors [];
	const uintptr_t vbase = (uintptr_t) & __Vectors;
	ASSERT((vbase & 0x03) == 0);
//	TP();

	//PRINTF("vbase=%p\n", (void *) vbase);
	const uintptr_t vbaseval = vbase | 0x01;	/* set Vectored mode */
	csr_write_mtvec(vbaseval);	/* Machine */
	csr_write_stvec(vbaseval);	/* for supervisor privileges */
	//csr_write_utvec(vbaseval);	/* for user privilege*/

	ASSERT(csr_read_mtvec() == vbaseval);

	PLIC->PLIC_MTH_REG = ARM_USER_PRIORITY;
	{
		unsigned i;
		for (i = 0; i < ARRAY_SIZE(PLIC->PLIC_MIE_REGn); ++ i)
			PLIC->PLIC_MIE_REGn [i] = 0;
	}

	csr_set_bits_mstatus(MSTATUS_MIE_BIT_MASK); // Enable interrupts routing
	csr_set_bits_mie(MIE_MEI_BIT_MASK);	// MEI
	//csr_set_bits_mie(MIE_MTI_BIT_MASK);	// MTI - timer

#endif /* CPUSTYLE_RISCV */
#if (__GIC_PRESENT == 1)
	GIC_SetBinaryPoint(GIC_BINARY_POINT);
#endif /* (__GIC_PRESENT == 1) */
}

void xtrap(void)
{
	dbg_putchar('#');
	for (;;)
		;
}

// SystemInit (on Core #0)
// Reset_CPUn_Handler ((on Core #1..)
static void sysinit_smp_initialize(void)
{
#if (__CORTEX_A == 9U)
	// not set the ACTLR.SMP
	// 0x02: L2 Prefetch hint enable
	__set_ACTLR(__get_ACTLR() | ACTLR_L1PE_Msk | ACTLR_FW_Msk | (UINT32_C(1) << 1));
	#if WITHSMPSYSTEM
		__set_ACTLR(__get_ACTLR() | ACTLR_SMP_Msk);
	#else /* WITHSMPSYSTEM */
		__set_ACTLR(__get_ACTLR() & ~ ACTLR_SMP_Msk);
	#endif /* WITHSMPSYSTEM */
	__ISB();
	__DSB();
#elif (__CORTEX_A == 53U) && defined(__aarch64__)
	// TODO
	//#warning To be done
	// Всё что надо делается в инициализации cache

#elif (__CORTEX_A == 53U) && ! defined(__aarch64__)
	/**
	 * DDI0500J_cortex_a53_r0p4_trm.pdf
	 * Set the SMPEN bit before enabling the caches, even if there is only one core in the system.
	 */
	__set_ACTLR(__get_ACTLR() | (UINT32_C(1) << 0));	// CPUACTLR write access control. The possible
	//__set_CPUACTLR(__get_CPUACTLR() | (UINT64_C(1) << 44));	// [44] ENDCCASCI Enable data cache clean as data cache clean/invalidate.

	__set_ACTLR(__get_ACTLR() | (UINT32_C(1) << 1));	// CPUECTLR write access control. The possible
	#if WITHSMPSYSTEM
		// set the CPUECTLR.SMPEN
		__set_CPUECTLR(__get_CPUECTLR() | CPUECTLR_SMPEN_Msk);
	#else /* WITHSMPSYSTEM */
		// clear the CPUECTLR.SMPEN
		__set_CPUECTLR(__get_CPUECTLR() & ~ CPUECTLR_SMPEN_Msk);
	#endif /* WITHSMPSYSTEM */
	// 4.5.28 Auxiliary Control Register
	// bit6: L2ACTLR write access control
	__set_ACTLR(__get_ACTLR() & ~ (UINT32_C(1) << 6));	/* не надо - но стояло как результат запуcка из UBOOT */
	__ISB();
	__DSB();

#elif (__CORTEX_A == 7U)
	#if WITHSMPSYSTEM
		// set the ACTLR.SMP
		__set_ACTLR(__get_ACTLR() | ACTLR_SMP_Msk);
	#else /* WITHSMPSYSTEM */
		// clear the ACTLR.SMP
		__set_ACTLR(__get_ACTLR() & ~ ACTLR_SMP_Msk);
	#endif /* WITHSMPSYSTEM */
	__ISB();
	__DSB();
#endif /* (__CORTEX_A == 9U) */
}

// ОБщая для всех процессоров инициализация
static void
sysinit_cache_initialize(void)
{
#if ! LINUX_SUBSYSTEM
	//PRINTF("dcache_rowsize=%u, icache_rowsize=%u\n", dcache_rowsize(), icache_rowsize());
	ASSERT(DCACHEROWSIZE == dcache_rowsize());
	ASSERT(ICACHEROWSIZE == icache_rowsize());
#if defined(__GIC_PRESENT) && (__GIC_PRESENT == 1U)
	//PRINTF("GIC_BINARY_POINT=%u\n", GIC_BINARY_POINT);
	ASSERT(GIC_BINARY_POINT == GIC_GetBinaryPoint());
#endif /* defined(__GIC_PRESENT) && (__GIC_PRESENT == 1U) */
#endif /* ! LINUX_SUBSYSTEM */

#if defined (__CORTEX_M)
	#if __ICACHE_PRESENT

		SCB_InvalidateICache();
		SCB_EnableICache();

	#endif /* __ICACHE_PRESENT */
	#if __DCACHE_PRESENT

		SCB_InvalidateDCache();
		SCB_EnableDCache();

	#endif /* __DCACHE_PRESENT */

	//dcache_clean_all();
	L1C_InvalidateDCacheAll();
	L1C_InvalidateICacheAll();
	L1C_InvalidateBTAC();
	L1C_EnableCaches();
	L1C_EnableBTAC();

#elif (__CORTEX_A != 0) || CPUSTYLE_ARM9
	L1C_InvalidateDCacheAll();
	L1C_InvalidateICacheAll();
	L1C_InvalidateBTAC();
	L1C_EnableCaches();
	L1C_EnableBTAC();

#elif CPUSTYLE_F133

	// RISC-V cache initialize
	// https://riscv.org/wp-content/uploads/2016/07/riscv-privileged-v1.9-1.pdf#page=49


	//	7.3.1 L1 Cache Extension Register
	//	C906 L1 cache related extended registers are mainly divided into:
	//	• Cache Enable and Mode Configuration: The Machine Mode Hardware Configuration Register (mhcr) enables switching of instruction and data caches as well as write allocation and
	//	Configuration for writeback mode. The supervisor mode hardware configuration register (shcr) is a map of mhcr and is a read-only register.
	//	• Dirty entry cleanup and invalidation operations: The Machine Mode Cache Operation Register (mcor) can dirty and invalidate instruction and data caches
	//	operation.
	//	• Cache Reads: Machine Mode Cache Access Instruction Registers (mcins), Cache Access Index Registers (mcindex) and Cache
	//	Access to data register 0/1 (mcdata0/1), through which the read operation of the instruction and data caches can be realized.
	//	The specific control register description can refer to the machine mode processor control and status extension register group.


	//	IE-Icache enable bit:
	//	• When IE=0, Icache is closed;
	//	• When IE=1, Icache is turned on.
	//	This bit will be reset to 1'b0.
	//	DE-Dcache enable bit:
	//	• When DE=0, Dcache is closed;
	//	• When DE=1, Dcache is on.
	//	This bit will be reset to 1'b0.
	//	WA - Cache Write Allocation Set Bits:
	//	• When WA=0, the data cache is in write non-allocate mode;
	//	• When WA=1, the data cache is in write allocate mode.
	//	This bit will be reset to 1'b0.
	//	WB - Cache Write Back Set Bits:
	//	• When WB=0, the data cache is in write through mode.
	//	• When WB=1, the data cache is in write back mode.
	//	C906 only supports write back mode, and WB is fixed to 1.
	//	RS-Address Return Stack Set Bits:
	//	• When RS=0, the return stack is closed;
	//	• When RS=1, the return stack is turned on.
	//	This bit will be reset to 1'b0.
	//	BPE - Allow Predictive Jump Set bit:
	//	• When BPE=0, predictive jumps are turned off;
	//	• When BPE=1, predictive jumps are turned on.
	//	This bit will be reset to 1'b0.
	//	BTB-Branch Target Prediction Enable Bit:
	//	• When BTB=0, branch target prediction is turned off.
	//	• When BTB=1, branch target prediction is on.
	//	This bit will be reset to 1'b0.
	//	WBR - Write Burst Enable Bit:
	//	• When WBR=0, write burst transfers are not supported.
	//	• When WBR=1, write burst transfers are supported.
	//	WBR is fixed to 1 in C906.


	//	https://github.com/DongshanPI/eLinuxCore_dongshannezhastu/blob/master/spl/arch/riscv/cpu/riscv64/mmu.c
	//	/*
	//	(0:1) When CACHE_SEL=2'b11, select instruction and data cache
	//	(4) When INV=1, the cache is invalidated
	//	(16) When BHT_INV=1, the data in the branch history table is invalidated
	//	(17) When TB_INV=1, the data in the branch target buffer is invalidated
	//	*/
	//	csr_write(CSR_MCOR, 0x70013);
	csr_write_mcor(0x70013);

	//
	//	/*
	//	(0) Icache is turned on when IE=1
	//	(1) Dcache is turned on when DE=1
	//	(2) When WA=1, the data cache is in write allocate mode (c906 is not supported)
	//	(3) When WB=1, the data cache is in writeback mode (c906 is fixed to 1)
	//	(4) When RS=1, return to the stack to open
	//	(5) When BPE=1, the prediction jump is turned on
	//	(6) When BTB=1, branch target prediction is enabled
	//	(8) Support write burst transmission write when WBR=1 (c906 fixed to 1)
	//	(12) When L0BTB=1, the prediction of the target of the first level branch is enabled
	//	*/
	//	csr_write(CSR_MHCR, 0x11ff);
	csr_write_mhcr(0x011FF);

	//
	//
	//	/*
	//	(15) When MM is 1, unaligned access is supported, and the hardware handles unaligned access
	//	(16) When UCME is 1, user mode can execute extended cache operation instructions
	//	(17) When CLINTEE is 1, CLINT-initiated superuser software interrupts and timer interrupts can be responded to
	//	(21) When the MAEE is 1, the address attribute bit is extended in the PTE of the MMU, and the user can configure the address attribute of the page
	//	(22) When the THEADISAE is 1, the C906 extended instruction set can be used
	//	*/
	//	csr_set(CSR_MXSTATUS, 0x638000);
	csr_set_bits_mxstatus(
			1 * (UINT32_C(1) << 15) |
			1 * (UINT32_C(1) << 16) |
			1 * (UINT32_C(1) << 17) |
			//1 * (UINT32_C(1) << 21) |
			1 * (UINT32_C(1) << 22) |
			0
			//0x638000
			);

	//
	//
	//	/*
	//	(2) DPLD=1, dcache prefetching is enabled
	//	(3,4,5,6,7) AMR=1, when a storage operation of three consecutive cache rows occurs, the storage operation of subsequent consecutive addresses is no longer written to L1Cache
	//	(8) IPLD=1ICACHE prefetch is enabled
	//	(9) LPE=1 cycle acceleration on
	//	(13,14) When DPLD is 2, 8 cache rows are prefetched
	//	*/
	//	csr_write(CSR_MHINT, 0x16e30c);
	csr_write_mhint(0x16E30C);

//	/*C906 will invalid all I-cache automatically when reset*/
//	/*you can invalid I-cache by yourself if necessarily*/
//	/*invalid I-cache*/
//	//	li x3, 0x33
//	//	csrc mcor, x3
//	//	li x3, 0x11
//	//	csrs mcor, x3
//
//	csr_clr_bits_mcor(0x0033);
//	csr_set_bits_mcor(0x0011);
//
//	// Allow Predictive Jump (C906-specific)
//	csr_set_bits_mhcr((uint_xlen_t) 1u << 5);	// BPE
//	// Branch Target Prediction Enable (C906-specific)
//	csr_set_bits_mhcr((uint_xlen_t) 1u << 6);	// BTB
//	// RS-Address Return Stack (C906-specific)
//	csr_set_bits_mhcr((uint_xlen_t) 1u << 4);	// RS
//	// enable I-cache (C906-specific)
//	csr_set_bits_mhcr((uint_xlen_t) 1u << 0);	// IE
//
//
//	/*C906 will invalid all D-cache automatically when reset*/
//	/*you can invalid D-cache by yourself if necessarily*/
//	/*invalid D-cache*/
////	li x3, 0x33
////	csrc mcor, x3
////	li x3, 0x12
////	csrs mcor, x3
//	csr_clr_bits_mcor(0x0033);
//	csr_set_bits_mcor(0x0012);
//
	// enable D-cache Write-allocate (C906-specific)
//	csr_set_bits_mhcr((uint_xlen_t) 1u << 2);	// WA
//	// enable D-cache Write-back (C906-specific)
//	csr_set_bits_mhcr((uint_xlen_t) 1u << 3);	// WB
//	// enable D-cache (C906-specific)
//	csr_set_bits_mhcr((uint_xlen_t) 1u << 1);	// DE
//
//	// Disable D-cache (C906-specific)
//	csr_clr_bits_mhcr((uint_xlen_t) 1u << 1);	// DE
	// 0x0108:
//	PRINTF("MHCR=%08X\n", (unsigned) csr_read_mhcr());

#endif /* CPUSTYLE_RISCV */
}

/* инициадизации кеш-памяти, специфические для CORE0 */
static void
sysinit_cache_L2_initialize(void)
{
#if (__L2C_PRESENT == 1) && defined (PL310_DATA_RAM_LATENCY)
	L2C_Disable();
	* (volatile uint32_t *) (L2C_310_BASE + 0x010C) = PL310_DATA_RAM_LATENCY;	// reg1_data_ram_control
	* (volatile uint32_t *) (L2C_310_BASE + 0x0108) = PL310_TAG_RAM_LATENCY;	// reg1_tag_ram_control
#endif /* (__L2C_PRESENT == 1) */
#if (__L2C_PRESENT == 1)
	// Enable Level 2 Cache
	L2C_InvAllByWay();
	L2C_Enable();
#endif
}


// инициализация контроллера питания (не только DDR память. бывает и GPIO)
void sysinit_pmic_initialize(void)
{
#if defined (BOARD_PMIC_INITIALIZE)
	BOARD_PMIC_INITIALIZE();
#endif /* BOARD_PMIC_INITIALIZE */
}


#if CPUSTYLE_VM14

/* Сбросить второе ядро чтобы не мешало процессу загрузки */
static void resetCPU(unsigned targetcore)
{
	ASSERT(targetcore != 0);

    const uint32_t psmask = UINT32_C(0x03) << (targetcore * 8);	/* SCU_PWR mask */
    ((volatile uint32_t *) SCU_CONTROL_BASE) [2] |= psmask;	/* reset target CPU */
}

//Закометарить следующую строку при сборке проекта для старта из флешки :
//#define USE_ALT_START_FROR_SECOND_CORE 1
//
#ifdef USE_ALT_START_FROR_SECOND_CORE
typedef void (*cpu_startup_remap)(void);
static cpu_startup_remap startup_remap __attribute__ ((section (".CPU_Internal"))) = {0};
#endif

/* функция вызывается из start-up до копирования в SRAM всех "быстрых" функций и до инициализации переменных
*/
// watchdog disable, clock initialize, cache enable
void
SystemInit_BOOT0(void)
{
	resetCPU(1);
	sysinit_fpu_initialize();
	sysinit_gpio_initialize();
	local_delay_initialize();
}

#endif /* CPUSTYLE_VM14 */

#if LINUX_SUBSYSTEM
// Stub
void
SystemInit(void)
{

}
#else /* LINUX_SUBSYSTEM */

// watchdog disable, clock initialize, cache enable
void
SystemInit(void)
{
#if ! WITHISBOOTLOADER_DDR
#if CPUSTYLE_VM14
	resetCPU(1);
#endif /* CPUSTYLE_VM14 */
	sysinit_fpu_initialize();
	sysinit_smp_initialize();
	sysinit_perfmeter_initialize();
	sysinit_vbar_initialize();		// interrupt vectors relocate
	sysinit_pll_initialize(0);		// PLL iniitialize - minimal freq
	local_delay_initialize();
	sysinit_gpio_initialize();
#ifdef BOARD_BLINK_INITIALIZE
	BOARD_BLINK_INITIALIZE();
#endif
	sysinit_debug_initialize();
	sysinit_pll_initialize(1);		// PLL iniitialize - overdrived freq
	local_delay_initialize();
	sysinit_pmic_initialize();
	sysinit_sdram_initialize();
	sysinit_mmu_tables();			// Инициализация таблиц. */
	sysinit_cache_initialize();		// caches iniitialize
	sysinit_cache_L2_initialize();	// L2 cache, SCU initialize
	sysinit_ttbr_initialize();		/* Загрузка TTBR, инвалидация кеш памяти и включение MMU */
#endif /* ! WITHISBOOTLOADER_DDR */
}

/* Функция, вызываемая для инициализации DDR памяти из XFEL */
void __attribute__((used)) SystemDRAMInit(void)
{
#if WITHISBOOTLOADER_DDR
#if CPUSTYLE_VM14
	resetCPU(1);
#endif /* CPUSTYLE_VM14 */
	sysinit_fpu_initialize();
	sysinit_smp_initialize();
	sysinit_perfmeter_initialize();
	sysinit_gpio_initialize();
	local_delay_initialize();
	sysinit_debug_initialize();
	sysinit_pmic_initialize();
	sysinit_sdram_initialize();
#endif /* WITHISBOOTLOADER_DDR */
}

#endif /* LINUX_SUBSYSTEM */

#if (__CORTEX_A != 0) || CPUSTYLE_ARM9

static void cortexa_cpuinfo(void)
{
#if defined(__aarch64__)
	volatile uint_fast32_t vvv;
	dbg_putchar('$');
	PRINTF("CPU%u: VBAR_EL3=%08X, TTBR0_EL3=%08X, SCTLR_EL1=%08X, SCTLR_EL3=%08X, TCR_EL3=%08X, sp=%08X, MPIDR_EL1=%08X\n",
			(unsigned) (__get_MPIDR_EL1() & 0x03),
			(unsigned) __get_VBAR_EL3(),
			(unsigned) __get_TTBR0_EL3(),
			(unsigned) __get_SCTLR_EL1(),
			(unsigned) __get_SCTLR_EL3(),
			(unsigned) __get_TCR_EL3(),
			(unsigned) (uintptr_t) & vvv,
			(unsigned) __get_MPIDR_EL1()
			);
#else
	volatile uint_fast32_t vvv;
	dbg_putchar('$');
	PRINTF("CPU%u: VBAR=%p, TTBR0=%p, cpsr=%08X, SCTLR=%08X, ACTLR=%08X, sp=%p, MPIDR=%08X\n",
			(unsigned) (__get_MPIDR() & 0x03),
			(void *) __get_VBAR(),
			(void *) __get_TTBR0(),
			(unsigned) __get_CPSR(),
			(unsigned) __get_SCTLR(),
			(unsigned) __get_ACTLR(),
			& vvv,
			(unsigned) __get_MPIDR()
			);
#endif
}

#if WITHSMPSYSTEM && ! WITHRTOS


static __ALIGNED(4) const uint32_t trampoline32 [] =
{
	0xE3A03003,	// 	mov	r3, #3
	0xEE0C3F50,	// 	mcr	15, 0, r3, cr12, cr0, {2}
	0xF57FF06F,	// 	isb	sy
	0xE320F003,	// 	wfi
	0xE320F002,	// 	wfe
	0xEAFFFFFD,	// 	b	10 <trampoline32+0x10>
};

#if CPUSTYLE_STM32MP1


/*
 * Cores secure magic numbers
 * Constant to be stored in bakcup register
 * BOOT_API_MAGIC_NUMBER_TAMP_BCK_REG_IDX
 */
//#define BOOT_API_A7_CORE0_MAGIC_NUMBER				0xCA7FACE0U
#define BOOT_API_A7_CORE1_MAGIC_NUMBER				0xCA7FACE1U

/*
 * TAMP_BCK4R register index
 * This register is used to write a Magic Number in order to restart
 * Cortex A7 Core 1 and make it execute @ branch address from TAMP_BCK5R
 */
//#define BOOT_API_CORE1_MAGIC_NUMBER_TAMP_BCK_REG_IDX		4U

/*
 * TAMP_BCK5R register index
 * This register is used to contain the branch address of
 * Cortex A7 Core 1 when restarted by a TAMP_BCK4R magic number writing
 */
//#define BOOT_API_CORE1_BRANCH_ADDRESS_TAMP_BCK_REG_IDX		5U

/*******************************************************************************
 * STM32MP1 handler called when a power domain is about to be turned on. The
 * mpidr determines the CPU to be turned on.
 * call by core 0 to activate core 1
 ******************************************************************************/

//#define HARDWARE_NCORES 2

static void aarch32_mp_cpuN_start(uintptr_t startfunc, unsigned targetcore)
{
	ASSERT(startfunc != 0);
	ASSERT(targetcore != 0);

	PWR->CR1 |= PWR_CR1_DBP;	// 1: Write access to RTC and backup domain registers enabled.
	(void) PWR->CR1;
	while ((PWR->CR1 & PWR_CR1_DBP) == 0)
		;

	RCC->MP_APB5ENSETR = RCC_MP_APB5ENSETR_RTCAPBEN;
	(void) RCC->MP_APB5ENSETR;
	RCC->MP_APB5LPENSETR = RCC_MP_APB5LPENSETR_RTCAPBLPEN;  // Включить тактирование
	(void) RCC->MP_APB5LPENSETR;

	RCC->MP_AHB5ENSETR = RCC_MP_AHB5ENSETR_BKPSRAMEN;
	(void) RCC->MP_AHB5ENSETR;
	RCC->MP_AHB5LPENSETR = RCC_MP_AHB5LPENSETR_BKPSRAMLPEN;
	(void) RCC->MP_AHB5LPENSETR;

	/* Write entrypoint in backup RAM register */
	TAMP->BKP5R = startfunc;	// Invoke at SVC context
	(void) TAMP->BKP5R;

	/* Write magic number in backup register */
	TAMP->BKP4R = BOOT_API_A7_CORE1_MAGIC_NUMBER;
	(void) TAMP->BKP4R;

//	PWR->CR1 &= ~ PWR_CR1_DBP;	// 0: Write access to RTC and backup domain registers disabled.
//	(void) PWR->CR1;
//	while ((PWR->CR1 & PWR_CR1_DBP) != 0)
//		;

	dcache_clean_all();	// startup code should be copied in to sysram for example.

	/* Generate an IT to core 1 */
	GIC_SendSGI(SGI8_IRQn, UINT32_C(1) << targetcore, 0x00);	// CPU1, filer=0
}

#elif CPUSTYLE_XC7Z

// See also:
//	https://stackoverflow.com/questions/60873390/zynq-7000-minimum-asm-code-to-init-cpu1-from-cpu0

//#define HARDWARE_NCORES 2

static void aarch32_mp_cpuN_start(uintptr_t startfunc, unsigned targetcore)
{
	ASSERT(startfunc != 0);
	ASSERT(targetcore != 0);

	* (volatile uint32_t *) 0xFFFFFFF0 = startfunc;	// Invoke at SVC context
	dcache_clean_all();	// startup code should be copied in to sysram for example.
	/* Generate an IT to core 1 */
	__SEV();
}

#elif CPUSTYLE_A64

// H3: R_CPUCFG @ 0x01F01C00

/*
 *
 * Read 0x01F01C00+0x1A4 register Get soft_entry_address
 */

static void aarch32_mp_cpuN_start(uintptr_t startfunc, unsigned targetcore)
{
	volatile uint32_t * const rvaddr = ((volatile uint32_t *) (R_CPUCFG_BASE + 0x1A4));	// See Allwinner_H5_Manual_v1.0.pdf, page 85
	const uint32_t CORE_RESET_MASK = UINT32_C(1) << (0 + targetcore);

	ASSERT(startfunc != 0);
	ASSERT(targetcore != 0);

	CPUX_CFG->C_RST_CTRL &= ~ CORE_RESET_MASK;	// CORE_RESET (3..0) assert

	* rvaddr = startfunc;	// C0_CPUX_CFG->C_CTRL_REG0 AA64nAA32 игнорироуется
	dcache_clean_all();	// startup code should be copied in to sysram for example.

	// Не влияют
	// Register width state.Determines which execution state the processor boots into after a cold reset.
	//C0_CPUX_CFG->C_CTRL_REG0 &= ~ (UINT32_C(1) << (24 + targetcore));	// AA64nAA32 0: AArch32 1: AArch64
	//C0_CPUX_CFG->C_CTRL_REG0 |=  (UINT32_C(1) << (24 + targetcore));	// AA64nAA32 0: AArch32 1: AArch64

	CPUX_CFG->C_RST_CTRL |= CORE_RESET_MASK;	// CORE_RESET (3..0) de-assert
}

static void aarch64_mp_cpuN_start(uintptr_t startfunc, unsigned targetcore)
{
	const uintptr_t startfunc32 = (uintptr_t) trampoline32;
	volatile uint32_t * const rvaddr = ((volatile uint32_t *) (R_CPUCFG_BASE + 0x1A4));	// See Allwinner_H5_Manual_v1.0.pdf, page 85
	const uint32_t CORE_RESET_MASK = UINT32_C(1) << (0 + targetcore);

	ASSERT(startfunc32 != 0);
	ASSERT(ptr_hi32(startfunc32) == 0);
	ASSERT(startfunc != 0);
	ASSERT(targetcore != 0);

	CPUX_CFG->C_RST_CTRL &= ~ CORE_RESET_MASK;	// CORE_RESET (3..0) assert

	* rvaddr = startfunc32;	// C0_CPUX_CFG->C_CTRL_REG0 AA64nAA32 игнорироуется
	CPUX_CFG->RVBARADDR [targetcore].LOW = ptr_lo32(startfunc);
	CPUX_CFG->RVBARADDR [targetcore].HIGH = ptr_hi32(startfunc);

	dcache_clean_all();	// startup code should be copied in to sysram for example.

	// Не влияют
	// Register width state.Determines which execution state the processor boots into after a cold reset.
	//C0_CPUX_CFG->C_CTRL_REG0 &= ~ (UINT32_C(1) << (24 + targetcore));	// AA64nAA32 0: AArch32 1: AArch64
	//C0_CPUX_CFG->C_CTRL_REG0 |=  (UINT32_C(1) << (24 + targetcore));	// AA64nAA32 0: AArch32 1: AArch64

	CPUX_CFG->C_RST_CTRL |= CORE_RESET_MASK;	// CORE_RESET (3..0) de-assert
}

#elif CPUSTYLE_H3
// H3: CPUCFG_BASE @ 0x01F01C00

/*
 *
 * Read 0x01F01C00+0x1A4 register Get soft_entry_address
 */

static void aarch32_mp_cpuN_start(uintptr_t startfunc, unsigned targetcore)
{
	volatile uint32_t * const rvaddr = ((volatile uint32_t *) (CPUCFG_BASE + 0x1A4));	// See Allwinner_H5_Manual_v1.0.pdf, page 85
	const uint32_t CORE_RESET_MASK = UINT32_C(3) << (0);

	ASSERT(startfunc != 0);
	ASSERT(targetcore != 0);

	CPUCFG->CPU [targetcore].CPU_RST_CTRL_REG &= ~ CORE_RESET_MASK;	// CORE_RESET (3..0) assert

	* rvaddr = startfunc;	// C0_CPUX_CFG->C_CTRL_REG0 AA64nAA32 игнорироуется
	dcache_clean_all();	// startup code should be copied in to sysram for example.

	CPUCFG->CPU [targetcore].CPU_RST_CTRL_REG |= CORE_RESET_MASK;	// CORE_RESET (3..0) de-assert
}

#elif CPUSTYLE_H616
// AWUSBFEX ID=0x00182300(H616) dflag=0x44 dlength=0x08 scratchpad=0x00027e00
// H616 version
// https://github.com/renesas-rcar/arm-trusted-firmware/blob/b5ad4738d907ce3e98586b453362db767b86f45d/plat/allwinner/sun50i_h616/include/sunxi_mmap.h#L42


static void aarch32_mp_cpuN_start(uintptr_t startfunc, unsigned targetcore)
{
	// AWUSBFEX ID=0x00182300(H616) dflag=0x44 dlength=0x08 scratchpad=0x00027e00
	// CPUSTYLE_H616
	// https://github.com/apritzel/u-boot/blob/3aaabfe9ff4bbcd11096513b1b28d1fb0a40800f/arch/arm/cpu/armv8/fel_utils.S#L39

	const uint32_t CORE_RESET_MASK = UINT32_C(1) << targetcore;	// CPU0_CORE_RESET
	volatile uint32_t * const rvaddr = ((volatile uint32_t *) (R_CPUCFG_BASE + 0x1C4 + targetcore * 4));
	//volatile uint32_t * const rvaddr = ((volatile uint32_t *) (SUNXI_RTC_BASE + 0x5c4 + targetcore * 4));

	/* Не влияет: */
//	C0_CPUX_CFG_H616->C0_CTRL_REG0 &= ~ (UINT32_C(1) << (targetcore + 24)); // 20, 24... AA64NAA32 0: AArch32 1: AArch64
//	C0_CPUX_CFG_H616->C0_CTRL_REG0 |= (UINT32_C(1) << (targetcore + 24)); // 20, 24... AA64NAA32 0: AArch32 1: AArch64

	ASSERT(startfunc != 0);
	ASSERT(targetcore != 0);

	C0_CPUX_CFG_H616->C0_RST_CTRL &= ~ CORE_RESET_MASK;	// CORE_RESET (3..0) 0: assert

	* rvaddr = startfunc;
	ASSERT(* rvaddr == startfunc);

	dcache_clean_all();	// startup code should be copied in to sysram for example.

	C0_CPUX_CFG_H616->C0_RST_CTRL |= CORE_RESET_MASK;	// 60... CORE_RESET 1: de-assert
}

static void aarch64_mp_cpuN_start(uintptr_t startfunc, unsigned targetcore)
{
	const uintptr_t startfunc32 = (uintptr_t) trampoline32;
	// AWUSBFEX ID=0x00182300(H616) dflag=0x44 dlength=0x08 scratchpad=0x00027e00
	// CPUSTYLE_H616
	// https://github.com/apritzel/u-boot/blob/3aaabfe9ff4bbcd11096513b1b28d1fb0a40800f/arch/arm/cpu/armv8/fel_utils.S#L39

	const uint32_t CORE_RESET_MASK = UINT32_C(1) << targetcore;	// CPU0_CORE_RESET
	volatile uint32_t * const rvaddr = ((volatile uint32_t *) (R_CPUCFG_BASE + 0x1C4 + targetcore * 4));
	//volatile uint32_t * const rvaddr = ((volatile uint32_t *) (SUNXI_RTC_BASE + 0x5c4 + targetcore * 4));

	/* Не влияет: */
//	C0_CPUX_CFG_H616->C0_CTRL_REG0 &= ~ (UINT32_C(1) << (targetcore + 24)); // 20, 24... AA64NAA32 0: AArch32 1: AArch64
//	C0_CPUX_CFG_H616->C0_CTRL_REG0 |= (UINT32_C(1) << (targetcore + 24)); // 20, 24... AA64NAA32 0: AArch32 1: AArch64

	ASSERT(startfunc32 != 0);
	ASSERT(ptr_hi32(startfunc32) == 0);
	ASSERT(startfunc != 0);
	ASSERT(targetcore != 0);

	C0_CPUX_CFG_H616->C0_RST_CTRL &= ~ CORE_RESET_MASK;	// CORE_RESET (3..0) 0: assert
	C0_CPUX_CFG_H616->RVBARADDR [targetcore].LOW = ptr_lo32(startfunc);
	C0_CPUX_CFG_H616->RVBARADDR [targetcore].HIGH = ptr_hi32(startfunc);

	* rvaddr = startfunc32;
	ASSERT(* rvaddr == startfunc32);

	dcache_clean_all();	// startup code should be copied in to sysram for example.

	C0_CPUX_CFG_H616->C0_RST_CTRL |= CORE_RESET_MASK;	// 60... CORE_RESET 1: de-assert
}

#elif CPUSTYLE_T507

// https://github.com/renesas-rcar/arm-trusted-firmware/blob/b5ad4738d907ce3e98586b453362db767b86f45d/plat/allwinner/sun50i_h616/include/sunxi_mmap.h#L42

static void aarch32_mp_cpuN_start(uintptr_t startfunc, unsigned targetcore)
{
	const uint32_t CORE_RESET_MASK = UINT32_C(1) << 0;	// CPUX_CORE_RESET
	volatile uint32_t * const rvaddr = ((volatile uint32_t *) (R_CPUCFG_BASE + 0x1c4 + targetcore * 4));

	ASSERT(startfunc != 0);
	ASSERT(targetcore != 0);

	C0_CPUX_CFG_T507->C0_CPUx_CTRL_REG  [targetcore] &= ~ CORE_RESET_MASK;	// CORE_RESET (3..0) 0: assert

	* rvaddr = startfunc;
	ASSERT(* rvaddr == startfunc);
	dcache_clean_all();	// startup code should be copied in to sysram for example.

	C0_CPUX_CFG_T507->C0_CPUx_CTRL_REG  [targetcore] |= CORE_RESET_MASK;	// CORE_RESET 1: de-assert
}

static void aarch64_mp_cpuN_start(uintptr_t startfunc, unsigned targetcore)
{
	const uintptr_t startfunc32 = (uintptr_t) trampoline32;
	const uint32_t CORE_RESET_MASK = UINT32_C(1) << 0;	// CPUX_CORE_RESET
	volatile uint32_t * const rvaddr = ((volatile uint32_t *) (R_CPUCFG_BASE + 0x1c4 + targetcore * 4));

	ASSERT(startfunc32 != 0);
	ASSERT(ptr_hi32(startfunc32) == 0);
	ASSERT(startfunc != 0);
	ASSERT(targetcore != 0);

	C0_CPUX_CFG_T507->C0_CPUx_CTRL_REG  [targetcore] &= ~ CORE_RESET_MASK;	// CORE_RESET (3..0) 0: assert

	* rvaddr = ptr_lo32(startfunc32);
	ASSERT(* rvaddr == startfunc32);

	CPU_SUBSYS_CTRL_T507->CPUx_CTRL_REG [targetcore] = (UINT32_C(1) << 0); // Register width state AA64NAA32 0: AArch32 1: AArch64
	CPU_SUBSYS_CTRL_T507->RVBARADDR [targetcore].LOW = ptr_lo32(startfunc);
	CPU_SUBSYS_CTRL_T507->RVBARADDR [targetcore].HIGH = ptr_hi32(startfunc);
	dcache_clean_all();	// startup code should be copied in to sysram for example.

	C0_CPUX_CFG_T507->C0_CPUx_CTRL_REG  [targetcore] |= CORE_RESET_MASK;	// CORE_RESET 1: de-assert
}

#elif CPUSTYLE_T113

//	3.4.2.4 CPU0 Hotplug Process
//
//	The Hotplug Flag Register is 0x070005C0.
//	The Soft Entry Address Register is 0x070005C4.

//	3.4.2.3 NON_CPU0 Boot Process
//
//	The Soft Entry Address Register of CPU0 is 0x070005C4
//	The Soft Entry Address Register of CPU1 is 0x070005C8

#define HARDWARE_HOTPLUG_FLAG 0xFA50392F	// CPU Hotplug Flag value

// In Allwinner h133 this i/o block named R_CPUCFG

//#define HARDWARE_NCORES 2

static void aarch32_mp_cpuN_start(uintptr_t startfunc, unsigned targetcore)
{
	const uint32_t CORE_RESET_MASK = UINT32_C(1) << targetcore;	// CPU0_CORE_RESET
	volatile uint32_t * const rvaddr = ((volatile uint32_t *) (R_CPUCFG_BASE + 0x1c4 + targetcore * 4));
	ASSERT(startfunc != 0);
	ASSERT(targetcore != 0);

	C0_CPUX_CFG->C0_RST_CTRL &= ~ CORE_RESET_MASK;
	//R_CPUCFG->SOFTENTRY [targetcore] = startfunc;
	* rvaddr = startfunc;
	ASSERT(* rvaddr == startfunc);
	dcache_clean_all();	// startup code should be copied in to sysram for example.
	C0_CPUX_CFG->C0_RST_CTRL |= CORE_RESET_MASK;
}

#elif CPUSTYLE_VM14

	//	.equ SPL_MAGIC,         0xdeadbeef
	//	.equ SPL_ADDR,          0x2000fff4
	//
	//
	//	LDR     r0, =SPL_MAGIC
	//	LDR     r3, =SPL_ADDR
	//	ADD     r1, pc, #0x4
	//	STR     r1, [r3]
	//	STR     r0, [r3, #0x4]
	//
	//	@LDR     r3, =PMCTR_BASE
	//	@LDR     r0, =0x1
	//	@STR     r0, [r3, #CORE_PWR_UP]

	//	@LDR     r4, =GATE_CORE_CTR
	//	@LDR     r0, [r4]
	//	@ORR     r0, r0, #0x1 //Enable L0_CLK
	//	@STR     r0, [r4]

	//	@LDR     r4, =GATE_SYS_CTR
	//	@LDR     r0, [r4]
	//	@ORR     r0, r0, #0x1 //Enable SYS_CLK
	//	@STR     r0, [r4]

// Страницы 74..78 документа Manual_1892VM14YA.pdf

static void aarch32_mp_cpuN_start(uintptr_t startfunc, unsigned targetcore)
{
	ASSERT(startfunc != 0);
	ASSERT(targetcore != 0);

    const uint32_t psmask = 0x03u << (targetcore * 8);	/* SCU_PWR mask */

//    volatile uint32_t * const SPL_ADDR = (volatile uint32_t *) 0x2000fff4;
//    volatile uint32_t * const SPL_MAGIC = (volatile uint32_t *) 0x2000fff8;
//    volatile uint32_t * const SPL_ADDRX = (volatile uint32_t *) 0x2000fffC;
//
//    SPL_MAGIC [0] = 0xdeadbeef;// Грузим стартовый адрес для ROM загрузчика
//    SPL_ADDR [0] = startfunc;// Грузим стартовый адрес для ROM загрузчика
//    SPL_ADDRX [0] = startfunc;

    ((volatile uint32_t *) SCU_CONTROL_BASE) [2] |= psmask;	/* reset target CPU */

    PMCTR->ALWAYS_MISC0 = startfunc;	/* необходимо для встроенного ROM */
    //
    SMCTR->BOOT_REMAP = 0x03; //SMCTR_BOOT_REMAP_BOOTROM;//SMCTR_BOOT_REMAP_SPRAM;
    dcache_clean_all();    // startup code should be copied in to sysram for example.

    PMCTR->WARM_BOOT_OVRD = 0x01;	/* с этим не реагирует на RESET button */

//    PMCTR->WARM_RST_EN = 1;//0x01;
//
    PMCTR->CPU1_WKP_MASK [0] = ~ 0u;
    PMCTR->CPU1_WKP_MASK [1] = ~ 0u;
    PMCTR->CPU1_WKP_MASK [2] = ~ 0u;
    PMCTR->CPU1_WKP_MASK [3] = ~ 0u;

    ((volatile uint32_t *) SCU_CONTROL_BASE) [2] &= ~ psmask;	/* Release target CPU */
}

#endif /* CPU types */

static LCLSPINLOCK_t cpu1init = LCLSPINLOCK_INIT;
static LCLSPINLOCK_t cpu1userstart [HARDWARE_NCORES];

// Инициализация второго  и далее ппрцессора - сюда попадаем из crt_CortexA_CPUn.S
__NO_RETURN void Reset_CPUn_Handler(void)
{
	sysinit_fpu_initialize();
	sysinit_smp_initialize();
	sysinit_perfmeter_initialize();
	sysinit_vbar_initialize();		// interrupt vectors relocate
	sysinit_cache_initialize();	// caches iniitialize
	sysinit_ttbr_initialize();		// Загрузка TTBR, инвалидация кеш памяти и включение MMU

	GIC_Enable();
	InitializeIrql(IRQL_IPC_ONLU);	// nested interrupts support

	cortexa_cpuinfo();
	arm_hardware_populte_second_initialize();
	global_enableIRQ();	//  __ASM volatile ("cpsie i" : : : "memory");
	LCLSPIN_UNLOCK(& cpu1init);

	unsigned core = arm_hardware_cpuid();
	LCLSPIN_LOCK(& cpu1userstart [core]);		/* ждем пока основной user thread не разрешит выполняться */
	LCLSPIN_UNLOCK(& cpu1userstart [core]);
	InitializeIrql(IRQL_USER);	// nested interrupts support

#if WITHLWIP && ! CPUSTYLE_ALLWINNER
	network_initialize();
#endif /* WITHLWIP */
#if CPUSTYLE_VM14
	{
		unsigned mask = 1u << 24; // GC24 - DBGLED1
		for (;;)
		{
			arm_hardware_pioc_outputs(mask, 1 * mask);
			local_delay_ms(200);
			arm_hardware_pioc_outputs(mask, 0 * mask);
			local_delay_ms(200);
		}
	}
#endif /* CPUSTYLE_VM14 */

	for (;;)
	{
		board_dpc_processing();		// user-mode функция обработки списков запросов dpc на текущем процессоре
	}
}

// Вызывается из main
// Запуск остальных процессоров
void cpump_initialize(void)
{
	unsigned core;

	SystemCoreClock = CPU_FREQ;
	cortexa_cpuinfo();
	lclspin_enable();	// Allwinner H3 - может работать с блокировками только после включения MMU
	LCLSPINLOCK_INITIALIZE(& cpu1init);
	for (core = 1; core < HARDWARE_NCORES && core < arm_hardware_clustersize(); ++ core)
	{

		LCLSPINLOCK_INITIALIZE(& cpu1userstart [core]);
		LCLSPIN_LOCK(& cpu1userstart [core]);
		LCLSPIN_LOCK(& cpu1init);

#if defined(__aarch64__)
		extern const uint64_t aarch64_reset_handlers [];	/* crt_CortexA53_CPUn.S */
		aarch64_mp_cpuN_start(aarch64_reset_handlers [core], core);
#else
		extern const uint32_t aarch32_reset_handlers [];	/* crt_CortexA_CPUn.S */
		aarch32_mp_cpuN_start(aarch32_reset_handlers [core], core);
#endif

		LCLSPIN_LOCK(& cpu1init);	/* ждем пока запустившийся процессор не освододит этот spinlock */
		LCLSPIN_UNLOCK(& cpu1init);
	}
}

/* остальным ядрам разрешаем выполнять прерывания */
void cpump_runuser(void)
{
	unsigned core;
	for (core = 1; core < HARDWARE_NCORES && core < arm_hardware_clustersize(); ++ core)
	{
		LCLSPIN_UNLOCK(& cpu1userstart [core]);
	}
}

#else /* WITHSMPSYSTEM */

void Reset_CPUn_Handler(void)
{
	ASSERT(0);
	for (;;)
		;
}

// Вызывается из main
void cpump_initialize(void)
{
	SystemCoreClock = CPU_FREQ;
	cortexa_cpuinfo();

}

/* остальным ядрам разрешаем выполнять прерывания */
void cpump_runuser(void)
{

}

#endif /* WITHSMPSYSTEM */

#else

/* остальным ядрам разрешаем выполнять прерывания */
void cpump_runuser(void)
{

}

#endif /*  (__CORTEX_A != 0) */

// секция init больше не нужна
void cpu_initdone(void)
{
#if WITHISBOOTLOADER

#if CPUSTYLE_R7S721

	if ((CPG.STBCR9 & CPG_STBCR9_BIT_MSTP93) == 0)
	{
#if 0
		// Когда загрузочный образ FPGA будт оставаться в SERIAL FLASH, запретить отключение.
		while ((SPIBSC0.CMNSR & (UINT32_C(1) << 0)) == 0)	// TEND bit
			;

		SPIBSC0.CMNCR = (SPIBSC0.CMNCR & ~ ((1 << SPIBSC_CMNCR_BSZ))) |	// BSZ
			(1 << SPIBSC_CMNCR_BSZ_SHIFT) |
			0;
		(void) SPIBSC0.CMNCR;	/* Dummy read */

		// SPI multi-io Read Cache Flush
		SPIBSC0.DRCR |= (UINT32_C(1) << SPIBSC_DRCR_RCF_SHIFT);	// RCF bit
		(void) SPIBSC0.DRCR;		/* Dummy read */

		local_delay_ms(50);

		SPIBSC0.SMCR = 0;
		(void) SPIBSC0.SMCR;	/* Dummy read */

#endif
		// spi multi-io hang off
		//CPG.STBCR9 |= CPG_STBCR9_BIT_MSTP93;	// Module Stop 93	- 1: Clock supply to channel 0 of the SPI multi I/O bus controller is halted.
		//(void) CPG.STBCR9;			/* Dummy read */
	}

#endif /* CPUSTYLE_R7S721 */

#endif /* WITHISBOOTLOADER */

	hangoffDATAFLASH();	// Отключить процессор от SERIAL FLASH
}

void arm_hardware_reset(void)
{
#if defined (__NVIC_PRIO_BITS)
	NVIC_SystemReset();
#endif /* defined (__NVIC_PRIO_BITS) */
#if CPUSTYLE_STM32MP1
	RCC->MP_GRSTCSETR = RCC_MP_GRSTCSETR_MPSYSRST_Msk;
#endif /* CPUSTYLE_STM32MP1 */
	for (;;)
		;
}

/* разрешение сторожевого таймера в устройстве */
void watchdog_initialize(void)
{
#if CPUSTYLE_STM32MP1
#endif /* CPUSTYLE_STM32MP1 */
}

/* перезапуск сторожевого таймера */
void watchdog_ping(void)
{
#if CPUSTYLE_STM32MP1
#endif /* CPUSTYLE_STM32MP1 */
}
