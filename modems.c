/* $Id$ */
//

#include "hardware.h"	/* зависящие от процессора функции работы с портами */
#include "board.h"
#include "audio.h"
#include "spifuncs.h"
#include "formats.h"	// for debug prints
#include "display.h"	// for debug prints

#include <string.h>
#include <math.h>
#include <ctype.h>
#include <assert.h>

#if WITHMODEM

//////////////////
// ++++++

// состояния конечного автомата модулятора
enum modem_tx_states
{
	MODEM_TX_STOPPED,
	MODEM_TX_VALUE_SCRAMBLE,
	MODEM_TX_VALUE_NO_SCRAMBLE,
	MODEM_TX_CONST,
	MODEM_TX_PAD1,
	MODEM_TX_MARKER,
	MODEM_TX_BODYLENGTH,
	MODEM_TX_BODY,
	MODEM_TX_CRC,
	MODEM_TX_PAD2,
	//
	MODEM_TX_STATES
};

// CRC-CCITT calculations:
// http://www.barrgroup.com/Embedded-Systems/How-To/CRC-Calculation-C-Code

static const uint_fast32_t MODEM_CRC_POLYNOM = (0x1021);	// CRC-CCITT
static const uint_fast32_t MODEM_CRC_INITVAL = 0xFFFF;
static const uint_fast32_t MODEM_CRC_MASK = 0xFFFF;
static const uint_fast32_t MODEM_CRC_BITS = 16;

// Параметры скремблера
static const uint_fast32_t MODEM_SCRAMBLER_POLYNOM = (0x21);
static const uint_fast32_t MODEM_SCRAMBLER_INITVAL = (0x7F);
static const uint_fast32_t MODEM_SCRAMBLER_LASTBIT = ((uint_fast32_t) 1 << 6);

static const uint_fast32_t MODEM_MARKER = 0xAB5D;
static const uint_fast32_t MODEM_MARKER_MASK = 0xFFFF;
static const uint_fast32_t MODEM_MARKER_BITS = 16;

// Общие пераметры пакета
static const uint_fast32_t MODEM_HEAD_BITS = 24;	// длина синхронизирующей последовательности в начале пакета
static const uint_fast32_t MODEM_OCTET_BITS = 8;
static const uint_fast32_t MODEM_TAIL_BITS = 8;

static const uint_fast32_t MODEM_SIZE_MASK = 0x03FF;	// Зависит от MODEMBUFFERSIZE8
static const uint_fast32_t MODEM_SIZE_BITS = 10;

enum { TXBPSKPAGES = 2 };

#define	MODEMBINADDRESSSIZE	12		// байтов в буфере адреса

typedef struct txbpskholder_tag
{
	unsigned activetxbpsksize;		// количество достоверных байтов в буфере
	uint8_t activetxbpskdata [MODEMBUFFERSIZE8];	// буфер с данными для передачи
} txdataholder_t;

static uint8_t ownaddressbuff [MODEMBINADDRESSSIZE];		// свой собственный адрес
static uint_fast8_t mastermode;		// 0 – slave, 1 - master

static txdataholder_t bhs [TXBPSKPAGES];
static volatile uint_fast8_t activetxbpskpage;

static uint_fast8_t modem_tx_state = MODEM_TX_STOPPED;
static uint_fast8_t modem_tx_return = MODEM_TX_STOPPED;
static uint_fast32_t modem_tx_crc;	/* накопление CRC для передачи */
static uint_fast32_t modem_tx_scrambler;
static uint_fast32_t modem_tx_acc;	/* битовая последовательность для модулятора */
static uint_fast32_t modem_tx_bitmask;/* положение бита в аккумуляторе для передачи */
static uint_fast32_t modem_tx_bitcount;/* количество одинаковых передаваемых битов */
static uint_fast32_t modem_tx_level;		// текущая позиция для передачи данных

// состояния конечного автомата демодулятора
enum modem_rx_states
{
	MODEM_RX_STOPPED,
	MODEM_RX_MARKER,
	MODEM_RX_BODYLENGTH,
	MODEM_RX_BODY,
	MODEM_RX_CRC,
	//
	MODEM_RX_STATES
};

static uint_fast8_t modem_rx_state = MODEM_RX_STOPPED;
static uint_fast32_t modem_rx_crc;	/* накопление CRC для приёма */
static uint_fast32_t modem_rx_acc;	/* битовая последовательность выхода демодулятора */
static uint_fast32_t modem_rx_scrambler;
static uint_fast32_t modem_rx_bits;	/* количество битов в аккумуляторе */
static uint_fast32_t modem_rx_length;/* длительность нахождения в состоянии (многофункциональная переменная) */
static uint8_t * modem_rx_buffer;
static size_t modem_rx_maxlength;	// размер буфера для сохранения данных принятых из радиоканала
static uint_fast32_t modem_rx_bodylen;	// принятый размер

// функции

// Обновление CRC для очередного бита
static uint_fast32_t crcupdate(
	uint_fast32_t crc,
	uint_fast8_t v		// очередной бит
	)
{
	const uint_fast32_t MODEM_CRC_LASTBIT = (uint_fast32_t) 1 << (MODEM_CRC_BITS - 1);
	if (((MODEM_CRC_LASTBIT & crc) != 0) != (v != 0))
		return (crc * 2) ^ MODEM_CRC_POLYNOM;
	else
		return (crc * 2);
}


