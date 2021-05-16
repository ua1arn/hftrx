# Middleware USB Host MCU Component

## Overview

**STM32Cube** is an STMicroelectronics original initiative to ease the developers life by reducing efforts, time and cost.

**STM32Cube** covers the overall STM32 products portfolio. It includes a comprehensive embedded software platform delivered for each STM32 series.
   * The CMSIS modules (core and device) corresponding to the ARM(tm) core implemented in this STM32 product.
   * The STM32 HAL-LL drivers, an abstraction layer offering a set of APIs ensuring maximized portability across the STM32 portfolio.
   * The BSP drivers of each evaluation, demonstration or nucleo board provided for this STM32 series.
   * A consistent set of middleware components such as RTOS, USB, FatFS, graphics, touch sensing library...
   * A full set of software projects (basic examples, applications, and demonstrations) for each board provided for this STM32 series.

Two models of publication are proposed for the STM32Cube embedded software:
   * The monolithic **MCU Package**: all STM32Cube software modules of one STM32 series are present (Drivers, Middleware, Projects, Utilities) in the repository (usual name **STM32Cubexx**, xx corresponding to the STM32 series).
   * The **MCU component**: each STM32Cube software module being part of the STM32Cube MCU Package, is delivered as an individual repository, allowing the user to select and get only the required software functions.

## Description

This **stm32_mw_usb_host** MCU component repository is one element **common to all** STM32Cube MCU embedded software packages, providing the **USB Host MCU Middleware** part.

## License

Copyright (c) 2015 STMicroelectronics.

This software component is licensed by  ST under Ultimate Liberty license SLA0044, the "License". You may not use this file except in compliance with this license. You may obtain a copy of the license [here](https://www.st.com/SLA0044).

## Release note

Details about the content of this release are available in the release note [here](https://htmlpreview.github.io/?https://github.com/STMicroelectronics/stm32_mw_usb_host/blob/master/Release_Notes.html).

## Compatibility information

This table shows the correspondence between the USB Host MW version and the corresponding HAL version of the targeted series. It is **crucial** that you use a consistent set of versions for the MW - HAL, as mentioned in this table.

USB Host | HAL |
---------- | ---------- |
Tag v3.y.z | Tag v1.1.4 ([stm32f1xx_hal_driver](https://github.com/STMicroelectronics/stm32f1xx_hal_driver))<br>Tag v1.2.4 ([stm32f2xx_hal_driver](https://github.com/STMicroelectronics/stm32f2xx_hal_driver))<br>Tag v1.7.8 ([stm32f4xx_hal_driver](https://github.com/STMicroelectronics/stm32f4xx_hal_driver))<br>Tag v1.2.8 ([stm32f7xx_hal_driver](https://github.com/STMicroelectronics/stm32f7xx_hal_driver))<br>Tag v1.4.0 ([stm32g0xx_hal_driver](https://github.com/STMicroelectronics/stm32g0xx_hal_driver))<br>Tag v1.9.0 ([stm32h7xx_hal_driver](https://github.com/STMicroelectronics/stm32h7xx_hal_driver))<br>Tag v1.12.0 ([stm32l4xx_hal_driver](https://github.com/STMicroelectronics/stm32l4xx_hal_driver))<br>(and following, if any, till next USB Host tag)

## Troubleshooting

If you have any issue with the **software content** of this repository, you can file an issue [here](https://github.com/STMicroelectronics/stm32_mw_usb_host/issues/new/choose).

For any other question related to the product, the tools, the environment, you can submit a topic to the [ST Community](https://community.st.com/s/).
