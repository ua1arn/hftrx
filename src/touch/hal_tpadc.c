/*
 * ===========================================================================================
 *
 *       Filename:  hal_tpadc.c
 *
 *    Description:  tpadc hal layer code
 *
 *        Version:  Melis3.0
 *         Create:  2020-1-14
 *       Revision:  none
 *       Compiler:  GCC:version 9.2.1 20170904 (release),ARM/embedded-7-branch revision 255204
 *
 *         Author:  liuyu@allwinnertech.com
 *   Organization:  SWC-BPD
 *  Last Modified:  2021-1-14
 *
 * ===========================================================================================
 */
/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

// Allwinner F133/t113-s3 resistive touch screen controller

#include "hardware.h"

#if TSC1_TYPE == TSC_TYPE_AWTPADC && 0

#include "formats.h"
#include "clocks.h"
#include "touch.h"

// https://github.com/RT-Thread/rt-thread/blob/master/bsp/allwinner/libraries/sunxi-hal/hal/source/tpadc/hal_tpadc.c


/* SET_BITS */
#define SETMASK(width, shift)   ((width?((-1U) >> (32-width)):0)  << (shift))
#define CLRMASK(width, shift)   (~(SETMASK(width, shift)))
#define GET_BITS(shift, width, reg)     \
    (((reg) & SETMASK(width, shift)) >> (shift))
#define SET_BITS(shift, width, reg, val) \
    (((reg) & CLRMASK(width, shift)) | (val << (shift)))

#define TPADC_WIDTH     4095  /*12bit 0XFFF*/
#define TPADC_CLK_IN        (12000000)
#define DCXO24M         (24000000)

/* TPADC register offset */
#define TP_CTRL0        0x00
#define TP_CTRL1        0x04
#define TP_CTRL2        0x08
#define TP_CTRL3        0x0c
#define TP_INT_FIFOC    0x10
#define TP_INT_FIFOS    0x14
#define TP_TPR          0x18
#define TP_CDAT         0x1c
#define TP_DATA     0x24

/* tpadc regsiter offset and width */
#define FIFO_OVERRUN_PENDING    17
#define FIFO_OVERRUN_PENDIN_WIDTH 1

#define FIFO_DATA_PENDING   16
#define FIFO_DATA_PENDING_WIDTH 1

#define TP_FIFO_FLUSH   4
#define TP_FIFO_FLUSH_WIDTH 1

#define TACQ    0
#define TACQ_WIDTH  16

#define FS_DIV  16
#define FS_DIV_WIDTH    4

#define ADC_CLK_DIVIDER 20
#define ADC_CLK_DIVIDER_WIDTH   2

#define ADC_FIRST_DLY_MODE  23
#define ADC_FIRST_DLY_MODE_WIDTH    1

#define ADC_FIRST_DLY   24
#define ADC_FIRST_DLY_WIDTH 8

#define TP_MODE_SELECT  4
#define TP_MODE_SELECT_WIDTH    1

#define TP_EN   5
#define TP_EN_WIDTH 1

#define TP_DEBOUNCE 12
#define TP_DEBOUNCE_WIDTH   8

#define PRE_MEA 0
#define PRE_MEA_WIDTH   24

#define PRE_MEA_EN  24
#define PRE_MEA_EN_WIDTH    1

#define TP_FIFO_MODE    26
#define TP_FIFO_MODE_WIDTH  2

#define TP_SENSITIVE    28
#define TP_SENSITIVE_WIDTH  4

#define FILTER_TYPE 0
#define FILTER_TYPE_WIDTH   2

#define FILTER_EN   2
#define FILTER_EN_WIDTH 1

#define TP_DOWN_IRQ_EN  0
#define TP_DOWN_IRQ_ENWIDTH 1

#define TP_UP_IRQ_EN    1
#define TP_UP_IRQ_EN_WIDTH  1

#define TP_DATA_DRQ_EN  7
#define TP_DATA_DRQ_EN_WIDTH    1

#define TP_FIFO_TRIG 8
#define TP_FIFO_TRIG_WIDTH  5

