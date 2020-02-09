/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#ifndef AUDIO_H_INCLUDED
#define AUDIO_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

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
			#define DMABUF32RTS0I	2		// RTS0, I	// previous - oldest
			#define DMABUF32RTS0Q	6		// RTS0, Q	// previous
			#define DMABUF32RTS1I	3		// RTS1, I	// current	- nevest
			#define DMABUF32RTS1Q	7		// RTS1, Q	// current
		#endif /* WITHRTS96 */

		// Slot S0, S4: Oldest sample (T-3)
		// Slot S1, S5: Old sample (T-2)
		// Slot S2, S6: Old sample (T-1)
		// Slot S3, S7: Newest sample (T-0)
		#define DMABUF32RXWFM0I	0		// WFM OLDEST
		#define DMABUF32RXWFM0Q	4		// WFM
		#define DMABUF32RXWFM1I	1		// WFM
		#define DMABUF32RXWFM1Q	5		// WFM
		#define DMABUF32RXWFM2I	2		// WFM
		#define DMABUF32RXWFM2Q	6		// WFM
		#define DMABUF32RXWFM3I	3		// WFM NEWEST
		#define DMABUF32RXWFM3Q	7		// WFM

		#define DMABUFSTEP16	2		// 2 - каждому сэмплу при обмене с AUDIO CODEC соответствует два числа в DMA буфере

	#elif CPUSTYLE_STM32F || CPUSTYLE_STM32MP1

		// buff data layout: I main/I sub/Q main/Q sub
		#define DMABUFSTEP32RX	8		// Каждому сэмплу соответствует восемь чисел в DMA буфере

		#define DMABUF32RX0I	0		// RX0, I
		#define DMABUF32RX1I	1		// RX1, I
		#define DMABUF32RX0Q	4		// RX0, Q
		#define DMABUF32RX1Q	5		// RX1, Q

		#if WITHRTS96
			#define DMABUF32RTS0I	2		// RTS0, I	// previous - oldest
			#define DMABUF32RTS0Q	6		// RTS0, Q	// previous
			#define DMABUF32RTS1I	3		// RTS1, I	// current	- nevest
			#define DMABUF32RTS1Q	7		// RTS1, Q	// current
		#endif /* WITHRTS96 */

		// Slot S0, S4: Oldest sample (T-3)
		// Slot S1, S5: Old sample (T-2)
		// Slot S2, S6: Old sample (T-1)
		// Slot S3, S7: Newest sample (T-0)
		#define DMABUF32RXWFM0I	0		// WFM OLDEST
		#define DMABUF32RXWFM0Q	4		// WFM
		#define DMABUF32RXWFM1I	1		// WFM
		#define DMABUF32RXWFM1Q	5		// WFM
		#define DMABUF32RXWFM2I	2		// WFM
		#define DMABUF32RXWFM2Q	6		// WFM
		#define DMABUF32RXWFM3I	3		// WFM NEWEST
		#define DMABUF32RXWFM3Q	7		// WFM
		
		#define DMABUFSTEP32TX	2		// 2 - каждому сэмплу соответствует два числа в DMA буфере	- I/Q
		#define DMABUF32TXI	0		// TX, I
		#define DMABUF32TXQ	1		// TX, Q

		#define DMABUFSTEP16	2		// 2 - каждому сэмплу при обмене с AUDIO CODEC соответствует два числа в DMA буфере

	#endif

#else /* WITHDSPEXTDDC */
	// buff data layout: ADC data/unused channel
	#define DMABUF32RX		0		// ADC data index
	#define DMABUFSTEP32RX	(WITHSAI1_FRAMEBITS / 32) //2		// 2 - каждому сэмплу соответствует два числа в DMA буфере
	#define DMABUF32RXI	0		// RX0, I
	#define DMABUF32RXQ	1		// RX0, Q

	#define DMABUFSTEP32TX	2		// 2 - каждому сэмплу соответствует два числа в DMA буфере	- I/Q
	#define DMABUF32TXI	0		// TX, I
	#define DMABUF32TXQ	1		// TX, Q

	#define DMABUFSTEP16	2		// 2 - каждому сэмплу при обмене с AUDIO CODEC соответствует два числа в DMA буфере

