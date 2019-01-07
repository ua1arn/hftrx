/* $Id$ */
/* Pin manipulation functioms */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#ifndef BOARD_H_INCLUDED
#define BOARD_H_INCLUDED

#include "synthcalcs.h"

#include <stdint.h>

void board_reset(void);			// выдача сигнала reset_n
void board_init_io(void);
//void board_init_io2(void);	// debug
void board_init_chips(void);
void board_init_chips2(void);
uint_fast8_t boad_fpga_adcoverflow(void);	/* получения признака переполнения АЦП приёмного тракта */
uint_fast8_t boad_mike_adcoverflow(void);	/* получения признака переполнения АЦП микрофонного тракта */


uint_fast32_t getvcoranges(uint_fast8_t vco, uint_fast8_t top);	/* функция для настройки ГУН */

void prog_dds1_ftw(const ftw_t * value);
void prog_dds1_ftw_sub(const ftw_t * value);
void prog_dds1_ftw_sub3(const ftw_t * value);
void prog_dds1_ftw_sub4(const ftw_t * value);
void prog_dds1_ftw_noioupdate(const ftw_t * value);
void prog_dds2_ftw(const ftw_t * value);
void prog_dds3_ftw(const ftw_t * value);
void prog_rts1_ftw(const ftw_t * value);	// Установка центральной частоты панорамного индикатора
void prog_xvtr_freq(uint_fast32_t f,uint_fast8_t enable);	// Установка частоты конвертора

typedef uint_fast32_t pllhint_t;

void si570_initialize(void);
void synth_lo1_setreference(uint_fast32_t freq);
pllhint_t si570_get_hint(uint_fast32_t f);
uint_fast16_t si570_get_divider(pllhint_t hint);
void si570_n(pllhint_t hint, const phase_t * value);

//void prog_pll1_r(const phase_t * value);
uint_fast8_t board_pll1_set_n(const phase_t * value, pllhint_t hint, uint_fast8_t stop);	/* установка требуемого делителя и конфигурации ГУН на основании hint (если требуется) */
pllhint_t board_pll1_get_hint(uint_fast32_t f);		/* выходной делитель за VCO и код VCO по выходной частоте */
uint_fast16_t board_pll1_get_divider(pllhint_t hint);	/* получение значения выхдного делителя из hint */
void board_pll1_set_vco(pllhint_t hint);		/* установка конфигурации ГУН на основании hint */
void board_pll1_set_vcodivider(pllhint_t hint);	/* установка выходного делителя ГУН на основании hint */

extern int_fast32_t si570_get_xtall_base(void); 
extern uint_fast16_t si570_xtall_offset; 
#define OSCSHIFT 15000UL	/* половина диапазона перестройки частоты опорного генератора (в герцах). */

void prog_pll2_n(const phase_t * value);
void prog_pll2_r(const phase_t * value);
void board_update(void);		/* вызвать эту функцию для вывода теневых значений в регистры */

void board_flt1regchanged(void);
void board_codec1regchanged(void);

void prog_dds1_setlevel(uint_fast8_t percent);

void board_set_lctl1(uint_fast8_t v); // управление внешним сигналом


void board_set_tx(uint_fast8_t v);	/* включение на передачу */
void board_set_opowerlevel(uint_fast8_t n);	/* установить выходную мощность WITHPOWERTRIMMIN..WITHPOWERTRIMMAX */

void board_set_att(uint_fast8_t v);
void board_set_antenna(uint_fast8_t v);
void board_set_agc(uint_fast8_t n);
void board_set_sleep(uint_fast8_t v);	/* перевести в режим минимального потребления */

void board_set_maxlabdac(uint_fast16_t n);	/* значение на выходе ЦАП для увода частоты опорного генератора PLL */

