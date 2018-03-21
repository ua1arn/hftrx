/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#ifndef AUDIO_H_INCLUDED
#define AUDIO_H_INCLUDED

#if WITHDSPEXTFIR && WITHI2SCLOCKFROMPIN
	#define ARMI2SMCLKX(scale)	(DUCDDC_FREQ * (uint_fast64_t) (scale) / FPGADECIMATION)
#else /* WITHDSPEXTFIR && WITHI2SCLOCKFROMPIN */
	#define ARMI2SMCLKX(scale)	(ARMSAIMCLK * (uint_fast64_t) (scale) / 256)
#endif /* WITHDSPEXTFIR && WITHI2SCLOCKFROMPIN */

#define ARMSAIRATE		(ARMSAIMCLK / 256)	// SAI sample rate (FPGA/IF CODEC side)

#if WITHDTMFPROCESSING

	#define ARMI2SRATE			((unsigned long) 8000)	// I2S sample rate audio codec (human side)
	#define ARMI2SRATEX(scale)	((unsigned long) (ARMI2SRATE * (scale)))	// I2S sample rate audio codec (human side)
	#define ARMI2SRATE100		((unsigned long) ARMI2SRATEX(100))

#else /* WITHDTMFPROCESSING */

	#define ARMI2SRATE			((unsigned long) (ARMI2SMCLK / 256))	// I2S sample rate audio codec (human side)
	#define ARMI2SRATEX(scale)	((unsigned long) (ARMI2SMCLKX(scale)))	// I2S sample rate audio codec (human side)
	#define ARMI2SRATE100		((unsigned long) (ARMI2SRATEX(100)))

#endif /* WITHDTMFPROCESSING */

#define MODEMBUFFERSIZE8	1024

#if WITHUSEDUALWATCH
	#define NTRX 2	/* количество трактов приемника. */
#else /* WITHUSEDUALWATCH */
	#define NTRX 1	/* количество трактов приемника. */
#endif /* WITHUSEDUALWATCH */

/* Применённая система диспетчеризации требует, 
   чтобы во всех буферах помещалось не меньше сэмплов, 
   чем в DMABUFFSIZE32RX 
 */
#if WITHDSPEXTDDC

	#if CPUSTYLE_R7S721

		// buff data layout: I main/I sub/Q main/Q sub
		#define DMABUFSTEP32RX	8		// Каждому сэмплу соответствует восемь чисел в DMA буфере

		#define DMABUF32RX0I	0		// RX0, I
		#define DMABUF32RX1I	1		// RX1, I
		#define DMABUF32RX0Q	4		// RX0, Q
		#define DMABUF32RX1Q	5		// RX1, Q

		#define DMABUFSTEP32TX	8		// Каждому сэмплу соответствует восемь чисел в DMA буфере
		#define DMABUF32TXI	0		// TX, I
		#define DMABUF32TXQ	4		// TX, Q

		#if WITHRTS96
			#define DMABUF32RTS0I	2		// RTS0, I	// previous
			#define DMABUF32RTS1I	3		// RTS1, I	// current
			#define DMABUF32RTS0Q	6		// RTS0, Q	// previous
			#define DMABUF32RTS1Q	7		// RTS1, Q	// current
		#endif /* WITHRTS96 */

		#define DMABUFSTEP16	2		// 2 - каждому сэмплу соответствует два числа в DMA буфере

	#elif CPUSTYLE_STM32F

		// buff data layout: I main/I sub/Q main/Q sub
		#define DMABUFSTEP32RX	8		// Каждому сэмплу соответствует восемь чисел в DMA буфере

		#define DMABUF32RX0I	0		// RX0, I
		#define DMABUF32RX1I	1		// RX1, I
		#define DMABUF32RX0Q	4		// RX0, Q
		#define DMABUF32RX1Q	5		// RX1, Q

		#if WITHRTS96
			#define DMABUF32RTS0I	2		// RTS0, I	// previous
			#define DMABUF32RTS1I	3		// RTS1, I	// current
			#define DMABUF32RTS0Q	6		// RTS0, Q	// previous
			#define DMABUF32RTS1Q	7		// RTS1, Q	// current
		#endif /* WITHRTS96 */
		
		#define DMABUFSTEP32TX	2		// 2 - каждому сэмплу соответствует два числа в DMA буфере	- I/Q
		#define DMABUF32TXI	0		// TX, I
		#define DMABUF32TXQ	1		// TX, Q

		#define DMABUFSTEP16	2		// 2 - каждому сэмплу соответствует два числа в DMA буфере

	#endif

