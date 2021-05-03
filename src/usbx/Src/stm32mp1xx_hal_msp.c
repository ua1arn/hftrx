/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : stm32mp1xx_hal_msp.c
  * Description        : This file provides code for the MSP Initialization
  *                      and de-Initialization codes.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN Define */

/* USER CODE END Define */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN Macro */

/* USER CODE END Macro */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* External functions --------------------------------------------------------*/
/* USER CODE BEGIN ExternalFunctions */

/* USER CODE END ExternalFunctions */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */
/**
  * Initializes the Global MSP.
  */
void HAL_MspInit(void)
{
  /* USER CODE BEGIN MspInit 0 */

  /* USER CODE END MspInit 0 */

  __HAL_RCC_HSEM_CLK_ENABLE();

//  /* System interrupt init*/
//  /* MemoryManagement_IRQn interrupt configuration */
//  HAL_NVIC_SetPriority(MemoryManagement_IRQn, 1, 0);
//  /* BusFault_IRQn interrupt configuration */
//  HAL_NVIC_SetPriority(BusFault_IRQn, 1, 0);
//  /* UsageFault_IRQn interrupt configuration */
//  HAL_NVIC_SetPriority(UsageFault_IRQn, 1, 0);
//  /* SVCall_IRQn interrupt configuration */
//  HAL_NVIC_SetPriority(SVCall_IRQn, 1, 0);
//  /* DebugMonitor_IRQn interrupt configuration */
//  HAL_NVIC_SetPriority(DebugMonitor_IRQn, 1, 0);
//  /* PendSV_IRQn interrupt configuration */
//  HAL_NVIC_SetPriority(PendSV_IRQn, 1, 0);
//
//  /* Peripheral interrupt init */
//  /* RCC_WAKEUP_IRQn interrupt configuration */
//  HAL_NVIC_SetPriority(RCC_WAKEUP_IRQn, 0, 0);
//  HAL_NVIC_EnableIRQ(RCC_WAKEUP_IRQn);
//
  /* USER CODE BEGIN MspInit 1 */

  /* USER CODE END MspInit 1 */
}

void OTG_HS_IRQHandler(void);
/* USER CODE BEGIN 1 */
/**
  * @brief  Initializes the PCD MSP.
  * @param  hpcd PCD handle
  * @retval None
  */
void HAL_PCD_MspInitZZZZZ(PCD_HandleTypeDef *hpcd)
{
#if CPUSTYLE_STM32MP1
	// Set 3.3 volt DETECTOR enable
	PWR->CR3 |= PWR_CR3_USB33DEN_Msk;
	(void) PWR->CR3;
	while ((PWR->CR3 & PWR_CR3_USB33DEN_Msk) == 0)
		;

	// Wait 3.3 volt REGULATOR ready
	while ((PWR->CR3 & PWR_CR3_USB33RDY_Msk) == 0)
		;

	RCC->MP_AHB2ENSETR = RCC_MC_AHB2ENSETR_USBOEN;
	(void) RCC->MP_AHB2ENSETR;
	RCC->MP_AHB2LPENSETR = RCC_MC_AHB2LPENSETR_USBOLPEN;
	(void) RCC->MP_AHB2LPENSETR;

	if (hpcd->Instance == USB1_OTG_HS)	// legacy name is USB_OTG_HS
	{
		if (hpcd->Init.phy_itface == USB_OTG_ULPI_PHY)
		{
			//USBD_HS_ULPI_INITIALIZE();

//			RCC->AHB1ENR |= RCC_AHB1ENR_USB1OTGHSEN | RCC_AHB1ENR_USB1OTGHSULPIEN;	/* USB/OTG HS with ULPI */
//			(void) RCC->AHB1ENR;
//			RCC->AHB1LPENR |= RCC_AHB1LPENR_USB1OTGHSLPEN; /* USB/OTG HS  */
//			(void) RCC->AHB1LPENR;
//			RCC->AHB1LPENR |= RCC_AHB1LPENR_USB1OTGHSULPILPEN; /* USB/OTG HS ULPI  */
//			(void) RCC->AHB1LPENR;
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


		if (hpcd->Init.use_dedicated_ep1 == ENABLE)
		{
			//arm_hardware_set_handler_system(OTG_HS_EP1_OUT_IRQn, device_OTG_HS_EP1_OUT_IRQHandler);
			//arm_hardware_set_handler_system(OTG_HS_EP1_IN_IRQn, device_OTG_HS_EP1_IN_IRQHandler);
		}
		arm_hardware_set_handler_system(OTG_IRQn, OTG_HS_IRQHandler);

	}

#elif CPUSTYLE_STM32H7XX

	//PWR->CR3 |= PWR_CR3_USBREGEN;

	//while ((PWR->CR3 & PWR_CR3_USB33RDY) == 0)
	//	;
	//PWR->CR3 |= PWR_CR3_USBREGEN;
	//while ((PWR->CR3 & PWR_CR3_USB33RDY) == 0)
	//	;
	PWR->CR3 |= PWR_CR3_USB33DEN;

	if (hpcd->Instance == USB1_OTG_HS)	// legacy name is USB_OTG_HS
	{
		if (hpcd->Init.phy_itface == USB_OTG_ULPI_PHY)
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


		if (hpcd->Init.use_dedicated_ep1 == ENABLE)
		{
			arm_hardware_set_handler_system(OTG_HS_EP1_OUT_IRQn, device_OTG_HS_EP1_OUT_IRQHandler);
			arm_hardware_set_handler_system(OTG_HS_EP1_IN_IRQn, device_OTG_HS_EP1_IN_IRQHandler);
		}
		arm_hardware_set_handler_system(OTG_HS_IRQn, device_OTG_HS_IRQHandler);

	}
	else if (hpcd->Instance == USB2_OTG_FS)	// legacy name is USB_OTG_FS
	{
		if (hpcd->Init.phy_itface == USB_OTG_ULPI_PHY)
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

	if (hpcd->Instance == USB_OTG_HS)
	{
		//const uint_fast32_t stm32f4xx_pllq = arm_hardware_stm32f7xx_pllq_initialize();	// Настроить выход PLLQ на 48 МГц
		//PRINTF(PSTR("HAL_PCD_MspInit: stm32f4xx_pllq=%lu, freq=%lu\n"), (unsigned long) stm32f4xx_pllq, PLL_FREQ / stm32f4xx_pllq);

		if (hpcd->Init.phy_itface == USB_OTG_ULPI_PHY)
		{
			USBD_HS_ULPI_INITIALIZE();

			PRINTF(PSTR("HAL_PCD_MspInit: HS and ULPI\n"));
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

			PRINTF(PSTR("HAL_PCD_MspInit: HS without ULPI\n"));
			RCC->AHB1ENR |= RCC_AHB1ENR_OTGHSEN;	/* USB/OTG HS  */
			(void) RCC->AHB1ENR;
			RCC->AHB1LPENR |= RCC_AHB1LPENR_OTGHSLPEN; /* USB/OTG HS  */
			(void) RCC->AHB1LPENR;
			RCC->AHB1LPENR &= ~ RCC_AHB1LPENR_OTGHSULPILPEN; /* USB/OTG HS ULPI  */
			(void) RCC->AHB1LPENR;
		}

		RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;	/* USB/OTG HS companion - VBUS? */
		(void) RCC->APB2ENR;

		if (hpcd->Init.use_dedicated_ep1 == ENABLE)
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
	else if (hpcd->Instance == USB_OTG_FS)
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

#endif
}

/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