#endif /* WITHDSPEXTDDC */


// Требования по кратности размера буфера для передачи по USB DMA
#if CPUSTYLE_R7S721
	#define HARDWARE_RTSDMABYTES	4
#else /* CPUSTYLE_R7S721 */
	#define HARDWARE_RTSDMABYTES	1
#endif /* CPUSTYLE_R7S721 */

#if WITHUAC2
	#define WITHUSENOFU_IN48 			0	// 1 - без использования Feature Unit, 0 - с использованием, игнорирование управления громкостью
	#define WITHUSENOFU_INRTS 			0	// 1 - без использования Feature Unit, 0 - с использованием, игнорирование управления громкостью
	#define WITHUSENOFU_IN48_INRTS 		0	// 1 - без использования Feature Unit, 0 - с использованием, игнорирование управления громкостью
	#define WITHUSENOFU_OUT48 			0	// 1 - без использования Feature Unit, 0 - с использованием, игнорирование управления громкостью
#else /* WITHUAC2 */
	#define WITHUSENOFU_IN48 			1	// 1 - без использования Feature Unit, 0 - с использованием, игнорирование управления громкостью
	#define WITHUSENOFU_INRTS 			1	// 1 - без использования Feature Unit, 0 - с использованием, игнорирование управления громкостью
	#define WITHUSENOFU_IN48_INRTS 		1	// 1 - без использования Feature Unit, 0 - с использованием, игнорирование управления громкостью
	#define WITHUSENOFU_OUT48 			1	// 1 - без использования Feature Unit, 0 - с использованием, игнорирование управления громкостью
#endif /* WITHUAC2 */

// Конфигурация потоков в Input Terminal Descriptor
// bNrChannels в 4.3.2.1 Input Terminal Descriptor образуется подсчетом битов в данном поле
// Может быть использовано AUDIO_CHANNEL_M
#define UACIN_CONFIG_IN48 			(AUDIO_CHANNEL_L | AUDIO_CHANNEL_R)
#define UACIN_CONFIG_INRTS 			(AUDIO_CHANNEL_L | AUDIO_CHANNEL_R)
#define UACIN_CONFIG_IN48_INRTS 	(AUDIO_CHANNEL_L | AUDIO_CHANNEL_R)
#define UACOUT_CONFIG_OUT48 		(AUDIO_CHANNEL_L | AUDIO_CHANNEL_R)

// IN/OUT path topology parameters
#define UAC2_IN_bNrChannels 2	//UAC_count_channels(wChannelConfig); 1: Only master channel controls, 3: master, left and right
#define UAC2_OUT_bNrChannels 2	//UAC_count_channels(wChannelConfig); 1: Only master channel controls, 3: master, left and right

// количество каналов в дескрипторах формата потока
#define UACIN_FMT_CHANNELS_AUDIO48			2
#define UACIN_FMT_CHANNELS_RTS				2	// I/Q всегда стерео
#define UACIN_FMT_CHANNELS_AUDIO48_RTS		2	// при совмещении аудио и I/Q всегда стерео

#if WITHUABUACOUTAUDIO48MONO
	// количество каналов в дескрипторах формата потока
	#define UACOUT_FMT_CHANNELS_AUDIO48	1
#else /* WITHUABUACOUTAUDIO48MONO */
	// количество каналов в дескрипторах формата потока
	#define UACOUT_FMT_CHANNELS_AUDIO48	2
#endif /* WITHUABUACOUTAUDIO48MONO */

// коррекция размера с учетом требуемого выравнивания
#define DMAHWEPADJUST(sz, granulation) (((sz) + ((granulation) - 1)) / (granulation) * (granulation))

/* константы. С запасом чтобы работало и при тактовой 125 МГц на FPGA при децимации 2560 = 48.828125 kHz sample rate */
//#define MSOUTSAMPLES	49 /* количество сэмплов за милисекунду в UAC OUT */
// без запаса - только для 48000
#define MSOUTSAMPLES	48 /* количество сэмплов за милисекунду в UAC OUT */
#define MSINSAMPLES		(MSOUTSAMPLES + 1) /* количество сэмплов за милисекунду в UAC IN */


