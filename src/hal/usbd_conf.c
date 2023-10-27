/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : usbd_conf.c
  * @version        : v1.0_Cube
  * @brief          : This file implements the board support package for the USB device library
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "hardware.h"
#include "formats.h"

#if WITHUSBHW

void Error_Handler(void)
{
	ASSERT(0);
	for (;;)
		;
}

#endif /* WITHUSBHW */

#if WITHUSBHW && defined (WITHUSBHW_DEVICE)

#include "gpio.h"
#include "src/usb/usbch9.h"

#include "usbd_def.h"
#include "usbd_core.h"
#include "usb_device.h"

#include "usbh_def.h"
#include "usbh_core.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
USBD_StatusTypeDef USBD_Get_USB_Status(HAL_StatusTypeDef hal_status);


/* USER CODE END PV */


/* PCD Handle Structure */
__ALIGN_BEGIN PCD_HandleTypeDef hpcd_USB_OTG __ALIGN_END;
/* USB Device Core handle declaration */
static __ALIGN_BEGIN USBD_HandleTypeDef hUsbDevice __ALIGN_END;


/* USER CODE BEGIN 1 */

void OTG_HS_IRQHandler(void)
{
  /* USER CODE BEGIN OTG_HS_IRQn 0 */

  /* USER CODE END OTG_HS_IRQn 0 */
  HAL_PCD_IRQHandler(& hpcd_USB_OTG);
  /* USER CODE BEGIN OTG_HS_IRQn 1 */

  /* USER CODE END OTG_HS_IRQn 1 */
}

void device_OTG_HS_IRQHandler(void)
{
  /* USER CODE BEGIN OTG_HS_IRQn 0 */

  /* USER CODE END OTG_HS_IRQn 0 */
  HAL_PCD_IRQHandler(& hpcd_USB_OTG);
  /* USER CODE BEGIN OTG_HS_IRQn 1 */

  /* USER CODE END OTG_HS_IRQn 1 */
}

void device_OTG_FS_IRQHandler(void)
{
  /* USER CODE BEGIN OTG_HS_IRQn 0 */

  /* USER CODE END OTG_HS_IRQn 0 */
  HAL_PCD_IRQHandler(& hpcd_USB_OTG);
  /* USER CODE BEGIN OTG_HS_IRQn 1 */

  /* USER CODE END OTG_HS_IRQn 1 */
}

void device_OTG_HS_EP1_OUT_IRQHandler(void)
{
	  HAL_PCD_IRQHandler(& hpcd_USB_OTG);
}

void device_OTG_HS_EP1_IN_IRQHandler(void)
{
	  HAL_PCD_IRQHandler(& hpcd_USB_OTG);
}


void device_USBI0_IRQHandler(void)
{
	HAL_PCD_IRQHandler(& hpcd_USB_OTG);
}

void device_USBI1_IRQHandler(void)
{
	HAL_PCD_IRQHandler(& hpcd_USB_OTG);
}

/* External functions --------------------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* Private functions ---------------------------------------------------------*/

/* USER CODE BEGIN 1 */
//#include "stm32mp1xx_ll_pwr.h"

/* USER CODE END 1 */

/*******************************************************************************
                       LL Driver Callbacks (PCD -> USB Device Library)
*******************************************************************************/
/* MSP Init */
void OTG_HS_IRQHandler(void);
void device_USBI0_IRQHandler(void);
void device_USBI1_IRQHandler(void);
void device_OTG_HS_EP1_OUT_IRQHandler(void);
void device_OTG_HS_EP1_IN_IRQHandler(void);
void device_OTG_HS_IRQHandler(void);
void device_OTG_FS_IRQHandler(void);