void board_setfanflag(uint_fast8_t v);	/* включить вентилятор */
void board_set_mainsubrxmode(uint_fast8_t v);	// Левый/правый, A - main RX, B - sub RX
void board_set_detector(uint_fast8_t v);
void board_set_nfm(uint_fast8_t v);
void board_set_nfmnbon(uint_fast8_t v);	/* Включние noise blanker на SW2014FM */
void board_set_filter(uint_fast16_t v);
void board_set_bandf(uint_fast8_t n);	/*  диапазонный фильтр приёмника */
void board_set_bandf2(uint_fast8_t n);	/* диапазонный фильтр передатчика */
void board_set_bandf3(uint_fast8_t n);	/* управление через разъем ACC */
void board_set_bcdfreq100k(uint_fast16_t bcdfreq);	/* Для выбора диапазона - частота с дискретностью 100 кГц */
void board_set_pabias(uint_fast8_t n);	/* установить ток покоя выходного каскада передатчика */
void board_set_bandfonhpf(uint_fast8_t n);	/* установить код выбора диапазонного фильтра, с которого включается ФВЧ перед УВЧ в SW20xx */
void board_set_bandfonuhf(uint_fast8_t n);	/* Установить код диапазонного фильтра, на котором вкдючать UHF */
void board_set_if4lsb(uint_fast8_t v);	/* требуется для приемников прямого преобразования */
//void board_set_fltsofter(uint_fast8_t n);/* Код управления сглаживанием скатов фильтра основной селекции на приёме */
void board_set_narrow(uint_fast8_t v);	/* установка режима према телеграфа (НЧ) */
void board_set_notch(uint_fast8_t v);		/* не нулевой аргумент - включение НЧ режекторного фильтра */
void board_set_notchnarrow(uint_fast8_t v);		/* не нулевой аргумент - включение НЧ режекторного фильтра в режиме CW */
void board_set_vox(uint_fast8_t v);		/* не нулевой аргумент - включение VOX (для внешних схем) */
void board_set_preamp(uint_fast8_t v); /* включение УВЧ */
void board_set_adcfifo(uint_fast8_t v);	/* Требуется формирование кода для ЦАП в режиме беззнакового кода */
void board_set_adcoffset(int_fast16_t n); /* смещение для выходного сигнала с АЦП */
void board_set_xvrtr(uint_fast8_t v);	/* Разрешить работу конвертора */
void board_set_dacstraight(uint_fast8_t v);	/* Требуется формирование кода для ЦАП в режиме беззнакового кода */
void board_set_dactest(uint_fast8_t v);	/* вместо выхода интерполятора к ЦАП передатчика подключается выход NCO */
void board_set_tx_inh_enable(uint_fast8_t v);	/* разрешение реакции на вход tx_inh */
void board_set_tx_bpsk_enable(uint_fast8_t v);	/* разрешение прямого формирования модуляции в FPGA */
void board_set_mode_wfm(uint_fast8_t v);
void board_set_dither(uint_fast8_t v);	/* управление зашумлением в LTC2208 */
void board_set_adcrand(uint_fast8_t v);	/* управление интерфейсом в LTC2208 */
void board_set_dacscale(uint_fast8_t n);	/* Использование амплитуды сигнала с ЦАП передатчика - 0..100% */
void board_set_dac1(uint_fast8_t n);	/* подстройка опорного генератора */
void board_set_bglight(uint_fast8_t n);	/* Включение подсветки дисплея */
void board_set_kblight(uint_fast8_t v);	/* Включение подсветки клавиатуры */
void board_set_blfreq(uint_fast32_t n);	/* установка делителя для формирования рабочей частоты преобразователя подсветки */
void board_set_txcw(uint_fast8_t v);	/* Включение передачи в обход балансного модулятора */
void board_set_txgate(uint_fast8_t v);	/* разрешение драйвера и оконечного усилителя */
void board_set_scalelo1(uint_fast8_t n);	/* SW-2011-RDX */
void board_set_tuner_C(uint_fast8_t n);	/* установка значение конденсатора в согласующем устройстве */
void board_set_tuner_L(uint_fast8_t n);	/* установка значение индуктивности в согласующем устройстве */
void board_set_tuner_type(uint_fast8_t v);	/* вариант повышающего/понижающего согласования */
void board_set_tuner_bypass(uint_fast8_t v);	/* обход согласующего устройства */
void board_set_autotune(uint_fast8_t v);	/* Находимся в режиме настройки согласующего устройства */

