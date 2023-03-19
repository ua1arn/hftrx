// ***************** simple_time.h ***********************************
//
//  MFBSP
//
//
// *******************************************************************
#ifndef _MFBSP_H_
#define _MFBSP_H_

#include "erlcommon.h"

#ifdef	__cplusplus
extern "C" {
#endif

#define MCBSP0_BUFF_LEN  64
#define MCBSP1_BUFF_LEN  64

#define MFBSPCH_CSR_RUN         1
#define MFBSPCH_CSR_WN		(1<<2)
#define MFBSPCH_CSR_WN_FULL	(0xF<<2)
#define MFBSPCH_CSR_CHEN	1<<12)
#define MFBSPCH_CSR_IM		(1<<13)
#define MFBSPCH_CSR_END		(1<<14)
#define MFBSPCH_CSR_DONE	(1<<15)
#define MFBSPCH_CSR_WCX		(1<<16)

typedef union
{
    unsigned int _val;
    struct
    {
        unsigned len          : 1;
        unsigned ltran        : 1;
        unsigned lclk_rate0   : 1;
        unsigned lstat        : 2;
        unsigned lrerr        : 1;
        unsigned ldw          : 1;
        unsigned srq_tx       : 1;
        unsigned srq_rx       : 1;
        unsigned spi_i2s_en   : 1;
        unsigned              : 1;
        unsigned lclk_rate41  : 4;
        unsigned              : 17;
    };
} MFBSP_CSR_REG;

typedef union
{
    unsigned int _val;
    struct
    {
        unsigned rclk_dir     : 1;
        unsigned tclk_dir     : 1;
        unsigned rcs_dir      : 1;
        unsigned tcs_dir      : 1;
        unsigned rd_dir       : 1;
        unsigned td_dir       : 1;
        unsigned ldat_dir     : 4;
    };
} MFBSP_DIR_REG;

typedef union
{
    unsigned int _val;
    struct
    {
        unsigned ren          : 1;
        unsigned rmode        : 1;
        unsigned rclk_cp      : 1;
        unsigned rcs_cp       : 1;
        unsigned              : 5;
        unsigned rdspmode     : 1;
        unsigned rneg         : 1;
        unsigned rdel         : 1;
        unsigned rwordcnt     : 6;
        unsigned rcsneg       : 1;
        unsigned rmbf         : 1;
        unsigned rwordlen     : 5;
        unsigned rpack        : 1;
        unsigned rsign        : 1;
        unsigned rswap        : 1;
        unsigned rclk_cont    : 1;
        unsigned rcs_cont     : 1;
        unsigned              : 2;
    };
} MFBSP_RCTR_REG;

typedef union
{
    unsigned int _val;
    struct
    {
        unsigned ten          : 1;
        unsigned tmode        : 1;
        unsigned td_zer_en    : 1;
        unsigned              : 1;
        unsigned              : 5;
        unsigned tdspmode     : 1;
        unsigned tneg         : 1;
        unsigned tdel         : 1;
        unsigned twordcnt     : 6;
        unsigned tcsneg       : 1;
        unsigned tmbf         : 1;
        unsigned twordlen     : 5;
        unsigned tpack        : 1;
        unsigned              : 1;
        unsigned rswap        : 1;
        unsigned tclk_cont    : 1;
        unsigned tcs_cont     : 1;
        unsigned ss0          : 1;
        unsigned ss1          : 1;
    };
} MFBSP_TCTR_REG;

typedef union
{
    unsigned int _val;
    struct
    {
        unsigned rclk_rate    : 10;
        unsigned              : 2;
        unsigned rss_rate     : 4;
        unsigned rcs_rate     : 16;
    };
} MFBSP_RCTR_RATE_REG;

typedef union
{
    unsigned int _val;
    struct
    {
        unsigned tclk_rate    : 10;
        unsigned              : 2;
        unsigned tss_rate     : 4;
        unsigned tcs_rate     : 16;
    };
} MFBSP_TCTR_RATE_REG;

typedef union
{
    unsigned int _val;
    struct
    {
        unsigned run    : 1;
        unsigned        : 1;
        unsigned wn     : 4;
        unsigned ipd    : 1;
        unsigned        : 5;
        unsigned chen   : 1;
        unsigned im     : 1;
        unsigned end    : 1;
        unsigned done   : 1;
        unsigned wcx    : 16;
    };
} MFBSP_DMA_CSR_REG;

typedef volatile struct
{
    volatile unsigned int BUF;
    volatile unsigned int CSR;
    volatile unsigned int DIR;
    volatile unsigned int GPIO;
    volatile unsigned int TCTR;
    volatile unsigned int RCTR;
    volatile unsigned int TSR;
    volatile unsigned int RSR;
    volatile unsigned int TCTR_RATE;
    volatile unsigned int RCTR_RATE;
    volatile unsigned int skip[54];
} mfbsp_port;

typedef volatile struct
{
    volatile unsigned int CSR;
    volatile unsigned int CP;
    volatile unsigned int IR;
    volatile unsigned int RUN;
    volatile unsigned int skip[12];
} mfbsp_dma_ch;

#define _MFBSPport0		((mfbsp_port volatile *)0x38086000)
#define _MFBSPport1		((mfbsp_port volatile *)0x38088000)

#define _MFBSPdmaRXch0		((mfbsp_dma_ch volatile *)0x38087040)
#define _MFBSPdmaRXch1		((mfbsp_dma_ch volatile *)0x38089040)

#define _MFBSPdmaTXch0		((mfbsp_dma_ch volatile *)0x38087000)
#define _MFBSPdmaTXch1		((mfbsp_dma_ch volatile *)0x38089000)

typedef struct
{
    unsigned :32;
    unsigned int IR;
    unsigned int CP;
    unsigned int CSR;
} dma_mfbsp_chain;

// Section: Functions
//
// Function: get_mfbsp_dev
//
// Gets pointer to selected MFBSP port registers.
//
// Parameters:
//      num     - number of requested MFBSP port
//
// Return:
//		NULL        - num is not in 0..3 range
//		Pointer to MFBSP<id> registers - otherway
//
// > get_mfbsp_dev(0);
//
mfbsp_port* get_mfbsp_dev(int num);

// Function: get_mfbsp_dma_dev
//
// Gets pointer to selected MFBSP port DMA registers.
//
// Parameters:
//      num     - number of requested MFBSP port
//      istx    - is transmitter (0 - receiver dma channel, 1 - transmitter)
//
// Return:
//		NULL        - num is not in 0..3 range
//		Pointer to MFBSP_DMA<id> registers - otherway
//
mfbsp_dma_ch* get_mfbsp_dma_dev(int num, int istx);

// Function: spi_transmitter_configure
//
// Setups MFBSP port transmitter registers. Setup to SPI mode
//
// Parameters:
//      id          - number of requested MFBSP port
//		len	        - length of transmitted word in bits [1..32]
//		cnt			- number of words in frame [1..32]
//		clk         - clock frequency of transmitter
//
// Return:
//		ERL_NO_ERROR - configuration procedure is successful
//		ERL_SYSTEM_ERROR - MFBSP port id is wrong
//
enum ERL_ERROR spi_transmitter_configure(int id, int len, int cnt, unsigned clk);

// Function: spi_receiver_configure
//
// Setups MFBSP port transmitter registers. Setup to SPI mode
//
// Parameters:
//      id      - number of requested MFBSP port
//		len		- length of transmitted word in bits [1..32]
//		cnt		- number of words in frame [1..32]
//
// Return:
//		ERL_NO_ERROR - configuration procedure is successful
//		ERL_SYSTEM_ERROR - MFBSP port id is wrong
//
enum ERL_ERROR spi_receiver_configure(int id, int len, int cnt);

// Function: mfbsp_dma_configure
//
// Setup DMA channel of MFBSP port
//
// Parameters:
//      id          - number of requested MFBSP port
//		array		- physical or virtual address of array. Must be aligned to 64 bit
//		size		- size of transmitted buffer in 64-bit words
//		istx        - is transmitter (1 - transmitter, 0 - receiver)
//
// Return:
//		ERL_NO_ERROR - configuration procedure is successful
//		ERL_SYSTEM_ERROR - MFBSP port id is wrong
//
enum ERL_ERROR mfbsp_dma_configure(int id, void * array, int size, int istx);

// Function: mfbsp_dma_run
//
// Starts dma memory channel of mfbsp port. The dma channel registers must be configured before by
// mfbsp_dma_configure function or by access to mfbsp_dma_ch structure directly.
//
// Parameters:
//      id          - number of requested MFBSP port
//		istx        - receive or transmit channel? (1 - transmitter, 0 - receiver)
//
// Return:
//		ERL_NO_ERROR - configuration procedure is successful
//		ERL_SYSTEM_ERROR - dma channel id wrong
//
enum ERL_ERROR mfbsp_dma_run(int id, int istx);

// Function: mfbsp_dma_wait
//
// Waits dma memory channel of mfbsp port stops transfer. This function blocks execution thread.
//
// Parameters:
//      id          - number of requested MFBSP port
//		istx        - receive or transmit channel? (1 - transmitter, 0 - receiver)
//
// Return:
//		ERL_NO_ERROR - configuration procedure is successful
//		ERL_SYSTEM_ERROR - dma channel id wrong
//
enum ERL_ERROR mfbsp_dma_wait(int id, int istx);

dma_mfbsp_chain * mfbsp_fill_link(dma_mfbsp_chain *link, void *addr, unsigned int size, unsigned int flags);
void mfbsp_fill_link2(dma_mfbsp_chain *link, dma_mfbsp_chain *link2, void *addr, unsigned int size, unsigned int flags);
enum ERL_ERROR mfbsp_start_chain(int id, int istx, dma_mfbsp_chain *link);
enum ERL_ERROR mfbsp_link_wait(int id, int istx);

enum ERL_ERROR spi_duplex_configure(int id, int len, int cnt, unsigned clk, int ismaster);

void mfbsp_init_dma();

#ifdef	__cplusplus
}
#endif

#endif // _MFBSP_H_