void HAL_PCD_MspInit(PCD_HandleTypeDef* pcdHandle)
{
#if CPUSTYLE_R7S721

	if (pcdHandle->Instance == & USB200)
	{

		/* ---- Supply clock to the USB20(channel 0) ---- */
		CPG.STBCR7 &= ~ CPG_STBCR7_MSTP71;	// Module Stop 71 0: Channel 0 of the USB 2.0 host/function module runs.
		(void) CPG.STBCR7;			/* Dummy read */
	    //memset(pcdHandle->Instance, 0, sizeof * pcdHandle->Instance);

		HARDWARE_USB0_INITIALIZE();

	}
	else if (pcdHandle->Instance == & USB201)
	{

		/* ---- Supply clock to the USB20(channel 1) ---- */
		CPG.STBCR7 &= ~ CPG_STBCR7_MSTP70;	// Module Stop 70 0: Channel 1 of the USB 2.0 host/function module runs.
		CPG.STBCR7 &= ~ CPG_STBCR7_MSTP71;	// Module Stop 71 0: Channel 0 of the USB 2.0 host/function module runs.
		(void) CPG.STBCR7;			/* Dummy read */
	    //memset(pcdHandle->Instance, 0, sizeof * pcdHandle->Instance);

		HARDWARE_USB1_INITIALIZE();
	}

#elif CPUSTYLE_STM32MP1
	// Set 3.3 volt DETECTOR enable
	LL_PWR_EnableUSBVoltageDetector();
	while (LL_PWR_IsEnabledUSBVoltageDetector() == 0)
		;

	// Wait 3.3 volt REGULATOR ready
	while (LL_PWR_IsActiveFlag_USB() == 0)
		;

	__HAL_RCC_USBO_CLK_ENABLE();
	__HAL_RCC_USBO_CLK_SLEEP_ENABLE();

	//RCC->MP_AHB2ENSETR = RCC_MP_AHB2ENSETR_USBOEN;
	(void) RCC->MP_AHB2ENSETR;
	//RCC->MP_AHB2LPENSETR = RCC_MP_AHB2LPENSETR_USBOLPEN;
	(void) RCC->MP_AHB2LPENSETR;

	__HAL_RCC_USBO_FORCE_RESET();
	__HAL_RCC_USBO_RELEASE_RESET();
	//RCC->MP_AHB2ENSETR = RCC_MP_AHB2ENSETR_USBOEN;
	(void) RCC->MP_AHB2ENSETR;
	//RCC->MP_AHB2LPENSETR = RCC_MP_AHB2LPENSETR_USBOLPEN;
	(void) RCC->MP_AHB2LPENSETR;

	if (pcdHandle->Instance == USB1_OTG_HS)	// legacy name is USB_OTG_HS
	{
		if (pcdHandle->Init.phy_itface == USB_OTG_ULPI_PHY)
		{
			ASSERT(0);	// ULPI not supported by CPU
			//USBD_HS_ULPI_INITIALIZE();

//			__HAL_RCC_USBO_CLK_ENABLE();
//			__HAL_RCC_USBO_CLK_SLEEP_ENABLE();
//			__HAL_RCC_USBOULPI_CLK_ENABLE();
//			__HAL_RCC_USBOULPI_CLK_SLEEP_ENABLE();

//			RCC->MP_AHB2ENSETR |= RCC_MP_AHB2ENSETR_USBOEN | RCC_MP_AHB2ENSETR_USBOULPIEN;	/* USB/OTG HS with ULPI */
//			(void) RCC->AHB1ENR;
//			RCC->MP_AHB2LPENSETR |= RCC_MP_AHB2LPENSETR_USBOLPEN; /* USB/OTG HS  */
//			(void) RCC->MP_AHB2LPENSETR;
//			RCC->MP_AHB2LPENSETR |= RCC_MP_AHB2LPENSETR_USBOULPILPEN; /* USB/OTG HS ULPI  */
//			(void) RCC->MP_AHB2LPENSETR;
		}
		else
		{
			USBD_HS_FS_INITIALIZE();
		}
		//RCC->APB4ENR |= RCC_APB4ENR_SYSCFGEN;	/* USB/OTG HS companion - VBUS? */
		//(void) RCC->APB4ENR;

//		RCC->AHB2ENR |= RCC_AHB2ENR_D2SRAM1EN;
//		(void) RCC->AHB2ENR;
//		RCC->AHB2ENR |= RCC_AHB2ENR_D2SRAM2EN;
//		(void) RCC->AHB2ENR;
//		RCC->AHB2ENR |= RCC_AHB2ENR_D2SRAM3EN;
//		(void) RCC->AHB2ENR;


		if (pcdHandle->Init.use_dedicated_ep1 == ENABLE)
		{
			//arm_hardware_set_handler_system(OTG_HS_EP1_OUT_IRQn, device_OTG_HS_EP1_OUT_IRQHandler);
			//arm_hardware_set_handler_system(OTG_HS_EP1_IN_IRQn, device_OTG_HS_EP1_IN_IRQHandler);
		}
		arm_hardware_set_handler_system(OTG_IRQn, device_OTG_HS_IRQHandler);

	}

#elif CPUSTYLE_STM32H7XX

	//PWR->CR3 |= PWR_CR3_USBREGEN;

	//while ((PWR->CR3 & PWR_CR3_USB33RDY) == 0)
	//	;
	//PWR->CR3 |= PWR_CR3_USBREGEN;
	//while ((PWR->CR3 & PWR_CR3_USB33RDY) == 0)
	//	;
	PWR->CR3 |= PWR_CR3_USB33DEN;

	if (pcdHandle->Instance == USB1_OTG_HS)	// legacy name is USB_OTG_HS
	{
		if (pcdHandle->Init.phy_itface == USB_OTG_ULPI_PHY)
		{
			USBD_HS_ULPI_INITIALIZE();
			RCC->AHB1ENR |= RCC_AHB1ENR_USB1OTGHSEN | RCC_AHB1ENR_USB1OTGHSULPIEN;	/* USB/OTG HS with ULPI */
			(void) RCC->AHB1ENR;
			RCC->AHB1LPENR |= RCC_AHB1LPENR_USB1OTGHSLPEN; /* USB/OTG HS  */
			(void) RCC->AHB1LPENR;
			RCC->AHB1LPENR |= RCC_AHB1LPENR_USB1OTGHSULPILPEN; /* USB/OTG HS ULPI  */
			(void) RCC->AHB1LPENR;
		}
		else
		{
			USBD_HS_FS_INITIALIZE();

			PRINTF(PSTR("HAL_PCD_MspInitEx: HS without ULPI\n"));

			RCC->AHB1ENR |= RCC_AHB1ENR_USB1OTGHSEN; /* USB/OTG HS  */
			(void) RCC->AHB1ENR;
			RCC->AHB1LPENR |= RCC_AHB1LPENR_USB1OTGHSLPEN; /* USB/OTG HS  */
			(void) RCC->AHB1LPENR;
			RCC->AHB1LPENR &= ~ RCC_AHB1LPENR_USB1OTGHSULPILPEN; /* USB/OTG HS ULPI  */
			(void) RCC->AHB1LPENR;
		}
		//RCC->APB4ENR |= RCC_APB4ENR_SYSCFGEN;	/* USB/OTG HS companion - VBUS? */
		//(void) RCC->APB4ENR;
		RCC->AHB2ENR |= RCC_AHB2ENR_D2SRAM1EN;
		(void) RCC->AHB2ENR;
		RCC->AHB2ENR |= RCC_AHB2ENR_D2SRAM2EN;
		(void) RCC->AHB2ENR;
		RCC->AHB2ENR |= RCC_AHB2ENR_D2SRAM3EN;
		(void) RCC->AHB2ENR;


		if (pcdHandle->Init.use_dedicated_ep1 == ENABLE)
		{
			arm_hardware_set_handler_system(OTG_HS_EP1_OUT_IRQn, device_OTG_HS_EP1_OUT_IRQHandler);
			arm_hardware_set_handler_system(OTG_HS_EP1_IN_IRQn, device_OTG_HS_EP1_IN_IRQHandler);
		}
		arm_hardware_set_handler_system(OTG_HS_IRQn, device_OTG_HS_IRQHandler);

	}
	else if (pcdHandle->Instance == USB2_OTG_FS)	// legacy name is USB_OTG_FS
	{
		if (pcdHandle->Init.phy_itface == USB_OTG_ULPI_PHY)
		{
			USBD_FS_INITIALIZE();
			RCC->AHB1ENR |= RCC_AHB1ENR_USB2OTGHSEN | RCC_AHB1ENR_USB2OTGHSULPIEN;	/* USB/OTG HS with ULPI */
			(void) RCC->AHB1ENR;
		}
		else
		{
			USBD_FS_INITIALIZE();
			RCC->AHB1ENR |= RCC_AHB1ENR_USB2OTGHSEN;	/* USB/OTG HS  */
			(void) RCC->AHB1ENR;
		}
		//RCC->APB4ENR |= RCC_APB4ENR_SYSCFGEN;	/* USB/OTG FS companion - VBUS? */
		//(void) RCC->APB4ENR;
		RCC->AHB2ENR |= RCC_AHB2ENR_D2SRAM1EN;
		(void) RCC->AHB2ENR;
		RCC->AHB2ENR |= RCC_AHB2ENR_D2SRAM2EN;
		(void) RCC->AHB2ENR;
		RCC->AHB2ENR |= RCC_AHB2ENR_D2SRAM3EN;
		(void) RCC->AHB2ENR;

		NVIC_SetVector(OTG_FS_IRQn, (uintptr_t) & device_OTG_FS_IRQHandler);
		NVIC_SetPriority(OTG_FS_IRQn, ARM_SYSTEM_PRIORITY);
		NVIC_EnableIRQ(OTG_FS_IRQn);	// OTG_FS_IRQHandler() enable

	}

#elif defined (STM32F40_41xxx)

	//const uint_fast32_t stm32f4xx_pllq = arm_hardware_stm32f7xx_pllq_initialize();	// Настроить выход PLLQ на 48 МГц
	//PRINTF(PSTR("HAL_PCD_MspInit: stm32f4xx_pllq=%lu, freq=%lu\n"), (unsigned long) stm32f4xx_pllq, PLL_FREQ / stm32f4xx_pllq);

	USBD_FS_INITIALIZE();
	RCC->AHB2ENR |= RCC_AHB2ENR_OTGFSEN;	/* USB/OTG FS  */
	(void) RCC->AHB2ENR;
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;	/* USB/OTG FS companion - VBUS? */
	(void) RCC->APB2ENR;

	NVIC_SetVector(OTG_FS_IRQn, (uintptr_t) & device_OTG_FS_IRQHandler);
	NVIC_SetPriority(OTG_FS_IRQn, ARM_SYSTEM_PRIORITY);
	NVIC_EnableIRQ(OTG_FS_IRQn);	// OTG_FS_IRQHandler() enable

#elif CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX

	if (pcdHandle->Instance == USB_OTG_HS)
	{
		//const uint_fast32_t stm32f4xx_pllq = arm_hardware_stm32f7xx_pllq_initialize();	// Настроить выход PLLQ на 48 МГц
		//PRINTF(PSTR("HAL_PCD_MspInit: stm32f4xx_pllq=%lu, freq=%lu\n"), (unsigned long) stm32f4xx_pllq, PLL_FREQ / stm32f4xx_pllq);

		if (pcdHandle->Init.phy_itface == USB_OTG_ULPI_PHY)
		{
			USBD_HS_ULPI_INITIALIZE();

			//PRINTF(PSTR("HAL_PCD_MspInit: USB_OTG_HS and ULPI\n"));
			RCC->AHB1ENR |= RCC_AHB1ENR_OTGHSEN;		/* USB/OTG HS  */
			(void) RCC->AHB1ENR;
			RCC->AHB1LPENR |= RCC_AHB1LPENR_OTGHSLPEN;		/* USB/OTG HS  */
			(void) RCC->AHB1LPENR;
			RCC->AHB1ENR |= RCC_AHB1ENR_OTGHSULPIEN;		/* USB/OTG HS with ULPI */
			(void) RCC->AHB1ENR;
			RCC->AHB1LPENR |= RCC_AHB1LPENR_OTGHSULPILPEN;	/* USB/OTG HS  */
			(void) RCC->AHB1LPENR;
		}
		else
		{
			USBD_HS_FS_INITIALIZE();

			//PRINTF(PSTR("HAL_PCD_MspInit: USB_OTG_HS without ULPI\n"));
			RCC->AHB1ENR |= RCC_AHB1ENR_OTGHSEN;	/* USB/OTG HS  */
			(void) RCC->AHB1ENR;
			RCC->AHB1LPENR |= RCC_AHB1LPENR_OTGHSLPEN; /* USB/OTG HS  */
			(void) RCC->AHB1LPENR;
			RCC->AHB1LPENR &= ~ RCC_AHB1LPENR_OTGHSULPILPEN; /* USB/OTG HS ULPI  */
			(void) RCC->AHB1LPENR;
		}

		RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;	/* USB/OTG HS companion - VBUS? */
		(void) RCC->APB2ENR;

		if (pcdHandle->Init.use_dedicated_ep1 == ENABLE)
		{
			NVIC_SetVector(OTG_HS_EP1_OUT_IRQn, (uintptr_t) & device_OTG_HS_EP1_OUT_IRQHandler);
			NVIC_SetPriority(OTG_HS_EP1_OUT_IRQn, ARM_SYSTEM_PRIORITY);
			NVIC_EnableIRQ(OTG_HS_EP1_OUT_IRQn);	// OTG_HS_EP1_OUT_IRQHandler() enable

			NVIC_SetVector(OTG_HS_EP1_IN_IRQn, (uintptr_t) & device_OTG_HS_EP1_IN_IRQHandler);
			NVIC_SetPriority(OTG_HS_EP1_IN_IRQn, ARM_SYSTEM_PRIORITY);
			NVIC_EnableIRQ(OTG_HS_EP1_IN_IRQn);	// OTG_HS_EP1_IN_IRQHandler() enable
		}
		NVIC_SetVector(OTG_HS_IRQn, (uintptr_t) & device_OTG_HS_IRQHandler);
		NVIC_SetPriority(OTG_HS_IRQn, ARM_SYSTEM_PRIORITY);
		NVIC_EnableIRQ(OTG_HS_IRQn);	// OTG_HS_IRQHandler() enable

	}
	else if (pcdHandle->Instance == USB_OTG_FS)
	{
		//const uint_fast32_t stm32f4xx_pllq = arm_hardware_stm32f7xx_pllq_initialize();	// Настроить выход PLLQ на 48 МГц
		//PRINTF(PSTR("HAL_PCD_MspInit: stm32f4xx_pllq=%lu, freq=%lu\n"), (unsigned long) stm32f4xx_pllq, PLL_FREQ / stm32f4xx_pllq);

		USBD_FS_INITIALIZE();
		RCC->AHB2ENR |= RCC_AHB2ENR_OTGFSEN;	/* USB/OTG FS  */
		(void) RCC->AHB2ENR;
		RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;	/* USB/OTG FS companion - VBUS? */
		(void) RCC->APB2ENR;

		NVIC_SetVector(OTG_FS_IRQn, (uintptr_t) & device_OTG_FS_IRQHandler);
		NVIC_SetPriority(OTG_FS_IRQn, ARM_SYSTEM_PRIORITY);
		NVIC_EnableIRQ(OTG_FS_IRQn);	// OTG_FS_IRQHandler() enable

	}

#elif CPUSTYLE_T507

//	void SetupUsbPhyc(USBPHYC_TypeDef * phy);
//
//	// Отличия в CCU
////	CCU->SMHC2_CLK_REG = 0;
////	CCU->SMHC_BGR_REG = 0;
//
//	CCU->USB0_CLK_REG |= 0x20000000;	// @0x0A70 was: 0x40000000
//
//	CCU->USB2_CLK_REG |= 0x60000000;	// WAS: 0 - USBPHY2RST, SCLK_GATING _USBPHY2
//	CCU->USB_BGR_REG |= 0x00400040; // WAS 01000100 - USBEHCI2_RST USBEHCI2_GATING

	{
		CCU->USB0_CLK_REG |= 0x20000000;	// @0x0A70 was: 0x40000000

		CCU->USB2_CLK_REG |= 0x60000000;	// WAS: 0 - USBPHY2RST, SCLK_GATING _USBPHY2
		CCU->USB_BGR_REG |= 0x00400040; // WAS 01000100 - USBEHCI2_RST USBEHCI2_GATING

//		const unsigned OHCIx_12M_SRC_SEL = 1;	// OHCI3_12M_SRC_SEL 01: 12M divided from 24 MHz
//
//		{
//			// PHY2 enable
//			CCU->USB2_CLK_REG = (CCU->USB2_CLK_REG & ~ (UINT32_C(0x03) << 24)) | (OHCIx_12M_SRC_SEL << 24);
//			CCU->USB2_CLK_REG |= (UINT32_C(1) << 30);	// USBPHY2_RST
//			CCU->USB2_CLK_REG |= (UINT32_C(1) << 29);	// SCLK_GATING_USBPHY2
//			SetupUsbPhyc(USBPHYC2);
//		}

	}
	arm_hardware_disable_handler(USB20_HOST0_OHCI_IRQn);
	arm_hardware_disable_handler(USB20_HOST0_EHCI_IRQn);
	arm_hardware_disable_handler(USB20_OTG_DEVICE_IRQn);

	CCU->USB_BGR_REG |= (UINT32_C(1) << 8);	// USBOTG_GATING
	CCU->USB_BGR_REG &= ~ (UINT32_C(1) << 24);	// USBOTG_RST
	CCU->USB_BGR_REG |= (UINT32_C(1) << 24);	// USBOTG_RST

	CCU->USB0_CLK_REG &= ~ (UINT32_C(1) << 31);	// SCLK_GATING_OHCI0
	CCU->USB0_CLK_REG &= ~ (UINT32_C(1) << 30);	// USBPHY0_RST
	CCU->USB0_CLK_REG |= (UINT32_C(1) << 30);	// USBPHY0_RST

	//SetupUsbPhyc(USB20_OTG_PHYC_BASE); // ?
	//USB20_OTG_PHYC->USB_CTRL = UINT32_C(0x4300CC01);	// младший бит не влияет... вообще ничего не влияет
	//USBPHYC0->USB_CTRL =  UINT32_C(0x4300CC00);	// влияет!
	arm_hardware_set_handler_system(USB20_OTG_DEVICE_IRQn, device_OTG_HS_IRQHandler);

	// Work
	//	USB20_OTG_PHYC_BASE:
	//	05100400  4300CC00 00000000 00000000 00000000 00000022 00000000 023438E4 00000000
	//	05100420  00000001 00000008 00000000 00000000 00000000 00000000 00000000 00000000
	//	05100440  00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
	//	05100460  00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
	// bad
	//	USB20_OTG_PHYC_BASE:
	//	05100400  40000000 00000000 00000000 00000000 00000002 00000000 023438E4 00000000
	//	05100420  00000001 00000004 00000000 00000000 00000000 00000000 00000000 00000000
	//	05100440  00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
	//	05100460  00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000

	USB20_OTG_PHYC->USB_CTRL = 0x4300CC00;
	USB20_OTG_PHYC->PHY_CTRL = 0x00000022;
	USB20_OTG_PHYC->HSIC_PHY_tune3 = 0x00000008;

	PRINTF("USB20_OTG_PHYC_BASE:\n");
	printhex32(USB20_OTG_PHYC_BASE, USB20_OTG_PHYC, 128);
	PRINTF("USBPHYC0:\n");
	printhex32(USBPHYC0_BASE, USBPHYC0, 128);

//	PRINTF("CCU:\n");
//	printhex32(CCU_BASE, CCU, sizeof * CCU);

#elif CPUSTYLE_A64

	//	Allwinner USB DRD support (musb_otg)
	//
	//	Allwinner USB DRD is based on the Mentor USB OTG controller, with a
	//	different register layout and a few missing registers.
	// Turn off EHCI0
//	PRINTF("1 HAL_PCD_MspInit: USBx->PHY_CTRL=%08lX\n", USBx->PHY_CTRL);
//	PRINTF("1 HAL_PCD_MspInit: USBx->USB_CTRL=%08lX\n", USBx->USB_CTRL);
//	PRINTF("1 HAL_PCD_MspInit: CCU->USB0_CLK_REG=%08lX\n", CCU->USB0_CLK_REG);

    arm_hardware_disable_handler(USBOTG0_IRQn);
    arm_hardware_disable_handler(USBEHCI0_IRQn);
    arm_hardware_disable_handler(USBOHCI0_IRQn);

//    PRINTF("USBPHY_CFG_REG = %08X\n", CCU->USBPHY_CFG_REG);

    CCU->USBPHY_CFG_REG &= ~ (1u << 8);	// SCLK_GATING_USBPHY0
    CCU->USBPHY_CFG_REG &= ~ (1u << 0);	// USBPHY0_RST
	// Turn on USBOTG0
    CCU->USBPHY_CFG_REG |= (1u << 8);	// SCLK_GATING_USBPHY0
    CCU->USBPHY_CFG_REG |= (1u << 0);	// USBPHY0_RST

    //CCU->USBPHY_CFG_REG |= (1u << 2);	// USBHSIC_RST ???

	CCU->BUS_SOFT_RST_REG0 &= ~ (1u << 29);	// USB-OHCI0_RST.
	CCU->BUS_SOFT_RST_REG0 &= ~ (1u << 25);	// USB-EHCI0_RST.

	CCU->BUS_CLK_GATING_REG0 &= ~ (1u << 29);	// USBOHCI0_GATING.
	CCU->BUS_CLK_GATING_REG0 &= ~ (1u << 25);	// USBEHCI0_GATING.

	CCU->BUS_CLK_GATING_REG0 |= (1u << 23);	// USB-OTG-Device_GATING.
	CCU->BUS_SOFT_RST_REG0 |= (1u << 23);	// USB-OTG-Device_RST.


	arm_hardware_set_handler_system(USBOTG0_IRQn, device_OTG_HS_IRQHandler);

	// https://github.com/abmwine/FreeBSD-src/blob/86cb59de6f4c60abd0ea3695ebe8fac26ff0af44/sys/dev/usb/controller/musb_otg_allwinner.c
	// https://github.com/abmwine/FreeBSD-src/blob/86cb59de6f4c60abd0ea3695ebe8fac26ff0af44/sys/dev/usb/controller/musb_otg.c

	// https://github.com/guanglun/r329-linux/blob/d6dced5dc9353fad5319ef5fb84e677e2b9a96b4/arch/arm64/boot/dts/allwinner/sun50i-r329.dtsi#L462
	//	/* A83T specific control bits for PHY0 */
	//	#define PHY_CTL_VBUSVLDEXT		BIT(5)
	//	#define PHY_CTL_SIDDQ			BIT(3)
	//	#define PHY_CTL_H3_SIDDQ		BIT(1)

//	USBOTG0->PHY_OTGCTL |= (1uL << 0); 	// Device mode. Route phy0 to OTG0
//
//	USBOTG0->USB_ISCR = 0;//0x4300FC00;	// после запуска из QSPI было 0x40000000
//	// Looks like 9.6.6.24 0x0810 PHY Control Register (Default Value: 0x0000_0008)
//	//USB0_PHY->PHY_CTRL = 0x20;		// после запуска из QSPI было 0x00000008 а из загрузчика 0x00020
//	USBOTG0->PHY_CTRL &= ~ (1uL << 3);	// PHY_CTL_SIDDQ
//	USBOTG0->PHY_CTRL |= (1uL << 5);	// PHY_CTL_VBUSVLDEXT
//    PRINTF("USBPHY_CFG_REG = %08X\n", CCU->USBPHY_CFG_REG);

#elif CPUSTYLE_T113 || CPUSTYLE_F133
	//	Allwinner USB DRD support (musb_otg)
	//
	//	Allwinner USB DRD is based on the Mentor USB OTG controller, with a
	//	different register layout and a few missing registers.
	// Turn off EHCI0
//	PRINTF("1 HAL_PCD_MspInit: USBx->PHY_CTRL=%08lX\n", USBx->PHY_CTRL);
//	PRINTF("1 HAL_PCD_MspInit: USBx->USB_CTRL=%08lX\n", USBx->USB_CTRL);
//	PRINTF("1 HAL_PCD_MspInit: CCU->USB0_CLK_REG=%08lX\n", CCU->USB0_CLK_REG);

    arm_hardware_disable_handler(USB0_DEVICE_IRQn);
    arm_hardware_disable_handler(USB0_EHCI_IRQn);
    arm_hardware_disable_handler(USB0_OHCI_IRQn);

	CCU->USB_BGR_REG &= ~ (1uL << 16);	// USBOHCI0_RST
	CCU->USB_BGR_REG &= ~ (1uL << 20);	// USBEHCI0_RST
	CCU->USB_BGR_REG &= ~ (1uL << 24);	// USBOTG0_RST

	CCU->USB_BGR_REG &= ~ (1uL << 0);	// USBOHCI0_GATING
	CCU->USB_BGR_REG &= ~ (1uL << 4);	// USBEHCI0_GATING

	// Turn on USBOTG0
	CCU->USB_BGR_REG |= (1uL << 8);	// USBOTG0_GATING
	CCU->USB_BGR_REG &= ~ (1uL << 24);	// USBOTG0_RST Assert
	CCU->USB_BGR_REG |= (1uL << 24);	// USBOTG0_RST De-assert

	// Enable
	CCU->USB0_CLK_REG &= ~ (1uL << 31);	// USB0_CLKEN - Gating Special Clock For OHCI0 0: Clock is OFF

	CCU->USB0_CLK_REG &= ~ (1uL << 30);	// USBPHY0_RSTN Assert
	CCU->USB0_CLK_REG |= (1uL << 30);	// USBPHY0_RSTN De-assert

	// https://github.com/abmwine/FreeBSD-src/blob/86cb59de6f4c60abd0ea3695ebe8fac26ff0af44/sys/dev/usb/controller/musb_otg_allwinner.c
	// https://github.com/abmwine/FreeBSD-src/blob/86cb59de6f4c60abd0ea3695ebe8fac26ff0af44/sys/dev/usb/controller/musb_otg.c

	// https://github.com/guanglun/r329-linux/blob/d6dced5dc9353fad5319ef5fb84e677e2b9a96b4/arch/arm64/boot/dts/allwinner/sun50i-r329.dtsi#L462
	//	/* A83T specific control bits for PHY0 */
	//	#define PHY_CTL_VBUSVLDEXT		BIT(5)
	//	#define PHY_CTL_SIDDQ			BIT(3)
	//	#define PHY_CTL_H3_SIDDQ		BIT(1)

	USBOTG0->PHY_OTGCTL |= (1uL << 0); 	// Device mode. Route phy0 to OTG0

	USBOTG0->USB_ISCR = 0;//0x4300FC00;	// после запуска из QSPI было 0x40000000
	// Looks like 9.6.6.24 0x0810 PHY Control Register (Default Value: 0x0000_0008)
	//USB0_PHY->PHY_CTRL = 0x20;		// после запуска из QSPI было 0x00000008 а из загрузчика 0x00020
	USBOTG0->PHY_CTRL &= ~ (1uL << 3);	// PHY_CTL_SIDDQ
	USBOTG0->PHY_CTRL |= (1uL << 5);	// PHY_CTL_VBUSVLDEXT

	arm_hardware_set_handler_system(USB0_DEVICE_IRQn, device_OTG_HS_IRQHandler);

#else
	#error HAL_PCD_MspInit should be implemented

#endif
}