#else /* WITHDSPEXTDDC */
	// buff data layout: ADC data/unused channel
	#define DMABUF32RX		0		// ADC data index
	#define DMABUFSTEP32RX	2		// 2 - каждому сэмплу соответствует два числа в DMA буфере
	#define DMABUF32RXI	0		// RX0, I
	#define DMABUF32RXQ	1		// RX0, Q

	#define DMABUFSTEP32TX	2		// 2 - каждому сэмплу соответствует два числа в DMA буфере	- I/Q
	#define DMABUF32TXI	0		// TX, I
	#define DMABUF32TXQ	1		// TX, Q

	#define DMABUFSTEP16	2		// 2 - каждому сэмплу соответствует два числа в DMA буфере

#endif /* WITHDSPEXTDDC */

#define DMABUFCLUSTER	8	// Cделано небольшое кодичество - чтобы не пропускало прерывания от валкодера при обработке звука
#define DMABUFCLUSTER	8	// Cделано небольшое кодичество - чтобы не пропускало прерывания от валкодера при обработке звука

#if ! WITHI2SHW

	// Конфигурации без аудиокодека
	// количество сэмплов в DMABUFFSIZE32RX и DMABUFFSIZE16 должно быть одинаковым.
	// Обеспечить вызовы функции buffers_resample(void)

	#define DMABUFFSIZE32RX (DMABUFCLUSTER * DMABUFSTEP32RX * 7)
	#define DMABUFFSIZE32TX (DMABUFCLUSTER * DMABUFSTEP32TX * 7 * 4)
	#define DMABUFFSIZE16 (DMABUFCLUSTER * DMABUFSTEP16 * 7)	/* размер под USB ENDPOINT PACKET SIZE В буфере помещаются пары значений - стерео кодек */

#else /* ! WITHI2SHW */
	#define DMABUFFSIZE32RX (DMABUFCLUSTER * DMABUFSTEP32RX)
	#define DMABUFFSIZE32TX (DMABUFCLUSTER * DMABUFSTEP32TX * 4)
	#define DMABUFFSIZE16 (DMABUFCLUSTER * DMABUFSTEP16 * 7)	/* размер под USB ENDPOINT PACKET SIZE В буфере помещаются пары значений - стерео кодек */
#endif /* ! WITHI2SHW */

// Параметры для канала передачи Real Time Spectrum - stereo, 32 bit, 192 kS/S
#define DMABUFSTEP192RTS 8	// 8: стерео по 32 бит, 6: стерео по 24 бит
#define DMABUFFSIZE192RTS (128/*288*/ * DMABUFSTEP192RTS)

#define DMABUFSTEP96RTS 6	// 6: стерео по 24 бит
#define DMABUFFSIZE96RTS 600 //((96 + 4) * DMABUFSTEP96RTS)		// 588 - должно быть кратно 4 байтам - для работы DMA в Renesas


#if WITHINTEGRATEDDSP

	#if 0 && CPUSTYLE_R7S721 && (__ARM_FP & 0x08)

		typedef double FLOAT_t;

		#define LOG10F	log10
		//#define LOGF	log
		//#define POWF	pow
		#define LOGF	local_log
		#define POWF	local_pow
		#define SINF	sin
		#define COSF	cos
		#define ATAN2F	atan2
		#define ATANF	atan
		//#define EXPF	exp
		#define EXPF	local_exp
		#define FABSF	fabs
		#define SQRTF	sqrt
		#define FMAXF	fmax
		#define FMINF	fmin
		#define LDEXPF	ldexp
		#define FREXPF	frexp
		
		#if defined (__ARM_FEATURE_FMA) || defined (FP_FAST_FMA)
			#define FMAF	fma
		#endif /* defined (__ARM_FEATURE_FMA) || defined (FP_FAST_FMA) */
		#define DSP_FLOAT_BITSMANTISSA 54

	#elif (__ARM_FP & 0x04)

		typedef float FLOAT_t;

		#define LOG10F	log10f
		//#define LOGF	logf
		//#define POWF	powf
		#define LOGF	local_log
		#define POWF	local_pow
		#define SINF	sinf
		#define COSF	cosf
		#define ATAN2F	atan2f
		#define ATANF	atanf
		//#define EXPF	expf
		#define EXPF	local_exp
		#define FABSF	fabsf
		#define SQRTF	sqrtf
		#define FMAXF	fmaxf
		#define FMINF	fminf
		#define LDEXPF	ldexpf
		#define FREXPF	frexpf

		#if defined (__ARM_FEATURE_FMA) || defined (FP_FAST_FMAF)
			#define FMAF	fmaf
		#endif /* defined (__ARM_FEATURE_FMA) || defined (FP_FAST_FMAF) */
		#define DSP_FLOAT_BITSMANTISSA 24

	#else

		#error This CPU not support floating point

	#endif

	/* для возможности работы с функциями сопроцессора NEON - vld1_f32 например */
	#define IV ivqv [0]
	#define QV ivqv [1]

	typedef struct
	{
		FLOAT_t ivqv [2];
	} FLOAT32P_t;

	typedef struct
	{
		int_fast32_t ivqv [2];
	} INT32P_t;

	void modem_demod_iq(FLOAT32P_t iq);
	FLOAT32P_t modem_get_tx_iq(uint_fast8_t suspend);
	uint_fast8_t getsampmlemike(INT32P_t * v);			/* получить очередной оцифрованый сэмпл с микрофона */

	FLOAT_t local_log(FLOAT_t x);
	FLOAT_t local_pow(FLOAT_t x, FLOAT_t y);

