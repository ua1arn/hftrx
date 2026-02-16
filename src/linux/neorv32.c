/*
 * By RA4ASN
 */

#include "hardware.h"	/* зависящие от процессора функции работы с портами */
#include "formats.h"	// for debug prints

#if LINUX_SUBSYSTEM && NEORV32_RT
#include "linux_subsystem.h"
#include "pcie_dev.h"

void neorv32_reset_assert(void)
{
	uint32_t v = xdma_read_user(AXI_LITE_PL_CONTROL);
	v &= ~ NEORV32_RESET_MASK;
	xdma_write_user(AXI_LITE_PL_CONTROL, v);
}

void neorv32_reset_deassert(void)
{
	uint32_t v = xdma_read_user(AXI_LITE_PL_CONTROL);
	v |= NEORV32_RESET_MASK;
	xdma_write_user(AXI_LITE_PL_CONTROL, v);
}

int neorv32_load_firmware(void)
{
	int fd_file = -1;
	uint8_t * file_data = NULL;
	struct stat st;
	size_t load_size;
	ssize_t bytes_read;

	fd_file = open(NEORV32_FW_NAME, O_RDONLY);
	if (fd_file < 0) {
		printf("Error: Cannot open firmware file '%s': %s\n", NEORV32_FW_NAME, strerror(errno));
		return -1;
	}

	if (fstat(fd_file, & st) < 0) {
		printf("Error: Cannot stat file '%s': %s\n", NEORV32_FW_NAME, strerror(errno));
		return -1;
	}

	load_size = st.st_size;

	file_data = (uint8_t *) malloc(load_size);
	if (! file_data) {
		printf("Error: Cannot allocate memory for firmware: %s\n", strerror(errno));
		return -1;
	}

	bytes_read = read(fd_file, file_data, load_size);
	if (bytes_read < 0) {
		printf("Error: Cannot read firmware file: %s\n", strerror(errno));
		return -1;
	}

	if ((size_t) bytes_read != load_size) {
		printf("Warning: Read only %zd bytes of %zu\n", bytes_read, load_size);
		load_size = bytes_read;
	}

	if (1) {
		printf("Read %zu bytes from firmware file\n", load_size);
		printf("First 16 bytes: ");

		for (size_t i = 0; i < 16 && i < load_size; i++)
			printf("%02x ", file_data[i]);

		printf("\n");
	}

	neorv32_reset_assert();
	usleep(1000);

	// load firmware to bram
	xdma_h2c_transfer(NEORV32_FW_LOAD_ADDR, load_size, file_data);
	usleep(1000);

	neorv32_reset_deassert();

	int status = 0;
	do {
		usleep(1000);
		status = xdma_read_user(AXI_LITE_PL_STATUS);
	} while (! (status & NEORV32_RUN_MASK));

	printf("neorv32 firmware loaded and run\n");
	free(file_data);

	return 1;
}

#endif /* LINUX_SUBSYSTEM && NEORV32_RT */