void HAL_PCD_MspDeInit(PCD_HandleTypeDef* pcdHandle)
{
#if CPUSTYLE_R7S721

	if (pcdHandle->Instance == & USB200)
	{
		//arm_hardware_disable_handler(USBI0_IRQn);
		pcdHandle->Instance->SYSCFG0 &= ~ USB_SYSCFG_USBE;
		pcdHandle->Instance->INTENB0 = 0;
		pcdHandle->Instance->INTENB1 = 0;
		//memset(pcdHandle->Instance, 0, sizeof * pcdHandle->Instance);

		/* ---- Stop clock to the USB20(channel 0) ---- */
		//CPG.STBCR7 |= CPG_STBCR7_MSTP71;	// Module Stop 71 1: Channel 0 of the USB 2.0 host/function module halts.
		//(void) CPG.STBCR7;			/* Dummy read */

		//HARDWARE_USB0_UNINITIALIZE();

	}
	else if (pcdHandle->Instance == & USB201)
	{
		//arm_hardware_disable_handler(USBI1_IRQn);
		pcdHandle->Instance->SYSCFG0 &= ~ USB_SYSCFG_USBE;
		pcdHandle->Instance->INTENB0 = 0;
		pcdHandle->Instance->INTENB1 = 0;
		//memset(pcdHandle->Instance, 0, sizeof * pcdHandle->Instance);

		/* ---- Stop clock to the USB20(channel 1) ---- */
		CPG.STBCR7 |= CPG_STBCR7_MSTP70;	// Module Stop 70 1: Channel 1 of the USB 2.0 host/function module halts.
		//CPG.STBCR7 |= CPG_STBCR7_MSTP71;	// Module Stop 71 1: Channel 0 of the USB 2.0 host/function module halts.
		(void) CPG.STBCR7;			/* Dummy read */

		//HARDWARE_USB1_UNINITIALIZE();
	}


#elif CPUSTYLE_STM32H7XX

	  if (pcdHandle->Instance == USB1_OTG_HS)
	  {
	    /* Peripheral interrupt Deinit*/
	    arm_hardware_disable_handler(OTG_HS_IRQn);
	  }
	  else if (pcdHandle->Instance == USB2_OTG_FS)
	  {
	    /* Peripheral interrupt Deinit*/
	    arm_hardware_disable_handler(OTG_FS_IRQn);
	  }

#elif CPUSTYLE_STM32MP1

	  if (pcdHandle->Instance == USB_OTG_HS)
	  {
	    /* Peripheral interrupt Deinit*/
	    arm_hardware_disable_handler(OTG_IRQn);

		__HAL_RCC_USBO_FORCE_RESET();
		__HAL_RCC_USBO_RELEASE_RESET();
		__HAL_RCC_USBO_CLK_DISABLE();
		__HAL_RCC_USBO_CLK_SLEEP_DISABLE();
	  }

#elif CPUSTYLE_STM32F

	#if defined (USB_OTG_HS)
		  if (pcdHandle->Instance == USB_OTG_HS)
		  {
			/* Peripheral interrupt Deinit*/
			arm_hardware_disable_handler(OTG_HS_IRQn);
		  }
	#endif /* defined (USB_OTG_HS) */
	#if defined (USB_OTG_FS)
		  if (pcdHandle->Instance == USB_OTG_FS)
		  {
			/* Peripheral interrupt Deinit*/
			arm_hardware_disable_handler(OTG_FS_IRQn);
		  }
	#endif /* defined (USB_OTG_FS) */

#elif CPUSTYLE_T507

	arm_hardware_disable_handler(USB20_OTG_DEVICE_IRQn);

	CCU->USB_BGR_REG &= ~ (UINT32_C(1) << 24);	// USBOTG_RST
	CCU->USB_BGR_REG &= ~ (UINT32_C(1) << 8);	// USBOTG_GATING

	CCU->USB0_CLK_REG &= ~ (UINT32_C(1) << 30);	// USBPHY0_RST

#elif CPUSTYLE_A64

	arm_hardware_disable_handler(USBOTG0_IRQn);

	CCU->BUS_SOFT_RST_REG0 &= ~ (1u << 23);	// USB-OTG-Device_RST.
	CCU->BUS_CLK_GATING_REG0 &= ~ (1u << 23);	// USB-OTG-Device_GATING.
    CCU->USBPHY_CFG_REG &= ~ (1u << 0);	// USBPHY0_RST
    CCU->USBPHY_CFG_REG &= ~ (1u << 8);	// SCLK_GATING_USBPHY0

#elif CPUSTYLE_T113 || CPUSTYLE_F133

	arm_hardware_disable_handler(USB0_DEVICE_IRQn);

	// Turn on USBOTG0
	CCU->USB_BGR_REG &= ~ (1uL << 24);	// USBOTG0_RST Assert
	CCU->USB_BGR_REG &= ~ (1uL << 8);	// USBOTG0_GATING
	CCU->USB0_CLK_REG &= ~ (1uL << 30);	// USBPHY0_RSTN

#else
	#error HAL_PCD_MspDeInit should be implemented
#endif
}