#define TP_DATA_XY_CHANGE   13
#define TP_DATA_XY_CHANGE_WIDTH 1

#define TP_DATA_IRQ_EN  16
#define TP_DATA_IRQ_EN_WIDTH    1

#define TP_FIFO_OVERRUN_IRQ 17
#define TP_FIFO_OVERRUN_IRQ_WIDTH   1

#define TP_DATAPEND     (1<<16)
#define TP_UPPEND       (1<<1)
#define TP_DOWNPEND     (1<<0)

#define OSC_FREQUENCY       24000000
#define HOSC            1
#define TP_IO_INPUT_MODE    0xfffff

#define TP_CH3_SELECT       (1 << 3) /* channale 3 select enable,  0:disable, 1:enable */
#define TP_CH2_SELECT       (1 << 2) /* channale 2 select enable,  0:disable, 1:enable */
#define TP_CH1_SELECT       (1 << 1) /* channale 1 select enable,  0:disable, 1:enable */
#define TP_CH0_SELECT       (1 << 0) /* channale 0 select enable,  0:disable, 1:enable */



//#define TPADC_DEBUG
//#ifdef TPADC_DEBUG
//#define PRINTF(fmt, arg...) hal_log_info(fmt, ##arg)
//#else
//#define PRINTF(fmt, arg...) do {}while(0)
//#endif
//
//#define PRINTF(fmt, arg...) hal_log_err(fmt, ##arg)

typedef enum
{
    TPADC_IRQ_ERROR = -4,
    TPADC_CHANNEL_ERROR = -3,
    TPADC_CLK_ERROR = -2,
    TPADC_ERROR = -1,
    TPADC_OK = 0,
} hal_tpadc_status_t;

typedef enum
{
    DATA_X = 0,
    DATA_Y,
    DATA_UP,
} data_flag_t;

typedef enum
{
    TP_CH_0 = 0,
    TP_CH_1,
    TP_CH_2,
    TP_CH_3,
    TP_CH_MAX,
} tp_channel_id;

typedef int (*tpadc_usercallback_t)(uint32_t data, data_flag_t flag);
typedef int (*tpadc_adc_usercallback_t)(uint32_t data, tp_channel_id channel);

typedef struct hal_tpadc
{
    unsigned long reg_base;
    uint32_t channel_num;
    uint32_t irq_num;
    uint32_t rate;
//    hal_clk_id_t bus_clk_id;
//    hal_clk_id_t mod_clk_id;
//    hal_reset_id_t rst_clk_id;
//    hal_clk_t   bus_clk;
//    hal_clk_t   mod_clk;
    struct reset_control    *rst_clk;
    tpadc_usercallback_t callback;
    tpadc_adc_usercallback_t adc_callback[TP_CH_MAX];
} hal_tpadc_t;

hal_tpadc_status_t hal_tpadc_init(void);
hal_tpadc_status_t hal_tpadc_exit(void);
hal_tpadc_status_t hal_tpadc_register_callback(tpadc_usercallback_t user_callback);

hal_tpadc_status_t hal_tpadc_adc_init(void);
hal_tpadc_status_t hal_tpadc_adc_channel_init(tp_channel_id channel);
hal_tpadc_status_t hal_tpadc_adc_channel_exit(tp_channel_id channel);
hal_tpadc_status_t hal_tpadc_adc_exit(void);
hal_tpadc_status_t hal_tpadc_adc_register_callback(tp_channel_id channel , tpadc_adc_usercallback_t user_callback);

hal_tpadc_status_t hal_tpadc_resume(void);
hal_tpadc_status_t hal_tpadc_suspend(void);



static hal_tpadc_t hal_tpadc;


static uint32_t readl(uintptr_t addr)
{
	return * (volatile uint32_t *) addr;
}

static void writel(uint32_t value, uintptr_t addr)
{
	* (volatile uint32_t *) addr = value;
}