#endif /* WITHINTEGRATEDDSP */


// Buffers interface functions
void buffers_initialize(void);

uint_fast8_t processmodem(void);

uintptr_t allocate_dmabuffer32tx(void);
uintptr_t allocate_dmabuffer32rx(void);
void release_dmabuffer32tx(uintptr_t addr);

uintptr_t allocate_dmabuffer16(void);
void release_dmabuffer16(uintptr_t addr);

uintptr_t getfilled_dmabufferx(uint_fast16_t * sizep);	/* получить буфер одного из типов, которые могут использоваться для передаяи аудиоданных в компьютер по USB */
uintptr_t getfilled_dmabufferxrts(uint_fast16_t * sizep);	/* получить буфер одного из типов, которые могут использоваться для передаяи аудиоданных в компьютер по USB */
void release_dmabufferx(uintptr_t addr);	/* освободить буфер одного из типов, которые могут использоваться для передаяи аудиоданных в компьютер по USB */

void refreshDMA_uacin(void); // Канал DMA ещё занят - оставляем в очереди, иначе получить данные через getfilled_dmabufferx

uintptr_t getfilled_dmabuffer32tx_main(void);
uintptr_t getfilled_dmabuffer32tx_sub(void);
uintptr_t getfilled_dmabuffer16phones(void);

uintptr_t dma_flush16tx(uintptr_t addr);
uintptr_t dma_flushxrtstx(uintptr_t addr, unsigned long size);

void dsp_extbuffer32rx(const uint32_t * buff);	// RX

void processing_dmabuffer16rx(uintptr_t addr);	// обработать буфер после оцифровки AF ADC
void processing_dmabuffer16rxuac(uintptr_t addr);	// обработать буфер после приёма пакета с USB AUDIO
void processing_dmabuffer32rx(uintptr_t addr);
void processing_dmabuffer32rts(uintptr_t addr);

void savesamplerecord16SD(int_fast16_t ch0, int_fast16_t ch1); /* to SD CARD */
void savesamplerecord16uacin(int_fast16_t ch0, int_fast16_t ch1); /* to USB AUDIO */
unsigned takerecordbuffer(void * * dest);
void releaserecordbuffer(void * dest);

// Обслуживание модема
size_t takemodemtxbuffer(uint8_t * * dest);	// Буферы с данными для передачи через модем
size_t takemodemtxbuffer_low(uint8_t * * dest);	// Буферы с данными для передачи через модем
uint_fast8_t statusmodemtxbuffer(void);		// есть ли буферы для передачи
size_t takemodemrxbuffer(uint8_t * * dest);	// Буферы с принятымти через модем данными
size_t takemodembuffer(uint8_t * * dest);	// Буферы для заполнения данными
size_t takemodembuffer_low(uint8_t * * dest);	// Буферы для заполнения данными
void savemodemrxbuffer(uint8_t * dest, unsigned size_t);	// Готов буфер с принятыми данными
void savemodemrxbuffer_low(uint8_t * dest, unsigned size_t);	// Готов буфер с принятыми данными
void savemodemtxbuffer(uint8_t * dest, unsigned size_t);	// Готов буфер с данными для передачи
void releasemodembuffer(uint8_t * dest);
void releasemodembuffer_low(uint8_t * dest);

