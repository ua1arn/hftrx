#include "hardware.h"
#include "formats.h"

#if WITHAUTOTUNER_N7DDCALGO
//#include "lcd.h"
//#include "FM25L16.h"
//#include "ADC_DMA.h"
//#include <built_in.h>

static char b[120];
void lcd_out(int,int, const char*);
int keypressed(int);
//volatile unsigned long long T_MILLIS = 0; //время с начала работы программы в mS
//
////возвращает время с начала работы программы в mS
//unsigned long long millis(void) {
//	return T_MILLIS;
//}
//
////обслуживание прерывания от Таймера 2
//void Timer2_interrupt(void)
//iv IVT_INT_TIM2
//{
//	TIM2_SR.UIF = 0;
//	T_MILLIS++;
//}

//возвращает код нажатой клавиши 1...8, или 0 если клавиша не нажата
// t - задержка
unsigned char read_key(unsigned int t) {
	unsigned char value = 0;
//	if (Button(&GPIOB_IDR, 0, t, 0))
//		value = 1;     //K0
//	if (Button(&GPIOB_IDR, 1, t, 0))
//		value = 2;     //K1
//	if (Button(&GPIOA_IDR, 15, t, 0))
//		value = 3;     //K2
//	if (Button(&GPIOB_IDR, 3, t, 0))
//		value = 4;     //K3
//	if (Button(&GPIOB_IDR, 4, t, 0))
//		value = 5;     //K4
//	if (Button(&GPIOB_IDR, 5, t, 0))
//		value = 6;     //K5
//	if (Button(&GPIOB_IDR, 6, t, 0))
//		value = 7;     //K6
//	if (Button(&GPIOB_IDR, 7, t, 0))
//		value = 8;     //K7
//
//	if (Button(&GPIOA_IDR, 10, t, 0))
//		value = 9;     //485
	return value;
}
//
////тоже что и read_key только ждет отпускания клавиши
////возвращает код нажатой клавиши 1...8, или 0 если клавиша не нажата
//// t - задержка
//
//#define LOW_TIME_PRESSED 1000 //короткое нажатие (менее 2cек)
//#define MED_TIME_PRESSED 3000 //среднее нажатие от 2-х до 4cек   добавляем в срарший полубайт 0х40
//#define HI_TIME_PRESSED 5000  //длинное нажатие (более 4сек)     добавляем в срарший полубайт 0х80
//
//unsigned char keypressed(unsigned int t) {
//	unsigned char k = 0, _key_code = 0;
//	unsigned int t_press = 0;       //время нажатия кнопки
//	unsigned long long old_t_millis;
//
//	_key_code = read_key(t);
//	if (_key_code != 0) {
//		old_t_millis = millis(); //начало временного отсчета нажатия клавиши
//		while (read_key(t) != 0)
//			; //ждем пока кнопка нажата и меряем длительность нажатия
//		t_press = millis() - old_t_millis; //кнопка была отпущена, вычисляем время в течении которого она была нажата
//	}
//
//	if (t_press <= LOW_TIME_PRESSED)
//		k = 0; // произошло короткое нажатие клавиши
//	else if ((t_press > LOW_TIME_PRESSED) && (t_press < MED_TIME_PRESSED))
//		k = 0x40; // произошло среднее нажатие клавиши
//	else if (t_press >= MED_TIME_PRESSED)
//		k = 0x80; // произошло длинное нажатие клавиши
//
//	_key_code |= k;
//
//	return _key_code;
//}

//#define ADC_FWD 0
//#define ADC_REF 1

//#define D_correction 1    //коррекция нелинейности характеристики детектора датчика
// 1- вкл 0-выкл
//#define K_Mult  10        //соотношение витков датчика Tandem-match

