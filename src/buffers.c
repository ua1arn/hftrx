/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
#include "hardware.h"	/* зависящие от процессора функции работы с портами */
#include "formats.h"	// for debug prints

//#define WITHBUFFERSDEBUG WITHDEBUG
#define BUFOVERSIZE 1

#if WITHINTEGRATEDDSP

#include "audio.h"
#include "buffers.h"

#if WITHUSBHW
#include "usb/usb200.h"
#include "usb/usbch9.h"
#endif /* WITHUSBHW */

//////////////////////////////////
// Система буферизации аудиоданных
//

enum { CNT16RX = DMABUFFSIZE16RX / DMABUFFSTEP16RX };
enum { CNT16TX = DMABUFFSIZE16TX / DMABUFFSTEP16TX };
enum { CNT32RX = DMABUFFSIZE32RX / DMABUFFSTEP32RX };
enum { CNT32TX = DMABUFFSIZE32TX / DMABUFFSTEP32TX };
enum { CNT32RTS = DMABUFFSIZE32RTS / DMABUFFSTEP32RTS };

#if WITHUSBHW && WITHUSBUAC

static volatile uint_fast8_t uacinalt = UACINALT_NONE;		/* выбор альтернативной конфигурации для UAC IN interface */
static volatile uint_fast8_t uacinrtsalt = UACINRTSALT_NONE;		/* выбор альтернативной конфигурации для RTS UAC IN interface */
static volatile uint_fast8_t uacoutalt;

#else /* WITHUSBHW && WITHUSBUAC */

static const uint_fast8_t uacinalt = 0;		/* выбор альтернативной конфигурации для UAC IN interface */
static const uint_fast8_t uacinrtsalt = 0;		/* выбор альтернативной конфигурации для RTS UAC IN interface */
static const uint_fast8_t uacoutalt = 0;

#endif /* WITHUSBHW && WITHUSBUAC */


static void savesampleout16stereo(int_fast32_t ch0, int_fast32_t ch1);
static void savesampleout16stereo_float(void * ctx, FLOAT_t ch0, FLOAT_t ch1);


#if WITHRTS192 || WITHRTS96

	static subscribeint32_t uacinrtssubscribe;

#endif /* WITHRTS96 */


#endif /* WITHINTEGRATEDDSP */


#if WITHBUFFERSDEBUG

static unsigned n1, n1wfm, n2, n3, n4, n5, n6, n7;
static unsigned e1, e2, e3, e4, e5, e6, e7, e8, e9, e10, e11, e12, e13, e14, purge16;
static unsigned nbadd, nbdel, nbzero, nbnorm;

static unsigned debugcount_ms10;	// с точностью 0.1 ms

static unsigned debugcount_uacout;
static unsigned debugcount_mikeadc;
static unsigned debugcount_phonesdac;
static unsigned debugcount_uacinrts;
static unsigned debugcount_uacin;
static unsigned debugcount_rx32adc;
static unsigned debugcount_rx32wfm;
static unsigned debugcount_tx32dac;

#endif /* WITHBUFFERSDEBUG */

static ticker_t buffticker;

/* вызывается из обработчика таймерного прерывания */
static void buffers_spool(void * ctx)
{
#if WITHBUFFERSDEBUG
	debugcount_ms10 += 10000 / TICKS_FREQUENCY;
#endif /* WITHBUFFERSDEBUG */
}

#if WITHBUFFERSDEBUG

/* Запрещение прерывений убрано для уменьшения влияения на работу системы. Иногда может ошибаться в расчете скорости */
static unsigned
getresetval(volatile unsigned * p)
{
	unsigned v;
	v = * p;
	* p = 0;
	return v;
}

#endif /* WITHBUFFERSDEBUG */

