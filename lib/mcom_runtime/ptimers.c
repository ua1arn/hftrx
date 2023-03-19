#include "ptimers.h"
#include "libdsp.h"
#include "pll.h"

#define GLOBAL_TIMER_BASE 0x39000200
#define PRIVATE_TIMERS_BASE 0x39000600

#define PTIMER_EN (1 << 0)
#define PTIMER_AUTOMODE_EN (1 << 1)
#define PTIMER_IRQ_EN (1 << 2)
#define PRTIMER_WDT_MODE (1 << 3)
#define PTIMER_PRESCALER(x) ((x) << 8)
#define PTIMER_INTR_EVENT (1 << 0)

#ifdef __DMA_TIME_COUNTING__
extern float _dma_working_time_counter;
extern float _dma_start_time;
#endif

volatile unsigned long plLastCounterValue = 0UL, plOverflows = 0;

unsigned int getPERIPHCLK();

inline private_timer_t *getPrivateTimerHandler()
{
    return (private_timer_t *)PRIVATE_TIMERS_BASE;
}

inline private_watchdog_timer_t *getPrivateWatchdogTimerHandler()
{
    return (private_watchdog_timer_t *)(PRIVATE_TIMERS_BASE + 0x20);
}

inline global_timer_t *getGlobalTimerHandler()
{
    return (global_timer_t *)GLOBAL_TIMER_BASE;
}

void initPrivateTimer(unsigned int Hz, int bAuto, int bInt)
{
    unsigned int csr = PTIMER_EN;
    private_timer_t *timer = getPrivateTimerHandler();

    // Disable timer
    timer->CntrlReg = 0;

    // Set Load Value
    timer->LoadReg = (getPERIPHCLK() / Hz) - 1;

    if (bAuto)
        csr |= PTIMER_AUTOMODE_EN;
    if (bInt)
        csr |= PTIMER_IRQ_EN;

    // Start timer
    timer->CntrlReg = csr;
}
unsigned int getPrivateTimerCounter()
{
    private_timer_t *timer = getPrivateTimerHandler();
    return timer->ValReg;
}
void resetPrivateTimerInt()
{
    private_timer_t *timer = getPrivateTimerHandler();
    timer->IntrStatus = PTIMER_INTR_EVENT;
}
void disablePrivateTimer()
{
    private_timer_t *timer = getPrivateTimerHandler();
    timer->CntrlReg = 0;
    timer->IntrStatus = PTIMER_INTR_EVENT;
}
void initPrivateWDT(unsigned int ms, int bAuto, int bInt)
{
    unsigned int csr = PTIMER_EN | PRTIMER_WDT_MODE;
    private_watchdog_timer_t *wdt = getPrivateWatchdogTimerHandler();

    // Disable WDT
    wdt->CntrlReg = 0;

    // Set prescaler and load value
    csr |= PTIMER_PRESCALER(7);
    wdt->LoadReg = (getPERIPHCLK() / 8000) * ms - 1;

    if (bAuto)
        csr |= PTIMER_AUTOMODE_EN;
    if (bInt)
        csr |= PTIMER_IRQ_EN;

    // Enable WDT
    wdt->CntrlReg = csr;
}
unsigned int getPrivateWDTCounter()
{
    private_watchdog_timer_t *wdt = getPrivateWatchdogTimerHandler();
    return wdt->ValReg;
}
void refreshPrivateWDT()
{
    private_watchdog_timer_t *wdt = getPrivateWatchdogTimerHandler();
    wdt->ValReg = wdt->LoadReg;
}
void disablePrivateWDT()
{
    private_watchdog_timer_t *wdt = getPrivateWatchdogTimerHandler();
    // Disable WDT
    wdt->CntrlReg = 0;

    // Enable Timer Mode
    wdt->DisableReg = 0x12345678;
    wdt->DisableReg = 0x87654321;
}
void initTimerModePrivateWDT(unsigned int Hz, int bAuto, int bInt)
{
    unsigned int csr = PTIMER_EN;
    private_watchdog_timer_t *wdt = getPrivateWatchdogTimerHandler();

    // Disable WDT
    disablePrivateWDT();

    // Set Load Value
    wdt->LoadReg = (getPERIPHCLK() / Hz) - 1;

    if (bAuto)
        csr |= PTIMER_AUTOMODE_EN;
    if (bInt)
        csr |= PTIMER_IRQ_EN;

    // Enable WDT
    wdt->CntrlReg = csr;
}
void resetTimerModePrivateWDTIntr()
{
    private_watchdog_timer_t *wdt = getPrivateWatchdogTimerHandler();
    wdt->ResetStatus = 1;
}
unsigned int getPERIPHCLK() { return (getCurrentCPUFreq() * 1000000) / 2; }

void start_private_timer()
{
    initPrivateTimer(0x1FFFFFFF, 1, 0);
    plLastCounterValue = 0UL;
    plOverflows = 0;
}

void risc_tics_start() { start_private_timer(); }

unsigned long long risc_tics_get()
{
    unsigned int valueNow = 0xFFFFFFFF - getPrivateTimerCounter();

    if (valueNow < plLastCounterValue) {
        plOverflows++;
    }
    plLastCounterValue = valueNow;

    return valueNow + (((unsigned long long)plOverflows) << 32);
}

float risc_ms_get()
{
    unsigned long long ticks = risc_tics_get();
    return ((float)ticks / (getPERIPHCLK() / 1000));
}

void sys_time_init()
{
    common_regs *dsp_common = (common_regs *)(_REGSCM);
    dsp_common->TOTAL_CLK_CNTR = 0;
#ifdef __DMA_TIME_COUNTING__
    _dma_working_time_counter = 0;
    _dma_start_time = 0;
#endif
    start_private_timer();
}

void sys_time_get(risc_sys_timer *tmr)
{
    unsigned int dsp_clk = getCurrentDSPFreq();
    common_regs *dsp_common = (common_regs *)(_REGSCM);
    tmr->dsp_ms_time =
        ((float)dsp_common->TOTAL_CLK_CNTR / (((float)dsp_clk) * 1000));
#ifdef __DMA_TIME_COUNTING__
    tmr->dma_ms_time = _dma_working_time_counter;
#else
    tmr->dma_ms_time = 0;
#endif
    tmr->sys_ms_time = risc_ms_get();
}