//#define  P_High 0         //включение возможности измерения высокой мощности () до 9999 ватт
// 1- вкл 0-выкл
//#define Ind1   50 //(Bcd2Dec(0x00) * 100 + Bcd2Dec(0x50))  // Ind1 = 0.05uH
//#define Ind2   100 //(Bcd2Dec(0x01) * 100 + Bcd2Dec(0x00)  // Ind2 = 0.1uH
//#define Ind3   220 //Bcd2Dec(0x02) * 100 + Bcd2Dec(0x20)  // Ind3 = 0.22uH
//#define Ind4   450 //Bcd2Dec(0x04) * 100 + Bcd2Dec(0x50)  // Ind4 = 0.45uH
//#define Ind5   1000 //Bcd2Dec(0x10) * 100 + Bcd2Dec(0x00)  // Ind5 = 1.0uH
//#define Ind6   2200 //Bcd2Dec(0x22) * 100 + Bcd2Dec(0x00)  // Ind6 = 2.2uH
//#define Ind7   4400 //Bcd2Dec(0x44) * 100 + Bcd2Dec(0x00)  // Ind7 = 4.4uH
//
//#define Cap1   10 //Bcd2Dec(0x00) * 100 + Bcd2Dec(0x10)  // Cap1 = 10pF
//#define Cap2   22 //Bcd2Dec(0x00) * 100 + Bcd2Dec(0x22)  // Cap2 = 22pF
//#define Cap3   47 //Bcd2Dec(0x00) * 100 + Bcd2Dec(0x47)  // Cap3 = 47pF
//#define Cap4   100 //Bcd2Dec(0x01) * 100 + Bcd2Dec(0x00)  // Cap4 = 100pF
//#define Cap5   220 //Bcd2Dec(0x02) * 100 + Bcd2Dec(0x20)  // Cap5 = 220pF
//#define Cap6   470 //Bcd2Dec(0x04) * 100 + Bcd2Dec(0x70)  // Cap6 = 470pF
//#define Cap7   1000 //Bcd2Dec(0x10) * 100 + Bcd2Dec(0x00)  // Cap7 =1000pF
//
//sbit Cap_5 at GPIOC_ODR.B8;
//sbit Cap_10 at GPIOC_ODR.B9;
//sbit Cap_22 at GPIOC_ODR.B10;
//sbit Cap_47 at GPIOC_ODR.B11;
//sbit Cap_100 at GPIOC_ODR.B12;
//sbit Cap_220 at GPIOC_ODR.B13;
//sbit Cap_470 at GPIOC_ODR.B14;
//sbit Cap_1000 at GPIOC_ODR.B15;
//
//sbit Cap_sw at GPIOB_ODR.B8;
////
//sbit Ind_002 at GPIOC_ODR.B0;
//sbit Ind_005 at GPIOC_ODR.B1;
//sbit Ind_011 at GPIOC_ODR.B2;
//sbit Ind_022 at GPIOC_ODR.B3;
//sbit Ind_045 at GPIOC_ODR.B4;
//sbit Ind_1 at GPIOC_ODR.B5;
//sbit Ind_22 at GPIOC_ODR.B6;
//sbit Ind_45 at GPIOC_ODR.B7;

static unsigned char ind = 0, cap = 0, gSW = 0, step_cap = 0, step_ind = 0,
		L_linear = 0, C_linear = 0, L_q = 7, C_q = 7, D_correction = 1,
		L_invert = 1, L_mult = 1, C_mult = 1, P_High = 0, K_Mult = 32,
		Overload = 0, Loss_ind = 0, Relay_off = 0;

static int min_for_start, max_for_start, max_swr;
static int globSWR, globPWR, P_max, swr_a;
static unsigned char rready = 0, p_cnt = 0;

/*int n7ddc_get_forward()
 {
 return ADC_DATA[ADC_FWD];
 }

 int get_reverse()
 {
 return ADC_DATA[ADC_REF];
 }*/

#define k 0.5  //k - коэффицент фильтра 0.0 - 1.0

#define TUS_SWRMIN (100)			// 1.0
#define TUS_SWRMAX (TUS_SWRMIN * 9)			// 4.0
#define TUS_SWR1p1 (TUS_SWRMIN * 11 / 10)	// SWR=1.1