void board_set_user1(uint_fast8_t v);
void board_set_user2(uint_fast8_t v);
void board_set_user3(uint_fast8_t v);
void board_set_user4(uint_fast8_t v);
void board_set_user5(uint_fast8_t v);

void board_set_reset_n(uint_fast8_t v);	/* установка сигнала RESET всем устройствам */
void board_set_flt_reset_n(uint_fast8_t v);	/* установка сигнала RESET блокам фильтров в FPGA */
void board_set_i2s_enable(uint_fast8_t v);	/* разрешение генерации тактовой частоты для I2S в FPGA */

void board_set_stage1level(uint_fast8_t v);		/* управление током первого каскада усиления hermes */
void board_set_stage2level(uint_fast8_t v);		/* управление током второго каскада усиления hermes */
void board_set_sdcardpoweron(uint_fast8_t n);	/* не-0: включить питание SD CARD */
void board_set_usbflashpoweron(uint_fast8_t n);	/* не-0: включить питание USB FLASH */
void board_set_attvalue(uint_fast8_t v);		/* установить значение аттенюатора сигнал-генератора */
void prog_dac1_a_value(uint_fast8_t v);
void prog_dac1_b_value(uint_fast8_t v);
void board_set_affilter(uint_fast8_t v);
void board_set_loudspeaker(uint_fast8_t v);
void board_set_digigainmax(uint_fast8_t v);		/* диапазон ручной регулировки цифрового усиления - максимальное значение */
void board_set_gvad605(uint_fast8_t v);		/* напряжение на AD605 (управление усилением тракта ПЧ */
void board_set_fsadcpower10(int_fast16_t v);		/*	Мощность, соответствующая full scale от IF ADC */
uint_fast32_t board_get_fqmeter(void);			/* получить значение измеренной частоты */
void board_ctl_set_vco(uint_fast8_t n);	// 0..3, 0..5 - code of VCO
void board_setlo2xtal(uint_fast8_t n);	// номер кварца для интерполяционного приемника
void board_codec2_nreset(uint_fast8_t v);	/* формирование сигнала "RESET" для codec2. 0 - снять ресет. */
void board_set_modem_speed100(uint_fast32_t v);	// скорость передачи с точностью 1/100 бод
void board_set_modem_mode(uint_fast8_t v);	// применяемая модуляция

void board_lcd_rs(uint_fast8_t v);	// выставить уровень на сигнале lcd register select - не требуется board_update
void board_lcd_reset(uint_fast8_t v);	// выставить уровень на сигнале lcd reset

/* звуки - подзвучка нажатий и самоконтроль в телеграфе */
/* вызывается при запрещённых прерываниях. */
void board_beep_initialize(void);

void board_sidetone_setfreq(uint_least16_t freq);	/* freq - частота в герцах. Минимум - 400 герц (определено набором команд CAT).*/
void board_keybeep_setfreq(uint_least16_t freq);	/* freq - частота в герцах. */
/* функции разрешения выдачи звукового сигнала - могут перекрываться. */
void board_sidetone_enable(uint_fast8_t state);
/* вызывается при запрещённых прерываниях. */
void board_keybeep_enable(uint_fast8_t state);
void board_testsound_enable(uint_fast8_t state);
void board_subtone_setfreq(uint_least16_t tonefreq01);	/* tonefreq - частота в десятых долях герца. */
void board_subtone_enable(uint_fast8_t state);

/* загрузка коэффициентов FIR фильтра в FPGA */
void board_fpga_fir_initialize(void);
void board_reload_fir(uint_fast8_t ifir, const int_fast32_t * const k, unsigned Ntap, unsigned CWidth); /* Выдача расчитанных параметров фильтра в FPGA (симметричные) */
/* управление полосовыми фильтрами - bandpass.c */
void bandf_calc_initialize(void);
uint8_t bandf_calc(uint_fast32_t freq);	/* получить номер диапазонного фильтра по частоте */
uint8_t bandf2_calc(uint_fast32_t freq);	/* получить номер диапазонного фильтра передатчика по частоте */
uint8_t bandf3_calc(uint_fast32_t freq);	/* получить код для управления через разъем ACC */

