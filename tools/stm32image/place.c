#include <stdio.h>

void placeto(unsigned sector)
{
	FILE * disk = fopen("test.img", "r+b");
	FILE * bootloader = fopen("app.stm32", "rb");

	if (disk == NULL || bootloader == NULL)
		return 1;
	
	fseek(disk, sector * 512uL, SEEK_SET);
	rewind(bootloader);
	for (;;)
	{
		int c = fgetc(bootloader);
		if (c == EOF)
			break;
		fputc(c, disk);
	}
	fclose(bootloader);
	fclose(disk);
}

int main(void)
{
	placeto(34);
	placeto(269);
	return 0;
}