//static unsigned int n7ddc_get_forward(void) {
//	static unsigned int val_tmp_0; //переменная для временного хранения результата измерения
//	unsigned int val = 100;
//	adcvalholder_t r;
//	adcvalholder_t f;
//	const uint_fast16_t swr = tuner_get_swr0(TUS_SWRMAX, & r, & f);
//
//	val = (1 - k) * val_tmp_0 + k * ADC_DATA[ADC_FWD];
//	val_tmp_0 = val;
//	return val;
//}

//k - коэффицент фильтра 0.0 - 1.0

static unsigned int get_reverse(void) {
	static unsigned int val_tmp_1; //переменная для временного хранения результата измерения
	unsigned int val = 0;
//	val = (1 - k) * val_tmp_1 + k * ADC_DATA[ADC_REF];
//	val_tmp_1 = val;

	return val;
}

static void set_ind(unsigned char Ind) {
	if (L_invert == 0) {
//		Ind_005 = Ind.B0;
//		Ind_011 = Ind.B1;
//		Ind_022 = Ind.B2;
//		Ind_045 = Ind.B3;
//		Ind_1 = Ind.B4;
//		Ind_22 = Ind.B5;
//		Ind_45 = Ind.B6;
		//
	} else {
//		Ind_005 = ~Ind.B0;
//		Ind_011 = ~Ind.B1;
//		Ind_022 = ~Ind.B2;
//		Ind_045 = ~Ind.B3;
//		Ind_1 = ~Ind.B4;
//		Ind_22 = ~Ind.B5;
//		Ind_45 = ~Ind.B6;
		//
	}
	n7ddc_settuner(Ind, cap, gSW);
}

static void set_cap(unsigned char Cap) {
//	Cap_10 = Cap.B0;
//	Cap_22 = Cap.B1;
//	Cap_47 = Cap.B2;
//	Cap_100 = Cap.B3;
//	Cap_220 = Cap.B4;
//	Cap_470 = Cap.B5;
//	Cap_1000 = Cap.B6;
	//
	n7ddc_settuner(ind, Cap, gSW);
}

static void set_sw(unsigned char SW) {  // 0 - IN,  1 - OUT
//	Cap_sw = SW;
	n7ddc_settuner(ind, cap, SW);
}

static void atu_reset(void) {
	ind = 0;
	cap = 0;
	gSW = 0;
	n7ddc_settuner(ind, cap, gSW);
}

//коррекция нелинейности характеристики детектора датчика
static int correction(int input) {
	//
	if (input <= 80)
		return 0;
	if (input <= 171)
		input += 244;
	else if (input <= 328)
		input += 254;
	else if (input <= 582)
		input += 280;
	else if (input <= 820)
		input += 297;
	else if (input <= 1100)
		input += 310;
	else if (input <= 2181)
		input += 430;
	else if (input <= 3322)
		input += 484;
	else if (input <= 4623)
		input += 530;
	else if (input <= 5862)
		input += 648;
	else if (input <= 7146)
		input += 743;
	else if (input <= 8502)
		input += 800;
	else if (input <= 10500)
		input += 840;
	else
		input += 860;
	//
	return input;
}

//измерение КСВ
void n7ddc_get_swr(void)
{
	adcvalholder_t r;
	adcvalholder_t f;
	const uint_fast16_t swr = tuner_get_swr0(TUS_SWRMAX, & r, & f) + TUS_SWRMIN;
	globSWR = swr;
	return;
}

static void sharp_cap(void) {
	char range, count, max_range, min_range;
	int min_SWR;
	range = step_cap * C_mult;
	//
	max_range = cap + range;
	if (max_range > 32 * C_mult - 1)
		max_range = 32 * C_mult - 1;
	if (cap > range)
		min_range = cap - range;
	else
		min_range = 0;
	cap = min_range;
	set_cap(cap);
	n7ddc_get_swr();
	if (globSWR == 0)
		return;
	min_SWR = globSWR;
	for (count = min_range + C_mult; count <= max_range; count += C_mult) {
		set_cap(count);
		n7ddc_get_swr();
		if (globSWR == 0)
			return;

		if (globSWR >= min_SWR) {
			local_delay_ms(10);
			n7ddc_get_swr();
		}

		if (globSWR >= min_SWR) {
			local_delay_ms(10);
			n7ddc_get_swr();
		}

		if (globSWR < min_SWR) {
			min_SWR = globSWR;
			cap = count;
			if (globSWR < 120)
				break;
		} else
			break;
	}
	set_cap(cap);
	return;
}