// Обновление скремблера для очередного бита
static uint_fast32_t scramblerpdate(
	uint_fast32_t crc
	)
{
	if ((MODEM_SCRAMBLER_LASTBIT & crc) == 0)
		return (crc * 2) ^ MODEM_SCRAMBLER_POLYNOM;
	else
		return (crc * 2);
}


static void
modem_tx_pushstate_value_scrambled(
	uint_fast8_t state,		// состояне после возврата
	uint_fast32_t value,		// значение для передачи
	uint_fast32_t length		// количество бит
	)
{
	modem_tx_state = MODEM_TX_VALUE_SCRAMBLE;
	modem_tx_return = state;
	modem_tx_acc = value;
	modem_tx_bitmask = (uint_fast32_t) 1 << (length - 1);
	modem_tx_bitcount = length;
}


static void
modem_tx_pushstate_value_not_scrambled(
	uint_fast8_t state,		// состояне после возврата
	uint_fast32_t value,		// значение для передачи
	uint_fast32_t length		// количество бит
	)
{
	modem_tx_state = MODEM_TX_VALUE_NO_SCRAMBLE;
	modem_tx_return = state;
	modem_tx_acc = value;
	modem_tx_bitmask = (uint_fast32_t) 1 << (length - 1);
	modem_tx_bitcount = length;
}

static void
modem_tx_pushstate_bits(
	uint_fast8_t state,		// состояне после возврата
	uint_fast8_t value,		// значение для передачи (младший бит)
	uint_fast32_t length		// количество бит
	)
{
	modem_tx_state = MODEM_TX_CONST;
	modem_tx_return = state;
	modem_tx_acc = value != 0;
	modem_tx_bitmask = 0x01;
	modem_tx_bitcount = length;
}

// сброс скрэмблера передатчика
static void modem_tx_initscrambler(void)
{
	modem_tx_scrambler = MODEM_SCRAMBLER_INITVAL;
}

// Возврат 0/1 - выход скрэмблера передатчика
static uint_fast8_t modem_tx_nextscramblebit(void)
{
	modem_tx_scrambler = scramblerpdate(modem_tx_scrambler);
	return modem_tx_scrambler & 0x01;
}

// сброс скрэмблера приёмника
static void modem_rx_initscrambler(void)
{
	modem_rx_scrambler = MODEM_SCRAMBLER_INITVAL;
}

// Возврат 0/1 - выход скрэмблера приёмника
static uint_fast8_t modem_rx_nextscramblebit(void)
{
	modem_rx_scrambler = scramblerpdate(modem_rx_scrambler);
	return modem_rx_scrambler & 0x01;
}

// функция перехода по конечному автомату состояний передатчика
static void modem_tx_frame_nextstate(
	uint_fast8_t suspend	// передавать модему ещё рано - не полностью завершено формирование огибающей
	)
{
	if (suspend)
		return;

	switch (modem_tx_state)
	{
	case MODEM_TX_PAD1:
		modem_tx_pushstate_bits(MODEM_TX_MARKER, 1, MODEM_HEAD_BITS);
		break;
	case MODEM_TX_MARKER:
		modem_tx_pushstate_value_not_scrambled(MODEM_TX_BODYLENGTH, MODEM_MARKER, MODEM_MARKER_BITS);
		break;

	case MODEM_TX_BODYLENGTH:
		{
			txdataholder_t * const bh = & bhs [activetxbpskpage];
			modem_tx_initscrambler();
			modem_tx_crc = MODEM_CRC_INITVAL;
			modem_tx_pushstate_value_scrambled(MODEM_TX_BODY, bh->activetxbpsksize - 1, MODEM_SIZE_BITS);
			modem_tx_level = 0;
		}
		break;

	case MODEM_TX_BODY:
		{
			txdataholder_t * const bh = & bhs [activetxbpskpage];
			if (modem_tx_level < bh->activetxbpsksize)
				modem_tx_pushstate_value_scrambled(MODEM_TX_BODY, bh->activetxbpskdata [modem_tx_level ++], MODEM_OCTET_BITS);
			else
			{
				modem_tx_pushstate_value_scrambled(MODEM_TX_PAD2, modem_tx_crc, MODEM_CRC_BITS);
				bh->activetxbpsksize = 0; /* Все данные использованы - подготавливаем буфер к заполнению */
			}
		}
		break;

	case MODEM_TX_PAD2:
		modem_tx_pushstate_bits(MODEM_TX_STOPPED, 1, MODEM_TAIL_BITS);
		break;
	}

}

