/*
 * File:   pll.h
 * Author: oen
 *
 * Created on March 15, 2016, 12:41 PM
 */

#ifndef PLL_H
#define PLL_H

#ifdef __cplusplus
extern "C" {
#endif
#define DEFAULT_XTI_CLOCK (24)
#define MIN_CPU_FREQ_MHZ DEFAULT_XTI_CLOCK
#define MAX_CPU_FREQ_MHZ (912)
#define MIN_DSP_FREQ_MHZ DEFAULT_XTI_CLOCK
#define MAX_DSP_FREQ_MHZ (720)
/*
 getCPUVoltage() returns voltage for CPU Freq from table
 */
unsigned int getCPUVoltage(unsigned int MHz);
/*
 getDSPVoltage() returns voltage for DSP Freq from table
 */
unsigned int getDSPVoltage(unsigned int MHz);
/*
 getCurrentCPUFreq() returns current CPU Freq from APLL ((SEL + 1) * 24MHz)
 */
unsigned int getCurrentCPUFreq();
/*
 getCurrentDSPFreq() returns current DSP Freq from DPLL ((SEL + 1) * 24MHz)
 */
unsigned int getCurrentDSPFreq();
/*
 setCPUFreq() sets CPU Freq in range MIN_CPU_FREQ_MHZ - MAX_CPU_FREQ_MHZ
 * 0 - success
 * 1 - CPU Freq is not valid
 * 2 - CPU voltage is not set
 */
int setCPUFreq(unsigned int MHz);
/*
 setDSPFreq() sets DSP Freq in range MIN_DSP_FREQ_MHZ - MAX_DSP_FREQ_MHZ
 * 0 - success
 * 1 - DSP Freq is not valid
 * 2 - DSP voltage is not set
 */
int setDSPFreq(unsigned int MHz);

int setSystemFreq(unsigned int MHz);
void setCPUPLLFreq(unsigned int MHz);
void setDSPPLLFreq(unsigned int MHz);
#ifdef __cplusplus
}
#endif

#endif /* PLL_H */
