aarch64-none-elf-gcc.exe -mcpu=cortex-A53 -Os -c tt.c
aarch64-none-elf-ld -o tt.elf tt.o
aarch64-none-elf-objdump.exe -d tt.elf >tt.lst
pause