// формирование бита для передачи
static uint_fast8_t modem_frame_getnextbit(
	uint_fast8_t suspend	// передавать модему ещё рано - не полностью завершено формирование огибающей
	 )
{
	if (suspend)
		return 1;
	uint_fast8_t v = ((modem_tx_acc & modem_tx_bitmask) != 0);
	modem_tx_crc = crcupdate(modem_tx_crc, v);
	switch (modem_tx_state)
	{
	default:
		v = 0;
		break;

	case MODEM_TX_VALUE_SCRAMBLE:
		v ^= modem_tx_nextscramblebit();
	case MODEM_TX_VALUE_NO_SCRAMBLE:
		modem_tx_bitmask /= 2;
	case MODEM_TX_CONST:
		if (-- modem_tx_bitcount == 0)
		{
			modem_tx_state = modem_tx_return;
		}
		break;
	}


	return v;
}

// обработка бита на приёме
static void
modem_frames_decode(
	uint_fast8_t v
	)
{
	//printf("modem_frames_decode: %d (st=%d, acc=%08lX)\n", v, modem_rx_state, modem_rx_acc);
	switch (modem_rx_state)
	{
	case MODEM_RX_STOPPED:
		// игнорирование потока
		modem_rx_maxlength = takemodembuffer_low(& modem_rx_buffer);
		modem_rx_state = MODEM_RX_MARKER;
		break;

	case MODEM_RX_MARKER:
		// поиск маркера при приёме
		modem_rx_acc = modem_rx_acc * 2 + (v != 0);
		if (++ modem_rx_bits >= MODEM_MARKER_BITS)
		{
			if ((modem_rx_acc & MODEM_MARKER_MASK) == MODEM_MARKER)
			{
				modem_rx_state = MODEM_RX_BODYLENGTH;
				modem_rx_bits = 0;
				modem_rx_initscrambler();
				modem_rx_crc = MODEM_CRC_INITVAL;
				modem_rx_length = 0;
			}
			else
			{
				// продолжаем поиск макрера
				modem_rx_bits = MODEM_MARKER_BITS;
			}
		}
		break;

	case MODEM_RX_BODYLENGTH:
		// получение размера блока (MODEM_SIZE_BITS бит)
		{
			const uint_fast8_t databit = (v != 0) ^ modem_rx_nextscramblebit();
			modem_rx_acc = modem_rx_acc * 2 + databit;
			modem_rx_crc = crcupdate(modem_rx_crc, databit);
			if (++ modem_rx_bits >= MODEM_SIZE_BITS)
			{
				modem_rx_bits = 0;
				modem_rx_bodylen = 1 + (modem_rx_acc & MODEM_SIZE_MASK);
				if (modem_rx_bodylen > modem_rx_maxlength)
					modem_rx_state = MODEM_RX_MARKER;	// ошибка - пытаемся принять следующий блок
				else
					modem_rx_state = MODEM_RX_BODY;
			}
		}
		break;

	case MODEM_RX_BODY:
		// получение полезных данных блока
		{
			const uint_fast8_t databit = (v != 0) ^ modem_rx_nextscramblebit();
			modem_rx_acc = modem_rx_acc * 2 + databit;
			modem_rx_crc = crcupdate(modem_rx_crc, databit);
			if (++ modem_rx_bits >= 8)
			{
				modem_rx_bits = 0;
				if (modem_rx_buffer != NULL)
				{
					modem_rx_buffer [modem_rx_length ++] = modem_rx_acc;
					if (modem_rx_length >= modem_rx_bodylen)
					{
						modem_rx_state = MODEM_RX_CRC;
					}
				}
			}
		}
		break;

	case MODEM_RX_CRC:
		{
			const uint_fast8_t databit = (v != 0) ^ modem_rx_nextscramblebit();
			//modem_rx_acc = modem_rx_acc * 2 + databit;
			modem_rx_crc = crcupdate(modem_rx_crc, databit);
			if (++ modem_rx_bits >= 16)	// количество битов в CRC
			{
				if ((modem_rx_crc & MODEM_CRC_MASK) == 0)
				{
					// Принят правильный блок данных
					savemodemrxbuffer_low(modem_rx_buffer, modem_rx_length);
					modem_rx_maxlength = takemodembuffer_low(& modem_rx_buffer);
				}
				else
				{
					// блок данных принят с ошибкой
				}
				modem_rx_state = MODEM_RX_MARKER;	// пытаемся принять следующий блок
			}
		}
		break;
	}
}

// -----

// Вызывается из обработчика - NMEA PARSER
// Формирование блока данных для передачи - сохранение очередного бацта
static void modem_placenextchartosend(uint_fast8_t page, uint_fast8_t c)
{
	if (page >= TXBPSKPAGES)
		return;
	txdataholder_t * const bh = & bhs [page];
	if (bh->activetxbpsksize < MODEMBUFFERSIZE8)
	{
		bh->activetxbpskdata [bh->activetxbpsksize ++] = c;
	}
}

// call from modem_spool - timer irq handler
static unsigned modem_getbuffsize(uint_fast8_t page)
{
	if (page >= TXBPSKPAGES)
		return 0;
	txdataholder_t * const bh = & bhs [page];
	return bh->activetxbpsksize;
}


// Вызывается из обработчика - NMEA PARSER
// Начать передачу (конец накопления дянных на передачу)
static void modem_flushsend(
	uint_fast8_t page
	)
{
	if (page >= TXBPSKPAGES)
		return;
	global_disableIRQ();
	activetxbpskpage = page;
	txdataholder_t * const bh = & bhs [page];
	if (bh->activetxbpsksize != 0)
	{
		modem_tx_state = MODEM_TX_PAD1;
	}
	global_enableIRQ();
}