void savesampleout16stereo(int_fast16_t ch0, int_fast16_t ch1);
void savesampleout32stereo(int_fast32_t ch0, int_fast32_t ch1);
void savesampleout96stereo(int_fast32_t ch0, int_fast32_t ch1);
void savesampleout192stereo(int_fast32_t ch0, int_fast32_t ch1);

uint32_t allocate_dmabuffer192rts(void);

int get_lout16(void); // тестовые функции
int get_rout16(void);

void prog_dsplreg(void);
void prog_fltlreg(void);
void prog_codec1reg(void);
void prog_codecreg_update(void);		// услолвное обновление регистров аудио кодека
void prog_dsplreg_update(void);
void prog_fltlreg_update(void);
void board_dsp1regchanged(void);
void prog_codec1reg(void);


void board_set_trxpath(uint_fast8_t v);	/* Тракт, к которому относятся все последующие вызовы. При перередаяе используется индекс 0 */
void board_set_mikemute(uint_fast8_t v);	/* отключить микрофонный усилитель */
void board_set_mik1level(uint_fast16_t v);	/* усиление микрофонного усилителя */
void board_set_agcrate(uint_fast8_t v);	/* на n децибел изменения входного сигнала 1 дБ выходного. UINT8_MAX - "плоская" АРУ */
void board_set_agc_t1(uint_fast8_t v);	/* подстройка параметра АРУ */
void board_set_agc_t2(uint_fast8_t v);	/* подстройка параметра АРУ */
void board_set_agc_t4(uint_fast8_t v);	/* подстройка параметра АРУ */
void board_set_agc_thung(uint_fast8_t v);	/* подстройка параметра АРУ */
void board_set_nfm_sql_lelel(uint_fast8_t v);	/* уровень открывания шумоподавителя NFM */
void board_set_squelch(uint_fast8_t v);	/* уровень открывания шумоподавителя */
void board_set_nfm_sql_off(uint_fast8_t v);	/* отключение шумоподавителя NFM */
void board_set_notch_freq(uint_fast16_t n);	/* частота NOTCH фильтра */
void board_set_notch_width(uint_fast16_t n);	/* полоса NOTCH фильтра */
void board_set_notch_on(uint_fast8_t v);	/* включение NOTCH фильтра */
void board_set_cwedgetime(uint_fast8_t n);	/* Время нарастания/спада огибающей телеграфа при передаче - в 1 мс */
void board_set_sidetonelevel(uint_fast8_t n);	/* Уровень сигнала самоконтроля в процентах - 0%..100% */
void board_set_subtonelevel(uint_fast8_t n);	/* Уровень сигнала CTCSS в процентах - 0%..100% */
void board_set_amdepth(uint_fast8_t n);		/* Глубина модуляции в АМ - 0..100% */
void board_set_swapiq(uint_fast8_t v);	/* Поменять местами I и Q сэмплы в потоке RTS96 */
void board_set_swaprts(uint_fast8_t v);	/* если используется конвертор на Rafael Micro R820T - требуется инверсия спектра */
void buffers_set_uacinalt(uint_fast8_t v);	/* выбор альтернативной конфигурации для UAC IN interface */
void buffers_set_uacoutalt(uint_fast8_t v);	/* выбор альтернативной конфигурации для UAC OUT interface */
void buffers_set_uacinrtsalt(uint_fast8_t v);	/* выбор альтернативной конфигурации для UAC IN interface */
void board_set_lo6(int_fast32_t f);
void board_set_fullbw6(int_fast16_t f);	/* Установка частоты среза фильтров ПЧ в алгоритме Уивера - параметр полная полоса пропускания */

void board_set_aflowcutrx(int_fast16_t v);		/* Нижняя частота среза фильтра НЧ */
void board_set_afhighcutrx(int_fast16_t v);	/* Верхняя частота среза фильтра НЧ */
void board_set_aflowcuttx(int_fast16_t v);		/* Нижняя частота среза фильтра НЧ */
void board_set_afhighcuttx(int_fast16_t v);	/* Верхняя частота среза фильтра НЧ */

