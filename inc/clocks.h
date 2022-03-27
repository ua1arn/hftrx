/* $Id: $ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//


#ifndef INC_CLOCKS_H_
#define INC_CLOCKS_H_

unsigned long stm32f4xx_get_hse_freq(void);
unsigned long stm32f4xx_get_hsi_freq(void);
unsigned long stm32f4xx_get_pllreference_freq(void);
unsigned long stm32f4xx_get_pll_freq(void);
unsigned long stm32f4xx_get_pll_p_freq(void);
unsigned long stm32f4xx_get_pll_q_freq(void);
unsigned long stm32f4xx_get_plli2s_freq(void);
unsigned long stm32f4xx_get_plli2s_q_freq(void);
unsigned long stm32f4xx_get_plli2s_r_freq(void);
unsigned long stm32f4xx_get_pllsai_freq(void);
unsigned long stm32f4xx_get_pllsai_q_freq(void);
unsigned long stm32f4xx_get_pllsai_r_freq(void);
unsigned long stm32f4xx_get_sysclk_freq(void);
unsigned long stm32f4xx_get_ahb_freq(void);
unsigned long stm32f4xx_get_hclk_freq(void);
unsigned long stm32f4xx_get_apb2_freq(void);
unsigned long stm32f4xx_get_apb1_freq(void);
unsigned long stm32f4xx_get_apb2timer_freq(void);
unsigned long stm32f4xx_get_apb1timer_freq(void);
unsigned long stm32f4xx_get_spi1_freq(void);
unsigned long stm32f4xx_get_tim3_freq(void);

unsigned long stm32f7xx_get_hse_freq(void);
unsigned long stm32f7xx_get_pll_freq(void);
unsigned long stm32f7xx_get_pllsai_freq(void);
unsigned long stm32f7xx_get_plli2s_freq(void);
unsigned long stm32f7xx_get_pll_p_freq(void);
unsigned long stm32f7xx_get_sys_freq(void);
unsigned long stm32f7xx_get_ahb_freq(void);
unsigned long stm32f7xx_get_pclk1_freq(void);
unsigned long stm32f7xx_get_pclk2_freq(void);
unsigned long stm32f7xx_get_usart1_freq(void);
unsigned long stm32f7xx_get_usart2_freq(void);
unsigned long stm32f7xx_get_usart3_freq(void);
unsigned long stm32f7xx_get_uart4_freq(void);
unsigned long stm32f7xx_get_uart5_freq(void);
unsigned long stm32f7xx_get_usart6_freq(void);
unsigned long stm32f7xx_get_uart7_freq(void);
unsigned long stm32f7xx_get_uart8_freq(void);

unsigned long stm32h7xx_get_hse_freq(void);
unsigned long stm32h7xx_get_pll1_freq(void);
unsigned long stm32h7xx_get_pll1_p_freq(void);
unsigned long stm32h7xx_get_pll1_q_freq(void);
unsigned long stm32h7xx_get_pll2_freq(void);
unsigned long stm32h7xx_get_pll2_p_freq(void);
unsigned long stm32h7xx_get_pll2_q_freq(void);
unsigned long stm32h7xx_get_pll3_freq(void);
unsigned long stm32h7xx_get_pll3_p_freq(void);
unsigned long stm32h7xx_get_pll3_q_freq(void);
unsigned long stm32h7xx_get_pll3_r_freq(void);
unsigned long stm32h7xx_get_sys_freq(void);
unsigned long stm32h7xx_get_stm32h7xx_get_d1cpre_freq(void);
unsigned long stm32h7xx_get_hclk3_freq(void);
unsigned long stm32h7xx_get_aclk_freq(void);
unsigned long stm32h7xx_get_hclk1_freq(void);
unsigned long stm32h7xx_get_hclk2_freq(void);
unsigned long stm32h7xx_get_apb1_freq(void);
unsigned long stm32h7xx_get_apb2_freq(void);
unsigned long stm32h7xx_get_timx_freq(void);
unsigned long stm32h7xx_get_pclk1_freq(void);
unsigned long stm32h7xx_get_timy_freq(void);
unsigned long stm32h7xx_get_pclk2_freq(void);
unsigned long stm32h7xx_get_per_freq(void);
unsigned long stm32h7xx_get_pclk3_freq(void);
unsigned long stm32h7xx_get_usart1_6_freq(void);
unsigned long stm32h7xx_get_usart2_to_8_freq(void);
unsigned long stm32h7xx_get_spi1_2_3_freq(void);
unsigned long stm32h7xx_get_spi4_5_freq(void);
unsigned long stm32h7xx_get_adc_freq(void);

void stm32mp1_pll_initialize(void);
void stm32mp1_usb_clocks_initialize(void);
void stm32mp1_audio_clocks_initialize(void);
unsigned long stm32mp1_get_hse_freq(void);
unsigned long stm32mp1_get_hsi_freq(void);
unsigned long stm32mp1_get_pll1_2_ref_freq(void);
unsigned long stm32mp1_get_pll3_ref_freq(void);
unsigned long stm32mp1_get_pll4_ref_freq(void);
unsigned long stm32mp1_get_pll1_freq(void);
unsigned long stm32mp1_get_pll1_p_freq(void);
unsigned long stm32mp1_get_mpuss_freq(void);
unsigned long stm32mp1_get_pll2_freq(void);
unsigned long stm32mp1_get_pll2_p_freq(void);
unsigned long stm32mp1_get_pll2_r_freq(void);
unsigned long stm32mp1_get_pll3_freq(void);
unsigned long stm32mp1_get_pll3_p_freq(void);
unsigned long stm32mp1_get_pll3_q_freq(void);
unsigned long stm32mp1_get_pll3_r_freq(void);
unsigned long stm32mp1_get_pll4_freq(void);
unsigned long stm32mp1_get_pll4_q_freq(void);
unsigned long stm32mp1_get_pll4_p_freq(void);
unsigned long stm32mp1_get_pll4_r_freq(void);
unsigned long stm32mp1_get_eth_freq(void);
unsigned long stm32mp1_get_ethptp_freq(void);
unsigned long stm32mp1_get_axiss_freq(void);
unsigned long stm32mp1_get_per_freq(void);
unsigned long stm32mp1_get_mcuss_freq(void);
unsigned long stm32mp1_get_aclk_freq(void);
unsigned long stm32mp1_get_hclk5_freq(void);
unsigned long stm32mp1_get_hclk6_freq(void);
unsigned long stm32mp1_get_mlhclk_freq(void);
unsigned long stm32mp1_get_pclk1_freq(void);
unsigned long stm32mp1_get_pclk2_freq(void);
unsigned long stm32mp1_get_pclk3_freq(void);
unsigned long stm32mp1_get_pclk4_freq(void);
unsigned long stm32mp1_get_pclk5_freq(void);
unsigned long stm32mp1_get_usbphy_freq(void);
unsigned long stm32mp1_get_usbotg_freq(void);
unsigned long stm32mp1_uart1_get_freq(void);
unsigned long stm32mp1_uart2_4_get_freq(void);
unsigned long stm32mp1_uart3_5_get_freq(void);
unsigned long stm32mp1_uart7_8_get_freq(void);
unsigned long stm32mp1_sdmmc1_2_get_freq(void);
unsigned long stm32mp1_get_timg1_freq(void);
unsigned long stm32mp1_get_timg2_freq(void);
unsigned long stm32mp1_get_spi1_freq(void);
unsigned long stm32mp1_get_adc_freq(void);
unsigned long stm32mp1_get_qspi_freq(void);

//unsigned long xc7z_get_pllsreference_freq(void);
//uint_fast64_t xc7z_get_arm_pll_freq(void);
//uint_fast64_t xc7z_get_ddr_pll_freq(void);
uint_fast64_t xc7z_get_io_pll_freq(void);
unsigned long xc7z_get_arm_freq(void);
unsigned long xc7z_get_ddr_x2clk_freq(void);
unsigned long xc7z_get_ddr_x3clk_freq(void);
unsigned long xc7z_get_uart_freq(void);
unsigned long xc7z_get_sdio_freq(void);
unsigned long xc7z_get_spi_freq(void);

// platform-independed functions.
unsigned long hardware_get_spi_freq(void);	// получение тактовой частоты тактирования блока SPI, использующенося в данной конфигурации
unsigned long hardware_get_apb1_freq(void);
unsigned long hardware_get_apb1_tim_freq(void);
unsigned long hardware_get_apb2_freq(void);
unsigned long hardware_get_apb2_tim_freq(void);


#endif /* INC_CLOCKS_H_ */
