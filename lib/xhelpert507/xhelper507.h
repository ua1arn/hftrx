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

void xbsave_pressure(float v);	// сохранить измеренное давление
void xbsave_position(unsigned id, int pos);	// сохранить позицию, в которой находится рулевая машинка
void xbsave_setpos(unsigned id, int pos);	// Установить желаемую позицию

void xbsavebins_float32(unsigned reg, float v);
void xbsavebins_int32(unsigned reg, int32_t v);
void xbsavebins_float64(unsigned reg, double v);

void xbsavemagn(double roll, double pitch, double jaw);

#define MAXPACKREGS 10
#define MAXPACKREGSEXT 13 // кроличество параметров для внешней коррекции
void xbsetregF(unsigned reg, unsigned argcount, const double * pv);
void xbsetregI(unsigned reg, unsigned argcount, const long * pv);
void xbsetregEXTF(const double * pv);	// РЕГИСТРЫ ВНЕШНЕЙ КОРРЕКЦИИ

#define GIRONAV_DEVADDR 1

#endif /* SRC_XHELPERT507_XHELPER507_H_ */
