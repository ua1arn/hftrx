/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#include "hardware.h"	/* зависящие от процессора функции работы с портами */

//#define WITHIRQTRACE 1	/* печать номеров возникающих прерываний */

#include "formats.h"	// for debug prints
#include "clocks.h"


////////////////////
/// поддержка отложенного вызова user-mode функций

typedef struct dpcdata
{
	void * tag1;
	LCLSPINLOCK_t lock;
	LIST_ENTRY dpclistentries;	// list of dpcobj_t - периодичски вызываемые функции
	LIST_ENTRY dpclistcalls;	// list of dpcobj_t = однократно вызываемые функции
	void * tag2;
} DPCDATA_t;

static DPCDATA_t dpcdatas [HARDWARE_NCORES];

/* инициализация списка user-mode опросных функций */
void board_dpc_initialize(void)
{
	unsigned i;
	for (i = 0; i < ARRAY_SIZE(dpcdatas); ++ i)
	{
		DPCDATA_t * const dpc = & dpcdatas [i];
		IRQLSPINLOCK_INITIALIZE(& dpc->lock);
		InitializeListHead(& dpc->dpclistentries);
		InitializeListHead(& dpc->dpclistcalls);
		dpc->tag1 = dpc;
		dpc->tag2 = dpc;
	}
}

void dpcobj_initialize(dpcobj_t * dp, udpcfn_t func, void * arg, const char * name)
{
	dp->flag = 0;
	dp->fn = func;
	dp->ctx = arg;
	dp->tag1 = dp;
	dp->tag2 = dp;
	local_snprintf_P(dp->name, ARRAY_SIZE(dp->name), "%s", name);
}

static void dpcobj_release(dpcobj_t * dp)
{
	IRQL_t oldIrql;
	DPCDATA_t * const dpc = & dpcdatas [dp->coreid];
	LCLSPINLOCK_t * const lock = & dpc->lock;
	ASSERT(dpc == dpc->tag1);
	ASSERT(dpc == dpc->tag2);
	ASSERT(dp == dp->tag1);
	ASSERT(dp == dp->tag2);
	IRQLSPIN_LOCK(lock, & oldIrql, DPCSYS_IRQL);
	dp->flag = 0;
	IRQLSPIN_UNLOCK(lock, oldIrql);
}

// возврат не-0 если уже занято
static uint_fast8_t dpcobj_accure(dpcobj_t * dp)
{
	uint_fast8_t v;

	IRQL_t oldIrql;
	DPCDATA_t * const dpc = & dpcdatas [dp->coreid];
	LCLSPINLOCK_t * const lock = & dpc->lock;
	ASSERT(dpc == dpc->tag1);
	ASSERT(dpc == dpc->tag2);
	ASSERT(dp == dp->tag1);
	ASSERT(dp == dp->tag2);

	IRQLSPIN_LOCK(lock, & oldIrql, DPCSYS_IRQL);
	v = dp->flag;
	dp->flag = 1;
	IRQLSPIN_UNLOCK(lock, oldIrql);

	return v;
}

/* Удалить функцию для периодического вызова */
uint_fast8_t board_dpc_delentry(dpcobj_t * dp)
{
	IRQL_t oldIrql;
	DPCDATA_t * const dpc = & dpcdatas [dp->coreid];
	LCLSPINLOCK_t * const lock = & dpc->lock;
	ASSERT(dpc == dpc->tag1);
	ASSERT(dpc == dpc->tag2);
	ASSERT(dp == dp->tag1);
	ASSERT(dp == dp->tag2);

	IRQLSPIN_LOCK(lock, & oldIrql, DPCSYS_IRQL);
	dp->delflag = 1;	/* удаление будет произведено при обработке списка */
	IRQLSPIN_UNLOCK(lock, oldIrql);

	return 1;
}

// получить core id текушего потока
// 0..HARDWARE_NCORES-1
uint_fast8_t board_dpc_coreid(void)
{
	return arm_hardware_cpuid();
}
// получить core id потока для обновления дисплея
// 0..HARDWARE_NCORES-1
uint_fast8_t board_dpc_display_coreid(void)
{
#if HARDWARE_NCORES > 2
	return 1;
#else /* */
	return 0;
#endif /* */
}

// Запрос отложенного вызова user-mode функций
/* добавить функцию для однократного вызова */
uint_fast8_t board_dpc_call(dpcobj_t * dp, uint_fast8_t coreid)
{
	//PRINTF("board_dpc_call: dp=%p, coreid=%u name='%s\n", dp, (unsigned) coreid, dp->name);
	IRQL_t oldIrql;
	DPCDATA_t * const dpc = & dpcdatas [coreid];
	LCLSPINLOCK_t * const lock = & dpc->lock;
	ASSERT(coreid < HARDWARE_NCORES);
	ASSERT(dpc == dpc->tag1);
	ASSERT(dpc == dpc->tag2);
	ASSERT(dp == dp->tag1);
	ASSERT(dp == dp->tag2);

	//if (coreid == 1) dbg_putchar('#');
	// предотвращение повторного включения в очередь того же запроса
	if (dpcobj_accure(dp))
		return 0;

	IRQLSPIN_LOCK(lock, & oldIrql, DPCSYS_IRQL);
	dp->coreid = coreid;
	InsertHeadList(& dpc->dpclistcalls, & dp->item);
	IRQLSPIN_UNLOCK(lock, oldIrql);

	return 1;
}