#define DMABUFSTEPUACIN16	(UACIN_FMT_CHANNELS_AUDIO48)		// 2 - каждому сэмплу соответствует два числа в  буфере для выдачи по USB в host

#define DMABUFFSIZEUACIN16 (MSINSAMPLES * DMABUFSTEPUACIN16)	/* размер под USB ENDPOINT PACKET SIZE В буфере помещаются пары значений - стерео кодек */

/* если приоритет прерываний USB не выше чем у аудиобработки - она должна длиться не более 1 мс (WITHRTS192 - 0.5 ms) */
#define DMABUFCLUSTER	19	// Прерывания по приему от IF CODEC или FPGA RX должны происходить не реже 1 раз в милисекунду (чтобы USB работать могло) */

#define DMABUFFSIZE16	(DMABUFCLUSTER * DMABUFSTEP16 * 4)		/* AF CODEC */
#define DMABUFFSIZE32RX (DMABUFCLUSTER * DMABUFSTEP32RX)		/* FPGA RX or IF CODEC RX */
#define DMABUFFSIZE32TX (DMABUFCLUSTER * DMABUFSTEP32TX * 4)	/* FPGA TX or IF CODEC TX	*/

// Параметры для канала передачи Real Time Spectrum - stereo, 32 bit, 192 kS/S
#define DMABUFSTEP192RTS 8	// 8: стерео по 32 бит, 6: стерео по 24 бит
#define DMABUFFSIZE192RTS_AJ (128/*288*/ * DMABUFSTEP192RTS)

#define DMABUFFSIZE192RTS DMAHWEPADJUST(DMABUFFSIZE192RTS_AJ, DMABUFSTEP192RTS * HARDWARE_RTSDMABYTES)

#define DMABUFSTEP96RTS 6	// 6: стерео по 24 бит
#define DMABUFFSIZE96RTS_AJ ((MSOUTSAMPLES * 2 + 1) * DMABUFSTEP96RTS) //((96 + 4) * DMABUFSTEP96RTS)		// 588 - должно быть кратно 4 байтам - для работы DMA в Renesas

#define DMABUFFSIZE96RTS DMAHWEPADJUST(DMABUFFSIZE96RTS_AJ, DMABUFSTEP96RTS * HARDWARE_RTSDMABYTES)

#if WITHRTS96

	// stereo, 24 bit samples
	#define HARDWARE_USBD_AUDIO_IN_SAMPLEBITS_RTS96		24
	#define VIRTUAL_AUDIO_PORT_DATA_SIZE_IN_RTS96		(DMABUFFSIZE96RTS * sizeof (uint8_t))

	#define HSINTERVAL_RTS96 4	// endpoint descriptor parameters
	#define FSINTERVAL_RTS96 1

#endif /* WITHRTS96 */
#if WITHRTS192

	// По каналу real-time спектра стерео, 32 бит, 192 кГц - 288*2*4 = 2304 байта
	// stereo, 32 bit samples
	#define HARDWARE_USBD_AUDIO_IN_SAMPLEBITS_RTS192	32
	#define VIRTUAL_AUDIO_PORT_DATA_SIZE_IN_RTS192		(DMABUFFSIZE192RTS * sizeof (int8_t))

	#define HSINTERVAL_RTS192 3	// 500 us
	#define FSINTERVAL_RTS192 1

#endif /* WITHRTS192 */


// stereo, 16 bit samples
// По звуковому каналу передается стерео, 16 бит, 48 кГц - 288 байт размер данных в ендпонтт
#define HARDWARE_USBD_AUDIO_IN_SAMPLEBITS_AUDIO48	16
#define UAC_IN48_DATA_SIZE (DMABUFFSIZEUACIN16 * sizeof (uint16_t))



/*
	For full-/high-speed isochronous endpoints, this value
	must be in the range from 1 to 16. The bInterval value
	is used as the exponent for a 2^(bInterval-1) value; e.g.,
	a bInterval of 4 means a period of 8 (2^(4-1))."

  */