void buffers_diagnostics(void)
{
#if 1 && WITHDEBUG && WITHINTEGRATEDDSP && WITHBUFFERSDEBUG && ! WITHBUFFERSSMALLDEBUG

//	LIST2PRINT(speexfree16);
//	LIST2PRINT(voicesfree32tx);
//	LIST2PRINT(voicesfree16rx);
//	LIST2PRINT(voicesfree16tx);
//	LIST2PRINT(speexready16);
//	LIST2PRINT(voicesready32tx);
	PRINTF(PSTR("\n"));
//	LIST3PRINT(voicesready16rx);
//	LIST3PRINT(voicesphones16tx);
//	LIST3PRINT(voicesmoni16tx);

//	#if WITHUSBUACIN
//		#if WITHRTS192
//			LIST2PRINT(voicesfree192rts);
//			LIST2PRINT(uacin192rts);
//		#elif WITHRTS96
//			LIST2PRINT(uacinrts96free);
//			LIST2PRINT(uacinrts96ready);
//		#endif
//		LIST2PRINT(uacin48free);
//		LIST2PRINT(uacin48ready);
//	#endif /* WITHUSBUACIN */
			PRINTF(PSTR("\n"));
	#if WITHUSBUACOUT
//		LIST3PRINT(voicesusb16rx);
//		LIST3PRINT(resample16rx);
		PRINTF(PSTR(" (NORMAL=%d), uacoutalt=%d, add=%u, del=%u, zero=%u, norm=%u "), RESAMPLE16NORMAL, uacoutalt, nbadd, nbdel, nbzero, nbnorm);
	#endif /* WITHUSBUACOUT */

		
	PRINTF(PSTR("\n"));

#endif

#if 1 && WITHDEBUG && WITHINTEGRATEDDSP && WITHBUFFERSDEBUG
#if WITHBUFFERSSMALLDEBUG
	const unsigned ms10 = getresetval(& debugcount_ms10);
	const unsigned mikeadc = getresetval(& debugcount_mikeadc);
	const unsigned phonesdac = getresetval(& debugcount_phonesdac);
	const unsigned rx32adc = getresetval(& debugcount_rx32adc);
	const unsigned tx32dac = getresetval(& debugcount_tx32dac);
	PRINTF(PSTR("FREQ: mikeadc=%u, phonesdac=%u, rx32adc=%u, tx32dac=%u\n"),
		mikeadc * 10000 / ms10,
		phonesdac * 10000 / ms10,
		rx32adc * 10000 / ms10,
		tx32dac * 10000 / ms10
		);
#else
	PRINTF(PSTR("n1=%u n1wfm=%u n2=%u n3=%u n4=%u n5=%u n6=%u n7=%u uacinalt=%d, purge16=%u\n"), n1, n1wfm, n2, n3, n4, n5, n6, n7, uacinalt, purge16);
	PRINTF(PSTR("e1=%u e2=%u e3=%u e4=%u e5=%u e6=%u e7=%u e8=%u e9=%u e10=%u e11=%u e12=%u e13=%u\n"), e1, e2, e3, e4, e5, e6, e7, e8, e9, e10, e11, e12, e13);

	{
		const unsigned ms10 = getresetval(& debugcount_ms10);
		const unsigned uacout = getresetval(& debugcount_uacout);
		const unsigned mikeadc = getresetval(& debugcount_mikeadc);
		const unsigned phonesdac = getresetval(& debugcount_phonesdac);
		const unsigned uacinrts = getresetval(& debugcount_uacinrts);
		const unsigned rx32adc = getresetval(& debugcount_rx32adc);
		const unsigned rx32wfm = getresetval(& debugcount_rx32wfm);
		const unsigned tx32dac = getresetval(& debugcount_tx32dac);
		const unsigned uacin = getresetval(& debugcount_uacin);

		PRINTF(PSTR("FREQ: uacout=%u, uacin=%u, uacinrts=%u, mikeadc=%u, phonesdac=%u, rx32adc=%u, rx32wfm=%u, tx32dac=%u\n"),
			uacout * 10000 / ms10, 
			uacin * 10000 / ms10, 
			uacinrts * 10000 / ms10,
			mikeadc * 10000 / ms10, 
			phonesdac * 10000 / ms10, 
			rx32adc * 10000 / ms10, 
			rx32wfm * 10000 / ms10, 
			tx32dac * 10000 / ms10
			);
	}
#endif /* WITHBUFFERSSMALLDEBUG */
#endif
}


#if WITHINTEGRATEDDSP

static void
savesampleout16tospeex(void * ctx, FLOAT_t ch0, FLOAT_t ch1)
{
	static speexel_t * buff = NULL;
	static unsigned n;

	if (buff == NULL)
	{
		buff = allocatespeexbuffer();
		n = 0;
	}

	buff [n] = ch0;		// sample value
#if WITHUSEDUALWATCH
	buff [n + FIRBUFSIZE] = ch1;	// sample value
#endif /* WITHUSEDUALWATCH */

	n += 1;

	if (n >= FIRBUFSIZE)
	{
		savespeexbuffer(buff);
		buff = NULL;
	}
}

deliverylist_t rtstargetsint;	// выход обработчика DMA приема от FPGA
deliverylist_t speexoutfloat;	// выход speex и фильтра
deliverylist_t afdemodoutfloat;	// выход приемника

#endif /* WITHINTEGRATEDDSP */

#if WITHINTEGRATEDDSP

// приняли данные от USB AUDIO
static RAMFUNC void
buffers_savefromuacout_rxresampler(uintptr_t addr)
{
#if WITHBUFFERSDEBUG
	// подсчёт скорости в сэмплах за секунду
	debugcount_uacout += DMABUFFSIZE16RX / DMABUFFSTEP16RX;	// в буфере пары сэмплов по два байта
	++ n2;
#endif /* WITHBUFFERSDEBUG */

#if WITHUSBHW && WITHUSBUAC && defined (WITHUSBHW_DEVICE)
	if (uacoutalt == 0)
	{
		release_dmabuffer16rxresampler(addr);
	}
	else
	{
		save_dmabuffer16rxresampler(addr);
	}

#else /* WITHUSBUAC */
	release_dmabuffer16rxresampler(addr);
#endif /* WITHUSBUAC */
}

// 16 bit, signed
// в паре значений, возвращаемых данной функцией, vi получает значение от микрофона. vq зарезервированно для работы ISB (две независимых боковых)
// При отсутствии данных в очереди - возвращаем 0
// TODO: переделать на denoise16_t
RAMFUNC uint_fast8_t getsampmleusb(FLOAT32P_t * v)
{
	enum { L, R };
	static aubufv_t * buff = NULL;
	static unsigned n = 0;	// позиция по выходному количеству

	if (buff == NULL)
	{
		buff = (aubufv_t *) getfilled_dmabuffer16rxresampler();
		if (buff == 0)
		{
			// Микрофонный кодек ещё не успел начать работать - возвращаем 0.
			return 0;
		}
		n = 0;
	}

	// Использование данных.
	v->ivqv [L] = adpt_input(& afcodecrx, buff [n * DMABUFFSTEP16RX + 0]);	// левый канал
	v->ivqv [R] = adpt_input(& afcodecrx, buff [n * DMABUFFSTEP16RX + 1]);	// правый канал

	if (++ n >= CNT16RX)
	{
		release_dmabuffer16rxresampler((uintptr_t) buff);
		buff = NULL;
	}
	return 1;
}

#if WITHUSBUAC && defined (WITHUSBHW_DEVICE)

static uint_fast8_t isaudio48(void)
{
#if WITHUSBHW && WITHUSBUAC && defined (WITHUSBHW_DEVICE)
	return UACINALT_AUDIO48 == uacinalt;
#else /* WITHUSBHW && WITHUSBUAC && defined (WITHUSBHW_DEVICE) */
	return 0;
#endif /* WITHUSBHW && WITHUSBUAC && defined (WITHUSBHW_DEVICE) */
}

