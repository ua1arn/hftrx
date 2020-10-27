/*
drivers/video/rockchip/transmitter/mipi_dsi.h
*/
#ifndef MIPI_DSI_H_
#define MIPI_DSI_H_

#ifdef CONFIG_MIPI_DSI_FT
#include "..\..\common\config.h"
#endif

//DSI DATA TYPE
#define DTYPE_DCS_SWRITE_0P		0x05 
#define DTYPE_DCS_SWRITE_1P		0x15 
#define DTYPE_DCS_LWRITE		0x39 
#define DTYPE_GEN_LWRITE		0x29 
#define DTYPE_GEN_SWRITE_2P		0x23 
#define DTYPE_GEN_SWRITE_1P		0x13
#define DTYPE_GEN_SWRITE_0P		0x03

//command transmit mode
#define HSDT			0x00
#define LPDT			0x01

//Video Mode
#define VM_NBMWSP		0x00  //Non burst mode with sync pulses
#define VM_NBMWSE		0x01  //Non burst mode with sync events
#define VM_BM			0x02  //Burst mode

//Video Pixel Format
#define VPF_16BPP		0x00
#define VPF_18BPP		0x01	 //packed
#define VPF_18BPPL		0x02     //loosely packed
#define VPF_24BPP		0x03

//Display Command Set
#define dcs_enter_idle_mode 		0x39
#define dcs_enter_invert_mode 		0x21
#define dcs_enter_normal_mode 		0x13
#define dcs_enter_partial_mode  	0x12
#define dcs_enter_sleep_mode  		0x10
#define dcs_exit_idle_mode  		0x38
#define dcs_exit_invert_mode  		0x20
#define dcs_exit_sleep_mode  		0x11
#define dcs_get_address_mode  		0x0b
#define dcs_get_blue_channel  		0x08
#define dcs_get_diagnostic_result  	0x0f
#define dcs_get_display_mode  		0x0d
#define dcs_get_green_channel  		0x07
#define dcs_get_pixel_format  		0x0c
#define dcs_get_power_mode  		0x0a
#define dcs_get_red_channel 		0x06
#define dcs_get_scanline 	 		0x45
#define dcs_get_signal_mode  		0x0e
#define dcs_nop				 		0x00
#define dcs_read_DDB_continue  		0xa8
#define dcs_read_DDB_start  		0xa1
#define dcs_read_memory_continue  	0x3e
#define dcs_read_memory_start  		0x2e
#define dcs_set_address_mode  		0x36
#define dcs_set_column_address  	0x2a
#define dcs_set_display_off  		0x28
#define dcs_set_display_on  		0x29
#define dcs_set_gamma_curve  		0x26
#define dcs_set_page_address  		0x2b
#define dcs_set_partial_area  		0x30
#define dcs_set_pixel_format  		0x3a
#define dcs_set_scroll_area  		0x33
#define dcs_set_scroll_start  		0x37
#define dcs_set_tear_off 	 		0x34
#define dcs_set_tear_on 	 		0x35
#define dcs_set_tear_scanline  		0x44
#define dcs_soft_reset 		 		0x01
#define dcs_write_LUT 		 		0x2d
#define dcs_write_memory_continue  	0x3c
#define dcs_write_memory_start 		0x2c

#if 0
typedef signed char s8;
typedef unsigned char uint8_t;

typedef signed short s16;
typedef unsigned short uint16_t;

typedef signed int s32;
typedef unsigned int uint32_t;

typedef signed long s64;
typedef unsigned long u64;
#endif


//iomux
#define OLD_RK_IOMUX 1

struct spi_t {
	uint32_t cs;
#if OLD_RK_IOMUX	
	char* cs_mux_name;
#endif	
	int sck;
#if OLD_RK_IOMUX	
	char* sck_mux_name;
#endif	
	int miso;
#if OLD_RK_IOMUX	
	char* miso_mux_name;
#endif	
	int mosi;
#if OLD_RK_IOMUX	
	char* mosi_mux_name;
#endif	
};