// Вызывается из обработчика - NMEA PARSER
// Прервать передачу
static void modem_cancelsending(uint_fast8_t page)
{
	if (page >= TXBPSKPAGES)
		return;
	txdataholder_t * const bh = & bhs [page];
	// очищаем буфер для следующего заполнения
	global_disableIRQ();
	bh->activetxbpsksize = 0;
	if (activetxbpskpage == page)
		modem_tx_state = MODEM_TX_STOPPED;
	global_enableIRQ();
}


//++++++++++++++++

// Очереди символов для обмена с host 
enum { qSZ = MODEMBUFFERSIZE8 * 4 };
static uint8_t queue [qSZ];
static volatile unsigned qp, qg;

// Передать символ в host
static uint_fast8_t	qput(uint_fast8_t c)
{
	unsigned qpt = qp;
	const unsigned next = (qpt + 1) % qSZ;
	if (next != qg)
	{
		queue [qpt] = c;
		qp = next;
		return 1;
	}
	return 0;
}

// Получить символ в host
static uint_fast8_t qget(uint_fast8_t * pc)
{
	if (qp != qg)
	{
		* pc = queue [qg];
		qg = (qg + 1) % qSZ;
		return 1;
	}
	return 0;
}

// получить состояние очереди передачи
static uint_fast8_t qempty(void)
{
	return qp == qg;
}

// Передать массив символов
static void qputs(const char * s, int n)
{
	const int f = n != 0;
	while (n --)
		qput(* s ++);

	if (f)
		HARDWARE_MODEM_ENABLETX(1);

}

// RX
// ---------

static uint_fast32_t bps31_tx_bitrateFTW = 0;


// Возвращает не-0 каждые 32 мс (31.25 Гц) - вызывается с частотой ARMI2SRATE
static int bpsk31_phase_tick(void)
{
	static uint_fast32_t bps31_tx_bitrateNCO;
	auto uint_fast32_t old = bps31_tx_bitrateNCO;
	return ((bps31_tx_bitrateNCO += bps31_tx_bitrateFTW) < old);
}

/* функция вызывается из пользовательской программы. */
// вызывается из user mode
// Возвращает не-0 для запроса на переход в режим передачи
uint_fast8_t 
modem_get_ptt(void)
{
	global_disableIRQ();
	//txdataholder_t * const bh = & bhs [activetxbpskpage];
	uint_fast8_t f = modem_tx_state != MODEM_TX_STOPPED;
	global_enableIRQ();

	return f;
}


static uint_fast8_t modem_getnextbit(
	uint_fast8_t suspend	// передавать модему ещё рано - не полностью завершено формирование огибающей
	 )
{
	modem_tx_frame_nextstate(suspend);
	return modem_frame_getnextbit(suspend);
}


// return 0/1 for 0/PI
static 
uint_fast8_t 
pbsk_get_phase(
	uint_fast8_t suspend	// передавать модему ещё рано - не полностью завершено формирование огибающей
	)
{
	static uint_fast8_t phase;

	if (bpsk31_phase_tick() && modem_getnextbit(suspend))
	{
		phase = ! phase;
	}
	return phase;
}

static volatile uint_fast8_t	glob_modemmode;		// применяемая модуляция (bpsk/qpsk)
static volatile uint_fast32_t	glob_modemspeed100;	// скорость передачи с точностью 1/100 бод

static unsigned short m_RxBitPhase;
static unsigned short m_RxBitFreqFTW;

static void bpsk_demod_initialize(void)
{
	m_RxBitPhase = 0;

	//debug_printf_P(PSTR("bpsk_demod_initialize: m_RxBitFreqFTW=%d\n"), m_RxBitFreqFTW);
}

// демодулятор BPSK
static void demod_bpsk2_symbol(int_fast32_t i, int_fast32_t q, int level)
{
	static int_fast32_t oldi, oldq;
	const int_fast64_t dot = (int_fast64_t) oldi * i + (int_fast64_t) oldq * q;
	const int_fast8_t bitv = ! (dot > 0);
	modem_frames_decode(bitv);

	oldi = i;
	oldq = q;
}