// UAC OUT samplerate
int_fast32_t dsp_get_samplerateuacout(void)
{
	return dsp_get_sampleratetx();
}

#if WITHRTS192 && WITHUSBHW && WITHUSBUAC

static uint_fast8_t isrts192(void)
{
#if WITHUSBHW && WITHUSBUAC
	#if WITHUSBUACIN2 && WITHRTS192
		return UACINRTSALT_RTS192 == uacinrtsalt;
	#elif WITHRTS192
		return UACINALT_RTS192 == uacinalt;
	#else /* WITHUSBUACIN2 */
		return 0;
	#endif /* WITHUSBUACIN2 */
#else /* WITHUSBHW && WITHUSBUAC */
	return 0;
#endif /* WITHUSBHW && WITHUSBUAC */
}

// Сохранить буфер сэмплов для передачи в компьютер
RAMFUNC void
save_dmabufferuacinrts192(uintptr_t addr)
{
	voice192rts_t * const p = CONTAINING_RECORD(addr, voice192rts_t, u.buff);
	ASSERT(p->tag2 == p);
	ASSERT(p->tag3 == p);
#if WITHBUFFERSDEBUG
	// подсчёт скорости в сэмплах за секунду
	debugcount_uacinrts += sizeof p->buff / sizeof p->buff [0] / DMABUFFSTEP192RTS;	// в буфере пары сэмплов по четыре байта
#endif /* WITHBUFFERSDEBUG */

	LCLSPIN_LOCK(& locklistrts);
	InsertHeadList2(& uacin192rts, & p->item);
	LCLSPIN_UNLOCK(& locklistrts);
}

static void buffers_savetonull192rts(voice192rts_t * p)
{
	ASSERT(p->tag2 == p);
	ASSERT(p->tag3 == p);
	LCLSPIN_LOCK(& locklistrts);
	InsertHeadList2(& voicesfree192rts, & p->item);
	LCLSPIN_UNLOCK(& locklistrts);
}


#endif /* WITHRTS192 && WITHUSBHW && WITHUSBUAC */

#if WITHRTS96 && WITHUSBHW && WITHUSBUAC && defined (WITHUSBHW_DEVICE)

static uint_fast8_t isrts96(void)
{
#if WITHUSBHW && WITHUSBUAC
	#if WITHUSBUACIN2 && WITHRTS96
		return uacinrtsalt == UACINRTSALT_RTS96;
	#elif WITHRTS96
		return uacinalt == UACINALT_RTS96;
	#else /* WITHUSBUACIN2 */
		return 0;
	#endif /* WITHUSBUACIN2 */
#else /* WITHUSBHW && WITHUSBUAC */
	return 0;
#endif /* WITHUSBHW && WITHUSBUAC */
}

#endif

#else

static uint_fast8_t isrts96(void)
{
	return 0;
}

#endif /*  WITHRTS96 && WITHUSBHW && WITHUSBUAC && defined (WITHUSBHW_DEVICE) */


// --- Коммутация потоков аудиоданных


// Этой функцией пользуются обработчики прерываний DMA
// обработать буфер после оцифровки IF ADC (спектроанализатор)
// Вызывается на ARM_REALTIME_PRIORITY уровне.
void RAMFUNC processing_dmabuffer32rx(uintptr_t addr)
{
	//ASSERT(addr != 0);
#if WITHBUFFERSDEBUG
	++ n1;
	// подсчёт скорости в сэмплах за секунду
	debugcount_rx32adc += CNT32RX;	// в буфере пары сэмплов по четыре байта
#endif /* WITHBUFFERSDEBUG */

	dsp_extbuffer32rx((IFADCvalue_t *) addr);

	dsp_processtx();	/* выборка семплов из источников звука и формирование потока на передатчик */

}

// Этой функцией пользуются обработчики прерываний DMA
// обработать буфер после оцифровки IF ADC (MAIN RX/SUB RX)
// Вызывается на ARM_REALTIME_PRIORITY уровне.
void RAMFUNC processing_dmabuffer32rts(uintptr_t addr)
{
	//ASSERT(addr != 0);
#if WITHBUFFERSDEBUG
	//++ n77;
	// подсчёт скорости в сэмплах за секунду
	//debugcount_rx32rtsadc += CNT32RTS;	// в буфере пары сэмплов по четыре байта
#endif /* WITHBUFFERSDEBUG */
	dsp_extbuffer32rts((IFADCvalue_t *) addr);
}


// Этой функцией пользуются обработчики прерываний DMA
// обработать буфер после оцифровки IF ADC (MAIN RX/SUB RX)
// Вызывается на ARM_REALTIME_PRIORITY уровне.
void RAMFUNC processing_dmabuffer32wfm(uintptr_t addr)
{
	//ASSERT(addr != 0);
#if WITHBUFFERSDEBUG
	++ n1wfm;
	// подсчёт скорости в сэмплах за секунду
	debugcount_rx32wfm += CNT32RX;	// в буфере пары сэмплов по четыре байта
#endif /* WITHBUFFERSDEBUG */
#if WITHWFM
	dsp_extbuffer32wfm((const IFADCvalue_t *) addr);
#endif /* WITHWFM */
}

#if WITHFPGAPIPE_CODEC1

// копирование полей из принятого от FPGA буфера
static uintptr_t RAMFUNC
pipe_dmabuffer16rx(uintptr_t addr16rx, uintptr_t addr32rx)
{
	// Предполагается что типы данных позволяют транзитом передавать сэмплы, не беспокоясь о преобразовании форматов
	unsigned i;
	IFADCvalue_t * const rx32 = (IFADCvalue_t *) addr32rx;
	aubufv_t * const rx16 = (aubufv_t *) addr16rx;

	ASSERT((unsigned) CNT32RX == (unsigned) CNT16RX);
	ASSERT(sizeof * rx32 == sizeof * rx16);
	for (i = 0; i < (unsigned) CNT32RX; ++ i)
	{
		rx16 [i * DMABUFFSTEP16RX + DMABUFF16RX_LEFT] = rx32 [i * DMABUFFSTEP32RX + DMABUFF32RX_CODEC1_LEFT];
		rx16 [i * DMABUFFSTEP16RX + DMABUFF16RX_RIGHT] = rx32 [i * DMABUFFSTEP32RX + DMABUFF32RX_CODEC1_RIGHT];
	}
	return addr16rx;
}