#define HSINTERVAL_AUDIO48 4	// endpoint descriptor parameters - для обеспечения 1 кГц периода
#define FSINTERVAL_AUDIO48 1

#define HSINTERVAL_8MS 7	// endpoint descriptor parameters - для обеспечения 10 ms периода
#define FSINTERVAL_8MS 8

#define HSINTERVAL_32MS 9	// endpoint descriptor parameters - для обеспечения 32 ms периода
#define FSINTERVAL_32MS 32

#define HSINTERVAL_256MS 12	// endpoint descriptor parameters - для обеспечения 255 ms периода (interrupt endpoint for CDC)
#define FSINTERVAL_255MS 255


#define UACOUT_AUDIO48_SAMPLEBITS	16

// буфер приема потока данных от USB к модуоятору
#define UAC_OUT48_DATA_SIZE	( \
	MSOUTSAMPLES * \
	((UACOUT_AUDIO48_SAMPLEBITS * UACOUT_FMT_CHANNELS_AUDIO48 + 7) / 8) \
	)


//#if WITHINTEGRATEDDSP

	#if (__ARM_FP & 0x08) && 0

		typedef double FLOAT_t;

		#define FLOORF	floor
		#define LOG10F	local_log10 //log10
		//#define LOGF	log
		//#define POWF	pow
		//#define LOG2F	log2
		#define LOGF	local_log
		#define POWF	local_pow
		#define LOG2F	local_log2
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

	#elif 1 //(__ARM_FP & 0x04)

		typedef float FLOAT_t;

		#define FLOORF	floorf
		#define LOG10F	local_log10 //log10f
		//#define LOGF	logf
		//#define POWF	powf
		//#define LOG2F	log2f
		#define LOGF	local_log
		#define POWF	local_pow
		#define LOG2F	local_log2
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


/* from "C Language Algorithms for Digital Signal Processing"
   by Paul M. Embree and Bruce Kimble, Prentice Hall, 1991 */


FLOAT_t local_exp(FLOAT_t x);
FLOAT_t local_pow(FLOAT_t x, FLOAT_t y);
FLOAT_t local_log(FLOAT_t x);
FLOAT_t local_log10(FLOAT_t X);
	
struct Complex
{
	FLOAT_t real;
	FLOAT_t imag;
};

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


#if WITHNOSPEEX
	#define FIRBUFSIZE 1024	/* это не порядок фильтра, просто размер буфера при передачи данных к user mode обработчику */

#else /* WITHNOSPEEX */
	#define FIRBUFSIZE SPEEXNN

#endif /* WITHNOSPEEX */

// Ограничение алгоритма генерации параметров фильтра - нечётное значение Ntap.
// Кроме того, для функций фильтрации с использованием симметрии коэффициентов, требуется кратность 2 половины Ntap

#define NtapValidate(n)	((unsigned) (n) / 8 * 8 + 1)
#define NtapCoeffs(n)	((unsigned) (n) / 2 + 1)

#if WITHDSPLOCALFIR
	/* Фильтрация квадратур осуществляется процессором */
	#define	Ntap_rx_AUDIO	NtapValidate(241)

#else /* WITHDSPLOCALFIR */
	#define	Ntap_rx_AUDIO	NtapValidate(511)
	#define Ntap_tx_MIKE	NtapValidate(241)
	#if WITHNOSPEEX
		////#define	Ntap_rx_AUDIO	NtapValidate(511)
		////#define Ntap_tx_MIKE	NtapValidate(241)

	#else /* WITHNOSPEEX */
		////#define	Ntap_rx_AUDIO	NtapValidate(SPEEXNN * 2 - 7)
		////#define Ntap_tx_MIKE	NtapValidate(241) //Ntap_rx_AUDIO

	#endif /* ! WITHDSPLOCALFIR */

#endif /* WITHDSPLOCALFIR */