static void sunxi_flush_fifo(unsigned long reg_base)
{
    uint32_t reg_val;

    reg_val = readl(reg_base + TP_INT_FIFOC);
    reg_val  = SET_BITS(TP_FIFO_FLUSH, TP_FIFO_FLUSH_WIDTH,reg_val, 0x1);

    writel(reg_val, reg_base + TP_INT_FIFOC);
}

static void sunxi_clear_fifo_status(unsigned long reg_base)
{
    uint32_t reg_val;

    reg_val = readl(reg_base + TP_INT_FIFOS);
    reg_val  = SET_BITS(FIFO_OVERRUN_PENDING, FIFO_OVERRUN_PENDIN_WIDTH,
                reg_val, 0x0);
    reg_val  = SET_BITS(FIFO_DATA_PENDING, FIFO_DATA_PENDING_WIDTH,
                reg_val, 0x0);

    writel(reg_val, reg_base + TP_INT_FIFOS);
}

static void sunxi_set_acqiure_time(unsigned long reg_base, uint32_t val)
{
    uint32_t reg_val;

    reg_val = readl(reg_base + TP_CTRL0);
    reg_val  = SET_BITS(TACQ, TACQ_WIDTH, reg_val, val);

    writel(reg_val, reg_base + TP_CTRL0);
}

static void sunxi_set_frequency_divider(unsigned long reg_base, uint32_t val)
{
    uint32_t reg_val;

    reg_val = readl(reg_base + TP_CTRL0);
    reg_val  = SET_BITS(FS_DIV, FS_DIV_WIDTH, reg_val, val);

    writel(reg_val, reg_base + TP_CTRL0);
}

static void sunxi_set_clk_divider(unsigned long reg_base, uint32_t val)
{
    uint32_t reg_val;

    reg_val = readl(reg_base + TP_CTRL0);
    reg_val  = SET_BITS(ADC_CLK_DIVIDER, ADC_CLK_DIVIDER_WIDTH, reg_val, val);

    writel(reg_val, reg_base + TP_CTRL0);
}

static void sunxi_select_delay_mode(unsigned long reg_base, uint32_t val)
{
    uint32_t reg_val;

    reg_val = readl(reg_base + TP_CTRL0);
    reg_val  = SET_BITS(ADC_FIRST_DLY_MODE, ADC_FIRST_DLY_MODE_WIDTH,reg_val, val);

    writel(reg_val, reg_base + TP_CTRL0);
}

static void sunxi_set_dealy_time(unsigned long reg_base, uint32_t val)
{
    uint32_t reg_val;

    reg_val = readl(reg_base + TP_CTRL0);
    reg_val  = SET_BITS(ADC_FIRST_DLY, ADC_FIRST_DLY_WIDTH,reg_val, val);

    writel(reg_val, reg_base + TP_CTRL0);
}

static void sunxi_clk_init(unsigned long reg_base)
{
    sunxi_set_acqiure_time(reg_base, 0x02); //2us
    sunxi_set_frequency_divider(reg_base, 0xb);
    sunxi_set_clk_divider(reg_base, 0x3);
    sunxi_select_delay_mode(reg_base, 0x1);
    sunxi_set_dealy_time(reg_base, 0xf);
}

static uint32_t sunxi_tpadc_adc_ch_select(unsigned long reg_base, tp_channel_id id)
{
    uint32_t reg_val;

    reg_val = readl(reg_base + TP_CTRL1);
    switch (id) {
    case TP_CH_0:
        reg_val |= TP_CH0_SELECT;
        break;
    case TP_CH_1:
        reg_val |= TP_CH1_SELECT;
        break;
    case TP_CH_2:
        reg_val |= TP_CH2_SELECT;
        break;
    case TP_CH_3:
        reg_val |= TP_CH3_SELECT;
        break;
    default:
        PRINTF("%s, invalid channel id!\n", __func__);
        return TPADC_ERROR;
    }
    writel(reg_val, reg_base + TP_CTRL1);

    return 0;
}