void board_set_afgain(uint_fast16_t v);	// Параметр для регулировки уровня на выходе аудио-ЦАП
void board_set_rfgain(uint_fast16_t v);	// Параметр для регулировки усиления ПЧ/ВЧ
void board_set_dspmode(uint_fast8_t v);	// Параметр для установки режима работы приёмника A/передатчика A
void board_set_lineinput(uint_fast8_t n);	// Включение line input вместо микрофона
void board_set_lineamp(uint_fast16_t v);	// Параметр для регулировки уровня на входе аудио-ЦАП при работе с LINE IN
void board_set_txaudio(uint_fast8_t v);	// Альтернативные источники сигнала при передаче
void board_set_mikebust20db(uint_fast8_t n);	// Включение предусилителя за микрофоном
void board_set_afmute(uint_fast8_t n);	// Отключение звука
void board_set_mikeequal(uint_fast8_t n);	// включение обработки сигнала с микрофона (эффекты, эквалайзер, ...)
void board_set_mikeequalparams(const uint_fast8_t * p);	// Эквалайзер 80Hz 230Hz 650Hz 	1.8kHz 5.3kHz
void board_set_mikeagc(uint_fast8_t n);		/* Включение программной АРУ перед модулятором */
void board_set_mikeagcscale(uint_fast8_t v);	/* На какую часть (в процентах) от полной амплитуды настроена АРУ микрофона */
void board_set_mikeagcgain(uint_fast8_t v);	/* Максимальное усидение АРУ микрофона */
void board_set_afresponcerx(int_fast8_t v);	/* изменение тембра звука в канале приемника */
void board_set_afresponcetx(int_fast8_t v);	/* изменение тембра звука в канале передатчика */
void board_set_mikehclip(uint_fast8_t gmikehclip);	/* Ограничитель */

void board_set_uacplayer(uint_fast8_t v);	/* режим прослушивания выхода компьютера в наушниках трансивера - отладочный режим */
void board_set_uacmike(uint_fast8_t v);	/* на вход трансивера берутся аудиоданные с USB виртуальной платы, а не с микрофона */

void dsp_initialize(void);

#if WITHINTEGRATEDDSP
	// Копрование информации о спектре с текущую строку буфера 
	// wfarray (преобразование к пикселям растра */
	void dsp_getspectrumrow(
		FLOAT_t * const hbase,
		uint_fast16_t dx	// pixel X width of display window
		);
	int dsp_mag2y(FLOAT_t mag, uint_fast16_t dy);	// Нормирование уровня сигнала к шкале
#endif /* WITHINTEGRATEDDSP */

int_fast32_t dsp_get_ifreq(void);		/* Получить значение входной ПЧ для обработки DSP */
int_fast32_t dsp_get_sampleraterx(void);	/* Получить значение частоты выборок выходного потока DSP */
int_fast32_t dsp_get_sampleraterxscaled(uint_fast8_t scale);	/* Получить значение частоты выборок выходного потока DSP */
int_fast32_t dsp_get_sampleratetx(void);	/* Получить значение частоты выборок входного потока DSP */
int_fast32_t dsp_get_samplerate100(void);	/* Получить значение частоты выборок выходного потока DSP */

int_fast32_t dsp_get_samplerateuacin_audio48(void);		// UAC IN samplerate
int_fast32_t dsp_get_samplerateuacin_rts96(void);		// UAC IN samplerate
int_fast32_t dsp_get_samplerateuacin_rts192(void);		// UAC IN samplerate
int_fast32_t dsp_get_samplerateuacout(void);	// UAC OUT samplerate

uint_fast8_t dsp_getsmeter(uint_fast8_t * tracemax, uint_fast8_t lower, uint_fast8_t upper, uint_fast8_t clean);	/* получить значение от АЦП s-метра */
uint_fast8_t dsp_getvox(void);	/* получить значение от детектора VOX */
uint_fast8_t dsp_getavox(void);	/* получить значение от детектора Anti-VOX */
uint_fast8_t dsp_getfreqdelta10(int_fast32_t * p, uint_fast8_t pathi);	/* Получить значение отклонения частоты с точностью 0.1 герца */

void dsp_speed_diagnostics(void);	/* DSP speed test */
void buffers_diagnostics(void);
void usbd_showstate(void);
void usbd_diagnostics(void);
void dtmftest(void);

void modem_initialze(void);
void modem_set_speed(uint_fast32_t speed100);	/* Установить скорость, параметр с точностью 1/100 бод */
void modem_set_mode(uint_fast8_t mode);			/* Установить модуляцию для модема */
void modem_rxpump(void);						// вызывается из user-mode программы
uint_fast8_t modem_get_ptt(void);


