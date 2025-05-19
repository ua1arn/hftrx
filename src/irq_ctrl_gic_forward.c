/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#include "hardware.h"

#if (__CORTEX_A != 0)

/**************************************************************************//**
 * @file     irq_ctrl_gic.c
 * @brief    Interrupt controller handling implementation for GIC
 * @version  V1.2.0
 * @date     30. October 2022
 ******************************************************************************/
/*
 * Copyright (c) 2017-2022 ARM Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stddef.h>

#include "RTE_Components.h"
#include CMSIS_device_header

#include "a-profile/irq_ctrl.h"

#if defined(__GIC_PRESENT) && (__GIC_PRESENT == 1U)

/// Number of implemented interrupt lines
#ifndef IRQ_GIC_LINE_COUNT
#define IRQ_GIC_LINE_COUNT      (1020U)
#endif

#ifndef IRQ_GIC_EXTERN_IRQ_TABLE
static IRQHandler_t IRQTable[IRQ_GIC_LINE_COUNT] = { 0U };
#else
extern IRQHandler_t IRQTable[IRQ_GIC_LINE_COUNT];
#endif
static uint32_t     IRQ_ID0;

/// Initialize interrupt controller.
int32_t IRQ_Initialize (void) {
  #ifndef IRQ_GIC_EXTERN_IRQ_TABLE
    uint32_t i;

    for (i = 0U; i < IRQ_GIC_LINE_COUNT; i++) {
      IRQTable[i] = (IRQHandler_t)NULL;
    }
    GIC_Enable();
  #endif
  return (0);
}


/// Register interrupt handler.
int32_t IRQ_SetHandler (IRQn_ID_t irqn, IRQHandler_t handler) {
  int32_t status;

  if ((irqn >= 0) && (irqn < (IRQn_ID_t)IRQ_GIC_LINE_COUNT)) {
    IRQTable[irqn] = handler;
    status =  0;
  } else {
    status = -1;
  }

  return (status);
}

/// The Interrupt Handler.
void IRQ_Handler (void) {
  IRQn_Type irqn = GIC_AcknowledgePending ();
  if (irqn < (IRQn_Type)IRQ_GIC_LINE_COUNT) {
    IRQTable[irqn]();
  }
  GIC_EndInterrupt (irqn);
}


/// Get the registered interrupt handler.
IRQHandler_t IRQ_GetHandler (IRQn_ID_t irqn) {
  IRQHandler_t h;

  // Ignore CPUID field (software generated interrupts)
  irqn &= 0x3FFU;

  if ((irqn >= 0) && (irqn < (IRQn_ID_t)IRQ_GIC_LINE_COUNT)) {
    h = IRQTable[irqn];
  } else {
    h = (IRQHandler_t)0;
  }

  return (h);
}


/// Enable interrupt.
int32_t IRQ_Enable (IRQn_ID_t irqn) {
  int32_t status;

  if ((irqn >= 0) && (irqn < (IRQn_ID_t)IRQ_GIC_LINE_COUNT)) {
    GIC_EnableIRQ ((IRQn_Type)irqn);
    status = 0;
  } else {
    status = -1;
  }

  return (status);
}


/// Disable interrupt.
int32_t IRQ_Disable (IRQn_ID_t irqn) {
  int32_t status;

  if ((irqn >= 0) && (irqn < (IRQn_ID_t)IRQ_GIC_LINE_COUNT)) {
    GIC_DisableIRQ ((IRQn_Type)irqn);
    status = 0;
  } else {
    status = -1;
  }

  return (status);
}


/// Get interrupt enable state.
uint32_t IRQ_GetEnableState (IRQn_ID_t irqn) {
  uint32_t enable;

  if ((irqn >= 0) && (irqn < (IRQn_ID_t)IRQ_GIC_LINE_COUNT)) {
    enable = GIC_GetEnableIRQ((IRQn_Type)irqn);
  } else {
    enable = 0U;
  }

  return (enable);
}


/// Configure interrupt request mode.
int32_t IRQ_SetMode (IRQn_ID_t irqn, uint32_t mode) {
  uint32_t val;
  uint8_t cfg;
  uint8_t secure;
  uint8_t cpu;
  int32_t status = 0;

  if ((irqn >= 0) && (irqn < (IRQn_ID_t)IRQ_GIC_LINE_COUNT)) {
    // Check triggering mode
    val = (mode & IRQ_MODE_TRIG_Msk);

    if (val == IRQ_MODE_TRIG_LEVEL) {
      cfg = 0x00U;
    } else if (val == IRQ_MODE_TRIG_EDGE) {
      cfg = 0x02U;
    } else {
      cfg = 0x00U;
      status = -1;
    }

    val = (mode & IRQ_MODE_MODEL_Msk);
    if (val == IRQ_MODE_MODEL_1N) {
      cfg |= 1;   // 1-N model
    }

    // Check interrupt type
    val = mode & IRQ_MODE_TYPE_Msk;

    if (val != IRQ_MODE_TYPE_IRQ) {
      status = -1;
    }

    // Check interrupt domain
    val = mode & IRQ_MODE_DOMAIN_Msk;

    if (val == IRQ_MODE_DOMAIN_NONSECURE) {
      secure = 0U;
    } else {
      // Check security extensions support
      val = GIC_DistributorInfo() & (1UL << 10U);

      if (val != 0U) {
        // Security extensions are supported
        secure = 1U;
      } else {
        secure = 0U;
        status = -1;
      }
    }

    // Check interrupt CPU targets
    val = mode & IRQ_MODE_CPU_Msk;

    if (val == IRQ_MODE_CPU_ALL) {
      cpu = 0xFFU;
    } else {
      cpu = (uint8_t)(val >> IRQ_MODE_CPU_Pos);
    }

    // Apply configuration if no mode error
    if (status == 0) {
      GIC_SetConfiguration((IRQn_Type)irqn, cfg);
      GIC_SetTarget       ((IRQn_Type)irqn, cpu);

      if (secure != 0U) {
        GIC_SetGroup ((IRQn_Type)irqn, secure);
      }
    }
  }

  return (status);
}


/// Get interrupt mode configuration.
uint32_t IRQ_GetMode (IRQn_ID_t irqn) {
  uint32_t mode;
  uint32_t val;

  if ((irqn >= 0) && (irqn < (IRQn_ID_t)IRQ_GIC_LINE_COUNT)) {
    mode = IRQ_MODE_TYPE_IRQ;

    // Get trigger mode
    val = GIC_GetConfiguration((IRQn_Type)irqn);

    if ((val & 2U) != 0U) {
      // Corresponding interrupt is edge triggered
      mode |= IRQ_MODE_TRIG_EDGE;
    } else {
      // Corresponding interrupt is level triggered
      mode |= IRQ_MODE_TRIG_LEVEL;
    }

    if (val & 1U) {
      mode |= IRQ_MODE_MODEL_1N;
    }
    // Get interrupt CPU targets
    mode |= GIC_GetTarget ((IRQn_Type)irqn) << IRQ_MODE_CPU_Pos;

  } else {
    mode = IRQ_MODE_ERROR;
  }

  return (mode);
}


/// Get ID number of current interrupt request (IRQ).
IRQn_ID_t IRQ_GetActiveIRQ (void) {
  IRQn_ID_t irqn;
  uint32_t prio;

  /* Dummy read to avoid GIC 390 errata 801120 */
  GIC_GetHighPendingIRQ();

  irqn = GIC_AcknowledgePending();

  __DSB();

  /* Workaround GIC 390 errata 733075 (GIC-390_Errata_Notice_v6.pdf, 09-Jul-2014)  */
  /* The following workaround code is for a single-core system.  It would be       */
  /* different in a multi-core system.                                             */
  /* If the ID is 0 or 0x3FE or 0x3FF, then the GIC CPU interface may be locked-up */
  /* so unlock it, otherwise service the interrupt as normal.                      */
  /* Special IDs 1020=0x3FC and 1021=0x3FD are reserved values in GICv1 and GICv2  */
  /* so will not occur here.                                                       */

  if ((irqn == 0) || (irqn >= 0x3FE)) {
    /* Unlock the CPU interface with a dummy write to Interrupt Priority Register */
    prio = GIC_GetPriority((IRQn_Type)0);
    GIC_SetPriority ((IRQn_Type)0, prio);

    __DSB();

    if ((irqn == 0U) && ((GIC_GetIRQStatus ((IRQn_Type)irqn) & 1U) != 0U) && (IRQ_ID0 == 0U)) {
      /* If the ID is 0, is active and has not been seen before */
      IRQ_ID0 = 1U;
    }
    /* End of Workaround GIC 390 errata 733075 */
  }

  return (irqn);
}