struct power_t {
	int	enable_pin;    //gpio that control power
#if OLD_RK_IOMUX	
	char* mux_name;
	uint32_t mux_mode;
#endif	
	uint32_t effect_value;
	
	char *name;
	uint32_t voltage;
	int (*enable)(void *);
	int (*disable)(void *);
};

struct reset_t {
	int	reset_pin;    //gpio that control reset
#if OLD_RK_IOMUX	
	char* mux_name;
	uint32_t mux_mode;
#endif	
	uint32_t effect_value;
	
	uint32_t time_before_reset;    //ms
 	uint32_t time_after_reset;
	
	int (*do_reset)(void *);
};

struct tc358768_t {
	uint32_t id;
	struct reset_t reset;
	struct power_t vddc;
	struct power_t vddio;
	struct power_t vdd_mipi;
	struct i2c_client *client;
	int (*gpio_init)(void *);
	int (*gpio_deinit)(void *);
	int (*power_up)(void);
	int (*power_down)(void);
};


struct ssd2828_t {
	uint32_t id;
	struct reset_t reset;
	struct power_t shut;
	struct power_t vddio;
	struct power_t vdd_mipi;
	
	struct spi_t spi;
	int (*gpio_init)(void *);
	int (*gpio_deinit)(void *);
	int (*power_up)(void);
	int (*power_down)(void);
};

struct mipi_dsi_ops {
	uint32_t id;
	char *name;
	int (*get_id)(void);
	int (*dsi_init)(void *, uint32_t n);
	int (*dsi_set_regs)(void *, uint32_t n);
	int (*dsi_enable_video_mode)(uint32_t enable);
	int (*dsi_enable_command_mode)(uint32_t enable);
	int (*dsi_enable_hs_clk)(uint32_t enable);
	int (*dsi_send_dcs_packet)(unsigned char *, uint32_t n);
	int (*dsi_read_dcs_packet)(unsigned char *, uint32_t n);
	int (*dsi_send_packet)(void *, uint32_t n);
	int (*dsi_is_active)(void);
	int (*power_up)(void);
	int (*power_down)(void);	
};


/* Screen description */
struct mipi_dsi_screen {
	
	uint16_t type;
	uint16_t face;
	uint8_t lcdc_id;
	uint8_t screen_id;

	/* Timing */
	uint32_t pixclock;
	uint16_t left_margin;
	uint16_t right_margin;
	uint16_t hsync_len;
	uint16_t upper_margin;
	uint16_t lower_margin;
	uint16_t vsync_len;
	
	/* Screen size */
	uint16_t x_res;
	uint16_t y_res;
	uint16_t width;
	uint16_t height;
	/* Pin polarity */
	uint8_t pin_hsync;
	uint8_t pin_vsync;
	uint8_t pin_den;
	uint8_t pin_dclk;

	/* MIPI DSI */
	uint8_t dsi_lane;
	uint8_t dsi_video_mode;
	uint32_t hs_tx_clk;

	/* Operation function*/
	int (*init)(void);
	int (*standby)(uint8_t enable);
};




int register_dsi_ops(struct mipi_dsi_ops *ops);
int del_dsi_ops(struct mipi_dsi_ops *ops);
int dsi_power_up(void);
int dsi_power_off(void);
int dsi_probe_current_chip(void);
int dsi_init(void *array, uint32_t n);
int dsi_is_active(void);
int dsi_enable_video_mode(uint32_t enable);
int dsi_enable_command_mode(uint32_t enable);
int dsi_enable_hs_clk(uint32_t enable);
int dsi_set_virtual_channel(uint32_t channel);

int dsi_set_regs(void *array, uint32_t n);
int dsi_send_dcs_packet(unsigned char *packet, uint32_t n);
int dsi_read_dcs_packet(unsigned char *packet, uint32_t n);
int dsi_send_packet(void *packet, uint32_t n);
#endif /* end of MIPI_DSI_H_ */