// копирование полей в передаваемый на FPGA буфер
static uintptr_t RAMFUNC
pipe_dmabuffer32tx(uintptr_t addr32tx, uintptr_t addr16tx)
{
	// Предполагается что типы данных позволяют транзитом передавать сэмплы, не беспокоясь о преобразовании форматов

	IFDACvalue_t * const tx32 = (IFDACvalue_t *) addr32tx;
	aubufv_t * const tx16 = (aubufv_t *) addr16tx;
	unsigned i;
	const FLOAT_t scale = 1.0 / 32;

	ASSERT((unsigned) CNT32TX == (unsigned) CNT16TX);
	ASSERT(DMABUFFSTEP16TX >= 2);
	ASSERT(sizeof * tx32 == sizeof * tx16);
	for (i = 0; i < (unsigned) CNT32TX; ++ i)
	{
		tx32 [i * DMABUFFSTEP32TX + DMABUFF32TX_CODEC1_LEFT] = tx16 [i * DMABUFFSTEP16TX + DMABUFF16TX_LEFT];
		tx32 [i * DMABUFFSTEP32TX + DMABUFF32TX_CODEC1_RIGHT] = tx16 [i * DMABUFFSTEP16TX + DMABUFF16TX_RIGHT];
//		tx32 [i * DMABUFFSTEP32TX + DMABUFF32TX_CODEC1_LEFT] = adpt_outputexact(& afcodectx, get_lout() * scale);
//		tx32 [i * DMABUFFSTEP32TX + DMABUFF32TX_CODEC1_RIGHT] = adpt_outputexact(& afcodectx, get_rout() * scale);
	}
	return addr32tx;
}

#endif /* WITHFPGAPIPE_CODEC1 */

#if WITHRTS192
// Этой функцией пользуются обработчики прерываний DMA
// обработать буфер после оцифровки - канал спектроанализатора
void RAMFUNC processing_dmabuffer32rts192(uintptr_t addr)
{
	//ASSERT(addr != 0);
#if WITHBUFFERSDEBUG
	++ n4;
#endif /* WITHBUFFERSDEBUG */
	voice192rts_t * const p = CONTAINING_RECORD(addr, voice192rts_t, u.buff);

#if ! WITHTRANSPARENTIQ
	unsigned i;
	for (i = 0; i < DMABUFFSIZE32RTS; i += DMABUFFSTEP32RTS)
	{
		const int32_t * const b = (const int32_t *) & p->u.buff [i];

        //saveIQRTSxx(NULL, b [0], b [1]);
        deliveryint(& rtstargetsint, b [0], b [1]);
	}
#endif /* ! WITHTRANSPARENTIQ */

	//save_dmabufferuacinrts192(p);
	buffers_savetonull192rts(p);
}
#endif /* WITHRTS192 */



/* при необходимости копируем сэмплы от кодекв */
uintptr_t processing_pipe32rx(uintptr_t addr)
{
#if WITHFPGAPIPE_CODEC1
	save_dmabuffer16rx(pipe_dmabuffer16rx(allocate_dmabuffer16rx(), addr));
#endif /* WITHFPGAPIPE_CODEC1 */
#if WITHFPGAPIPE_RTS96
	//processing_dmabuffer32rts(addr);
#endif /* WITHFPGAPIPE_RTS96 */
#if WITHFPGAPIPE_RTS192
	//processing_dmabuffer32rts(addr);
#endif /* WITHFPGAPIPE_RTS192 */
	return addr;
}

/* при необходимости добавляем слоты для передачи на кодек */
uintptr_t processing_pipe32tx(uintptr_t addr)
{
#if WITHFPGAPIPE_CODEC1
	const uintptr_t addr16 = getfilled_dmabuffer16txphones();
	pipe_dmabuffer32tx(addr, addr16);
	release_dmabuffer16txphones(addr16);	/* освоюождаем буфер как переданный */
#endif /* WITHFPGAPIPE_CODEC1 */
#if WITHFPGAPIPE_NCORX0
#endif /* WITHFPGAPIPE_NCORX0 */
#if WITHFPGAPIPE_NCORX1
#endif /* WITHFPGAPIPE_NCORX1 */
#if WITHFPGAPIPE_NCORTS
#endif /* WITHFPGAPIPE_NCORTS */
	return addr;
}

// Отладочная функция для тестирования обмена с кодеком
void dsp_calctx(void)
{
#if WITHBUFFERSDEBUG
	// подсчёт скорости в сэмплах за секунду
	debugcount_phonesdac += DMABUFFSIZE16TX / DMABUFFSTEP16TX;	// в буфере пары сэмплов по два байта
#endif /* WITHBUFFERSDEBUG */
}

// Отладочная функция для тестирования обмена с кодеком
void dsp_calcrx(void)
{
#if WITHBUFFERSDEBUG
	// подсчёт скорости в сэмплах за секунду
	debugcount_mikeadc += DMABUFFSIZE16RX / DMABUFFSTEP16RX;	// в буфере пары сэмплов по два байта
#endif /* WITHBUFFERSDEBUG */
}

//////////////////////////////////////////
// Поэлементное заполнение буфера IF DAC