static void sharp_ind(void) {
	char range, count, max_range, min_range;
	int min_SWR;
	range = step_ind * L_mult;
	//
	max_range = ind + range;
	if (max_range > 32 * L_mult - 1)
		max_range = 32 * L_mult - 1;

	if (ind > range)
		min_range = ind - range;
	else
		min_range = 0;

	ind = min_range;
	set_ind(ind);

	n7ddc_get_swr();

	if (globSWR == 0)
		return;
	min_SWR = globSWR;
	for (count = min_range + L_mult; count <= max_range; count += L_mult) {
		set_ind(count);
		n7ddc_get_swr();
		if (globSWR == 0)
			return;
		if (globSWR >= min_SWR) {
			local_delay_ms(10);
			n7ddc_get_swr();
		}

		if (globSWR >= min_SWR) {
			local_delay_ms(10);
			n7ddc_get_swr();
		}

		if (globSWR < min_SWR) {
			min_SWR = globSWR;
			ind = count;

			if (globSWR < 120)
				break;
		} else
			break;
	}
	set_ind(ind);
	return;
}

static void coarse_cap(void) {
	char step = 3;
	char count;
	int min_swr;

	cap = 0;
	set_cap(cap);
	step_cap = step;
	n7ddc_get_swr();

	if (globSWR == 0)
		return;
	min_swr = globSWR + globSWR / 20;
	for (count = step; count <= 31;) {
		set_cap(count * C_mult);
		n7ddc_get_swr();
		if (globSWR == 0)
			return;
		if (globSWR < min_swr) {
			min_swr = globSWR + globSWR / 20;
			cap = count * C_mult;
			step_cap = step;
			if (globSWR < 120)
				break;
			count += step;
			if (C_linear == 0 && count == 9)
				count = 8;
			else if (C_linear == 0 && count == 17) {
				count = 16;
				step = 4;
			}
		} else
			break;
	}
	set_cap(cap);
	return;
}

static void coarse_tune(void) {
	char step = 3;
	char count;
	char mem_cap, mem_step_cap;
	int min_swr;

	mem_cap = 0;
	step_ind = step;
	mem_step_cap = 3;
	min_swr = globSWR + globSWR / 20;
	for (count = 0; count <= 31;) {
		set_ind(count * L_mult);
		coarse_cap();
		n7ddc_get_swr();

		if (globSWR == 0)
			return;
		if (globSWR < min_swr) {
			min_swr = globSWR + globSWR / 20;
			ind = count * L_mult;
			mem_cap = cap;
			step_ind = step;
			mem_step_cap = step_cap;

			if (globSWR < 120)
				break;
			count += step;
			if (L_linear == 0 && count == 9)
				count = 8;
			else if (L_linear == 0 && count == 17) {
				count = 16;
				step = 4;
			}
		} else
			break;
	}
	cap = mem_cap;
	set_ind(ind);
	set_cap(cap);
	step_cap = mem_step_cap;
	local_delay_ms(10);
	return;
}