/* 0:TAPADC  1: ADC*/
static void sunxi_tpadc_mode_select(unsigned long reg_base, uint32_t val)
{
    uint32_t reg_val;

    reg_val = readl(reg_base + TP_CTRL1);
    reg_val  = SET_BITS(TP_MODE_SELECT, TP_MODE_SELECT_WIDTH,reg_val, val);

    writel(reg_val, reg_base + TP_CTRL1);
}


static void sunxi_tpadc_enable(unsigned long reg_base, uint32_t val)
{
    uint32_t reg_val;

    reg_val = readl(reg_base + TP_CTRL1);
    reg_val  = SET_BITS(TP_EN, TP_EN_WIDTH, reg_val, val);

    writel(reg_val, reg_base + TP_CTRL1);
}

static void sunxi_set_up_debou_time(unsigned long reg_base, uint32_t val)
{
    uint32_t reg_val;

    reg_val = readl(reg_base + TP_CTRL1);
    reg_val  = SET_BITS(TP_DEBOUNCE, TP_DEBOUNCE_WIDTH, reg_val, val);

    writel(reg_val, reg_base + TP_CTRL1);
}

static void sunxi_set_pressure_thresholed(unsigned long reg_base, uint32_t val)
{
    uint32_t reg_val;

    reg_val = readl(reg_base + TP_CTRL2);
    reg_val  = SET_BITS(PRE_MEA, PRE_MEA_WIDTH, reg_val, val);

    writel(reg_val, reg_base + TP_CTRL2);
}

static void sunxi_pressure_enable(unsigned long reg_base)
{
    uint32_t reg_val;

    reg_val = readl(reg_base + TP_CTRL2);
    reg_val  = SET_BITS(PRE_MEA_EN, PRE_MEA_EN_WIDTH, reg_val, 0x1);

    writel(reg_val, reg_base + TP_CTRL2);
}

/* 0: x_data y_data x_data y_data */
static void sunxi_tp_mode_fifo_select(unsigned long reg_base, uint32_t val)
{
    uint32_t reg_val;

    reg_val = readl(reg_base + TP_CTRL2);
    reg_val  = SET_BITS(TP_FIFO_MODE, TP_FIFO_MODE_WIDTH, reg_val, val);

    writel(reg_val, reg_base + TP_CTRL2);
}

static void sunxi_set_sensitivity(unsigned long reg_base, uint32_t val)
{
    uint32_t reg_val;

    reg_val = readl(reg_base + TP_CTRL2);
    reg_val  = SET_BITS(TP_SENSITIVE, TP_SENSITIVE_WIDTH, reg_val, val);

    writel(reg_val, reg_base + TP_CTRL2);
}

static void sunxi_set_filter_type(unsigned long reg_base, uint32_t val)
{
    uint32_t reg_val;

    reg_val = readl(reg_base + TP_CTRL3);
    reg_val  = SET_BITS(FILTER_TYPE, FILTER_TYPE_WIDTH, reg_val, val);

    writel(reg_val, reg_base + TP_CTRL3);
}
static void sunxi_filter_enable(unsigned long reg_base)
{
    uint32_t reg_val;

    reg_val = readl(reg_base + TP_CTRL3);
    reg_val  = SET_BITS(FILTER_EN, FILTER_EN_WIDTH, reg_val, 0x1);

    writel(reg_val, reg_base + TP_CTRL3);
}

static void sunxi_downirq_enable(unsigned long reg_base)
{
    uint32_t reg_val;

    reg_val = readl(reg_base + TP_INT_FIFOC);
    reg_val  = SET_BITS(TP_DOWN_IRQ_EN, TP_DOWN_IRQ_ENWIDTH, reg_val, 0x1);

    writel(reg_val, reg_base + TP_INT_FIFOC);
}

static void sunxi_upirq_enable(unsigned long reg_base)
{
    uint32_t reg_val;

    reg_val = readl(reg_base + TP_INT_FIFOC);
    reg_val  = SET_BITS(TP_UP_IRQ_EN, TP_UP_IRQ_EN_WIDTH, reg_val, 0x1);

    writel(reg_val, reg_base + TP_INT_FIFOC);
}

