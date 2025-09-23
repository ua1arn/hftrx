aarch64-none-elf-gcc.exe -I ../../inc -I ../../CMSIS_6/CMSIS/Core/Include -mcpu=cortex-A53 -Os -c run64.c
aarch64-none-elf-ld -o run64.elf run64.o
aarch64-none-elf-objdump.exe -d run64.elf >run64.lst
pause