// Вызывается из ARM_REALTIME_PRIORITY обработчика прерывания
// 32 bit, signed
void savesampleout32stereo(int_fast32_t ch0, int_fast32_t ch1)
{
	static IFDACvalue_t * buff = NULL;
	static unsigned level32tx;

	if (buff == NULL)
	{
		buff = (IFDACvalue_t *) allocate_dmabuffer32tx();
		level32tx = 0;
	}

	buff [level32tx + DMABUF32TXI] = ch0;
	buff [level32tx + DMABUF32TXQ] = ch1;

#if defined(DDS1_TYPE) && (DDS1_TYPE == DDS_TYPE_GW2A_V0)

#elif defined(DDS1_TYPE) && (DDS1_TYPE == DDS_TYPE_FPGAV1) && ! (CTLREGMODE_OLEG4Z_V1 || CTLREGMODE_OLEG4Z_V2)
	/* установка параметров приемника, передаваемых чрез I2S канал в FPGA */
	buff [level32tx + DMABUF32TX_NCO1] = dspfpga_get_nco1();
	buff [level32tx + DMABUF32TX_NCO2] = dspfpga_get_nco2();
	buff [level32tx + DMABUF32TX_NCORTS] = dspfpga_get_ncorts();
#endif /* (DDS1_TYPE == DDS_TYPE_FPGAV1) && ! (CTLREGMODE_OLEG4Z_V1 || CTLREGMODE_OLEG4Z_V2) */

#if CPUSTYLE_XC7Z && WITHLFM
	if (iflfmactive())
	{
		ftw_t v = dspfpga_get_nco1();
		xcz_dds_ftw(& v);
		v = dspfpga_get_ncorts();
		xcz_dds_rts(& v);
	}
#endif /* CPUSTYLE_XC7Z && WITHLFM */

	if ((level32tx += DMABUFFSTEP32TX) >= DMABUFFSIZE32TX)
	{
		save_dmabuffer32tx((uintptr_t) buff);
		buff = NULL;
	}
}

//////////////////////////////////////////
// Поэлементное заполнение DMA буфера AF DAC
/* вызывается при запрещённых глобавльных прерываниях */
static void savesampleout16stereo(int_fast32_t ch0, int_fast32_t ch1)
{
	// если есть инициализированный канал для выдачи звука
	static aubufv_t * buff = NULL;
	static unsigned n;

	if (buff == NULL)
	{
		buff = (aubufv_t *) allocate_dmabuffer16txphones();
		n = 0;
	}

	buff [n * DMABUFFSTEP16TX + DMABUFF16TX_LEFT] = ch0;	// sample value
	buff [n * DMABUFFSTEP16TX + DMABUFF16TX_RIGHT] = ch1;	// sample value

	if (++ n >= CNT16TX)
	{
		save_dmabuffer16txphones((uintptr_t) buff);
		buff = NULL;
	}
}

/* вызывается при запрещённых глобальных прерываниях */
static void savesampleout16stereo_float(void * ctx, FLOAT_t ch0, FLOAT_t ch1)
{
	savesampleout16stereo(
		adpt_output(& afcodectx, ch0),
		adpt_output(& afcodectx, ch1)
		);
}


#if WITHUSBUAC && WITHUSBHW && defined (WITHUSBHW_DEVICE)

/* выборка нужного количества байт из UAC OUT буфера. Расширение знака не требуется, результат обрабатывается функцией transform_do32 */
static int32_t fetch_le(const uint8_t * p, size_t usbsz)
{
	switch (usbsz)
	{
	default:
	case 1:
		return p [0];
	case 2:
		return p [1] * 256u + p [0];
	case 3:
		return (p [2] * 256u + p [1]) * 256u + p [0];
	case 4:
		return ((p [3] * 256u + p [2]) * 256u + p [1]) * 256u + p [0];
	}
}

/* Поместить в буфер UAC IN endpoint значения сжмпла */
unsigned place_le(uint8_t * p, int32_t value, size_t usbsz)
{
	switch (usbsz)
	{
	default:
		ASSERT(0);
		break;
	case 1:
		p [0] = (uint8_t) (value >> 0);
		break;
	case 2:
		p [1] = (uint8_t) (value >> 8);
		p [0] = (uint8_t) (value >> 0);
		break;
	case 3:
		p [2] = (uint8_t) (value >> 16);
		p [1] = (uint8_t) (value >> 8);
		p [0] = (uint8_t) (value >> 0);
		break;
	case 4:
		p [3] = (uint8_t) (value >> 24);
		p [2] = (uint8_t) (value >> 16);
		p [1] = (uint8_t) (value >> 8);
		p [0] = (uint8_t) (value >> 0);
		break;
	}
	return usbsz;
}

#if WITHRTS96

	// Поэлементное заполнение буфера RTS96

	// Вызывается из ARM_REALTIME_PRIORITY обработчика прерывания
	// vl, vr: 32 bit, signed - преобразуем к требуемому формату для передачи по USB здесь.
	void savesampleout96stereo(void * ctx, int_fast32_t ch0, int_fast32_t ch1)
	{
		// если есть инициализированный канал для выдачи звука
		static uint8_t * buff = NULL;
		static unsigned n;

		if (buff == NULL)
		{
			if (! isrts96())
				return;
			buff = (uint8_t *) allocate_dmabufferuacinrts96();
			n = 0;
		}
		else if (! isrts96())
		{
			release_dmabufferuacinrts96((uintptr_t) buff);
			buff = NULL;
			return;
		}

		n += place_le(buff + n, transform_do32(& if2rts96out, ch0), UACIN_RTS96_SAMPLEBYTES);	// sample value
		n += place_le(buff + n, transform_do32(& if2rts96out, ch1), UACIN_RTS96_SAMPLEBYTES);	// sample value

		if (n >= UACIN_RTS96_DATASIZE_DMAC)
		{
			save_dmabufferuacinrts96((uintptr_t) buff);
			buff = NULL;
		}
	}

#endif /* WITHRTS96 */

