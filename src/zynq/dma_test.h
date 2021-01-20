#include "hardware.h"
#if CPUSTYLE_XC7Z
void xc7z_dma_intHandler(void);
void xc7z_dma_init(void);
void xc7z_dma_transmit(u32 *buffer, size_t buffer_len, u32 nRepeats);
#endif