static void demod_bpsk(int_fast32_t RxSin, int_fast32_t RxCos)
{

	enum 
	{
		BITFILTERLENGTH = 16,	// не трогать
	};

	typedef long SAMPLEHOLDER_T;
		//saved values for bit synchronization filter taps
	static SAMPLEHOLDER_T m_RxAmpFil [BITFILTERLENGTH];

	{	
		const SAMPLEHOLDER_T ISum = RxSin;
		const SAMPLEHOLDER_T QSum = RxCos;

		int	level = 30;
		long ampl = (ISum >> 16) * (ISum >> 16) + (QSum >> 16) * (QSum >> 16);

		// логарифм по основнияю 2
		while (ampl > 0)
		{
			level += 1;
			ampl >>= 1;
		}

		//Select 1 of 16
		const int BitPhaseInt = m_RxBitPhase >> 12;
		ASSERT(BitPhaseInt < BITFILTERLENGTH);
		m_RxAmpFil [BitPhaseInt] = (SAMPLEHOLDER_T)level;

		int i;
		ampl = 0;
		for (i = 0; i < BITFILTERLENGTH/2; i += 1)
		{
			ampl += m_RxAmpFil[i] - m_RxAmpFil [i + (BITFILTERLENGTH/2)];
		}

		//The correction is the amplitude times a synchronization gain, which is empirical.
		const long BitPhaseCorrection = (long) (ampl * 4);

		long NextSymPhase;	//long enough to contain the 17th bit when 16 bit adds overflow
		NextSymPhase = (long)m_RxBitPhase + (long) m_RxBitFreqFTW - BitPhaseCorrection;
		m_RxBitPhase = (unsigned short)NextSymPhase;

		if (NextSymPhase > 0xFFFF)
		{
			//We're at the centre of the bit:  31.25 Hz
			demod_bpsk2_symbol(ISum, QSum, level);
		}
	}
}

/////////////////////////////
// Интерфейсная функция демодулятора
// Вызыается с частотой ARMI2SRATE герц
// iq - квадратура, полученная от радиотракта

void modem_demod_iq(FLOAT32P_t iq)
{
	switch (glob_modemmode)
	{
	case 0:
		// BPSK
		demod_bpsk(iq.IV, iq.QV);
		break;
	case 1:
		// QPSK
		//demod_qpsk(iq.IV, iq.QV);
		break;
	}
}
// Интерфейсная функция модулятора
// Вызыается с частотой ARMI2SRATE герц
// версия для высокоскоростных модемов
int modem_get_tx_b(
	uint_fast8_t suspend	// передавать модему ещё рано - не полностью завершено формирование огибающей
	)
{

	return pbsk_get_phase(suspend);	
}
// Интерфейсная функция модулятора
// Вызыается с частотой ARMI2SRATE герц
// версия для низкоскоростных модемов
FLOAT32P_t modem_get_tx_iq(
	uint_fast8_t suspend	// передавать модему ещё рано - не полностью завершено формирование огибающей
	)
{
	switch (glob_modemmode)
	{
	case 0:
		// BPSK
		{
			const int_fast32_t ph = (1 - pbsk_get_phase(suspend) * 2);
			const FLOAT32P_t iq = { { ph, ph } };
			return iq;
		}
	case 1:
		// QPSK
		{
			const FLOAT32P_t iq = { { 1, 0 } };
			return iq;
		}
	default:
		{
			const FLOAT32P_t iq = { { 1, 0 } };
			return iq;
		}

	}
}


static void modem_set_tx_speed(uint_fast32_t speed100)
{
	// TX
	bps31_tx_bitrateFTW = ((uint_fast64_t) speed100 << 32) / (ARMI2SRATE100);
}

static void modem_set_rx_speed(uint_fast32_t speed100)
{
	// RX
	m_RxBitFreqFTW = (((uint_fast64_t) speed100 << 16) / (ARMI2SRATE100));
}


/* Установить скорость, параметр с точностью 1/100 бод */
void modem_set_speed(uint_fast32_t speed100)
{
	if (glob_modemspeed100 != speed100)
	{
		glob_modemspeed100 = speed100;

		modem_set_tx_speed(speed100);
		modem_set_rx_speed(speed100);
	}
}

/* Установить модуляцию для модема */
void modem_set_mode(uint_fast8_t modemmode)
{
	if (glob_modemmode != modemmode)
	{
		glob_modemmode = modemmode;	
	}
}


/* вызывается при разрешённых прерываниях. */
void modem_initialze(void)
{
	// получение признака работы MASTER
#if CTLREGMODE_STORCH_V4
	arm_hardware_piof_outputs(0x0002, 0x0002);
	arm_hardware_piof_inputs(0x0001);
	local_delay_ms(100);
	mastermode = (GPIOF->IDR & 0x01) == 0;
#else
	mastermode = 1;
#endif /* CTLREGMODE_STORCH_V4 */

#if defined (UID_BASE)
	// формирование буфера собственного адреса
	const uint32_t * const uidbase = (const uint32_t *) UID_BASE;
	ownaddressbuff [0x00] = uidbase [0] >> 24;
	ownaddressbuff [0x01] = uidbase [0] >> 16;
	ownaddressbuff [0x02] = uidbase [0] >> 8;
	ownaddressbuff [0x03] = uidbase [0] >> 0;
	ownaddressbuff [0x04] = uidbase [1] >> 24;
	ownaddressbuff [0x05] = uidbase [1] >> 16;
	ownaddressbuff [0x06] = uidbase [1] >> 8;
	ownaddressbuff [0x07] = uidbase [1] >> 0;
	ownaddressbuff [0x08] = uidbase [2] >> 24;
	ownaddressbuff [0x09] = uidbase [2] >> 16;
	ownaddressbuff [0x0A] = uidbase [2] >> 8;
	ownaddressbuff [0x0B] = uidbase [2] >> 0;
#endif

	bpsk_demod_initialize();

	modem_set_speed(3125);		// default value: 31.25 baud
}