#if WITHRTS192

	// NOT USED
	// Поэлементное заполнение буфера RTS192

	// Вызывается из ARM_REALTIME_PRIORITY обработчика прерывания
	// vl, vr: 32 bit, signed - преобразуем к требуемому формату для передачи по USB здесь.
	void savesampleout192stereo(void * ctx, int_fast32_t ch0, int_fast32_t ch1)
	{
		// если есть инициализированный канал для выдачи звука
		static uint8_t * buff = NULL;
		static unsigned n;

		if (p == NULL)
		{
			if (! isrts192())
				return;
			buff = (uint8_t *) allocate_dmabufferuacinrts192();
			n = 0;
		}
		else if (! isrts192())
		{
			release_dmabufferuacinrts192((uintptr_t) buff);
			p = NULL;
			return;
		}

		n += place_le(buff + n, transform_do32(& if2rts192out, ch0), UACIN_RTS192_SAMPLEBYTES);	// sample value
		n += place_le(buff + n, transform_do32(& if2rts192out, ch1), UACIN_RTS192_SAMPLEBYTES);	// sample value

		if (n >= UACIN_RTS192_DATASIZE_DMAC)
		{
			save_dmabufferuacinrts192((uintptr_t) buff);
			buff = NULL;
		}
	}

#endif /* WITHRTS192 */

#if WITHUSBUACOUT && defined (WITHUSBHW_DEVICE)


void processing_dmabufferuacout48(uintptr_t addr)
{
	uacout_buffer_save((const uint8_t *) addr, UACOUT_AUDIO48_DATASIZE_DMAC, UACOUT_FMT_CHANNELS_AUDIO48, UACOUT_AUDIO48_SAMPLEBYTES);

	release_dmabufferuacout48(addr);
}

#endif /* WITHUSBUACOUT */

// Вызывается из ARM_REALTIME_PRIORITY обработчика прерывания
// vl, vr: 16 bit, signed - требуемый формат для передачи по USB.

void savesampleuacin48(int_fast32_t ch0, int_fast32_t ch1)
{
#if WITHUSBHW && WITHUSBUACIN && defined (WITHUSBHW_DEVICE)
	// если есть инициализированный канал для выдачи звука
	static uint8_t * buff = NULL;
	static unsigned n = 0;

	if (buff == NULL)
	{
		if (! isaudio48())
			return;
		buff = (uint8_t *) allocate_dmabufferuacin48();
		n = 0;
	}
	else if (! isaudio48())
	{
		release_dmabufferuacin48((uintptr_t) buff);
		buff = NULL;
		return;
	}

	n += place_le(buff + n, ch0, UACIN_AUDIO48_SAMPLEBYTES); // sample value
#if UACIN_FMT_CHANNELS_AUDIO48 > 1
	n += place_le(buff + n, ch1, UACIN_AUDIO48_SAMPLEBYTES); // sample value
#endif /* UACIN_FMT_CHANNELS_AUDIO48 */

	if (n >= UACIN_AUDIO48_DATASIZE_DMAC)
	{
		save_dmabufferuacin48((uintptr_t) buff);
		buff = NULL;
	}
#endif /* WITHUSBHW && WITHUSBUACIN && defined (WITHUSBHW_DEVICE) */
}

#else /* WITHUSBUAC */

void savesampleout96stereo(void * ctx, int_fast32_t ch0, int_fast32_t ch1)
{
}

void savesampleout192stereo(void * ctx, int_fast32_t ch0, int_fast32_t ch1)
{
}

void savesampleuacin48(int_fast16_t ch0, int_fast16_t ch1)
{
}

#endif /* WITHUSBUAC */

#if WITHUSBUAC && WITHUSBHW

/* +++ UAC OUT data save */

void
buffers_set_uacinalt(uint_fast8_t v)	/* выбор альтернативной конфигурации для UAC IN interface */
{
	//PRINTF(PSTR("buffers_set_uacinalt: v=%d\n"), (int) v);
	uacinalt = v;
}

void
buffers_set_uacinrtsalt(uint_fast8_t v)	/* выбор альтернативной конфигурации для UAC IN interface */
{
	//PRINTF(PSTR("buffers_set_uacinrtsalt: v=%d\n"), (int) v);
	uacinrtsalt = v;
}

uint_fast8_t buffers_get_uacoutalt(void)
{
	return uacoutalt;
}

void
buffers_set_uacoutalt(uint_fast8_t v)	/* выбор альтернативной конфигурации для UAC OUT interface */
{
	//PRINTF(PSTR("buffers_set_uacoutalt: v=%d\n"), (int) v);
	uacoutalt = v;

	if (v == 0)
	{
		purge_dmabuffer16rxresampler();
	}
}

static uintptr_t uacoutaddr;	// address of DMABUFFSIZE16 * размер сэмпла * количество каналов bytes
static uint_fast16_t uacoutbufflevel;	// количество байтовЮ на которые заполнен буфер

/* вызывается при запрещённых прерываниях. */
void uacout_buffer_initialize(void)
{
	uacoutaddr = 0;
	uacoutbufflevel = 0;
}

/* вызыватся из ARM_SYSTEM_PRIORITY функции обработчика прерывания */
void uacout_buffer_start(void)
{
	if (uacoutaddr)
		TP();
}
/* вызыватся из ARM_SYSTEM_PRIORITY функции обработчика прерывания */
void uacout_buffer_stop(void)
{
	if (uacoutaddr != 0)
	{
		IRQL_t oldIrql;
		RiseIrql(IRQL_REALTIME, & oldIrql);
		release_dmabuffer16rx(uacoutaddr);
		LowerIrql(oldIrql);
		uacoutaddr = 0;
		uacoutbufflevel = 0;
	}
}