// Запрос отложенного вызова user-mode функций
/* добавить функцию для периодического вызова */
uint_fast8_t board_dpc_addentry(dpcobj_t * dp, uint_fast8_t coreid)
{
	//PRINTF("board_dpc_addentry: dp=%p, coreid=%u name='%s\n", dp, (unsigned) coreid, dp->name);
	IRQL_t oldIrql;
	DPCDATA_t * const dpc = & dpcdatas [coreid];
	LCLSPINLOCK_t * const lock = & dpc->lock;
	ASSERT(coreid < HARDWARE_NCORES);
	ASSERT(dpc == dpc->tag1);
	ASSERT(dpc == dpc->tag2);
	ASSERT(dp == dp->tag1);
	ASSERT(dp == dp->tag2);

	// предотвращение повторного включения в очередь того же запроса
	if (dpcobj_accure(dp))
		return 0;

	IRQLSPIN_LOCK(lock, & oldIrql, DPCSYS_IRQL);
	dp->coreid = coreid;
	dp->delflag = 0;
	InsertHeadList(& dpc->dpclistentries, & dp->item);
	IRQLSPIN_UNLOCK(lock, oldIrql);

	return 1;
}

// user-mode функция обработки списков запросов dpc на текущем процессоре
void board_dpc_processing(void)
{
	const uint_fast8_t coreid = board_dpc_coreid();
	DPCDATA_t * const dpc = & dpcdatas [coreid];
	LCLSPINLOCK_t * const lock = & dpc->lock;
//	char b [32];
//	local_snprintf_P(b, ARRAY_SIZE(b), "coreid=%u", coreid);
//	ASSERT3(coreid < HARDWARE_NCORES, __FILE__, __LINE__, b);
//	ASSERT3(dpc == dpc->tag1, __FILE__, __LINE__, b);
//	ASSERT3(dpc == dpc->tag2, __FILE__, __LINE__, b);
	ASSERT(coreid < HARDWARE_NCORES);
	ASSERT(dpc == dpc->tag1);
	ASSERT(dpc == dpc->tag2);
	// Выполнение периодического вызова user-mode функций по списку
	//if (coreid == 1) dbg_putchar('+');
	{
		IRQL_t oldIrql;
		PRLIST_ENTRY t;
		IRQLSPIN_LOCK(lock, & oldIrql, DPCSYS_IRQL);
		PRLIST_ENTRY const list = & dpc->dpclistentries;
		PRLIST_ENTRY tnext;
		for (t = list->Blink; t != list; t = tnext)
		{
			ASSERT(t != NULL);
			tnext = t->Blink;
			dpcobj_t * const dp = CONTAINING_RECORD(t, dpcobj_t, item);
			IRQLSPIN_UNLOCK(lock, oldIrql);
//			ASSERT3(dp == dp->tag1, __FILE__, __LINE__, b);
//			ASSERT3(dp == dp->tag2, __FILE__, __LINE__, b);
			ASSERT(dp == dp->tag1);
			ASSERT(dp == dp->tag2);

			ASSERT(coreid == dp->coreid);
			if (dp->delflag)
			{
				/* удалние функции, ранее помещённой в список */
				RemoveEntryList(& dp->item);
				//RemoveEntryList(t);
			}
			else
			{
				/* выполнение функции, ранее помещённой в список */
				(* dp->fn)(dp->ctx);
			}

			IRQLSPIN_LOCK(lock, & oldIrql, DPCSYS_IRQL);
		}
		IRQLSPIN_UNLOCK(lock, oldIrql);
	}
	//if (coreid == 1) dbg_putchar('|');
	// Выполнение однократно вызываемых user-mode функций по списку
	{
		IRQL_t oldIrql;
		IRQLSPIN_LOCK(lock, & oldIrql, DPCSYS_IRQL);
		PRLIST_ENTRY const list = & dpc->dpclistcalls;
		while (! IsListEmpty(list))
		{
			PRLIST_ENTRY t = RemoveTailList(list);
			IRQLSPIN_UNLOCK(lock, oldIrql);
			ASSERT(t != NULL);

			dpcobj_t * const dp = CONTAINING_RECORD(t, dpcobj_t, item);
			ASSERT(dp == dp->tag1);
			ASSERT(dp == dp->tag2);
			ASSERT(coreid == dp->coreid);
			(* dp->fn)(dp->ctx);
			dpcobj_release(dp);

			IRQLSPIN_LOCK(lock, & oldIrql, DPCSYS_IRQL);
		}
		IRQLSPIN_UNLOCK(lock, oldIrql);
	}
#if WITHSMPSYSTEM
	//__DMB();
	//__WFI();
#endif /* WITHSMPSYSTEM */
	//if (coreid == 1) dbg_putchar('-');
	thread_yield();	// хотим завершить выполнение кванта, не дожидаясь прерывания
}

void testsloopprocessing(void)
{
	board_dpc_processing();		// обработка отложенного вызова user mode функций для данного ядра
	watchdog_ping();
}