static void sub_tune(void) {
	int swr_mem, ind_mem, cap_mem;
	//
	swr_mem = globSWR;
	coarse_tune();
	if (globSWR == 0) {
		atu_reset();
		return;
	}
	n7ddc_get_swr();
	if (globSWR < 120)
		return;

	sharp_ind();
	if (globSWR == 0) {
		atu_reset();
		return;
	}
	n7ddc_get_swr();
	if (globSWR < 120)
		return;

	sharp_cap();
	if (globSWR == 0) {
		atu_reset();
		return;
	}
	n7ddc_get_swr();

	if (globSWR < 120)
		return;
	//
	if (globSWR < 200 && globSWR < swr_mem && (swr_mem - globSWR) > 100)
		return;
	swr_mem = globSWR;
	ind_mem = ind;
	cap_mem = cap;
	//
	if (gSW == 1)
		gSW = 0;
	else
		gSW = 1;

	atu_reset();
	set_sw(gSW);
	local_delay_ms(50);
	n7ddc_get_swr();
	if (globSWR < 120)
		return;
	//
	coarse_tune();
	if (globSWR == 0) {
		atu_reset();
		return;
	}
	n7ddc_get_swr();
	if (globSWR < 120)
		return;
	sharp_ind();

	if (globSWR == 0) {
		atu_reset();
		return;
	}

	n7ddc_get_swr();
	if (globSWR < 120)
		return;

	sharp_cap();
	if (globSWR == 0) {
		atu_reset();
		return;
	}
	n7ddc_get_swr();
	if (globSWR < 120)
		return;
	//
	if (globSWR > swr_mem) {
		if (gSW == 1)
			gSW = 0;
		else
			gSW = 1;
		set_sw(gSW);
		ind = ind_mem;
		cap = cap_mem;
		set_ind(ind);
		set_cap(cap);
		globSWR = swr_mem;
	}
	return;
}

static void tune(void) {
	p_cnt = 0;
	P_max = 0;
	//
	rready = 0;
	n7ddc_get_swr();
	if (globSWR < 110)
		return;

	atu_reset();

	local_delay_ms(50);
	n7ddc_get_swr();
	swr_a = globSWR;
	if (globSWR < 110)
		return;
	if (max_swr > 110 && globSWR > max_swr)
		return;

	sub_tune();

	if (globSWR == 0) {
		atu_reset();
		return;
	}
	if (globSWR < 120)
		return;

	if (C_q == 5 && L_q == 5)
		return;

	if (L_q > 5) {
		step_ind = L_mult;
		L_mult = 1;
		sharp_ind();
	}
	if (globSWR < 120)
		return;
	if (C_q > 5) {
		step_cap = C_mult;  // = C_mult
		C_mult = 1;
		sharp_cap();
	}
	if (L_q == 5)
		L_mult = 1;
	else if (L_q == 6)
		L_mult = 2;
	else if (L_q == 7)
		L_mult = 4;
	if (C_q == 5)
		C_mult = 1;
	else if (C_q == 6)
		C_mult = 2;
	else if (C_q == 7)
		C_mult = 4;
	return;
}

void Print_Lcd(void) {
	sprintf(b, "globSWR=%2u.%02u globPWR=%3u.%01uW", globSWR / 100,
			globSWR % 100, globPWR / 10, globPWR % 10);
	lcd_out(1, 1, b);

}