/**
  * @brief  Setup stage callback
  * @param  hpcd: PCD handle
  * @retval None
  */
#if (USE_HAL_PCD_REGISTER_CALLBACKS == 1U)
static void PCD_SetupStageCallback(PCD_HandleTypeDef *hpcd)
#else
void HAL_PCD_SetupStageCallback(PCD_HandleTypeDef *hpcd)
#endif /* USE_HAL_PCD_REGISTER_CALLBACKS */
{
#if 0
	static unsigned offs;
	vtty_printhex_irqsystem((offs += 256) & 0xFFFF, (uint8_t *)hpcd->Setup, 8);
#endif
  USBD_LL_SetupStage((USBD_HandleTypeDef*)hpcd->pData, (uint8_t *)hpcd->Setup);
}

/**
  * @brief  Data Out stage callback.
  * @param  hpcd: PCD handle
  * @param  epnum: Endpoint number
  * @retval None
  */
#if (USE_HAL_PCD_REGISTER_CALLBACKS == 1U)
static void PCD_DataOutStageCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum)
#else
void HAL_PCD_DataOutStageCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum)
#endif /* USE_HAL_PCD_REGISTER_CALLBACKS */
{
  USBD_LL_DataOutStage((USBD_HandleTypeDef*)hpcd->pData, epnum, hpcd->OUT_ep[epnum].xfer_buff);
}

/**
  * @brief  Data In stage callback.
  * @param  hpcd: PCD handle
  * @param  epnum: Endpoint number
  * @retval None
  */
#if (USE_HAL_PCD_REGISTER_CALLBACKS == 1U)
static void PCD_DataInStageCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum)
#else
void HAL_PCD_DataInStageCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum)
#endif /* USE_HAL_PCD_REGISTER_CALLBACKS */
{
  USBD_LL_DataInStage((USBD_HandleTypeDef*)hpcd->pData, epnum, hpcd->IN_ep[epnum].xfer_buff);
}


static volatile unsigned sofcount;
static uint32_t softicks;
unsigned hamradio_get__getsoffreq(void)
{
#if WITHUSBHW && defined (WITHUSBHW_DEVICE)

	IRQL_t oldIrql;
	RiseIrql(IRQL_SYSTEM, & oldIrql);
	unsigned count = sofcount;
	sofcount = 0;
	LowerIrql(oldIrql);

	uint32_t nowticks = sys_now();
	uint32_t difftime = nowticks - softicks;
	softicks = nowticks;
	unsigned n = count * 1000 / difftime;
	return n;
#else
	return  0;
#endif /* WITHUSBHW && defined (WITHUSBHW_DEVICE) */
}

/**
  * @brief  SOF callback.
  * @param  hpcd: PCD handle
  * @retval None
  */
#if (USE_HAL_PCD_REGISTER_CALLBACKS == 1U)
static void PCD_SOFCallback(PCD_HandleTypeDef *hpcd)
#else
void HAL_PCD_SOFCallback(PCD_HandleTypeDef *hpcd)
#endif /* USE_HAL_PCD_REGISTER_CALLBACKS */
{
	++ sofcount;
  USBD_LL_SOF((USBD_HandleTypeDef*)hpcd->pData);
}

/**
  * @brief  Reset callback.
  * @param  hpcd: PCD handle
  * @retval None
  */
#if (USE_HAL_PCD_REGISTER_CALLBACKS == 1U)
static void PCD_ResetCallback(PCD_HandleTypeDef *hpcd)
#else
void HAL_PCD_ResetCallback(PCD_HandleTypeDef *hpcd)
#endif /* USE_HAL_PCD_REGISTER_CALLBACKS */
{
  USBD_SpeedTypeDef speed = USBD_SPEED_FULL;

  if ( hpcd->Init.speed == PCD_SPEED_HIGH)
  {
    speed = USBD_SPEED_HIGH;
  }
  else if ( hpcd->Init.speed == PCD_SPEED_FULL)
  {
    speed = USBD_SPEED_FULL;
  }
  else
  {
    Error_Handler();
  }
    /* Set Speed. */
  USBD_LL_SetSpeed((USBD_HandleTypeDef*)hpcd->pData, speed);

  /* Reset Device. */
  USBD_LL_Reset((USBD_HandleTypeDef*)hpcd->pData);
}