void board_rtc_getdate(
	uint_fast16_t * year,
	uint_fast8_t * month,
	uint_fast8_t * dayofmonth
	);
void board_rtc_gettime(
	uint_fast8_t * hour,
	uint_fast8_t * minute,
	uint_fast8_t * secounds
	);
void board_rtc_getdatetime(
	uint_fast16_t * year,
	uint_fast8_t * month,	// 01-12
	uint_fast8_t * dayofmonth,
	uint_fast8_t * hour,
	uint_fast8_t * minute,
	uint_fast8_t * secounds
	);
void board_rtc_setdate(
	uint_fast16_t year,
	uint_fast8_t month,
	uint_fast8_t dayofmonth
	);
void board_rtc_settime(
	uint_fast8_t hour,
	uint_fast8_t minute,
	uint_fast8_t secounds
	);
void board_rtc_setdatetime(
	uint_fast16_t year,
	uint_fast8_t month,
	uint_fast8_t dayofmonth,
	uint_fast8_t hour,
	uint_fast8_t minute,
	uint_fast8_t secounds
	);
uint_fast8_t board_rtc_chip_initialize(void);

#if defined (NVRAM_TYPE) && (NVRAM_TYPE != NVRAM_TYPE_NOTHING)

	void nvram_initialize(void);
	void nvram_set_abytes(uint_fast8_t v);
	#ifndef NVRAM_END
		#error NVRAM_END required, may be missing NVRAM_TYPE
	#endif
	#if (NVRAM_END > 255)
		typedef uint_least16_t nvramaddress_t;				/* можно сделать 8 бит. смещение в NVRAM. Если MENUNONVRAM - только меняем в памяти */
	#else /* (NVRAM_END > 255) */
		typedef uint_least8_t nvramaddress_t;				/* можно сделать 8 бит. смещение в NVRAM. Если MENUNONVRAM - только меняем в памяти */
	#endif /* (NVRAM_END > 255) */

	void save_i32(nvramaddress_t addr, uint32_t v);	/* сохранение по указанному индексу в FRAM одного 32-битного слова */
	uint_fast32_t restore_i32(nvramaddress_t addr); /* выборка по указанному индексу из FRAM одного 32-битного слова */
	void save_i16(nvramaddress_t addr, uint16_t v); /* сохранение по указанному индексу в FRAM одного 16-битного слова */
	uint_fast16_t restore_i16(nvramaddress_t addr); /* выборка по указанному индексу из FRAM одного 16-битного слова */
	void save_i8(nvramaddress_t addr, uint8_t v); /* сохранение по указанному индексу в FRAM одного байта */
	uint_fast8_t restore_i8(nvramaddress_t addr); /* выборка по указанному индексу из FRAM одного байта */

#else /* defined (NVRAM_TYPE) && (NVRAM_TYPE != NVRAM_TYPE_NOTHING) */

#endif /* defined (NVRAM_TYPE) && (NVRAM_TYPE != NVRAM_TYPE_NOTHING) */


/* получить значение forward & reflected ADC */
adcvalholder_t board_getswrmeter(
	adcvalholder_t * reflected, 	// в знаяениях АЦП
	uint_fast8_t swrcalibr	// 90..110 - коррекция
	);	
uint_fast8_t board_getpwrmeter(
	uint_fast8_t * toptrace	// peak hold
	); 

uint_fast16_t 
mcp3208_read(
	spitarget_t target,
	uint_fast8_t diff,
	uint_fast8_t adci,
	uint_fast8_t * valid
	);

void board_adc_initialize(void);
void board_usb_initialize(void);
void board_usb_activate(void);

void usbd_descriptors_initialize(uint_fast8_t deschs);
void hardware_usbd_dma_initialize(void);
void hardware_usbd_dma_enable(void);

