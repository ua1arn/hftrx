/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#include "hardware.h"	/* зависящие от процессора функции работы с портами */
#include "formats.h"	// for debug prints
#include <string.h>
#include <stdlib.h>	 // For aligned_alloc
#include <math.h>

#include "Kalman.h"
#include "MPU6050_6Axis_MotionApps20.h"

//#define addr7bit 0x68
//static const unsigned addr8bit = addr7bit << 1;

static MPU6050 mpu;
static volatile bool mpuFlag = false;  // флаг прерывания готовности
static uint8_t fifoBuffer[45];         // буфер

// https://github.com/mstrens/oXs_on_RP2040/blob/a64f62c50c127fc187487e2c1b48715f25dc0f20/src/mpu.cpp#L69
// https://github.com/ElectronicCats/mpu6050/tree/master/src

extern "C" {
	static void dmpReady(void)
	{
	  mpuFlag = true;
	}

}

// board GY-6500. chip TDK MPU6500 test
// addr8bit=0xD0, addr7bit=0x68
// AD0 = 0: 0x68
// AD0 = 1: 0x69
// https://alexgyver.ru/arduino-mpu6050/

// https://github.com/ElectronicCats/mpu6050/tree/master/src

void mpu6500_test(void)
{
	// инициализация DMP и прерывания
	  mpu.initialize();
	  mpu.dmpInitialize();
	  mpu.setDMPEnabled(true);
//	  arm_hardware_set_handler_system(0, 0, dmpReady);
//	  attachInterrupt(0, dmpReady, RISING);
	// прерывание готовности. Поднимаем флаг
	for (;;)
	{
		// переменные для расчёта (ypr можно вынести в глобал)
		Quaternion q;
		VectorFloat gravity;
		float ypr[3];
		// расчёты
		mpu.dmpGetQuaternion(&q, fifoBuffer);
		mpu.dmpGetGravity(&gravity, &q);
		mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);

		if (mpu.dmpGetCurrentFIFOPacket(fifoBuffer))
		{
			// переменные для расчёта (ypr можно вынести в глобал)
			Quaternion q;
			VectorFloat gravity;
			float ypr[3];
			// расчёты
			mpu.dmpGetQuaternion(&q, fifoBuffer);
			mpu.dmpGetGravity(&gravity, &q);
			mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);

			// выводим результат в радианах (-3.14, 3.14)
			PRINTF("z/y/x: %f %f %f\n", ypr[0], ypr[1], ypr[2]); // вокруг оси Z, Y, Z
		}
	}
}