/**
  * @brief  Suspend callback.
  * When Low power mode is enabled the debug cannot be used (IAR, Keil doesn't support it)
  * @param  hpcd: PCD handle
  * @retval None
  */
#if (USE_HAL_PCD_REGISTER_CALLBACKS == 1U)
static void PCD_SuspendCallback(PCD_HandleTypeDef *hpcd)
#else
void HAL_PCD_SuspendCallback(PCD_HandleTypeDef *hpcd)
#endif /* USE_HAL_PCD_REGISTER_CALLBACKS */
{
  /* Inform USB library that core enters in suspend Mode. */
  USBD_LL_Suspend((USBD_HandleTypeDef*)hpcd->pData);
#if (CPUSTYLE_STM32MP1 || CPUSTYLE_STM32F)
  __HAL_PCD_GATE_PHYCLOCK(hpcd);
  /* Enter in STOP mode. */
  /* USER CODE BEGIN 2 */
  if (hpcd->Init.low_power_enable)
  {
    /* Set SLEEPDEEP bit and SleepOnExit of Cortex System Control Register. */
////    SCB->SCR |= (uint32_t)((uint32_t)(SCB_SCR_SLEEPDEEP_Msk | SCB_SCR_SLEEPONEXIT_Msk));
  }
  /* USER CODE END 2 */
#endif /* (CPUSTYLE_STM32MP1 || CPUSTYLE_STM32F) */
}

/**
  * @brief  Resume callback.
  * When Low power mode is enabled the debug cannot be used (IAR, Keil doesn't support it)
  * @param  hpcd: PCD handle
  * @retval None
  */
#if (USE_HAL_PCD_REGISTER_CALLBACKS == 1U)
static void PCD_ResumeCallback(PCD_HandleTypeDef *hpcd)
#else
void HAL_PCD_ResumeCallback(PCD_HandleTypeDef *hpcd)
#endif /* USE_HAL_PCD_REGISTER_CALLBACKS */
{
  /* USER CODE BEGIN 3 */

  /* USER CODE END 3 */
  USBD_LL_Resume((USBD_HandleTypeDef*)hpcd->pData);
}

/**
  * @brief  ISOOUTIncomplete callback.
  * @param  hpcd: PCD handle
  * @param  epnum: Endpoint number
  * @retval None
  */
#if (USE_HAL_PCD_REGISTER_CALLBACKS == 1U)
static void PCD_ISOOUTIncompleteCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum)
#else
void HAL_PCD_ISOOUTIncompleteCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum)
#endif /* USE_HAL_PCD_REGISTER_CALLBACKS */
{
  USBD_LL_IsoOUTIncomplete((USBD_HandleTypeDef*)hpcd->pData, epnum);
}

/**
  * @brief  ISOINIncomplete callback.
  * @param  hpcd: PCD handle
  * @param  epnum: Endpoint number
  * @retval None
  */
#if (USE_HAL_PCD_REGISTER_CALLBACKS == 1U)
static void PCD_ISOINIncompleteCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum)
#else
void HAL_PCD_ISOINIncompleteCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum)
#endif /* USE_HAL_PCD_REGISTER_CALLBACKS */
{
  USBD_LL_IsoINIncomplete((USBD_HandleTypeDef*)hpcd->pData, epnum);
}

/**
  * @brief  Connect callback.
  * @param  hpcd: PCD handle
  * @retval None
  */
#if (USE_HAL_PCD_REGISTER_CALLBACKS == 1U)
static void PCD_ConnectCallback(PCD_HandleTypeDef *hpcd)
#else
void HAL_PCD_ConnectCallback(PCD_HandleTypeDef *hpcd)
#endif /* USE_HAL_PCD_REGISTER_CALLBACKS */
{
  USBD_LL_DevConnected((USBD_HandleTypeDef*)hpcd->pData);
}

/**
  * @brief  Disconnect callback.
  * @param  hpcd: PCD handle
  * @retval None
  */
#if (USE_HAL_PCD_REGISTER_CALLBACKS == 1U)
static void PCD_DisconnectCallback(PCD_HandleTypeDef *hpcd)
#else
void HAL_PCD_DisconnectCallback(PCD_HandleTypeDef *hpcd)
#endif /* USE_HAL_PCD_REGISTER_CALLBACKS */
{
  USBD_LL_DevDisconnected((USBD_HandleTypeDef*)hpcd->pData);
}


#if CPUSTYLE_R7S721

#elif (CPUSTYLE_STM32MP1 || CPUSTYLE_STM32F)


/*  TXn min size = 16 words. (n  : Transmit FIFO index)
    When a TxFIFO is not used, the Configuration should be as follows:
        case 1 :  n > m    and Txn is not used    (n,m  : Transmit FIFO indexes)
       --> Txm can use the space allocated for Txn.
       case2  :  n < m    and Txn is not used    (n,m  : Transmit FIFO indexes)
       --> Txn should be configured with the minimum space of 16 words
   The FIFO is used optimally when used TxFIFOs are allocated in the top
       of the FIFO.Ex: use EP1 and EP2 as IN instead of EP1 and EP3 as IN ones.
   When DMA is used 3n * FIFO locations should be reserved for internal DMA registers */

static uint32_t usbd_makeTXFSIZ(uint_fast16_t base, uint_fast16_t size)
{
	return
		((uint32_t) size << USB_OTG_DIEPTXF_INEPTXFD_Pos) |
		((uint32_t) base << USB_OTG_DIEPTXF_INEPTXSA_Pos) |
		0;
}

// Преобразование размера в байтах размера данных в требования к fifo
// Расчет аргумента функции HAL_PCDEx_SetRxFiFo, HAL_PCDEx_SetTxFiFo
static uint_fast16_t size2buff4(uint_fast16_t size)
{
	const uint_fast16_t size4 = (size + 3) / 4;		// размер в 32-бит значениях
	return MAX(0x10, size4);
}

