
#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#include "hardware.h"
#include "formats.h"

/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE.
 *
 * See http://www.freertos.org/a00110.html
 *----------------------------------------------------------*/

/*
 * The FreeRTOS Cortex-A port implements a full interrupt nesting model.
 *
 * Interrupts that are assigned a priority at or below
 * configMAX_API_CALL_INTERRUPT_PRIORITY (which counter-intuitively in the ARM
 * generic interrupt controller [GIC] means a priority that has a numerical
 * value above configMAX_API_CALL_INTERRUPT_PRIORITY) can call FreeRTOS safe API
 * functions and will nest.
 *
 * Interrupts that are assigned a priority above
 * configMAX_API_CALL_INTERRUPT_PRIORITY (which in the GIC means a numerical
 * value below configMAX_API_CALL_INTERRUPT_PRIORITY) cannot call any FreeRTOS
 * API functions, will nest, and will not be masked by FreeRTOS critical
 * sections (although it is necessary for interrupts to be globally disabled
 * extremely briefly as the interrupt mask is updated in the GIC).
 *
 * FreeRTOS functions that can be called from an interrupt are those that end in
 * "FromISR".  FreeRTOS maintains a separate interrupt safe API to enable
 * interrupt entry to be shorter, faster, simpler and smaller.
 */
#define configMAX_API_CALL_INTERRUPT_PRIORITY	18

#define configUSE_PORT_OPTIMISED_TASK_SELECTION	1
#define configUSE_TICKLESS_IDLE					0
#define configTICK_RATE_HZ						( ( TickType_t ) 1000 )
#define configUSE_PREEMPTION					1
#define configUSE_IDLE_HOOK						1
#define configUSE_TICK_HOOK						0
#define configMAX_PRIORITIES					( 7 )
#define configMINIMAL_STACK_SIZE				( ( unsigned short ) 8192 ) /* Large in case configUSE_TASK_FPU_SUPPORT is 2 in which case all tasks have an FPU context. */
#define configAPPLICATION_ALLOCATED_HEAP 1
#define configTOTAL_HEAP_SIZE					( 1024 * 1024 )
#define configMAX_TASK_NAME_LEN					( 10 )
#define configUSE_TRACE_FACILITY				1
#define configUSE_16_BIT_TICKS					0
#define configIDLE_SHOULD_YIELD					1
#define configUSE_MUTEXES						1
#define configQUEUE_REGISTRY_SIZE				8
#define configCHECK_FOR_STACK_OVERFLOW			0//1
#define configUSE_RECURSIVE_MUTEXES				1
#define configUSE_MALLOC_FAILED_HOOK			1
#define configUSE_APPLICATION_TASK_TAG			0
#define configUSE_COUNTING_SEMAPHORES			1
#define configUSE_QUEUE_SETS					1
#define configSUPPORT_STATIC_ALLOCATION			1

/* Include the query-heap CLI command to query the free heap space. */
#define configINCLUDE_QUERY_HEAP_COMMAND		0

/* Co-routine definitions. */
#define configUSE_CO_ROUTINES 					0
#define configMAX_CO_ROUTINE_PRIORITIES 		( 2 )

/* Software timer definitions. */
#define configUSE_TIMERS						1
#define configTIMER_TASK_PRIORITY				( configMAX_PRIORITIES - 1 )
#define configTIMER_QUEUE_LENGTH				5
#define configTIMER_TASK_STACK_DEPTH			( configMINIMAL_STACK_SIZE * 2 )

/* If configUSE_TASK_FPU_SUPPORT is set to 1 (or undefined) then each task will
be created without an FPU context, and a task must call vTaskUsesFPU() before
making use of any FPU registers.  If configUSE_TASK_FPU_SUPPORT is set to 2 then
tasks are created with an FPU context by default, and calling vTaskUsesFPU() has
no effect. */
#define configUSE_TASK_FPU_SUPPORT				2

/* Set the following definitions to 1 to include the API function, or zero
to exclude the API function. */
#define INCLUDE_vTaskPrioritySet				1
#define INCLUDE_uxTaskPriorityGet				1
#define INCLUDE_vTaskDelete						1
#define INCLUDE_vTaskCleanUpResources			1
#define INCLUDE_vTaskSuspend					1
#define INCLUDE_vTaskDelayUntil					1
#define INCLUDE_vTaskDelay						1
#define INCLUDE_xTimerPendFunctionCall			0
#define INCLUDE_eTaskGetState					1
#define INCLUDE_xTaskAbortDelay					1
#define INCLUDE_xTaskGetTaskHandle				1
#define INCLUDE_xTaskGetHandle					0//1
#define INCLUDE_xSemaphoreGetMutexHolder		1
#define INCLUDE_xTaskGetCurrentTaskHandle 1

/* This demo makes use of one or more example stats formatting functions.  These
format the raw data provided by the uxTaskGetSystemState() function in to human
readable ASCII form.  See the notes in the implementation of vTaskList() within
FreeRTOS/Source/tasks.c for limitations. */
#define configUSE_STATS_FORMATTING_FUNCTIONS	0

/* The private watchdog is used to generate run time stats. */
#define configGENERATE_RUN_TIME_STATS 0

/*
#define configINCLUDE_FREERTOS_TASK_C_ADDITIONS_H 1
#define FREERTOS_TASKS_C_ADDITIONS_INIT tasks_print
*/

/* Normal assert() semantics without relying on the provision of an assert.h
header file. */
//extern void uart_printf(const char *str, ...);
#define configASSERT( x ) if( ( x ) == 0 ) {PRINTF("assert %s %s:%d\n", #x, __FILE__, __LINE__ ); while(1);}

/* If configTASK_RETURN_ADDRESS is not defined then a task that attempts to
return from its implementing function will end up in a "task exit error"
function - which contains a call to configASSERT().  However this can give GCC
some problems when it tries to unwind the stack, as the exit error function has
nothing to return to.  To avoid this define configTASK_RETURN_ADDRESS to 0.  */
#define configTASK_RETURN_ADDRESS	NULL


/****** Hardware specific settings. *******************************************/

/*
 * The application must provide a function that configures a peripheral to
 * create the FreeRTOS tick interrupt, then define configSETUP_TICK_INTERRUPT()
 * in FreeRTOSConfig.h to call the function.  This file contains a function
 * that is suitable for use on the Zynq MPU.  FreeRTOS_Tick_Handler() must
 * be installed as the peripheral's interrupt handler.
 */
void vConfigureTickInterrupt( void );
#define configSETUP_TICK_INTERRUPT() vConfigureTickInterrupt()

void vClearTickInterrupt( void );
#define configCLEAR_TICK_INTERRUPT() vClearTickInterrupt()

#define configINTERRUPT_CONTROLLER_BASE_ADDRESS 		GIC_DISTRIBUTOR_BASE //( 0x03021000 )
#define configINTERRUPT_CONTROLLER_CPU_INTERFACE_OFFSET (GIC_INTERFACE_BASE - GIC_DISTRIBUTOR_BASE) //( 0x1000 )
#define configUNIQUE_INTERRUPT_PRIORITIES				32




#endif /* FREERTOS_CONFIG_H */