/// Get ID number of current fast interrupt request (FIQ).
IRQn_ID_t IRQ_GetActiveFIQ (void) {
  return ((IRQn_ID_t)-1);
}


/// Signal end of interrupt processing.
int32_t IRQ_EndOfInterrupt (IRQn_ID_t irqn) {
  int32_t status;
  IRQn_Type irq = (IRQn_Type)irqn;

  irqn &= 0x3FFU;

  if ((irqn >= 0) && (irqn < (IRQn_ID_t)IRQ_GIC_LINE_COUNT)) {
    GIC_EndInterrupt (irq);

    if (irqn == 0) {
      IRQ_ID0 = 0U;
    }

    status = 0;
  } else {
    status = -1;
  }

  return (status);
}


/// Set interrupt pending flag.
int32_t IRQ_SetPending (IRQn_ID_t irqn) {
  int32_t status;

  if ((irqn >= 0) && (irqn < (IRQn_ID_t)IRQ_GIC_LINE_COUNT)) {
    GIC_SetPendingIRQ ((IRQn_Type)irqn);
    status = 0;
  } else {
    status = -1;
  }

  return (status);
}

/// Get interrupt pending flag.
uint32_t IRQ_GetPending (IRQn_ID_t irqn) {
  uint32_t pending;

  if ((irqn >= 16) && (irqn < (IRQn_ID_t)IRQ_GIC_LINE_COUNT)) {
    pending = GIC_GetPendingIRQ ((IRQn_Type)irqn);
  } else {
    pending = 0U;
  }

  return (pending & 1U);
}