enum nmeaparser_states
{
	NMEAST_INITIALIZED,
	NMEAST_OPENED,	// встретился символ '$'
	NMEAST_CHSHI,	// прём старшего символа контрольной суммы
	NMEAST_CHSLO,	// приём младшего символа контрольной суммы


	//
	NMEAST_COUNTSTATES

};


static uint_fast8_t nmeaparser_state = NMEAST_INITIALIZED;
static uint_fast8_t nmeaparser_checksum;
static uint_fast8_t nmeaparser_chsval;
static uint_fast8_t nmeaparser_param;		// номер принимаемого параметра в строке
static uint_fast8_t nmeaparser_chars;		// количество символов, помещённых в буфер

#define NMEA_PARAMS			5
#define NMEA_CHARSSMALL		16
#define NMEA_CHARSBIG		257
#define NMEA_BIGFIELD		3	// номер большого поля

static char nmeaparser_buffsmall [NMEA_PARAMS] [NMEA_CHARSSMALL];
static char nmeaparser_buffbig [NMEA_CHARSBIG];

static unsigned nmeaparser_get_buffsize(uint_fast8_t field)
{
	switch (field)
	{
	case NMEA_BIGFIELD:
		return NMEA_CHARSBIG;
	default:
		return NMEA_CHARSSMALL;
	}
}

static char * nmeaparser_get_buff(uint_fast8_t field)
{
	switch (field)
	{
	case NMEA_BIGFIELD:
		return nmeaparser_buffbig;
	default:
		return nmeaparser_buffsmall [field];
	}
}

static unsigned char calcxorv(
	const char * s,
	size_t len
	)
{
	unsigned char r = '*';
	while (len --)
		r ^= (unsigned char) * s ++;
	return r & 0xff;
}

static unsigned char hex2int(uint_fast8_t c)
{
	if (isdigit(c))
		return c - '0';
	if (isupper(c))
		return c - 'A' + 10;
	if (islower(c))
		return c - 'a' + 10;
	return 0;
}

static size_t nmeaparser_sendbin_buffer(int index, const uint8_t * databuff, size_t datalen)
{
	static const char bin2hex [] = "0123456789ABCDEF";
	size_t pos;
	char buff [NMEA_CHARSBIG + 30];
	const size_t size = sizeof buff / sizeof buff [0];
	const size_t TAILLEN = (1 + 2 + 2 + 1);	// *HH\r\n\0 at end
	// Формирование заголовка
	size_t len = local_snprintf_P(
		buff, 
		size, 
		PSTR("$GPMDR,%d,%d,"),
		(int) 1,
		(int) index
		);

	// Формирование тела данных
	for (pos = 0; pos < datalen && len < (size - len - TAILLEN) && pos < 64; )
	{
		const uint_fast8_t v = (uint8_t) databuff [pos ++];
		buff [len ++] = bin2hex [v / 16];
		buff [len ++] = bin2hex [v % 16];
	}
	// Добавление разделителя
	buff [len ++] = '*';

	// Подсчет контрольной суммы
	{
		const uint_fast8_t v = calcxorv(buff + 1, len - 1);
		buff [len ++] = bin2hex [v / 16];
		buff [len ++] = bin2hex [v % 16];
	}

	buff [len ++] = '\r';
	buff [len ++] = '\n';
	buff [len] = '\0';
		
	qput(0xff);
	qputs(buff, len);
	return pos;
}

static unsigned ticks;
static unsigned volatile rxerrchar;

static int numvolts1(int val)	// Напряжение в сотнях милливольт т.е. 151 = 15.1 вольта
{
	return val / 10;
}
static int numvolts01(int val	)// Напряжение в сотнях милливольт т.е. 151 = 15.1 вольта
{
	if (val < 0)
		val = - val;
	return val % 10;
}

static int numamps1(int val)	// Ток в десятках милиампер (до 2.55 ампера), может быть отрицательным
{
	return val / 100;
}
static int numamps001(int val)	// Ток в десятках милиампер (до 2.55 ампера), может быть отрицательным
{
	if (val < 0)
		val = - val;
	return val % 100;
}


static int 
isownaddressmatch(
	uint8_t * buff,
	size_t len
	)
{
#if CTLREGMODE_STORCH_V4
	if (len < (MODEMBINADDRESSSIZE * 2))
		return 0;	// сообщение меньше чем надо для двух полей адреса - не совпало
	return memcmp(ownaddressbuff, buff + len - MODEMBINADDRESSSIZE * 2, MODEMBINADDRESSSIZE) == 0;
#else /* CTLREGMODE_STORCH_V4 */
	return 1;
#endif /* CTLREGMODE_STORCH_V4 */
}