static void sunxi_drq_enable(unsigned long reg_base)
{
    uint32_t reg_val;

    reg_val = readl(reg_base + TP_INT_FIFOC);
    reg_val  = SET_BITS(TP_DATA_DRQ_EN, TP_DATA_DRQ_EN_WIDTH, reg_val, 0x1);

    writel(reg_val, reg_base + TP_INT_FIFOC);
}

static void sunxi_set_trig_level(unsigned long reg_base, uint32_t val)
{
    uint32_t reg_val;

    reg_val = readl(reg_base + TP_INT_FIFOC);
    reg_val  = SET_BITS(TP_FIFO_TRIG, TP_FIFO_TRIG_WIDTH, reg_val, val);

    writel(reg_val, reg_base + TP_INT_FIFOC);
}

static void sunxi_xydata_change_enable(unsigned long reg_base, uint32_t val)
{
    uint32_t reg_val;

    reg_val = readl(reg_base + TP_INT_FIFOC);
    reg_val = SET_BITS(TP_DATA_XY_CHANGE, TP_DATA_XY_CHANGE_WIDTH, reg_val, val);

    writel(reg_val, reg_base + TP_INT_FIFOC);
}

static void sunxi_irq_enable(unsigned long reg_base)
{
    uint32_t reg_val;

    reg_val = readl(reg_base + TP_INT_FIFOC);
    reg_val = SET_BITS(TP_DATA_IRQ_EN, TP_DATA_IRQ_EN_WIDTH, reg_val, 0x1);

    writel(reg_val, reg_base + TP_INT_FIFOC);
}

static void sunxi_irq_disable(unsigned long reg_base)
{
    uint32_t reg_val;

    reg_val = readl(reg_base + TP_INT_FIFOC);
    reg_val = SET_BITS(TP_DATA_IRQ_EN, TP_DATA_IRQ_EN_WIDTH, reg_val, 0x0);

    writel(reg_val, reg_base + TP_INT_FIFOC);
}

static void sunxi_overrun_enable(unsigned long reg_base)
{
    uint32_t reg_val;

    reg_val = readl(reg_base + TP_INT_FIFOC);
    reg_val = SET_BITS(TP_FIFO_OVERRUN_IRQ, TP_FIFO_OVERRUN_IRQ_WIDTH, reg_val, 0x1);

    writel(reg_val, reg_base + TP_INT_FIFOC);
}

static uint32_t sunxi_tpadc_irq_status(unsigned long reg_base)
{
    return readl(reg_base + TP_INT_FIFOS);
}

static void sunxi_tpadc_clear_pending(unsigned long reg_base)
{
    int reg;

    reg = readl(reg_base + TP_INT_FIFOS);
    writel(reg, reg_base + TP_INT_FIFOS);
}

static uint32_t sunxi_tpadc_ch_select(unsigned long reg_base, tp_channel_id id)
{
    uint32_t reg_val;

    reg_val = readl(reg_base + TP_CTRL1);
    switch (id) {
    case TP_CH_0:
        reg_val |= TP_CH0_SELECT;
        break;
    case TP_CH_1:
        reg_val |= TP_CH1_SELECT;
        break;
    case TP_CH_2:
        reg_val |= TP_CH2_SELECT;
        break;
    case TP_CH_3:
        reg_val |= TP_CH3_SELECT;
        break;
    default:
        PRINTF("%s, invalid channel id!\n", __func__);
        return -1;
    }
    writel(reg_val, reg_base + TP_CTRL1);

    return 0;
}

static int sunxi_tpadc_ch_deselect(unsigned long reg_base, tp_channel_id id)
{
    uint32_t reg_val;

    reg_val = readl(reg_base + TP_CTRL1);
    switch (id) {
    case TP_CH_0:
        reg_val &= ~TP_CH0_SELECT;
        break;
    case TP_CH_1:
        reg_val &= ~TP_CH1_SELECT;
        break;
    case TP_CH_2:
        reg_val &= ~TP_CH2_SELECT;
        break;
    case TP_CH_3:
        reg_val &= ~TP_CH3_SELECT;
        break;
    default:
        PRINTF("%s, invalid channel id!\n", __func__);
        return -1;
    }
    writel(reg_val, reg_base + TP_CTRL1);

    return 0;
}

