#ifndef _SDHCI_T113_H_
#define _SDHCI_T113_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "aw-sdhci.h"
#include "aw-sdcard.h"

extern struct sdhci_t HCI;
extern struct sdcard_t CARD;

int sdhci_t113_init(struct sdhci_t * sdhci);
int sdhci_t113_reset(struct sdhci_t * sdhci);
int sdhci_t113_setclock(struct sdhci_t * sdhci, uint32_t clock);
int sdhci_t113_setwidth(struct sdhci_t * sdhci, uint32_t width);
int sdhci_t113_transfer(struct sdhci_t * sdhci, struct sdhci_cmd_t * cmd, struct sdhci_data_t * dat);

#ifdef __cplusplus
}
#endif

#endif
