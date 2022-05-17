#include "xil_types.h"
#include "xil_assert.h"
#include "xil_exception.h"
#include "xpseudo_asm.h"
#include "xdebug.h"

#include "formats.h"
#include "hardware.h"

/************************** Constant Definitions ****************************/

#define DEBUG	1

/**************************** Type Definitions ******************************/

typedef struct {
	Xil_ExceptionHandler Handler;
	void *Data;
} XExc_VectorTableEntry;

/***************** Macros (Inline Functions) Definitions ********************/

/************************** Function Prototypes *****************************/
static void Xil_ExceptionNullHandler(void *Data);
/************************** Variable Definitions *****************************/
/*
 * Exception vector table to store handlers for each exception vector.
 */

XExc_VectorTableEntry XExc_VectorTable[XIL_EXCEPTION_ID_LAST + 1] =
{

};

static void Xil_ExceptionNullHandler(void *Data)
{

}


void Xil_ExceptionInit(void)
{
	return;
}

void Xil_ExceptionRegisterHandler(u32 Exception_id,
				    Xil_ExceptionHandler Handler,
				    void *Data)
{

}

void Xil_GetExceptionRegisterHandler(u32 Exception_id,
					Xil_ExceptionHandler *Handler,
					void **Data)
{

}

void Xil_ExceptionRemoveHandler(u32 Exception_id)
{

}


void Xil_DataAbortHandler(void *CallBackRef){
	(void) CallBackRef;
	DAbort_Handler();
}

/*****************************************************************************/
/**
*
* Default Prefetch abort handler which prints prefetch fault status register through
* which information about instruction prefetch fault can be acquired
*
* @return	None.
*
****************************************************************************/
void Xil_PrefetchAbortHandler(void *CallBackRef){
	(void) CallBackRef;
	PAbort_Handler();
}
/*****************************************************************************/
/**
*
* Default undefined exception handler which prints address of the undefined
* instruction if debug prints are enabled
*
* @return	None.
*
****************************************************************************/
void Xil_UndefinedExceptionHandler(void *CallBackRef){
	(void) CallBackRef;
	Undef_Handler();
}
