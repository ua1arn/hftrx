/*
 * xhelper507.h
 *
 *  Created on: 21 сент. 2023 г.
 *      Author: User
 */

#ifndef SRC_XHELPERT507_XHELPER507_H_
#define SRC_XHELPERT507_XHELPER507_H_


void user_uart0_initialize(void);
void user_uart1_initialize(void);
void user_uart2_initialize(void);
void user_uart3_initialize(void);
void user_uart4_initialize(void);
void user_uart5_initialize(void);

void uart0_spool(void);
void uart1_spool(void);
void uart2_spool(void);
void uart3_spool(void);
void uart4_spool(void);
void uart5_spool(void);

enum { qSZ = 512 };
typedef struct u8queue
{
	uint8_t buffer [qSZ];
	unsigned qg;
	unsigned qp;
} u8queue_t;

void uint8_queue_init(u8queue_t * q);
uint_fast8_t uint8_queue_put(u8queue_t * q, uint_fast8_t c);
uint_fast8_t uint8_queue_get(u8queue_t * q, uint_fast8_t * pc);
uint_fast8_t uint8_queue_empty(const u8queue_t * q);

void xbsave_pressure(float v);
void xbsave_position(unsigned id, int pos);
void xbsave_setpos(unsigned id, int pos);	// set point

void xbsavebins_float32(unsigned reg, float v);
void xbsavebins_int32(unsigned reg, int32_t v);
void xbsavebins_float64(unsigned reg, double v);

void xbsavemagn(double roll, double pitch, double jaw);
int xbreginc(unsigned reg);	// register increment size

#define MAXPACKREGS 10

void xbsetregF(unsigned reg, unsigned argcount, const double * pv);
void xbsetregI(unsigned reg, unsigned argcount, const long * pv);

#endif /* SRC_XHELPERT507_XHELPER507_H_ */
