#include "hardware.h"
#include "board.h"
#include "spi.h"
#include "formats.h"

#if TSC1_TYPE == TSC_TYPE_SOFTWARE
#include "softtsc.h"

static void spi_request(uint8_t reg, uint8_t * buf, uint8_t len)
{
	prog_select_impl(targetsofttsc);
	prog_val8_impl(targetsofttsc, reg);
	prog_spi_read_frame(targetsofttsc, buf, len);
	prog_unselect_impl();

//	for (uint8_t i = 0; i < len; i ++)
//		PRINTF("%d; ", buf[i]);
//	PRINTF("\n");
}

uint_fast8_t softtsc_getXY(uint_fast16_t * xr, uint_fast16_t * yr)
{
	uint8_t buf[5];
	uint8_t command = SOFTTSC_READ;
	uint16_t x1, y1;
	static uint16_t x = 0, y = 0, release_count = 0;

	spi_request(command, buf, 5);

	if (buf[4] == 1)
	{
		x1 = ((buf[3] << 8) | buf[2]);
		y1 = ((buf[1] << 8) | buf[0]);
		release_count = 2;

		if ((x1 < DIM_X) && (y1 < DIM_Y))
		{
			x = x1;
			y = y1;
		}

		* xr = x;
		* yr = y;
		return 1;
	}
	else
	{
		if (release_count)
		{
			release_count --;
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
}

uint_fast8_t softtsc_initialize(void)
{
	uint8_t buf;
	uint8_t command = SOFTTSC_INIT;

	spi_request(command, & buf, 1);

	if (buf == 0xBA)
		return 1;
	else
		return 0;
}
#endif /* #if TSC1_TYPE == TSC_TYPE_SOFTWARE */