void modem_spool(void)
{
	size_t len;
	uint8_t * buff;
	enum { NTS = NTICKS(1000) };

	if (++ ticks >= NTS)
	{
		ticks = 0;
		static unsigned seq;

		char buff [100];
#if CTLREGMODE_STORCH_V4
		// new version
		// $GPMDR,2,160550000,30000,13.4,+1.55,75,003E003A3234510D37343138,NN*HH
		const uint32_t * const uidbase = (const uint32_t *) UID_BASE;
		const uint_fast8_t volt = hamradio_get_volt_value();	// Напряжение в сотнях милливольт т.е. 151 = 15.1 вольта
		const int_fast16_t drain = hamradio_get_pacurrent_value();	// Ток в десятках милиампер (до 2.55 ампера), может быть отрицательным
		const size_t len = local_snprintf_P(buff, sizeof buff / sizeof buff [0], 
			PSTR("$GPMDR,"
			"%ld,"	// type of information
			"%ld,"	// freq
			"%ld,"	// baudrate * 100
			"%d.%d,"	// voltage
			"%+d.%02d,"	// current
			"%d,"	// mastermode
			"%08lX%08lX%08lX,"	// uid
			"%u,"	// buff0
			"%u,"	// buff1
			"%d*"),  
			2L, 
			(long) hamradio_get_freq_rx(), 
			(long) hamradio_get_modem_baudrate100(), 
			numvolts1(volt), numvolts01(volt),
			numamps1(drain), numamps001(drain),
			mastermode != 0,
			(unsigned long) uidbase [0], (unsigned long) uidbase [1], (unsigned long) uidbase [2], 
			modem_getbuffsize(0),
			modem_getbuffsize(1),
			seq ++
			);
#else /* CTLREGMODE_STORCH_V4 */
		// ADACTA version
		const size_t len = local_snprintf_P(buff, sizeof buff / sizeof buff [0], 
			PSTR("$GPMDR,%ld,%ld,%u,%u,%d,%d*"),  
			2L, 
			(long) hamradio_get_freq_rx(), 
			seq ++, 
			rxerrchar, 
			gettxstate(), 
			modem_rx_state
			);
#endif /* CTLREGMODE_STORCH_V4 */
		unsigned xorv = calcxorv(buff + 1, len - 1);
		const size_t len2 = local_snprintf_P(buff + len, sizeof buff / sizeof buff [0] - len, PSTR("%02X\r\n"), xorv);
		qput(0xff);
		qputs(buff, len + len2);
	}
	
	len = takemodemrxbuffer(& buff);
	if (buff != NULL)
	{
		if (len != 0)
		{
			//  Если есть буфер с принятыми данными - передать в компьютер
			if (mastermode != 0 || isownaddressmatch(buff, len))
			{
				size_t pos = 0;
				int index = 0;
				do
					pos += nmeaparser_sendbin_buffer(index ++, buff + pos, len - pos);
				while (pos < len);
			}
		}
		releasemodembuffer(buff);
	}
}

static volatile uint_fast8_t paramschangedfreq;
static volatile uint_fast32_t modemfreq;
static volatile uint_fast8_t paramschangedspeed;
static volatile uint_fast32_t modemspeed100;
static volatile uint_fast8_t paramschangedmode;
static volatile uint_fast32_t modemmode;

// Функция обновления (частоты, ...) - вызывается из основного цикла программы, user mode.

uint_fast8_t processmodem(void)
{
	uint_fast8_t any = 0;	// если что-то поменялось в режимах
	uint_fast8_t f;

	disableIRQ();
	f = paramschangedfreq;
	paramschangedfreq = 0;
	enableIRQ();

	if (f)
	{
		any = 1;
		modemchangefreq(modemfreq);
	}

	disableIRQ();
	f = paramschangedspeed;
	paramschangedspeed = 0;
	enableIRQ();

	if (f)
	{
		any = 1;
		modemchangespeed(modemspeed100);
	}

	disableIRQ();
	f = paramschangedmode;
	paramschangedmode = 0;
	enableIRQ();

	if (f)
	{
		any = 1;
		modemchangemode(modemmode);
	}
	
	return any;	
}