static uint32_t hal_tpadc_data_read(unsigned long reg_base)
{
    //hal_tpadc_t *tpadc = &hal_tpadc;

    return readl(reg_base + TP_DATA);
}

static int hal_tpadc_callback(uint32_t data, data_flag_t flag)
{
    PRINTF("tpadc : tpadc mode interrupt, data_flag is %d\n", (int) flag);
    return 0;
}

static int hal_tpadc_adc_callback(uint32_t data, tp_channel_id channel)
{
    PRINTF("tpadc : auxiliary adc mode interrupt\n");
    return 0;
}

static void sunxi_tpadc_setup(hal_tpadc_t *tpadc)
{
    uint32_t i;

    tpadc->reg_base = (unsigned long)TPADC_BASE;
    tpadc->irq_num = SUNXI_TPADC_IRQ;
    tpadc->rst_clk_id = (hal_reset_id_t)RST_BUS_TPADC;
    tpadc->bus_clk_id = (hal_clk_id_t)CLK_BUS_TPADC;
    tpadc->mod_clk_id = (hal_clk_id_t)CLK_TPADC;

    tpadc->callback = hal_tpadc_callback;
    for(i=0; i<TP_CH_MAX; i++)
        tpadc->adc_callback[i] = hal_tpadc_adc_callback;
}

static hal_tpadc_status_t hal_tpadc_clk_init(hal_tpadc_t *tpadc)
{
    hal_clk_type_t  clk_type = HAL_SUNXI_CCU;
    hal_clk_id_t bus_clk_id = tpadc->bus_clk_id;
    hal_clk_id_t mod_clk_id = tpadc->mod_clk_id;

    hal_reset_type_t reset_type = HAL_SUNXI_RESET;
    hal_reset_id_t  tpadc_reset_id = tpadc->rst_clk_id;
    struct reset_control *reset;

    reset = hal_reset_control_get(reset_type, tpadc_reset_id);
    if (hal_reset_control_deassert(reset))
    {
        PRINTF("tpadc reset deassert  failed!\n");
        return TPADC_ERROR;
    }
    hal_reset_control_put(reset);

    tpadc->mod_clk = hal_clock_get(clk_type, mod_clk_id);
    if (hal_clock_enable(tpadc->mod_clk))
    {
        PRINTF("tpadc mod clk enable mclk failed!\n");
        return TPADC_ERROR;
    }

    tpadc->bus_clk = hal_clock_get(clk_type, bus_clk_id);
    if (hal_clock_enable(tpadc->bus_clk))
    {
        PRINTF("tpadc bus clk enable mclk failed!\n");
        return TPADC_ERROR;
    }

    return TPADC_OK;
}

static hal_tpadc_status_t hal_tpadc_clk_exit(hal_tpadc_t *tpadc)
{
    hal_clock_disable(tpadc->bus_clk);
    hal_clock_put(tpadc->bus_clk);

    hal_clock_disable(tpadc->mod_clk);
    hal_clock_put(tpadc->mod_clk);

    return TPADC_OK;
}

/* the rtpadc tpadc mode interface */
static irqreturn_t tpadc_handler(int irq, void *dev)
{
    hal_tpadc_t *tpadc = (hal_tpadc_t *)dev;
    uint32_t  reg_val;
    uint32_t x_data, y_data;
    uint32_t i;

    reg_val = sunxi_tpadc_irq_status(tpadc->reg_base);
    if (reg_val & TP_DATAPEND) {
        x_data = hal_tpadc_data_read(tpadc->reg_base);
        tpadc->callback(x_data, DATA_X);
        y_data = hal_tpadc_data_read(tpadc->reg_base);
        tpadc->callback(y_data, DATA_Y);
    }

    if (reg_val & TP_UPPEND) {
        tpadc->callback(0, DATA_UP);
    }

    if (reg_val & TP_DOWNPEND) {
        PRINTF("touch down \n");
    }

    sunxi_tpadc_clear_pending(tpadc->reg_base);

    return TPADC_OK;
}
//
//hal_tpadc_status_t hal_tpadc_register_callback(tpadc_usercallback_t user_callback)
//{
//    hal_tpadc_t *tpadc = &hal_tpadc;
//
//    if (user_callback == NULL)
//        return TPADC_ERROR;
//    else
//        tpadc->callback = user_callback;
//
//    return TPADC_OK;
//}