#if WITHDSPEXTFIR || WITHDSPEXTDDC
	// Параметры фильтров в случае использования FPGA с фильтром на квадратурных каналах
	//#define Ntap_trxi_IQ		1535	// Фильтр в FPGA (1024+512-1)
	#define Ntap_trxi_IQ		1023	// Фильтр в FPGA
	#define HARDWARE_COEFWIDTH	24		// Разрядность коэффициентов. format is S0.22
	#define HARDWARE_DACSCALE	(0.82)	// на сколько уменьшаем от возможного выходной код для предотвращения переполнения выходлного сумматора
#else
	#define HARDWARE_DACSCALE	(1)	// на сколько уменьшаем от возможного выходной код для предотвращения переполнения выходлного сумматора

#endif /* WITHDSPEXTFIR || WITHDSPEXTDDC */

#if WITHDSPLOCALFIR
	/* Фильтрация квадратур осуществляется процессором */
	#if CPUSTYLE_R7S721
		#define Ntap_rx_SSB_IQ	NtapValidate(241)	// SSB/CW filters: complex numbers, floating-point implementation
		#define Ntap_tx_SSB_IQ	NtapValidate(241)	// SSB/CW TX filter: complex numbers, floating-point implementation
		#define Ntap_tx_MIKE	NtapValidate(105)	// single samples, floating point implementation

	#elif CPUSTYLE_STM32F7XX
		#define Ntap_rx_SSB_IQ	NtapValidate(241)	// SSB/CW filters: complex numbers, floating-point implementation
		#define Ntap_tx_SSB_IQ	NtapValidate(241)	// SSB/CW TX filter: complex numbers, floating-point implementation
		#define Ntap_tx_MIKE	NtapValidate(105)	// single samples, floating point implementation

	#else
		#define Ntap_rx_SSB_IQ	NtapValidate(181)	// SSB/CW filters: complex numbers, floating-point implementation
		#define Ntap_tx_SSB_IQ	NtapValidate(181)	// SSB/CW TX filter: complex numbers, floating-point implementation
		#define Ntap_tx_MIKE	NtapValidate(105)	// single samples, floating point implementation

	#endif

#endif /* WITHDSPLOCALFIR */

uint_fast8_t modem_getnextbit(
	uint_fast8_t suspend	// передавать модему ещё рано - не полностью завершено формирование огибающей
	 );
// обработка бита на приёме
void
modem_frames_decode(
	uint_fast8_t v
	);

uint_fast8_t getsampmlemike(INT32P_t * v);			/* получить очередной оцифрованый сэмпл с микрофона */
uint_fast8_t getsampmlemoni(INT32P_t * v);			/* получить очередной сэмпл для самоконтроля */

FLOAT_t local_log(FLOAT_t x);
FLOAT_t local_pow(FLOAT_t x, FLOAT_t y);

//#endif /* WITHINTEGRATEDDSP */


// Buffers interface functions
void buffers_initialize(void);

uint_fast8_t processmodem(void);

uintptr_t allocate_dmabuffer32tx(void);
uintptr_t allocate_dmabuffer32rx(void);
void release_dmabuffer32tx(uintptr_t addr);

uintptr_t allocate_dmabuffer16(void);
void release_dmabuffer16(uintptr_t addr);

uintptr_t getfilled_dmabufferx(uint_fast16_t * sizep);	/* получить буфер одного из типов, которые могут использоваться для передаяи аудиоданных в компьютер по USB */
void release_dmabufferx(uintptr_t addr);	/* освободить буфер одного из типов, которые могут использоваться для передаяи аудиоданных в компьютер по USB */
// WITHUSBUACIN2 specific
uintptr_t getfilled_dmabufferxrts(uint_fast16_t * sizep);	/* получить буфер одного из типов, которые могут использоваться для передаяи аудиоданных в компьютер по USB */
void release_dmabufferxrts(uintptr_t addr);	/* освободить буфер одного из типов, которые могут использоваться для передаяи аудиоданных в компьютер по USB */

void refreshDMA_uacin(void); // Канал DMA ещё занят - оставляем в очереди, иначе получить данные через getfilled_dmabufferx