void n7ddc_tune(void) {

//	unsigned i;
//	unsigned char key_code;                //код нажатой кнопки

//	//настройка таймера временных отсчетов
//	RCC_APB1ENR.TIM2EN = 1;       // Enable clock gating for timer module 2
//	TIM2_CR1.CEN = 0;             // Disable timer
//	TIM2_PSC = 0;                 // Set timer prescaler.
//	TIM2_ARR = 63999;
//	NVIC_IntEnable(IVT_INT_TIM2); // Enable timer interrupt
//	TIM2_DIER.UIE = 1;            // Update interrupt enable
//	TIM2_CR1.CEN = 1;             // Enable timer
//
//	//инициализация LCD
//	Lcd_Init();
//	local_delay_ms(100);
//	Lcd_Cmd(_LCD_CLEAR);               // очистить индикатор
//	Lcd_Cmd(_LCD_CURSOR_OFF);          // погасить курсор
//	local_delay_ms(100);
//	set_chars();
//
//	//инициализация SPI
//	// CLK - PA5, MISO - PA6, MOSI - PA7 ( SPI1 по умолчанию )
//	SPI1_Init();
//	GPIO_Alternate_Function_Enable(&_GPIO_MODULE_SWJ_JTAGDISABLE);
//
//	//предварительная установка всего, что висит на SPI
//	GPIO_Config(&GPIOA_BASE, _GPIO_PINMASK_4, _GPIO_CFG_DIGITAL_OUTPUT); // CS для  FM25L16

	//nvram_initialize();          //инициализация FM25L16

	//инициализация ADC
//	ADC_DMA_Config();
//
//	// инициализация выводов управления реле согласующего устройства
//	GPIO_Config(&GPIOB_BASE, _GPIO_PINMASK_8, _GPIO_CFG_DIGITAL_OUTPUT);  // CSH
//	GPIO_Config(&GPIOC_BASE, _GPIO_PINMASK_LOW, _GPIO_CFG_DIGITAL_OUTPUT); // L0..L7
//	GPIO_Config(&GPIOC_BASE, _GPIO_PINMASK_HIGH, _GPIO_CFG_DIGITAL_OUTPUT); // C0..C7
//
//	//инициализация пинов для клавиатуры
//	GPIO_Config(&GPIOB_BASE, _GPIO_PINMASK_0,
//			_GPIO_CFG_DIGITAL_INPUT | _GPIO_CFG_PULL_UP);     //K0
//	GPIO_Config(&GPIOB_BASE, _GPIO_PINMASK_1,
//			_GPIO_CFG_DIGITAL_INPUT | _GPIO_CFG_PULL_UP);     //K1
//	GPIO_Config(&GPIOA_BASE, _GPIO_PINMASK_15,
//			_GPIO_CFG_DIGITAL_INPUT | _GPIO_CFG_PULL_UP);    //K2
//	GPIO_Config(&GPIOB_BASE, _GPIO_PINMASK_3,
//			_GPIO_CFG_DIGITAL_INPUT | _GPIO_CFG_PULL_UP);     //K3
//	GPIO_Config(&GPIOB_BASE, _GPIO_PINMASK_4,
//			_GPIO_CFG_DIGITAL_INPUT | _GPIO_CFG_PULL_UP);     //K4
//	GPIO_Config(&GPIOB_BASE, _GPIO_PINMASK_5,
//			_GPIO_CFG_DIGITAL_INPUT | _GPIO_CFG_PULL_UP);     //K5
//	GPIO_Config(&GPIOB_BASE, _GPIO_PINMASK_6,
//			_GPIO_CFG_DIGITAL_INPUT | _GPIO_CFG_PULL_UP);     //K6
//	GPIO_Config(&GPIOB_BASE, _GPIO_PINMASK_7,
//			_GPIO_CFG_DIGITAL_INPUT | _GPIO_CFG_PULL_UP);     //K7

	//GPIO_Config(&GPIOA_BASE, _GPIO_PINMASK_10, _GPIO_CFG_DIGITAL_INPUT | _GPIO_CFG_PULL_UP);     //485 RX
	//GPIO_Config(&GPIOA_BASE, _GPIO_PINMASK_9, _GPIO_CFG_DIGITAL_OUTPUT);     //485 TX
	TP();
	atu_reset();

	atu_reset();
	unsigned i;
	for (i = 0; i <= 5; i++) //на всякий случай 5 проходов
			{
		n7ddc_get_swr();
		if (globSWR <= 120)
			break;
		else
			tune();
	}
	TP();
	return;

//	for (;;) {
//		n7ddc_get_pwr();
//		n7ddc_get_swr();
//		Print_Lcd();
//		local_delay_ms(100);
//
//		switch (key_code = keypressed(5)) {
//		case 0x00:
//			break;
//		case 0x01: {
//			lcd_out(2, 1, "TUNE");
//			lcd_out(2, 1, "OK  ");
//		}
//			break;
//
//			//case 0x41: lcd_out(1, 1, "41"); break;
//			//case 0x81: lcd_out(1, 1, "81"); break;
//		}
//
//	}

}

#endif /* WITHAUTOTUNER_N7DDCALGO */
