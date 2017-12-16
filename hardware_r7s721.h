/* $Id$ */

#ifndef HARDWARE_R7S721_H_INCLUDED
#define HARDWARE_R7S721_H_INCLUDED 1

#define IRQ_GIC_LINE_COUNT           (587)   /* The number of interrupt sources */

//#define INTC_LEVEL_SENSITIVE    (0)     /* Level sense  */
//#define INTC_EDGE_TRIGGER       (1)     /* Edge trigger */

void r7s721_intc_initialize(void);
void r7s721_intc_registintfunc(uint_fast16_t int_id, void (* func)(void));

void r7s721_handler_interrupt(uint32_t icciar);

void r7s721_ttb_initialize(void);
void r7s721_ttb_map(unsigned long va,	/* virtual address */unsigned long la	/* linear (physical) address */);

#endif /* HARDWARE_R7S721_H_INCLUDED */