uintptr_t getfilled_dmabuffer32tx_main(void);
uintptr_t getfilled_dmabuffer32tx_sub(void);
uintptr_t getfilled_dmabuffer16phones(void);

void dsp_extbuffer32rx(const int32_t * buff);	// RX
void dsp_extbuffer32wfm(const int32_t * buff);	// RX
void dsp_addsidetone(int16_t * buff);			// перед передачей по DMA в аудиокодек

void processing_dmabuffer16rx(uintptr_t addr);	// обработать буфер после оцифровки AF ADC
void processing_dmabuffer16rxuac(uintptr_t addr);	// обработать буфер после приёма пакета с USB AUDIO
void processing_dmabuffer32rx(uintptr_t addr);
void processing_dmabuffer32rts(uintptr_t addr);
void processing_dmabuffer32wfm(uintptr_t addr);

void savesamplerecord16SD(int_fast16_t ch0, int_fast16_t ch1); /* to SD CARD */
void savesamplerecord16uacin(int_fast16_t ch0, int_fast16_t ch1); /* to USB AUDIO */
unsigned takerecordbuffer(void * * dest);
void releaserecordbuffer(void * dest);
/* data to play */
unsigned savesamplesplay_user(
	const void * buff,
	unsigned length
	);
void spoolplayfile(void);
void playwavfile(const char * filename);
uint_fast8_t isplayfile(void);
void playwavstop(void);

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

void savesampleout16stereo_user(int_fast32_t ch0, int_fast32_t ch1);
void savesampleout16stereo(int_fast32_t ch0, int_fast32_t ch1);
void savesampleout32stereo(int_fast32_t ch0, int_fast32_t ch1);
void savesampleout96stereo(int_fast32_t ch0, int_fast32_t ch1);
void savesampleout192stereo(int_fast32_t ch0, int_fast32_t ch1);
void savemoni16stereo(int_fast32_t ch0, int_fast32_t ch1);

#if WITHINTEGRATEDDSP
	#include "speex\arch.h"
	#include "speex\speex_preprocess.h"

	#if WITHNOSPEEX
		typedef float32_t speexel_t;

	#else /* WITHNOSPEEX */
		typedef float32_t speexel_t;

	#endif /* WITHNOSPEEX */
	uint_fast8_t takespeexready_user(speexel_t * * dest);
	void releasespeexbuffer_user(speexel_t * t);
	void savesampleout16tospeex(speexel_t ch0, speexel_t ch1);
#endif /* WITHINTEGRATEDDSP */


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
void board_set_squelch(uint_fast8_t v);	/* уровень открывания шумоподавителя */
void board_set_notch_freq(uint_fast16_t n);	/* частота NOTCH фильтра */
void board_set_notch_width(uint_fast16_t n);	/* полоса NOTCH фильтра */
void board_set_notch_on(uint_fast8_t v);	/* включение NOTCH фильтра */
void board_set_cwedgetime(uint_fast8_t n);	/* Время нарастания/спада огибающей телеграфа при передаче - в 1 мс */
void board_set_sidetonelevel(uint_fast8_t n);	/* Уровень сигнала самоконтроля в процентах - 0%..100% */
void board_set_monilevel(uint_fast8_t n);	/* Уровень сигнала самопрослушивания в процентах - 0%..100% */
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
void board_set_nfmdeviation100(uint_fast8_t v);	/* Девиация в NFM (сотни герц) */

