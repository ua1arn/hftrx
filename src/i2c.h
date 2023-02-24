/*
 * i2c.h
 *
 *  Created on: 23 февр. 2023 г.
 *      Author: admin
 */

#ifndef SRC_SYS_I2C_H_
#define SRC_SYS_I2C_H_
#include "hardware.h"

#include <stdint.h>

#include "gpio.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

unsigned char I2C_ReadBuffer(unsigned char slaveAddr, unsigned char* pBuffer, unsigned char ReadAddr, unsigned short NumByteToRead);
unsigned char I2C_WriteByte(unsigned char slaveAddr, unsigned char* pBuffer, unsigned char WriteAddr);
void i2c_init(uint8_t TWIx);

extern unsigned int I2C_ERROR_CODE;
extern unsigned int I2C_ERROR_COUNT;
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* SRC_SYS_I2C_H_ */