hal_tpadc_status_t hal_tpadc_init(void)
{
    hal_tpadc_t *tpadc = &hal_tpadc;
    uint32_t clk_in;

    sunxi_tpadc_setup(tpadc);

    if (hal_tpadc_clk_init(tpadc))
    {
        PRINTF("tpadc init clk error\n");
        return TPADC_ERROR;
    }

    /* clear status */
    sunxi_flush_fifo(tpadc->reg_base);
    sunxi_clear_fifo_status(tpadc->reg_base);

    sunxi_clk_init(tpadc->reg_base);

    sunxi_tpadc_mode_select(tpadc->reg_base, 0);
    sunxi_tpadc_enable(tpadc->reg_base, 1);
    sunxi_set_up_debou_time(tpadc->reg_base, 0x00);

    sunxi_set_pressure_thresholed(tpadc->reg_base, 0x800fff);
    sunxi_pressure_enable(tpadc->reg_base);
    sunxi_tp_mode_fifo_select(tpadc->reg_base, 0x00);
    sunxi_set_sensitivity(tpadc->reg_base, 0xf);

    sunxi_set_filter_type(tpadc->reg_base, 0x1);
    sunxi_filter_enable(tpadc->reg_base);

    sunxi_downirq_enable(tpadc->reg_base);
    sunxi_upirq_enable(tpadc->reg_base);
    sunxi_drq_enable(tpadc->reg_base);
    sunxi_set_trig_level(tpadc->reg_base, 0x1);
    sunxi_xydata_change_enable(tpadc->reg_base, 0);
    sunxi_irq_enable(tpadc->reg_base);
    sunxi_overrun_enable(tpadc->reg_base);

    sunxi_tpadc_clear_pending(tpadc->reg_base);

    if (request_irq(tpadc->irq_num, tpadc_handler, 0, "tpadc", tpadc))
    {
        PRINTF("tpadc request irq(%d) failed \n", (int) tpadc->irq_num);
    return TPADC_ERROR;
    }

    enable_irq(tpadc->irq_num);

    PRINTF("tpadc init success\n");

    return TPADC_OK;
}

hal_tpadc_status_t hal_tpadc_exit(void)
{
    hal_tpadc_t *tpadc = &hal_tpadc;

    sunxi_irq_disable(tpadc->reg_base);
    free_irq(tpadc->irq_num, tpadc);
    hal_tpadc_clk_exit(tpadc);

    return TPADC_OK;
}

hal_tpadc_status_t hal_tpadc_resume(void)
{
    hal_tpadc_t *tpadc = &hal_tpadc;

    hal_tpadc_clk_init(tpadc);
    sunxi_tpadc_enable(tpadc->reg_base, 1);

    return TPADC_OK;
}

hal_tpadc_status_t hal_tpadc_suspend(void)
{
    hal_tpadc_t *tpadc = &hal_tpadc;

    sunxi_tpadc_enable(tpadc->reg_base, 0);
    hal_tpadc_clk_exit(tpadc);

    return TPADC_OK;
}