void board_set_afgain(uint_fast16_t v);	// Параметр для регулировки уровня на выходе аудио-ЦАП
void board_set_ifgain(uint_fast16_t v);	// Параметр для регулировки усиления ПЧ/ВЧ
void board_set_dspmode(uint_fast8_t v);	// Параметр для установки режима работы приёмника A/передатчика A
void board_set_lineinput(uint_fast8_t n);	// Включение line input вместо микрофона
void board_set_lineamp(uint_fast16_t v);	// Параметр для регулировки уровня на входе аудио-ЦАП при работе с LINE IN
void board_set_txaudio(uint_fast8_t v);	// Альтернативные источники сигнала при передаче
void board_set_mikebust20db(uint_fast8_t n);	// Включение предусилителя за микрофоном
void board_set_afmute(uint_fast8_t n);	// Отключение звука
void board_set_mikeequal(uint_fast8_t n);	// включение обработки сигнала с микрофона (эффекты, эквалайзер, ...)
void board_set_mikeequalparams(const uint_fast8_t * p);	// Эквалайзер 80Hz 230Hz 650Hz 	1.8kHz 5.3kHz
void board_set_mikeagc(uint_fast8_t n);		/* Включение программной АРУ перед модулятором */
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
	uint_fast8_t dsp_getspectrumrow(
		FLOAT_t * const hbase,
		uint_fast16_t dx,	// pixel X width (pixels) of display window
		uint_fast8_t zoompow2	// horisontal magnification power of two
		);
	// Нормирование уровня сигнала к шкале
	// возвращает значения от 0 до ymax включительно
	// 0 - минимальный сигнал, ymax - максимальный
	int dsp_mag2y(FLOAT_t mag, int ymax, int_fast16_t topdb, int_fast16_t bottomdb);

	void saveIQRTSxx(FLOAT_t iv, FLOAT_t qv);	// формирование отображения спектра

#endif /* WITHINTEGRATEDDSP */

int_fast32_t dsp_get_ifreq(void);		/* Получить значение входной ПЧ для обработки DSP */
int_fast32_t dsp_get_sampleraterx(void);	/* Получить значение частоты выборок выходного потока DSP */
int_fast32_t dsp_get_sampleraterxscaled(uint_fast8_t scale);	/* Получить значение частоты выборок выходного потока DSP */
int_fast32_t dsp_get_sampleratetx(void);	/* Получить значение частоты выборок входного потока DSP */
int_fast32_t dsp_get_samplerate100(void);	/* Получить значение частоты выборок выходного потока DSP */

int_fast32_t dsp_get_samplerateuacin_audio48(void);		// UAC IN samplerate
int_fast32_t dsp_get_samplerateuacin_RTS96(void);		// UAC IN samplerate
int_fast32_t dsp_get_samplerateuacin_RTS192(void);		// UAC IN samplerate
int_fast32_t dsp_get_samplerateuacin_rts(void);			// RTS samplerate
int_fast32_t dsp_get_samplerateuacout(void);			// UAC OUT samplerate

uint_fast8_t dsp_getsmeter(uint_fast8_t * tracemax, uint_fast8_t lower, uint_fast8_t upper, uint_fast8_t clean);	/* получить значение от АЦП s-метра */
uint_fast8_t dsp_getvox(uint_fast8_t fullscale);	/* получить значение от детектора VOX */
uint_fast8_t dsp_getavox(uint_fast8_t fullscale);	/* получить значение от детектора Anti-VOX */
uint_fast8_t dsp_getfreqdelta10(int_fast32_t * p, uint_fast8_t pathi);	/* Получить значение отклонения частоты с точностью 0.1 герца */
uint_fast8_t dsp_getmikeadcoverflow(void); /* получения признака переполнения АЦП микрофонного тракта */

void dsp_speed_diagnostics(void);	/* DSP speed test */
void buffers_diagnostics(void);
void dtmftest(void);
void dsp_recalceq(uint_fast8_t pathi, float * frame);	// for SPEEX - equalizer in frequency domain
void dsp_recalceq_coeffs(uint_fast8_t pathi, float * dCoeff, int iCoefNum);	// calculate 1/2 of coefficients
void fir_expand_symmetric(FLOAT_t * dCoeff, int Ntap);			// Duplicate symmetrical part of coeffs.

void modem_initialze(void);
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
void uacout_buffer_save_system(const uint8_t * buff, uint_fast16_t size);
void uacout_buffer_save_realtime(const uint8_t * buff, uint_fast16_t size);

/* Получение пары (левый и правый) сжмплов для воспроизведения через аудиовыход трансивера.
 * Возврат 0, если нет ничего для воспроизведения.
 */
uint_fast8_t takewavsample(INT32P_t * rv, uint_fast8_t suspend);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* AUDIO_H_INCLUDED */