static void usbd_fifo_initialize(PCD_HandleTypeDef * hpcd, uint_fast16_t fullsize, uint_fast8_t bigbuff, uint_fast8_t dma)
{
#if CPUSTYLE_STM32MP1
	dma = 1;
	bigbuff = 1;
#endif /* CPUSTYLE_STM32MP1 */
	unsigned offset;
	const int add3tx = bigbuff ? 3 : 1;	// tx fifo add places
	const int mul2 = bigbuff ? 3 : 1;	// tx fifo buffers
	PCD_TypeDef * const USBx = hpcd->Instance;

	//PRINTF(PSTR("usbd_fifo_initialize: bigbuff=%d, fullsize=%u, power-on GRXFSIZ=%u\n"), (int) bigbuff, (unsigned) fullsize, USBx->GRXFSIZ & USB_OTG_GRXFSIZ_RXFD);
	// DocID028270 Rev 2 (RM0410): 41.11.3 FIFO RAM allocation
	// DocID028270 Rev 2 (RM0410): 41.16.6 Device programming model

/*
3. Set up the Data FIFO RAM for each of the FIFOs –
	Program the OTG_GRXFSIZ register, to be able to receive control OUT data and setup data.
	If thresholding is not enabled, at a minimum, this must be equal to
	1 max packet size of control endpoint 0 +
	2 Words (for the status of the control OUT data packet) +
	10 Words (for setup packets). –
	Program the OTG_DIEPTXF0 register (depending on the FIFO number chosen)
	to be able to transmit control IN data.
	At a minimum, this must be equal to 1 max packet size of control endpoint 0.

*/
	uint_fast16_t maxoutpacketsize4 = size2buff4(USB_OTG_MAX_EP0_SIZE);

	// добавление шести обеспечивает работу конфигурации с единственным устройством HID
	maxoutpacketsize4 += 6;

	//uint_fast16_t base4;
	uint_fast8_t numcontrolendpoints = 1;
	uint_fast8_t numoutendpoints = 1;

	// при addplaces = 3 появился звук на передаче в трансивер (при 2-х компортах)
	// но если CDC и UAC включать поодиночке, обмен не нарушается и при 0.
	// todo: найти все-таки документ https://www.synopsys.com/ip_prototyping_kit_usb3otgv2_drd_pc.pdf
	// еще интересен QL-Hi-Speed-USB-2.0-OTG-Controller-Data-Sheet.pdf

	uint_fast8_t addplaces = 3;

	const uint_fast16_t full4 = fullsize / 4;
	uint_fast16_t last4 = full4;
	uint_fast16_t base4 = 0;

#if WITHUSBCDCACM
	// параметры TX FIFO для ендпоинтов, в которые никогда не будут идти данные для передачи
	const uint_fast16_t size4dummy = 0;//0x10;//bigbuff ? 0x10 : 4;
	//last4 -= size4dummy;
	const uint_fast16_t last4dummy = last4;
#endif /* WITHUSBCDCACM */

	//PRINTF(PSTR("usbd_fifo_initialize1: 4*(full4-last4)=%u\n"), 4 * (full4 - last4));

#if WITHUSBUAC

#if WITHUSBUACIN2
	#if WITHRTS96
		const int nuacinpackets = 1 * mul2, nuacoutpackets = 1 * mul2;
	#elif WITHRTS192
		const int nuacinpackets = 1 * mul2, nuacoutpackets = 1 * mul2;
	#else /* WITHRTS96 || WITHRTS192 */
		const int nuacinpackets = 1 * mul2, nuacoutpackets = 1 * mul2;
	#endif /* WITHRTS96 || WITHRTS192 */
#else /* WITHUSBUACIN2 */
	#if WITHRTS96
		const int nuacinpackets = 1 * mul2, nuacoutpackets = 1 * mul2;
	#elif WITHRTS192
		const int nuacinpackets = 1 * mul2, nuacoutpackets = 1 * mul2;
	#else /* WITHRTS96 || WITHRTS192 */
		const int nuacinpackets = 2 * mul2, nuacoutpackets = 1 * mul2;
	#endif /* WITHRTS96 || WITHRTS192 */
#endif /* WITHUSBUACIN2 */

#if WITHUSBUACIN
	{
		/* endpoint передачи звука в компьютер */
		const uint_fast8_t pipe = (USBD_EP_AUDIO_IN) & 0x7F;

		const uint_fast16_t uacinmaxpacket = usbd_getuacinmaxpacket();

		const uint_fast16_t size4 = nuacinpackets * (size2buff4(uacinmaxpacket) + add3tx);
		ASSERT(last4 >= size4);
		last4 -= size4;
		USBx->DIEPTXF [pipe - 1] = usbd_makeTXFSIZ(last4, size4);
		//PRINTF(PSTR("usbd_fifo_initialize2 - UAC %u bytes: 4*(full4-last4)=%u\n"), 4 * size4, 4 * (full4 - last4));
	}
#if WITHUSBUACIN2
	{
		/* endpoint передачи звука (спектра) в компьютер */
		const uint_fast8_t pipe = (USBD_EP_RTS_IN) & 0x7F;

		const int nuacinpackets = 1 * mul2;
		const uint_fast16_t uacinmaxpacket = usbd_getuacinrtsmaxpacket();

		const uint_fast16_t size4 = nuacinpackets * (size2buff4(uacinmaxpacket) + add3tx);
		ASSERT(last4 >= size4);
		last4 -= size4;
		USBx->DIEPTXF [pipe - 1] = usbd_makeTXFSIZ(last4, size4);
		//PRINTF(PSTR("usbd_fifo_initialize3 - UAC3 %u bytes: 4*(full4-last4)=%u\n"), 4 * size4, 4 * (full4 - last4));
	}
#endif /* WITHUSBUACIN2 */
#endif /* WITHUSBUACIN */
#if WITHUSBUACOUT
	{
		numoutendpoints += 1;
		maxoutpacketsize4 = MAX(maxoutpacketsize4, nuacoutpackets * size2buff4(usbd_getuacoutmaxpacket()));
	}
#endif /* WITHUSBUACOUT */
#endif /* WITHUSBUAC */

#if WITHUSBCDCACM
	for (offset = 0; offset < WITHUSBCDCACM_N; ++ offset)
	{
		{
			const uint_fast8_t pipeint =  USBD_CDCACM_INT_EP(USBD_EP_CDCACM_INT, offset) & 0x7F;
			USBx->DIEPTXF [pipeint - 1] = usbd_makeTXFSIZ(last4dummy, size4dummy);
		}
		{
			/* полнофункциональное устройство */
			const uint_fast8_t pipe = USBD_CDCACM_IN_EP(USBD_EP_CDCACM_IN, offset) & 0x7F;
			numoutendpoints += 1;
			if (bigbuff == 0 && offset > 0)
			{
				// на маленьких контроллерах только первый USB CDC может обмениваться данными
				USBx->DIEPTXF [pipe - 1] = usbd_makeTXFSIZ(last4dummy, size4dummy);

			}
			else
			{
				#if WITHUSBUAC
					#if WITHUSBUACIN2
						const int ncdcindatapackets = 1 * mul2, ncdcoutdatapackets = 3;
					#elif WITHRTS96 || WITHRTS192
						const int ncdcindatapackets = 2 * mul2, ncdcoutdatapackets = 3;
					#else /* WITHRTS96 || WITHRTS192 */
						const int ncdcindatapackets = 2 * mul2, ncdcoutdatapackets = 3;
					#endif /* WITHRTS96 || WITHRTS192 */
				#else /* WITHUSBUAC */
					const int ncdcindatapackets = 4, ncdcoutdatapackets = 4;
				#endif /* WITHUSBUAC */

				maxoutpacketsize4 = MAX(maxoutpacketsize4, ncdcoutdatapackets * size2buff4(VIRTUAL_COM_PORT_OUT_DATA_SIZE));


				const uint_fast16_t size4 = ncdcindatapackets * (size2buff4(VIRTUAL_COM_PORT_IN_DATA_SIZE) + add3tx);
				ASSERT(last4 >= size4);
				last4 -= size4;
				USBx->DIEPTXF [pipe - 1] = usbd_makeTXFSIZ(last4, size4);
				//PRINTF(PSTR("usbd_fifo_initialize4 CDC %u bytes: 4*(full4-last4)=%u\n"), 4 * size4, 4 * (full4 - last4));
			}
		}

	}
#endif /* WITHUSBCDCACM */

#if WITHUSBCDCEEM
	{
		/* полнофункциональное устройство */
		const uint_fast8_t pipe = USBD_EP_CDCEEM_IN & 0x7F;

		numoutendpoints += 1;
		const int ncdceemindatapackets = 1 * mul2 + 1, ncdceemoutdatapackets = 3;

		maxoutpacketsize4 = MAX(maxoutpacketsize4, ncdceemoutdatapackets * size2buff4(USBD_CDCEEM_BUFSIZE));


		const uint_fast16_t size4 = ncdceemindatapackets * (size2buff4(USBD_CDCEEM_BUFSIZE) + add3tx);
		ASSERT(last4 >= size4);
		last4 -= size4;
		USBx->DIEPTXF [pipe - 1] = usbd_makeTXFSIZ(last4, size4);
		//PRINTF(PSTR("usbd_fifo_initialize5 EEM %u bytes: 4*(full4-last4)=%u\n"), 4 * size4, 4 * (full4 - last4));
	}
#endif /* WITHUSBCDCEEM */

#if WITHUSBDMTP
	{
		const uint_fast8_t pipe = USBD_EP_MTP_IN & 0x7F;
		const uint_fast8_t pipeint = USBD_EP_MTP_INT & 0x7F;

		numoutendpoints += 1;
		const int nmtpindatapackets = 1 * mul2 + 1, nmtpoutdatapackets = 3;
		const int nmtpintdatapackets = 1;

		maxoutpacketsize4 = MAX(maxoutpacketsize4, nmtpoutdatapackets * size2buff4(MTP_DATA_MAX_PACKET_SIZE));


		const uint_fast16_t size4 = nmtpindatapackets * (size2buff4(MTP_DATA_MAX_PACKET_SIZE) + add3tx);
		ASSERT(last4 >= size4);
		last4 -= size4;
		USBx->DIEPTXF [pipe - 1] = usbd_makeTXFSIZ(last4, size4);


		const uint_fast16_t size4int = nmtpintdatapackets * (size2buff4(MTP_CMD_PACKET_SIZE) + add3tx);
		ASSERT(last4 >= size4int);
		last4 -= size4int;
		USBx->DIEPTXF [pipeint - 1] = usbd_makeTXFSIZ(last4, size4int);

		//PRINTF(PSTR("usbd_fifo_initialize5 MTP %u bytes: 4*(full4-last4)=%u\n"), 4 * size4, 4 * (full4 - last4));
	}
#endif /* WITHUSBDMTP */
#if WITHUSBDMSC
#warning WITHUSBDMSC not finished
	{

	}
#endif /* WITHUSBDMSC */
#if WITHUSBRNDIS
	{
		/* полнофункциональное устройство */
		const uint_fast8_t pipe = (USBD_EP_RNDIS_IN + 0) & 0x7F;
		const uint_fast8_t pipeint = (USBD_EP_RNDIS_INT + 0) & 0x7F;
		numoutendpoints += 1;
		const int
			nrndisindatapackets = 3,
			nrndisintdatapackets = 3,
			nrndisoutdatapackets = 3;

		maxoutpacketsize4 = MAX(maxoutpacketsize4, nrndisoutdatapackets * size2buff4(USBD_RNDIS_OUT_BUFSIZE));


		const uint_fast16_t size4 = nrndisindatapackets * (size2buff4(USBD_RNDIS_IN_BUFSIZE) + add3tx);
		ASSERT(last4 >= size4);
		last4 -= size4;
		USBx->DIEPTXF [pipe - 1] = usbd_makeTXFSIZ(last4, size4);
		const uint_fast16_t size4int = nrndisintdatapackets * (size2buff4(USBD_RNDIS_INT_SIZE) + add3tx);
		ASSERT(last4 >= size4int);
		last4 -= size4int;
		USBx->DIEPTXF [pipeint - 1] = usbd_makeTXFSIZ(last4, size4int);
		//PRINTF(PSTR("usbd_fifo_initialize4 RNDIS %u bytes: 4*(full4-last4)=%u\n"), 4 * size4, 4 * (full4 - last4));
	}

#endif /* WITHUSBRNDIS */

#if WITHUSBHID
	{
		/* ... устройство */
		const uint_fast8_t pipe = USBD_EP_HIDKEYBOARD_INT & 0x7F;

		//const uint_fast16_t size4 = size2buff4(HIDMOUSE_INT_DATA_SIZE);
		const uint_fast16_t size4 = size2buff4(HIDKEYBOARD_INT_DATA_SIZE);
		ASSERT(last4 >= size4);
		last4 -= size4;
		USBx->DIEPTXF [pipe - 1] = usbd_makeTXFSIZ(last4, size4);
		//PRINTF(PSTR("usbd_fifo_initialize8 HID %u bytes: 4*(full4-last4)=%u\n"), 4 * size4, 4 * (full4 - last4));
	}
#endif /* WITHUSBHID */

	//PRINTF(PSTR("usbd_fifo_initialize9: 4*(full4-last4)=%u\n"), 4 * (full4 - last4));

	/* control endpoint TX FIFO */
	{
		/* Установить размер TX FIFO EP0 */
		const uint_fast16_t size4 = 2 * (size2buff4(USB_OTG_MAX_EP0_SIZE) + add3tx);
		ASSERT(last4 >= size4);
		last4 -= size4;
		USBx->DIEPTXF0_HNPTXFSIZ = usbd_makeTXFSIZ(last4, size4);
		//PRINTF(PSTR("usbd_fifo_initialize10 TX FIFO %u bytes: 4*(full4-last4)=%u\n"), 4 * size4, 4 * (full4 - last4));
	}
	/* control endpoint RX FIFO */
	{
		/* Установить размер RX FIFO -  теперь все что осталоь - используем last4 вместо size4 */
		// (4 * number of control endpoints + 6) +
		// ((largest USB packet used / 4) + 1 for status information) +
		// (2 * number of OUT endpoints) +
		// 1 for Global NAK
		const uint_fast16_t size4 =
				(4 * numcontrolendpoints + 6) +
				(maxoutpacketsize4 + 1) +
				(2 * numoutendpoints) +
				1 +
				addplaces;

		//PRINTF(PSTR("usbd_fifo_initialize11 RX FIFO %u bytes: 4*(full4-last4)=%u bytes (last4=%u, size4=%u)\n"), 4 * size4, 4 * (full4 - last4), last4, size4);
		ASSERT(last4 >= size4);
		USBx->GRXFSIZ = (USBx->GRXFSIZ & ~ USB_OTG_GRXFSIZ_RXFD) |
			(last4 << USB_OTG_GRXFSIZ_RXFD_Pos) |	// was: size4 - то что осталось
			0;
		base4 += size4;
	}

	if (base4 > last4 || last4 > full4)
	{
		char b [64];
		PRINTF(PSTR("usbd_fifo_initialize error: base4=%u, last4=%u, fullsize=%u\n"), (base4 * 4), (last4 * 4), fullsize);
//		local_snprintf_P(b, sizeof b / sizeof b [0], PSTR("used=%u"), (base4 * 4) + (fullsize - last4 * 4));
//		colmain_setcolors(COLORMAIN_RED, BGCOLOR);
//		display_at(0, 0, b);
		for (;;)
			;
	}
	else
	{
		//PRINTF(PSTR("usbd_fifo_initialize: base4=%u, last4=%u, fullsize=%u\n"), (base4 * 4), (last4 * 4), fullsize);
#if 0
		// Диагностическая выдача использованного объёма FIFO RAM
		char b [64];

		local_snprintf_P(b, sizeof b / sizeof b [0], PSTR("used=%u"), (base4 * 4) + (fullsize - last4 * 4));
		colmain_setcolors(COLORMAIN_GREEN, BGCOLOR);
		display_at(0, 0, b);
		HARDWARE_DELAY_MS(2000);
#endif
	}

	(void) USB_FlushRxFifo(USBx);
	(void) USB_FlushTxFifo(USBx, 0x10U); /* all Tx FIFOs */
}