/* Интерфейс к AF кодеку */
typedef struct codec1if_tag
{
	void (* initialize)(void);
	void (* setvolume)(uint_fast16_t gain, uint_fast8_t mute, uint_fast8_t mutespk);	/* Установка громкости на наушники */
	void (* setlineinput)(uint_fast8_t linein, uint_fast8_t mikebust20db, uint_fast16_t mikegain, uint_fast16_t linegain);	/* Выбор LINE IN как источника для АЦП вместо микрофона */
	void (* setprocparams)(uint_fast8_t procenable, const uint_fast8_t * gains);	/* параметры обработки звука с микрофона (эхо, эквалайзер, ...) */
	const char * label;									/* Название кодека (всегда последний элемент в структуре) */
} codec1if_t;

const codec1if_t * board_getaudiocodecif(void);		// получить интерфейс управления кодеком в сторону оператора

/* Интерфейс к IF кодеку */
typedef struct codec2if_tag
{
	void (* initialize)(void);
	const char * label;
} codec2if_t;

const codec2if_t * board_getfpgacodecif(void);		// получить интерфейс управления кодеком для получения данных о радиосигнале

/* +++ UAC OUT data save */
void uacout_buffer_initialize(void);
void uacout_buffer_start(void);
void uacout_buffer_stop(void);
void uacout_buffer_save(const uint8_t * buff, uint_fast16_t size);


#define USBALIGN_BEGIN __attribute__ ((aligned (32)))
#define USBALIGN_END /* nothing */

#if WITHRTS96

	// stereo, 24 bit samples
	#define HARDWARE_USBD_AUDIO_IN_SAMPLEBITS_RTS96		24
	#define HARDWARE_USBD_AUDIO_IN_CHANNELS_RTS		2
	#define VIRTUAL_AUDIO_PORT_DATA_SIZE_IN_RTS96		(DMABUFFSIZE96RTS * sizeof (uint8_t))

	#define HSINTERVAL_RTS96 4	// endpoint descriptor parameters
	#define FSINTERVAL_RTS96 1

#endif /* WITHRTS96 */
#if WITHRTS192

	// По каналу real-time спектра стерео, 32 бит, 192 кГц - 288*2*4 = 2304 байта
	// stereo, 32 bit samples
	#define HARDWARE_USBD_AUDIO_IN_SAMPLEBITS_RTS192	32
	#define HARDWARE_USBD_AUDIO_IN_CHANNELS_RTS		2
	#define VIRTUAL_AUDIO_PORT_DATA_SIZE_IN_RTS192		(DMABUFFSIZE192RTS * sizeof (int8_t))

	#define HSINTERVAL_RTS192 3	// 500 us
	#define FSINTERVAL_RTS192 1

#endif /* WITHRTS192 */

// stereo, 16 bit samples
// По звуковому каналу передается стерео, 16 бит, 48 кГц - 288 байт размер данных в ендпонтт
#define HARDWARE_USBD_AUDIO_IN_SAMPLEBITS_AUDIO48	16
#define HARDWARE_USBD_AUDIO_IN_CHANNELS_AUDIO48		2
#define VIRTUAL_AUDIO_PORT_DATA_SIZE_IN_AUDIO48		(DMABUFFSIZE16 * sizeof (uint16_t))

#define HSINTERVAL_AUDIO48 4	// endpoint descriptor parameters - для обеспечсения 1 кГц периода
#define FSINTERVAL_AUDIO48 1

#define HARDWARE_USBD_AUDIO_OUT_SAMPLEBITS	16
#if WITHUABUACOUTAUDIO48MONO
	#define HARDWARE_USBD_AUDIO_OUT_CHANNELS	1
#else /* WITHUABUACOUTAUDIO48MONO */
	#define HARDWARE_USBD_AUDIO_OUT_CHANNELS	2
#endif /* WITHUABUACOUTAUDIO48MONO */

// используются свои буферы
#define VIRTUAL_AUDIO_PORT_DATA_SIZE_OUT	( \
	((ARMI2SRATE100 + 99) / 100000) * \
	((HARDWARE_USBD_AUDIO_OUT_SAMPLEBITS * HARDWARE_USBD_AUDIO_OUT_CHANNELS + 7) / 8) \
	)

#define HARDWARE_USBD_AUDIO_IN_CHANNELS	2	/* для всех каналов в IN направлении */

#endif /* AUDIO_H_INCLUDED */
