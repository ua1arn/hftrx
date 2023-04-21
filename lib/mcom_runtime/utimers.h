// ******************** utimers.h ***********************************
//                 Arm-Dsp runtime library
//
// This file implements interface to universal timers. They works from
// SPLL frequency. There are 8 UTimers, num is in range 0..7
//
// (c) multicore.ru
//
// *******************************************************************

#ifndef UTIMERS_H
#define UTIMERS_H

#ifdef __cplusplus
extern "C" {
#endif

#define TIMERS_BASE 0x38026000

#define TIMER0_LOAD_COUNT2 *((volatile unsigned int *)0x380260B0)
#define TIMER1_LOAD_COUNT2 *((volatile unsigned int *)0x380260B4)
#define TIMERS_INT_STATUS *((volatile unsigned int *)0x380260A0)
#define TIMERS_EOI *((volatile unsigned int *)0x380260A4)
#define TIMERS_INT_RAW_STATUS *((volatile unsigned int *)0x380260A8)

typedef struct {
    volatile unsigned int LoadReg;    // Timers Load Register
    volatile unsigned int ValReg;     // Timers Counter Register
    volatile unsigned int CntrlReg;   // Timers Control Register
    volatile unsigned int EOIReg;     // Timers End-of-Interrupt Register
    volatile unsigned int IntrStatus; // Timers Interrupt Status Register
} universal_timer_t;

universal_timer_t *getUTimerHandler(int num);
enum ERL_ERROR initUTimer(int num, int load_val, int enable_int, int enable_auto_reload);
unsigned int getUTimerCounter(int num);
unsigned int resetUTimerInt(int num);
unsigned int disableUTimer(int num);

// Function: start_utimer
//
// Setup and starts universal timer for clock counting
//
// Parameters:
//      num      - number of timer (0-7)
//
// Return:
//      enum ERL_ERROR   - error type
//
enum ERL_ERROR start_utimer(int num);

// Function: utimer_tics_get
//
// Get clock since "start_utimer" in tics of SPLL
//
// Parameters:
//      num      - number of timer (0-7)
//
// Return:
//      unsigned long long   - ticks
//
unsigned long long utimer_tics_get(int num);

#ifdef __cplusplus
}
#endif

#endif /* UTIMERS_H */