/* вызыватся из не-realtime функции обработчика прерывания */
// Работает на ARM_SYSTEM_PRIORITY
/* вызыватся из realtime функции обработчика прерывания */
// Работает на ARM_REALTIME_PRIORITY
void uacout_buffer_save(const uint8_t * buff, uint_fast16_t size, uint_fast8_t ichannels, uint_fast8_t usbsz)
{
	const size_t dmabuffer16size = DMABUFFSIZE16RX * sizeof (aubufv_t);	// размер в байтах

	for (;;)
	{
		const uint_fast16_t insamples = size / usbsz / ichannels;	// количество сэмплов во входном буфере
		const uint_fast16_t outsamples = (dmabuffer16size - uacoutbufflevel) / sizeof (aubufv_t) / DMABUFFSTEP16RX;
		const uint_fast16_t chunksamples = ulmin16(insamples, outsamples);
		const size_t inchunk = chunksamples * usbsz * ichannels;
		const size_t outchunk = chunksamples * sizeof (aubufv_t) * DMABUFFSTEP16RX;	// разхмер в байтах

		if (chunksamples == 0)
			break;
		if (uacoutaddr == 0)
		{
			IRQL_t oldIrql;
			RiseIrql(IRQL_REALTIME, & oldIrql);
			uacoutaddr = allocate_dmabuffer16rxresampler();
			LowerIrql(oldIrql);
			uacoutbufflevel = 0;
		}

		if (ichannels == 1)
		{
			ASSERT(DMABUFFSTEP16RX == 2);
			// копирование нужного количества сэмплов с прербразованием из моно в стерео
			const uint8_t * src = buff;
			aubufv_t * dst = (aubufv_t *) (uacoutaddr + uacoutbufflevel);
			uint_fast16_t n = chunksamples;
			while (n --)
			{
				const aufastbufv_t v = transform_do32(& uac48out2afcodecrx, fetch_le(src, usbsz));
				* dst ++ = v;
				* dst ++ = v;
				src += usbsz;
			}
		}
		else
		{
			ASSERT(DMABUFFSTEP16RX == ichannels);
			// требуется преобразование формата из 16-бит семплов ко внутреннему формату aubufv_t
			/* копирование 16 бит сэмплов с расширением */
			const uint8_t * src = buff;
			aubufv_t * dst = (aubufv_t *) ((uint8_t *) uacoutaddr + uacoutbufflevel);
			uint_fast16_t n = chunksamples * ichannels;
			while (n --)
			{
				const aufastbufv_t v = transform_do32(& uac48out2afcodecrx, fetch_le(src, usbsz));
				* dst ++ = v;
				src += usbsz;
			}
		}

		size -= inchunk;	// проход по входому буферу
		buff += inchunk;	// проход входому буферу

		if ((uacoutbufflevel += outchunk) >= dmabuffer16size)
		{
			IRQL_t oldIrql;
			RiseIrql(IRQL_REALTIME, & oldIrql);
			buffers_savefromuacout_rxresampler(uacoutaddr);
			LowerIrql(oldIrql);
			uacoutaddr = 0;
			uacoutbufflevel = 0;
		}
	}
}
/* --- UAC OUT data save */

/* получить буфер одного из типов, которые могут использоваться для передаяи аудиоданных в компьютер по USB */
uintptr_t getfilled_dmabufferuacinX(uint_fast16_t * sizep)
{
#if WITHBUFFERSDEBUG
	++ n6;
#endif /* WITHBUFFERSDEBUG */
	switch (uacinalt)
	{
	case UACINALT_NONE:
		return 0;

	case UACINALT_AUDIO48:
		* sizep = UACIN_AUDIO48_DATASIZE_DMAC;
		return getfilled_dmabufferuacin48();

#if ! WITHUSBUACIN2

#if WITHRTS96
	case UACINALT_RTS96:
		* sizep = UACIN_RTS96_DATASIZE_DMAC;
		return getfilled_dmabufferuacinrts96();
#endif /* WITHRTS192 */

#if WITHRTS192
	case UACINALT_RTS192:
		* sizep = UACIN_RTS192_DATASIZE_DMAC;
		return getfilled_dmabufferuacinrts192();
#endif /* WITHRTS192 */

#endif /* ! WITHUSBUACIN2 */

	default:
		PRINTF(PSTR("getfilled_dmabufferuacinX: uacinalt=%u\n"), uacinalt);
		ASSERT(0);
		return 0;
	}
}

#if WITHUSBUACIN2

/* получить буфер одного из типов, которые могут использоваться для передаяи аудиоданных в компьютер по USB */
uintptr_t getfilled_dmabufferuacinrtsX(uint_fast16_t * sizep)
{
#if WITHBUFFERSDEBUG
	++ n6;
#endif /* WITHBUFFERSDEBUG */
	switch (uacinrtsalt)
	{
	case UACINRTSALT_NONE:
		return 0;

#if WITHUSBUACIN2 && WITHUSBHW

#if WITHRTS96
	case UACINRTSALT_RTS96:
		* sizep = UACIN_RTS96_DATASIZE_DMAC;
		return getfilled_dmabufferuacinrts96();
#endif /* WITHRTS192 */

#if WITHRTS192
	case UACINRTSALT_RTS192:
		* sizep = UACIN_RTS192_DATASIZE_DMAC;
		return getfilled_dmabufferuacinrts192();
#endif /* WITHRTS192 */

#endif /* WITHUSBUACIN2 && WITHUSBHW */

	default:
		PRINTF(PSTR("getfilled_dmabufferuacinrtsX: uacinrtsalt=%u\n"), uacinrtsalt);
		ASSERT(0);
		return 0;
	}
}

#endif /* WITHUSBUACIN2 */

#endif /* WITHUSBUAC */