void
usbd_pipes_initialize(USBD_HandleTypeDef * pdev)
{
	PCD_HandleTypeDef * const hpcd = pdev->pData;
	if (USB_Is_OTG_HS(hpcd->Instance))
	{
		// У OTH_HS размер FIFO 4096 байт
		usbd_fifo_initialize(hpcd, 4096, 1, hpcd_USB_OTG.Init.dma_enable);
	}
	else
	{
		// У OTH_FS размер FIFO 1280 байт
		usbd_fifo_initialize(hpcd, 1280, 0, hpcd_USB_OTG.Init.dma_enable);
	}
}


#endif /* (CPUSTYLE_STM32MP1 || CPUSTYLE_STM32F) */

#if defined (WITHUSBHW_DEVICE)

static uint_fast8_t hardware_usbd_get_vbusnow0(void)
{
#if CPUSTYLE_R7S721
	return (WITHUSBHW_DEVICE->INTSTS0 & USB_INTSTS0_VBSTS) != 0;

#elif (CPUSTYLE_STM32F || CPUSTYLE_STM32MP1) && defined (USB_OTG_GOTGCTL_BSESVLD_Msk) && WITHUSBDEV_VBUSSENSE
	return (WITHUSBHW_DEVICE->GOTGCTL & USB_OTG_GOTGCTL_BSESVLD_Msk) != 0;

#else /* CPUSTYLE_R7S721 */
	return 0;

#endif /* CPUSTYLE_R7S721 */
}

/*******************************************************************************
                       LL Driver Interface (USB Device Library --> PCD)
*******************************************************************************/

/**
  * @brief  Initializes the low level portion of the device driver.
  * @param  pdev: Device handle
  * @retval USBD status
  */
USBD_StatusTypeDef USBD_LL_Init(USBD_HandleTypeDef *pdev)
{
	/* Link The driver to the stack */
	//	hpcd->pData = pdev;
	//	pdev->pData = hpcd;
	/* Init USB Ip. */
	hpcd_USB_OTG.Instance = WITHUSBHW_DEVICE;

	/* Link the driver to the stack. */
	hpcd_USB_OTG.pData = pdev;
	pdev->pData = & hpcd_USB_OTG;
#if CPUSTYLE_R7S721
	hpcd_USB_OTG.Init.dev_endpoints = 15;
	// Значение ep0_mps и speed обновится после reset шины
	#if WITHUSBDEV_HSDESC
		//hpcd->Init.pcd_speed = PCD_SPEED_HIGH;
		hpcd_USB_OTG.Init.speed = PCD_SPEED_HIGH;
		//hpcd->Init.ep0_mps = USB_OTG_MAX_EP0_SIZE; //USB_OTG_HS_MAX_PACKET_SIZE;
	#else /* WITHUSBDEV_HSDESC */
		//hpcd->Init.pcd_speed = PCD_SPEED_FULL;
		hpcd_USB_OTG.Init.speed = PCD_SPEED_FULL;
		//hpcd->Init.ep0_mps = USB_OTG_MAX_EP0_SIZE; //USB_OTG_FS_MAX_PACKET_SIZE;
	#endif /* WITHUSBDEV_HSDESC */
	hpcd_USB_OTG.Init.phy_itface = USB_OTG_EMBEDDED_PHY;

	hpcd_USB_OTG.Init.dma_enable = ENABLE;
	hpcd_USB_OTG.Init.dma_enable = DISABLE;

#else
	hpcd_USB_OTG.Init.dev_endpoints = 9;	// STM32MP1/STM32H7: 1 i/o, 8 in, 8 out
	#if WITHUSBDEV_HSDESC
		hpcd_USB_OTG.Init.speed = PCD_SPEED_HIGH;
	#else /* WITHUSBDEV_HSDESC */
		hpcd_USB_OTG.Init.speed = PCD_SPEED_FULL;
	#endif /* WITHUSBDEV_HSDESC */
	#if WITHUSBDEV_DMAENABLE
		hpcd_USB_OTG.Init.dma_enable = ENABLE;
	#else /* WITHUSBDEV_DMAENABLE */
		hpcd_USB_OTG.Init.dma_enable = DISABLE;
	#endif /* WITHUSBDEV_DMAENABLE */
	#if WITHUSBDEV_HIGHSPEEDPHYC
		hpcd_USB_OTG.Init.phy_itface = USB_OTG_HS_EMBEDDED_PHY;
	#elif WITHUSBDEV_HIGHSPEEDULPI
		hpcd_USB_OTG.Init.phy_itface = USB_OTG_ULPI_PHY;
	#else
		hpcd_USB_OTG.Init.phy_itface = USB_OTG_EMBEDDED_PHY;
	#endif
	hpcd_USB_OTG.Init.Sof_enable = DISABLE;
	hpcd_USB_OTG.Init.low_power_enable = DISABLE;
	hpcd_USB_OTG.Init.lpm_enable = DISABLE;
#if WITHUSBDEV_VBUSSENSE
	hpcd_USB_OTG.Init.vbus_sensing_enable = ENABLE;
#else /* WITHUSBDEV_VBUSSENSE */
	hpcd_USB_OTG.Init.vbus_sensing_enable = DISABLE;
#endif /* WITHUSBDEV_VBUSSENSE */
	hpcd_USB_OTG.Init.use_dedicated_ep1 = DISABLE;
	hpcd_USB_OTG.Init.use_external_vbus = DISABLE;

#endif
	if (HAL_PCD_Init( & hpcd_USB_OTG) != HAL_OK)
	{
		Error_Handler();
	}

#if (USE_HAL_PCD_REGISTER_CALLBACKS == 1U)
	/* Register USB PCD CallBacks */
	HAL_PCD_RegisterCallback(& hpcd_USB_OTG, HAL_PCD_SOF_CB_ID, PCD_SOFCallback);
	HAL_PCD_RegisterCallback(& hpcd_USB_OTG, HAL_PCD_SETUPSTAGE_CB_ID, PCD_SetupStageCallback);
	HAL_PCD_RegisterCallback(& hpcd_USB_OTG, HAL_PCD_RESET_CB_ID, PCD_ResetCallback);
	HAL_PCD_RegisterCallback(& hpcd_USB_OTG, HAL_PCD_SUSPEND_CB_ID, PCD_SuspendCallback);
	HAL_PCD_RegisterCallback(& hpcd_USB_OTG, HAL_PCD_RESUME_CB_ID, PCD_ResumeCallback);
	HAL_PCD_RegisterCallback(& hpcd_USB_OTG, HAL_PCD_CONNECT_CB_ID, PCD_ConnectCallback);
	HAL_PCD_RegisterCallback(& hpcd_USB_OTG, HAL_PCD_DISCONNECT_CB_ID, PCD_DisconnectCallback);

	HAL_PCD_RegisterDataOutStageCallback(& hpcd_USB_OTG, PCD_DataOutStageCallback);
	HAL_PCD_RegisterDataInStageCallback(& hpcd_USB_OTG, PCD_DataInStageCallback);
	HAL_PCD_RegisterIsoOutIncpltCallback(& hpcd_USB_OTG, PCD_ISOOUTIncompleteCallback);
	HAL_PCD_RegisterIsoInIncpltCallback(& hpcd_USB_OTG, PCD_ISOINIncompleteCallback);
#endif /* USE_HAL_PCD_REGISTER_CALLBACKS */

	usbd_pipes_initialize(pdev);

	return USBD_OK;
}
/**
  * @brief  De-Initializes the low level portion of the device driver.
  * @param  pdev: Device handle
  * @retval USBD status
  */
USBD_StatusTypeDef USBD_LL_DeInit(USBD_HandleTypeDef *pdev)
{
  HAL_StatusTypeDef hal_status = HAL_OK;
  USBD_StatusTypeDef usb_status = USBD_OK;

  hal_status = HAL_PCD_DeInit(pdev->pData);

  usb_status =  USBD_Get_USB_Status(hal_status);

  return usb_status;
}

#endif /* defined (WITHUSBHW_DEVICE) */

/**
  * @brief  Starts the low level portion of the device driver.
  * @param  pdev: Device handle
  * @retval USBD status
  */
USBD_StatusTypeDef USBD_LL_Start(USBD_HandleTypeDef *pdev)
{
  HAL_StatusTypeDef hal_status = HAL_OK;
  USBD_StatusTypeDef usb_status = USBD_OK;

  hal_status = HAL_PCD_Start(pdev->pData);

  usb_status =  USBD_Get_USB_Status(hal_status);

  return usb_status;
}