/* the rtpadc adc mode interface*/
static irqreturn_t tpadc_adc_handler(int irq, void *dev)
{
    hal_tpadc_t *tpadc = (hal_tpadc_t *)dev;
    uint32_t reg_val;
    uint32_t data;

    reg_val = sunxi_tpadc_irq_status(tpadc->reg_base);
    if (reg_val & TP_DATAPEND) {
        reg_val = readl(tpadc->reg_base + TP_CTRL1);
        if (reg_val & TP_CH0_SELECT)
        {
            data = hal_tpadc_data_read(tpadc->reg_base);
            if (tpadc->adc_callback[0])
                tpadc->adc_callback[0](data, TP_CH0_SELECT);
        }

        if (reg_val & TP_CH1_SELECT)
        {
            data = hal_tpadc_data_read(tpadc->reg_base);
            if (tpadc->adc_callback[1])
                tpadc->adc_callback[1](data, TP_CH1_SELECT);
        }

        if (reg_val & TP_CH2_SELECT)
        {
            data = hal_tpadc_data_read(tpadc->reg_base);
            if (tpadc->adc_callback[2])
                tpadc->adc_callback[2](data, TP_CH2_SELECT);
        }

        if (reg_val & TP_CH3_SELECT)
        {
            data = hal_tpadc_data_read(tpadc->reg_base);
            if (tpadc->adc_callback[3])
                tpadc->adc_callback[3](data, TP_CH3_SELECT);
        }

        if (reg_val & 0x0)
        {
            PRINTF("no channel init, you need init 1~4 channel first\n");
            return -IRQ_HANDLED;
        }
    }

    sunxi_tpadc_clear_pending(tpadc->reg_base);

    return IRQ_NONE;
}

hal_tpadc_status_t hal_tpadc_adc_register_callback(tp_channel_id channel, tpadc_adc_usercallback_t user_adc_callback)
{
    hal_tpadc_t *tpadc = &hal_tpadc;

    if (user_adc_callback == NULL)
        return TPADC_ERROR;
    else
        tpadc->adc_callback[channel] = user_adc_callback;

    return TPADC_OK;
}

hal_tpadc_status_t hal_tpadc_adc_init(void)
{
    hal_tpadc_t *tpadc = &hal_tpadc;
    uint32_t clk_in;

    sunxi_tpadc_setup(tpadc);

    if (hal_tpadc_clk_init(tpadc))
    {
        PRINTF("tpadc init clk error\n");
        return TPADC_ERROR;
    }

    /* clear status */
    sunxi_flush_fifo(tpadc->reg_base);
    sunxi_clear_fifo_status(tpadc->reg_base);

    sunxi_clk_init(tpadc->reg_base);

    sunxi_tpadc_mode_select(tpadc->reg_base, 1);
    sunxi_tpadc_enable(tpadc->reg_base, 1);

    sunxi_set_filter_type(tpadc->reg_base, 0x1);
    sunxi_filter_enable(tpadc->reg_base);

    sunxi_downirq_enable(tpadc->reg_base);
    sunxi_upirq_enable(tpadc->reg_base);
    sunxi_set_trig_level(tpadc->reg_base, 0x3);
    sunxi_irq_enable(tpadc->reg_base);

    sunxi_tpadc_clear_pending(tpadc->reg_base);

    if (request_irq(tpadc->irq_num, tpadc_adc_handler, 0, "tpadc", tpadc))
    {
        PRINTF("tpadc request irq(%d) failed \n", (int) tpadc->irq_num);
    return TPADC_ERROR;
    }

    enable_irq(tpadc->irq_num);

    PRINTF("tpadc init success\n");

    return TPADC_OK;
}

hal_tpadc_status_t hal_tpadc_adc_channel_init(tp_channel_id channel)
{
    hal_tpadc_t *tpadc = &hal_tpadc;

    if (sunxi_tpadc_adc_ch_select(tpadc->reg_base, channel))
    {
        PRINTF("TPADC adc mode channel select error\n");
        return -1;
    }

    return 0;
}

hal_tpadc_status_t hal_tpadc_adc_exit()
{
    hal_tpadc_t *tpadc = &hal_tpadc;

    sunxi_irq_disable(tpadc->reg_base);
    free_irq(tpadc->irq_num, tpadc);

    hal_clock_disable(tpadc->bus_clk);
    hal_clock_put(tpadc->bus_clk);

    hal_clock_disable(tpadc->mod_clk);
    hal_clock_put(tpadc->mod_clk);

    return TPADC_OK;
}

#endif /* TSC1_TYPE == TSC_TYPE_AWTPADC */

