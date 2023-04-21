#include "utimers.h"
#include "erlcommon.h"
#include "libdsp.h"

volatile unsigned int ulLastCounterValue = 0UL, ulOverflows = 0;

inline universal_timer_t *getUTimerHandler(int num)
{
    if ((num < 0) || (num > 7)) return 0;
    return (universal_timer_t *)(TIMERS_BASE + (num * 0x14));
}

enum ERL_ERROR initUTimer(int num, int load_val, int enable_int, int enable_auto_reload)
{
    universal_timer_t *timer = getUTimerHandler(num);
    if (timer == 0)
        return ERL_PROGRAM_ERROR;
    enable_auto_reload &= 1;
    // Disable timer
    timer->CntrlReg = 0;
    // Set Load Value
    timer->LoadReg = load_val;
    // Start timer
    if (enable_int)
        timer->CntrlReg = 1 | (enable_auto_reload<<1); // Гребаный элвис даже этого не сделал
    else
        timer->CntrlReg = 5 | (enable_auto_reload<<1);
    return ERL_NO_ERROR;
}
unsigned int getUTimerCounter(int num)
{
    universal_timer_t *timer = getUTimerHandler(num);
    if (timer == 0) return 0;
    return timer->ValReg;
}
unsigned int resetUTimerInt(int num)
{
    universal_timer_t *timer = getUTimerHandler(num);
    if (timer == 0) return 0;
    volatile unsigned int read_int = timer->EOIReg;
    return read_int;
}
unsigned int disableUTimer(int num)
{
    universal_timer_t *timer = getUTimerHandler(num);
    if (timer == 0) return 0;
    timer->CntrlReg = 0;
    volatile unsigned int read_int = timer->EOIReg;
    return read_int;
}

enum ERL_ERROR start_utimer(int num)
{
    int ret = initUTimer(num, 0xFFFFFFFF, 0, 0);
    ulLastCounterValue = 0UL;
    ulOverflows = 0UL;
    return ret;
}

unsigned long long utimer_tics_get(int num)
{
    if ((num < 0) || (num > 7)) return 0;
    //
    unsigned int valueNow = 0xFFFFFFFF - getUTimerCounter(num);
    //
    if (valueNow < ulLastCounterValue) 
    {
        ulOverflows++;
    }
    ulLastCounterValue = valueNow;

    return valueNow + (((unsigned long long)ulOverflows) << 32);
}