/**
  * @brief  Stops the low level portion of the device driver.
  * @param  pdev: Device handle
  * @retval USBD status
  */
USBD_StatusTypeDef USBD_LL_Stop(USBD_HandleTypeDef *pdev)
{
  HAL_StatusTypeDef hal_status = HAL_OK;
  USBD_StatusTypeDef usb_status = USBD_OK;

  hal_status = HAL_PCD_Stop(pdev->pData);

  usb_status =  USBD_Get_USB_Status(hal_status);

  return usb_status;
}

/**
  * @brief  Opens an endpoint of the low level driver.
  * @param  pdev: Device handle
  * @param  ep_addr: Endpoint number
  * @param  ep_type: Endpoint type
  * @param  ep_mps: Endpoint max packet size
  * @retval USBD status
  */
USBD_StatusTypeDef USBD_LL_OpenEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr, uint8_t ep_type, uint16_t ep_mps)
{
  HAL_StatusTypeDef hal_status = HAL_OK;
  USBD_StatusTypeDef usb_status = USBD_OK;

  hal_status = HAL_PCD_EP_Open(pdev->pData, ep_addr, ep_mps, ep_type);

  usb_status =  USBD_Get_USB_Status(hal_status);

  return usb_status;
}

/**
  * @brief  Closes an endpoint of the low level driver.
  * @param  pdev: Device handle
  * @param  ep_addr: Endpoint number
  * @retval USBD status
  */
USBD_StatusTypeDef USBD_LL_CloseEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr)
{
  HAL_StatusTypeDef hal_status = HAL_OK;
  USBD_StatusTypeDef usb_status = USBD_OK;

  hal_status = HAL_PCD_EP_Close(pdev->pData, ep_addr);

  usb_status =  USBD_Get_USB_Status(hal_status);

  return usb_status;
}

/**
  * @brief  Flushes an endpoint of the Low Level Driver.
  * @param  pdev: Device handle
  * @param  ep_addr: Endpoint number
  * @retval USBD status
  */
USBD_StatusTypeDef USBD_LL_FlushEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr)
{
  HAL_StatusTypeDef hal_status = HAL_OK;
  USBD_StatusTypeDef usb_status = USBD_OK;

  hal_status = HAL_PCD_EP_Flush(pdev->pData, ep_addr);

  usb_status =  USBD_Get_USB_Status(hal_status);

  return usb_status;
}

/**
  * @brief  Sets a Stall condition on an endpoint of the Low Level Driver.
  * @param  pdev: Device handle
  * @param  ep_addr: Endpoint number
  * @retval USBD status
  */
USBD_StatusTypeDef USBD_LL_StallEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr)
{
  HAL_StatusTypeDef hal_status = HAL_OK;
  USBD_StatusTypeDef usb_status = USBD_OK;

  hal_status = HAL_PCD_EP_SetStall(pdev->pData, ep_addr);

  usb_status =  USBD_Get_USB_Status(hal_status);

  return usb_status;
}

/**
  * @brief  Clears a Stall condition on an endpoint of the Low Level Driver.
  * @param  pdev: Device handle
  * @param  ep_addr: Endpoint number
  * @retval USBD status
  */
USBD_StatusTypeDef USBD_LL_ClearStallEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr)
{
  HAL_StatusTypeDef hal_status = HAL_OK;
  USBD_StatusTypeDef usb_status = USBD_OK;

  hal_status = HAL_PCD_EP_ClrStall(pdev->pData, ep_addr);

  usb_status =  USBD_Get_USB_Status(hal_status);

  return usb_status;
}

/**
  * @brief  Returns Stall condition.
  * @param  pdev: Device handle
  * @param  ep_addr: Endpoint number
  * @retval Stall (1: Yes, 0: No)
  */
uint8_t USBD_LL_IsStallEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr)
{
  PCD_HandleTypeDef *hpcd = (PCD_HandleTypeDef*) pdev->pData;

  if((ep_addr & 0x80) == 0x80)
  {
    return hpcd->IN_ep[ep_addr & 0x7F].is_stall;
  }
  else
  {
    return hpcd->OUT_ep[ep_addr & 0x7F].is_stall;
  }
}

/**
  * @brief  Assigns a USB address to the device.
  * @param  pdev: Device handle
  * @param  dev_addr: Device address
  * @retval USBD status
  */
USBD_StatusTypeDef USBD_LL_SetUSBAddress(USBD_HandleTypeDef *pdev, uint8_t dev_addr)
{
  HAL_StatusTypeDef hal_status = HAL_OK;
  USBD_StatusTypeDef usb_status = USBD_OK;

  hal_status = HAL_PCD_SetAddress(pdev->pData, dev_addr);

  usb_status =  USBD_Get_USB_Status(hal_status);

  return usb_status;
}

/**
  * @brief  Transmits data over an endpoint.
  * @param  pdev: Device handle
  * @param  ep_addr: Endpoint number
  * @param  pbuf: Pointer to data to be sent
  * @param  size: Data size
  * @retval USBD status
  */
USBD_StatusTypeDef USBD_LL_Transmit(USBD_HandleTypeDef *pdev, uint8_t ep_addr, const uint8_t *pbuf, uint32_t size)
{
	HAL_StatusTypeDef hal_status = HAL_OK;
	USBD_StatusTypeDef usb_status = USBD_OK;

	if (pbuf != NULL && size != 0)
	{
		ASSERT(((uintptr_t) pbuf % DCACHEROWSIZE) == 0);
		dcache_clean((uintptr_t) pbuf, size);
	}

	hal_status = HAL_PCD_EP_Transmit(pdev->pData, ep_addr, pbuf, size);

	usb_status =  USBD_Get_USB_Status(hal_status);

	return usb_status;
}

/**
  * @brief  Prepares an endpoint for reception.
  * @param  pdev: Device handle
  * @param  ep_addr: Endpoint number
  * @param  pbuf: Pointer to data to be received
  * @param  size: Data size
  * @retval USBD status
  */
USBD_StatusTypeDef USBD_LL_PrepareReceive(USBD_HandleTypeDef *pdev, uint8_t ep_addr, uint8_t *pbuf, uint32_t size)
{
	HAL_StatusTypeDef hal_status = HAL_OK;
	USBD_StatusTypeDef usb_status = USBD_OK;

	if (pbuf != NULL && size != 0)
		dcache_clean_invalidate((uintptr_t) pbuf, size);

	hal_status = HAL_PCD_EP_Receive(pdev->pData, ep_addr, pbuf, size);

	usb_status =  USBD_Get_USB_Status(hal_status);

	return usb_status;
}

/**
  * @brief  Returns the last transfered packet size.
  * @param  pdev: Device handle
  * @param  ep_addr: Endpoint number
  * @retval Recived Data Size
  */
uint32_t USBD_LL_GetRxDataSize(USBD_HandleTypeDef *pdev, uint8_t ep_addr)
{
  return HAL_PCD_EP_GetRxCount((PCD_HandleTypeDef*) pdev->pData, ep_addr);
}

/**
  * @brief  Send LPM message to user layer
  * @param  hpcd: PCD handle
  * @param  msg: LPM message
  * @retval None
  */
void HAL_PCDEx_LPM_Callback(PCD_HandleTypeDef *hpcd, PCD_LPM_MsgTypeDef msg)
{
#if (CPUSTYLE_STM32MP1 || CPUSTYLE_STM32F)
  switch (msg)
  {
  case PCD_LPM_L0_ACTIVE:
    if (hpcd->Init.low_power_enable)
    {
////      SystemClock_Config();

      /* Reset SLEEPDEEP bit of Cortex System Control Register. */
////      SCB->SCR &= (uint32_t)~((uint32_t)(SCB_SCR_SLEEPDEEP_Msk | SCB_SCR_SLEEPONEXIT_Msk));
    }
    __HAL_PCD_UNGATE_PHYCLOCK(hpcd);
    USBD_LL_Resume(hpcd->pData);
    break;

  case PCD_LPM_L1_ACTIVE:
    __HAL_PCD_GATE_PHYCLOCK(hpcd);
    USBD_LL_Suspend(hpcd->pData);

    /* Enter in STOP mode. */
    if (hpcd->Init.low_power_enable)
    {
      /* Set SLEEPDEEP bit and SleepOnExit of Cortex System Control Register. */
////     SCB->SCR |= (uint32_t)((uint32_t)(SCB_SCR_SLEEPDEEP_Msk | SCB_SCR_SLEEPONEXIT_Msk));
    }
    break;
  }
#endif /* (CPUSTYLE_STM32MP1 || CPUSTYLE_STM32F) */
}

/**
  * @brief  Static single allocation.
  * @param  size: Size of allocated memory
  * @retval None
  */
//void *USBD_static_malloc(uint32_t size)
//{
//  static uint32_t mem[(sizeof(USBD_CDC_HandleTypeDef)/4)+1];/* On 32-bit boundary */
//  return mem;
//}

/**
  * @brief  Dummy memory free
  * @param  p: Pointer to allocated  memory address
  * @retval None
  */
void USBD_static_free(void *p)
{

}

/**
  * @brief  Delays routine for the USB Device Library.
  * @param  Delay: Delay in ms
  * @retval None
  */
void USBD_LL_Delay(uint32_t Delay)
{
  HAL_Delay(Delay);
}

/**
  * @brief  Retuns the USB status depending on the HAL status:
  * @param  hal_status: HAL status
  * @retval USB status
  */
USBD_StatusTypeDef USBD_Get_USB_Status(HAL_StatusTypeDef hal_status)
{
  USBD_StatusTypeDef usb_status = USBD_OK;

  switch (hal_status)
  {
    case HAL_OK :
      usb_status = USBD_OK;
    break;
    case HAL_ERROR :
      usb_status = USBD_FAIL;
    break;
    case HAL_BUSY :
      usb_status = USBD_BUSY;
    break;
    case HAL_TIMEOUT :
      usb_status = USBD_FAIL;
    break;
    default :
      usb_status = USBD_FAIL;
    break;
  }
  return usb_status;
}

#endif /* WITHUSBHW && defined (WITHUSBHW_DEVICE) */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
