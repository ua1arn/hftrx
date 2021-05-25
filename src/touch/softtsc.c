#include "hardware.h"
#include "board.h"
#include "formats.h"

#if TSC1_TYPE == TSC_TYPE_SOFTWARE
#include "softtsc.h"

uint_fast8_t softtsc_getXY(uint_fast16_t * xr, uint_fast16_t * yr)
{
	uint8_t buf[5];
	uint8_t command = SOFTTSC_READ;

	u32 statusTX = XIicPs_MasterSendPolled(& xc7z_iic, & command, 1, SOFTW_I2C_ADDR);
	if (statusTX != XST_SUCCESS) {
		PRINTF("1 XIicPs_MasterSendPolled error %d\n", statusTX);
		return 0;
	}

	while (XIicPs_BusIsBusy(& xc7z_iic));

	u32 statusRX = XIicPs_MasterRecvPolled(& xc7z_iic, buf, 5, SOFTW_I2C_ADDR);
	if (statusRX != XST_SUCCESS) {
		PRINTF("1 XIicPs_MasterRecvPolled error %d\n", statusRX);
		return 0;
	}

	if (buf[0] == 1)
	{
		uint16_t x = ((buf[1] << 8) | buf[2]);
		uint16_t y = ((buf[3] << 8) | buf[4]);
		//PRINTF("%d %d\n", x, y);
		* xr = x;
		* yr = y;
		return 1;
	}
	else
	{
		* xr = 0;
		* yr = 0;
		return 0;
	}
}

uint_fast8_t softtsc_initialize(void)
{
	uint8_t buf[1];
	uint8_t command = SOFTTSC_INIT;

	u32 statusTX = XIicPs_MasterSendPolled(& xc7z_iic, & command, 1, SOFTW_I2C_ADDR);
	if (statusTX != XST_SUCCESS) {
		PRINTF("1 XIicPs_MasterSendPolled error %d\n", statusTX);
		return 0;
	}

	while (XIicPs_BusIsBusy(& xc7z_iic));

	u32 statusRX = XIicPs_MasterRecvPolled(& xc7z_iic, buf, 1, SOFTW_I2C_ADDR);
	if (statusRX != XST_SUCCESS) {
		PRINTF("1 XIicPs_MasterRecvPolled error %d\n", statusRX);
		return 0;
	}

	if (buf[0] == 0xBA)
		return 1;
	else
		return 0;
}
#endif /* #if TSC1_TYPE == TSC_TYPE_SOFTWARE */