uint_fast8_t board_get_adcinputs(void);	/* получить количество каналов АЦП, задействованных в устройстве */
uint_fast8_t board_get_adcch(uint_fast8_t index);	/* получить канал АЦП но индексу в таблице */

void board_adc_store_data(uint_fast8_t i, adcvalholder_t v);
void board_adc_filtering(void);	// Функция вызывается из обработчика прерывания после получения значения от последнего канала АЦП

void 
modemchangefreq(
	uint_fast32_t f		// частота, которую устанавливаем по команде от modem
	);

void 
modemchangespeed(
	uint_fast32_t speed100		// скорость, которую устанавливаем по команде от modem
	);

void 
modemchangemode(
	uint_fast32_t modemmode		// режим, который устанавливаем по команде от modem
	);

int gettxstate(void);


enum 
{
	BOARD_ADCFILTER_DIRECT,		/* фильтрация не применяется (значение для всех каналов по умолчанию) */
	BOARD_ADCFILTER_AVERAGEPWR,	/* усреднение для измерителя мощности - только один канал */
	BOARD_ADCFILTER_TRACETOP3S,	/* Отслеживание максимума с постоянной времени 3 секунды */
	BOARD_ADCFILTER_LPF,			/* ФНЧ, параметр задается в виде числа с фиксированной точкой */
#if WITHPOTFILTERS
	BOARD_ADCFILTER_HISTERESIS2,	/* гистерезис в 2 уровня квантования */
#endif /* WITHPOTFILTERS */
	//
	BOARD_ADCFILTER_TYPECOUNT
};

uint_fast8_t board_getadc_filtered_u8(uint_fast8_t i, uint_fast8_t lower, uint_fast8_t upper);	/* получить значение от АЦП в диапазоне lower..upper (включая границы) */
uint_fast16_t board_getadc_filtered_u16(uint_fast8_t i, uint_fast16_t lower, uint_fast16_t upper);	/* получить значение от АЦП в диапазоне lower..upper (включая границы) */
uint_fast8_t board_getadc_smoothed_u8(uint_fast8_t i, uint_fast8_t lower, uint_fast8_t upper);	/* при изменении отфильтрованного значения этого АЦП возвращаемое значение на каждом вызове приближается к нему на 1 */
uint_fast8_t board_getadc_unfiltered_u8(uint_fast8_t i, uint_fast8_t lower, uint_fast8_t upper);	/* получить значение от АЦП в диапазоне lower..upper (включая границы) */
uint_fast16_t board_getadc_unfiltered_u16(uint_fast8_t i, uint_fast16_t lower, uint_fast16_t upper);	/* получить значение от АЦП в диапазоне lower..upper (включая границы) */
uint_fast32_t board_getadc_unfiltered_u32(uint_fast8_t i, uint_fast32_t lower, uint_fast32_t upper);	/* получить значение от АЦП в диапазоне 0..255 */
adcvalholder_t board_getadc_filtered_truevalue(uint_fast8_t i);	/* получить значение от АЦП */
adcvalholder_t board_getadc_unfiltered_truevalue(uint_fast8_t i);	/* получить значение от АЦП */
adcvalholder_t board_getadc_fsval(uint_fast8_t i);	/* получить максимальное возможное значение от АЦП */
//void hardware_set_adc_filter(uint_fast8_t i, uint_fast8_t v);	/* установить способ фильтрации данных (в момент выборки их регистра АЦП */
//void hardware_set_adc_filterLPF(uint_fast8_t i, uint_fast8_t k);	/* Установить способ фильтрации LPF и частоту среза - параметр 1.0..0.0, умноженное на BOARD_ADCFILTER_LPF_DENOM */
#define BOARD_ADCFILTER_LPF_DENOM	128		/* положение фиксированной точки при фильтрации BOARD_ADCFILTER_LPF */


enum
{
	STTE_OK,		// можно переходить к следующем состоянию.
	STTE_BYUSY,		// вложенная state machine еще выполняет свои фкнкции - требуется повторный вход.
	//
	STTE_count
};

typedef uint_fast8_t STTE_t;


#endif /* BOARD_H_INCLUDED */