/* предполагается что тут значения нормирования в диапазоне -1..+1 */
void deliveryfloat(deliverylist_t * list, FLOAT_t ch0, FLOAT_t ch1)
{
	PLIST_ENTRY t;
	IRQL_t oldIrql;

	IRQLSPIN_LOCK(& list->listlock, & oldIrql);
	for (t = list->head.Blink; t != & list->head; t = t->Blink)
	{
		subscribefloat_t * const p = CONTAINING_RECORD(t, subscribefloat_t, item);
		(p->cb)(p->ctx, ch0, ch1);
	}
	IRQLSPIN_UNLOCK(& list->listlock, oldIrql);
}

/* предполагается что тут значения нормирования в диапазоне -1..+1 */
void deliveryfloat_buffer(deliverylist_t * list, const FLOAT_t * ch0, const FLOAT_t * ch1, unsigned n)
{
	IRQL_t oldIrql;
	PLIST_ENTRY t;

	IRQLSPIN_LOCK(& list->listlock, & oldIrql);
	for (t = list->head.Blink; t != & list->head; t = t->Blink)
	{
		subscribefloat_t * const p = CONTAINING_RECORD(t, subscribefloat_t, item);
		unsigned i;
		for (i = 0; i < n; ++ i)
		{
			(p->cb)(p->ctx, ch0 [i], ch1 [i]);
		}
	}
	IRQLSPIN_UNLOCK(& list->listlock, oldIrql);
}

void deliveryint(deliverylist_t * list, int_fast32_t ch0, int_fast32_t ch1)
{
	IRQL_t oldIrql;
	PLIST_ENTRY t;

	IRQLSPIN_LOCK(& list->listlock, & oldIrql);
	for (t = list->head.Blink; t != & list->head; t = t->Blink)
	{
		subscribeint32_t * const p = CONTAINING_RECORD(t, subscribeint32_t, item);
		(p->cb)(p->ctx, ch0, ch1);
	}
	IRQLSPIN_UNLOCK(& list->listlock, oldIrql);
}

void subscribefloat(deliverylist_t * list, subscribefloat_t * target, void * ctx, void (* pfn)(void * ctx, FLOAT_t ch0, FLOAT_t ch1))
{
	IRQL_t oldIrql;

	target->cb = pfn;
	target->ctx = ctx;
	IRQLSPIN_LOCK(& list->listlock, & oldIrql);
	InsertHeadList(& list->head, & target->item);
	IRQLSPIN_UNLOCK(& list->listlock, oldIrql);
}

void unsubscribefloat(deliverylist_t * list, subscribefloat_t * target)
{
	IRQL_t oldIrql;

	IRQLSPIN_LOCK(& list->listlock, & oldIrql);
	RemoveEntryList(& target->item);
	IRQLSPIN_UNLOCK(& list->listlock, oldIrql);
}

void subscribeint32(deliverylist_t * list, subscribeint32_t * target, void * ctx, void (* pfn)(void * ctx, int_fast32_t ch0, int_fast32_t ch1))
{
	IRQL_t oldIrql;

	target->cb = pfn;
	target->ctx = ctx;
	IRQLSPIN_LOCK(& list->listlock, & oldIrql);
	InsertHeadList(& list->head, & target->item);
	IRQLSPIN_UNLOCK(& list->listlock, oldIrql);
}

void unsubscribeint32(deliverylist_t * list, subscribeint32_t * target)
{
	IRQL_t oldIrql;

	IRQLSPIN_LOCK(& list->listlock, & oldIrql);
	RemoveEntryList(& target->item);
	IRQLSPIN_UNLOCK(& list->listlock, oldIrql);
}

void deliverylist_initialize(deliverylist_t * list, IRQL_t irqlv)
{
	InitializeListHead(& list->head);
	IRQLSPINLOCK_INITIALIZE(& list->listlock, irqlv);
}

#endif /* WITHINTEGRATEDDSP */

// инициализация системы буферов
void buffers_initialize(void)
{
#if WITHBUFFERSDEBUG
	ticker_initialize(& buffticker, 1, buffers_spool, NULL);
	ticker_add(& buffticker);
#endif /* WITHBUFFERSDEBUG */

#if WITHINTEGRATEDDSP

	deliverylist_initialize(& rtstargetsint, IRQL_REALTIME);
	deliverylist_initialize(& speexoutfloat, IRQL_REALTIME);
	deliverylist_initialize(& afdemodoutfloat, IRQL_REALTIME);


#if WITHUSBHEADSET || WITHSKIPUSERMODE || CTLSTYLE_V3D

	// Обход user mode шумоподавителя
	static subscribefloat_t afsample16register;
	subscribefloat(& afdemodoutfloat, & afsample16register, NULL, savesampleout16stereo_float);

#else /* WITHSKIPUSERMODE */

	static subscribefloat_t afsample16register;
	static subscribefloat_t afsample16registertospeex;

	subscribefloat(& speexoutfloat, & afsample16register, NULL, savesampleout16stereo_float);
	subscribefloat(& afdemodoutfloat, & afsample16registertospeex, NULL, savesampleout16tospeex);

#endif /* WITHSKIPUSERMODE */

//	ASSERT((DMABUFFSIZE_UACIN % HARDWARE_RTSDMABYTES) == 0);
//	ASSERT((DMABUFFSIZE192RTS % HARDWARE_RTSDMABYTES) == 0);
//	ASSERT((DMABUFFSIZE96RTS % HARDWARE_RTSDMABYTES) == 0);

	#if WITHRTS192 && WITHUSBUACIN && WITHUSBHW && defined (WITHUSBHW_DEVICE)

	subscribeint32(& rtstargetsint, & uacinrtssubscribe, NULL, savesampleout192stereo);

	#elif WITHRTS96 && WITHUSBUACIN && WITHUSBHW && defined (WITHUSBHW_DEVICE)

	subscribeint32(& rtstargetsint, & uacinrtssubscribe, NULL, savesampleout96stereo);

	#endif /* WITHRTS192 */



#endif /* WITHINTEGRATEDDSP */
}
