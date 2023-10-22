/*
 * uart0.c
 *
 *	Работа с магнитным компасом
 */

#include "hardware.h"
#include "formats.h"
#include "bootloader.h"
#include "board.h"
#include "gpio.h"

#include "xhelper507.h"

#if WITHCTRLBOARDT507

// компас - RS232 9600 8N1

#define PERIODSPOOL 2000	// период опроса, мс


const uint8_t PREAMBLE_DATA = 0x55;	// Маркерный символ
enum { SIZE_DATA     = 11 };

// Типы значений, передаваемых в пакете данны (после маркерного символа)
enum codes
{
	VTIME             = 0x50,
	VACCELERATION     = 0x51,
	VANGULAR_VELOCITY = 0x52,
	VANGLE            = 0x53,
	VMAGNETIC         = 0x54,
	VQUATERNION       = 0x55
};

// Очереди символов для обмена

// Очередь символов для передачи в канал обмена
static u8queue_t txq;
// Очередь принятых симвоов из канала обменна
static u8queue_t rxq;

// передача символа в канал. Ожидание, если очередь заполнена
static int nmeaX_putc(int c)
{
	IRQL_t oldIrql;
	uint_fast8_t f;

	do {
		RiseIrql(IRQL_SYSTEM, & oldIrql);
		f = uint8_queue_put(& txq, c);
		hardware_uart0_enabletx(1);
		LowerIrql(oldIrql);
	} while (! f);
	return c;
}

// Передача в канал указанного массива. Ожидание, если очередь заполнена
static void uartX_write(const uint8_t * buff, size_t n)
{
	while (n --)
	{
		const uint8_t c = * buff ++;
		nmeaX_putc(c);
	}
}

static void uartX_format(const char * format, ...)
{
	char b [256];
	int n, i;
	va_list	ap;
	va_start(ap, format);

	n = vsnprintf(b, sizeof b / sizeof b [0], format, ap);

	for (i = 0; i < n; ++ i)
		nmeaX_putc(b [i]);

	va_end(ap);
}

// callback по принятому символу. сохранить в очередь для обработки в user level
void user_uart0_onrxchar(uint_fast8_t c)
{
	IRQL_t oldIrql;

	RiseIrql(IRQL_SYSTEM, & oldIrql);
	uint8_queue_put(& rxq, c);
	LowerIrql(oldIrql);
}

// callback по готовности последовательного порта к пердаче
void user_uart0_ontxchar(void * ctx)
{
	uint_fast8_t c;
	if (uint8_queue_get(& txq, & c))
	{
		hardware_uart0_tx(ctx, c);
		if (uint8_queue_empty(& txq))
			hardware_uart0_enabletx(0);
	}
	else
	{
		hardware_uart0_enabletx(0);
	}
}

// передача запроса раз в PERIODSPOOL милисекунд
// ВЫполняется из USER LEVEL
// Заглушка
static void uart0_dpc_spool(void * ctx)
{
	//TP();
}

static ticker_t uart0_ticker;
static dpcobj_t uart0_dpc_lock;

/* system-mode function */
static void uart0_timer_event(void * ctx)
{
	(void) ctx;	// приходит NULL

	board_dpc_call(& uart0_dpc_lock);	// Запрос отложенногог выполнения USER-MODE функции
}

static int state;	// состояния разбора (соответствует номеру байта в принятом пакете).
static int valuet;

static uint8_t varray [SIZE_DATA];
static unsigned cks;

static double convert(uint8_t low, uint8_t high, double a, int b) {

    return (uint16_t) (high * 256 + low) * b / a;
}

/* Функционирование USER MODE обработчиков */
void uart0_spool(void)
{
	uint_fast8_t c;
	uint_fast8_t f;
	IRQL_t oldIrql;

	RiseIrql(IRQL_SYSTEM, & oldIrql);
    f = uint8_queue_get(& rxq, & c);
	LowerIrql(oldIrql);

	if (f)
	{
		varray [state] = c;
		switch (state)
		{
		case 0:
			if (c == 0x55)
				++ state;
			cks = c;
			break;
		case 1:
			switch (c)
			{
			case VTIME:
			case VACCELERATION:
			case VANGULAR_VELOCITY:
			case VANGLE:
			case VMAGNETIC:
			case VQUATERNION:
				valuet = c;
				++ state;
				cks += c;
				break;
			default:
				state = 0;
				break;
			}
			break;
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
			cks += c;
			++ state;
			break;
		case 10:
			if ((cks & 0xFF) == c)
			{
				switch (valuet)
				{
				case VANGLE:
					{
						float roll = convert(varray [2], varray [3], 32768, 180);
						float pitch = convert(varray [4], varray [5], 32768, 180);
						float jaw = convert(varray [6], varray [7], 32768, 180);

						//printhex(0, varray, 11);
						xbsavemagn(roll, pitch, jaw);
					}
					break;
				}
				//printhex(0, varray, 11);
			}
			else
			{
				// Wrong ckecksum, wait nex sync
//				PRINTF("cks = %02X ", cks & 0xFF);
//				printhex(0, varray, 11);
			}
			state = 0;
			break;
		}
		//PRINTF("%02X ", c);

	}
}

void user_uart0_initialize(void)
{
	static uint8_t txb [512];
	uint8_queue_init(& txq, txb, ARRAY_SIZE(txb));
	static uint8_t rxb [512];
	uint8_queue_init(& rxq, rxb, ARRAY_SIZE(rxb));

	hardware_uart0_initialize(0, 9600, 8, 0, 0);
	hardware_uart0_set_speed(9600);
	hardware_uart0_enablerx(1);
	hardware_uart0_enabletx(0);

	dpcobj_initialize(& uart0_dpc_lock, uart0_dpc_spool, NULL);
	ticker_initialize(& uart0_ticker, NTICKS(PERIODSPOOL), uart0_timer_event, NULL);
	ticker_add(& uart0_ticker);
}

#endif /* WITHCTRLBOARDT507 */
