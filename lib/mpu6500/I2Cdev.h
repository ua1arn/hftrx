// Arduino compatibility librardy

#ifndef I2CDEV_H_INCLUDED
#define I2CDEV_H_INCLUDED 1

#include <stdint.h>
// 1000ms default read timeout (modify with "I2Cdev::readTimeout = [ms];")
#define I2CDEV_DEFAULT_READ_TIMEOUT     ((uint32_t)1) // RP2040 I2C functions with timeout use microseconds so we have to multiply by 10^3

class I2Cdev {
public:
    I2Cdev();

    static bool readBit(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint8_t *data, uint16_t timeout=I2Cdev::readTimeout, void *wireObj=0);
    static bool readBitW(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint16_t *data, uint16_t timeout=I2Cdev::readTimeout, void *wireObj=0);
    static bool readBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t *data, uint16_t timeout=I2Cdev::readTimeout, void *wireObj=0);
    static bool readBitsW(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint16_t *data, uint16_t timeout=I2Cdev::readTimeout, void *wireObj=0);
    static bool readByte(uint8_t devAddr, uint8_t regAddr, uint8_t *data, uint16_t timeout=I2Cdev::readTimeout, void *wireObj=0);
    static bool readWord(uint8_t devAddr, uint8_t regAddr, uint16_t *data, uint16_t timeout=I2Cdev::readTimeout, void *wireObj=0);
    static int8_t readBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *data, uint16_t timeout=I2Cdev::readTimeout, void *wireObj=0);
    static int8_t readWords(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint16_t *data, uint16_t timeout=I2Cdev::readTimeout, void *wireObj=0);

    static bool writeBit(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint8_t data, void *wireObj=0);
    static bool writeBitW(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint16_t data, void *wireObj=0);
    static bool writeBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t data, void *wireObj=0);
    static bool writeBitsW(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint16_t data, void *wireObj=0);
    static bool writeByte(uint8_t devAddr, uint8_t regAddr, uint8_t data, void *wireObj=0);
    static bool writeWord(uint8_t devAddr, uint8_t regAddr, uint16_t data, void *wireObj=0);
    static bool writeBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, const uint8_t *data, void *wireObj=0);
    static bool writeWords(uint8_t devAddr, uint8_t regAddr, uint8_t length, const uint16_t *data, void *wireObj=0);

    static uint16_t readTimeout;
};

#endif /* I2CDEV_H_INCLUDED */
