// ******************** ptimers.h ***********************************
//                 Arm-Dsp runtime library
//
// This file implements interface to private and watchdog timers
//
// (c) multicore.ru
//
// *******************************************************************

#ifndef PTIMERS_H
#define PTIMERS_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    volatile unsigned int LoadReg;    // Private Timer Load Register
    volatile unsigned int ValReg;     // Private Timer Counter Register
    volatile unsigned int CntrlReg;   // Private Timer Control Register
    volatile unsigned int IntrStatus; // Private Timer Interrupt Status Register
} private_timer_t;

typedef struct {
    volatile unsigned int LoadReg;     // Watchdog Load Register
    volatile unsigned int ValReg;      // Watchdog Counter Register
    volatile unsigned int CntrlReg;    // Watchdog Control Register
    volatile unsigned int IntrStatus;  // Watchdog Interrupt Status Register
    volatile unsigned int ResetStatus; // Watchdog Reset Status Register
    volatile unsigned int DisableReg;  // Watchdog Disable Register
} private_watchdog_timer_t;

typedef struct {
    volatile unsigned long long
        Counter; // Global Timer Counter Registers, 0x00 and 0x04
    volatile unsigned int CntrlReg;   // Global Timer Control Register
    volatile unsigned int IntrStatus; // Global Timer Interrupt Status Register
    volatile unsigned long long
        Comparator; // Comparator Value Registers, 0x10 and 0x14
    volatile unsigned int AutoIncReg; // Auto-increment Register, 0x18
} global_timer_t;

typedef struct {
    float sys_ms_time;
    float dsp_ms_time;
    float dma_ms_time;
} risc_sys_timer;

private_timer_t *getPrivateTimerHandler();
private_watchdog_timer_t *getPrivateWatchdogTimerHandler();
global_timer_t *getGlobalTimerHandler();
void initPrivateTimer(unsigned int Hz, int bAuto, int bInt);
unsigned int getPrivateTimerCounter();
void resetPrivateTimerInt();
void disablePrivateTimer();
void initPrivateWDT(unsigned int ms, int bAuto, int bInt);
unsigned int getPrivateWDTCounter();
void refreshPrivateWDT();
void disablePrivateWDT();
void initTimerModePrivateWDT(unsigned int Hz, int bAuto, int bInt);
void resetTimerModePrivateWDTIntr();

// Function: start_private_timer
//
// Setup and starts private timer for clock counting
//
void start_private_timer();

// Function: risc_tics_start
//
// Setup and starts private timer for clock in tics
//
void risc_tics_start();

// Function: risc_tics_get
//
// Get clock since "risc_tics_start" in tics.
//
// Return:
//      unsigned long long   - clocks
//
unsigned long long risc_tics_get();

// Function: risc_ms_get
//
// Get clock since "start_private_timer" in miliseconds
//
// Return:
//      float   - miliseconds
//
float risc_ms_get();

// Function: sys_time_init
//
// Initialize timers
//
void sys_time_init();

// Function: sys_time_get
//
// Fills struct arm_sys_timer to get dma, dsp and private time in milliseconds.
//
void sys_time_get(risc_sys_timer *tmr);

#ifdef __cplusplus
}
#endif

#endif /* PTIMERS_H */