/// Clear interrupt pending flag.
int32_t IRQ_ClearPending (IRQn_ID_t irqn) {
  int32_t status;

  if ((irqn >= 16) && (irqn < (IRQn_ID_t)IRQ_GIC_LINE_COUNT)) {
    GIC_ClearPendingIRQ ((IRQn_Type)irqn);
    status = 0;
  } else {
    status = -1;
  }

  return (status);
}


/// Set interrupt priority value.
int32_t IRQ_SetPriority (IRQn_ID_t irqn, uint32_t priority) {
  int32_t status;

  if ((irqn >= 0) && (irqn < (IRQn_ID_t)IRQ_GIC_LINE_COUNT)) {
    GIC_SetPriority ((IRQn_Type)irqn, priority);
    status = 0;
  } else {
    status = -1;
  }

  return (status);
}


/// Get interrupt priority.
uint32_t IRQ_GetPriority (IRQn_ID_t irqn) {
  uint32_t priority;

  if ((irqn >= 0) && (irqn < (IRQn_ID_t)IRQ_GIC_LINE_COUNT)) {
    priority = GIC_GetPriority ((IRQn_Type)irqn);
  } else {
    priority = IRQ_PRIORITY_ERROR;
  }

  return (priority);
}


/// Set priority masking threshold.
int32_t IRQ_SetPriorityMask (uint32_t priority) {
  GIC_SetInterfacePriorityMask (GICInterface_PMR_Priority(priority));
  return (0);
}


/// Get priority masking threshold
uint32_t IRQ_GetPriorityMask (void) {
  return GIC_GetInterfacePriorityMask();
}


/// Set priority grouping field split point
int32_t IRQ_SetPriorityGroupBits (uint32_t bits) {
  int32_t status;

  if (bits == IRQ_PRIORITY_Msk) {
    bits = 7U;
  }

  if (bits < 8U) {
    GIC_SetBinaryPoint (7U - bits);
    status = 0;
  } else {
    status = -1;
  }

  return (status);
}


/// Get priority grouping field split point
uint32_t IRQ_GetPriorityGroupBits (void) {
  uint32_t bp;

  bp = GIC_GetBinaryPoint() & 0x07U;

  return (7U - bp);
}

#endif

#endif /* (__CORTEX_A != 0) && CPUSTYLE_ARM && (! defined(__aarch64__)) */