/* вызывается из обработчика прерываний */
// принятый символ с последовательного порта
void modem_parsechar(uint_fast8_t c)
{
	switch (nmeaparser_state)
	{
	case NMEAST_INITIALIZED:
		if (c == '$')
		{
			nmeaparser_checksum = '*';
			nmeaparser_state = NMEAST_OPENED;
			nmeaparser_param = 0;		// номер принимаемого параметра в строке
			nmeaparser_chars = 0;		// количество символов, помещённых в буфер
		}
		break;

	case NMEAST_OPENED:
		nmeaparser_checksum ^= c;
		if (c == ',')
		{
			// закрываем буфер параметра, переходим к следующему параметру
			nmeaparser_get_buff(nmeaparser_param) [nmeaparser_chars] = '\0';
			nmeaparser_param += 1;
			nmeaparser_chars = 0;
		}
		else if (c == '*')
		{
			// закрываем буфер параметра, переходим к следующему параметру
			nmeaparser_get_buff(nmeaparser_param) [nmeaparser_chars] = '\0';
			nmeaparser_param += 1;
			// переходим к приёму контрольной суммы
			nmeaparser_state = NMEAST_CHSHI;
		}
		else if (nmeaparser_param < NMEA_PARAMS && nmeaparser_chars < (nmeaparser_get_buffsize(nmeaparser_param) - 1))
		{
			nmeaparser_get_buff(nmeaparser_param) [nmeaparser_chars] = c;
			nmeaparser_chars += 1;
			//stat_l1 = stat_l1 > nmeaparser_chars ? stat_l1 : nmeaparser_chars;
		}
		else
			nmeaparser_state = NMEAST_INITIALIZED;	// при ошибках формата строки
		break;

	case NMEAST_CHSHI:
		nmeaparser_chsval = hex2int(c) * 16;
		nmeaparser_state = NMEAST_CHSLO;
		break;

	case NMEAST_CHSLO:
		//debugstate();
		nmeaparser_state = NMEAST_INITIALIZED;
		////if (nmeaparser_checksum == (nmeaparser_chsval + hex2int(c)))	// для тесто проверка контрольной суммы отключена
		{
			if (nmeaparser_param >= 2 && strcmp(nmeaparser_get_buff(0), "GPMDS") == 0)
			{
				const unsigned code = strtoul(nmeaparser_get_buff(1) , NULL, 10);
				if (nmeaparser_param >= 4)
				{
					const unsigned page = strtoul(nmeaparser_get_buff(2) , NULL, 10);
					switch (code)
					{
					case 1:
						// заполняем буфер
						{

							char * const buff = nmeaparser_get_buff(3);
							const size_t j = strlen(buff);
							unsigned i;
							for (i = 0; (i + 1) < j; i += 2)
							{
								unsigned v = hex2int(buff [i + 0]);
								v = v * 16 + hex2int(buff [i + 1]);
								modem_placenextchartosend(page, v);
							}
						}
						break;
					case 2:
						// Ранее накопленные данные передать с указанием адреса получателя
						{
							char * const buff = nmeaparser_get_buff(3);
							const size_t j = strlen(buff);
							unsigned i;
							for (i = 0; (i + 1) < j && i < (MODEMBINADDRESSSIZE * 2); i += 2)
							{
								unsigned v = hex2int(buff [i + 0]);
								v = v * 16 + hex2int(buff [i + 1]);
								modem_placenextchartosend(page, v);
							}
#if CTLREGMODE_STORCH_V4
							// target addrss
							for (; i < (MODEMBINADDRESSSIZE * 2); i += 2)
							{
								modem_placenextchartosend(page, 0);
							}
							// sender's address
							for (i = 0; i < MODEMBINADDRESSSIZE; ++ i)
							{
								modem_placenextchartosend(page, ownaddressbuff [i]);
							}
#endif /* CTLREGMODE_STORCH_V4 */
							modem_flushsend(page);
						}
						break;

					}
				}
				else if (nmeaparser_param >= 3)
				{
					const unsigned p2 = strtoul(nmeaparser_get_buff(2) , NULL, 10);
					switch (code)
					{
					case 2:
						// Ранее накопленные данные передать к мастеру (без указания адреса олучателя)
						{
#if CTLREGMODE_STORCH_V4
							unsigned i;
							// target address
							for (i = 0; i < MODEMBINADDRESSSIZE; ++ i)
							{
								modem_placenextchartosend(p2, 0);
							}
							// sender's address
							for (i = 0; i < MODEMBINADDRESSSIZE; ++ i)
							{
								modem_placenextchartosend(p2, ownaddressbuff [i]);
							}
#endif /* CTLREGMODE_STORCH_V4 */
							modem_flushsend(p2);
						}
						break;

					case 3:
						modem_cancelsending(p2);
						// Сбросить ранее накопленные данные (не передавать)
						break;
					case 4:
						// Установить рабочую частоту
						// Частота в герцах
						{
							modemfreq = p2;
							paramschangedfreq = 1;
							
						}
						break;
					case 5:
						// Установить скорость передачи в эфире
						// Baud rate (в сотых долях бода)
						{
							modemspeed100 = p2;
							paramschangedspeed = 1;
						}
						break;
					case 6:
						// Установить тип модуляции
						// 0 – BPSK, 1 - QPSK
						{
							modemmode = p2;
							paramschangedmode = 1;
						}
						break;
					case 7:
						// Установить режтм арбты себя в режиме мастер (1) или слэйв (0) - 0 по включению питания
						// 0 – slave, 1 - master
						{
							mastermode = p2;
						}
						break;
					}
				}
			}
		}
		break;

	default:
		break;
	}
}

/* вызывается из обработчика прерываний */
// произошла потеря символа (символов) при получении данных с CAT компорта
void modem_rxoverflow(void)
{
	++ rxerrchar;
}
/* вызывается из обработчика прерываний */
void modem_disconnect(void)
{
	
}

/* вызывается из обработчика прерываний */
// компорт готов передавать
void modem_sendchar(void * ctx)
{
	uint_fast8_t c;
	if (qget(& c))
	{
		HARDWARE_MODEM_TX(ctx, c);
		if (qempty())
			HARDWARE_MODEM_ENABLETX(0);
	}
	else
	{
		HARDWARE_MODEM_ENABLETX(0);
	}
}

#else /* WITHMODEM */

// Функция обновления (частоты, ...) - вызывается из основного цикла программы, user mode.

uint_fast8_t processmodem(void)
{
	return 0;
}
#endif /* WITHMODEM */
