del *.o
del *.map
del *.asm
del *.elf
del *.bin
del *.txt

pause

set CARGS= -Ofast -std=c99 -ffunction-sections -fdata-sections -Wall -Wextra -Wno-unused-variable -I "../../../CMSIS_5/CMSIS/Core_A/Include" 

xtensa-hifi4-elf-gcc -x assembler-with-cpp %CARGS% -c startup.S

xtensa-hifi4-elf-gcc %CARGS% -c startup_c.c
xtensa-hifi4-elf-gcc %CARGS% -c main.c

xtensa-hifi4-elf-gcc %CARGS% -nostartfiles -nodefaultlibs -T hifi4.ld -Wl,-Map=main.map,--cref -Wl,--gc-sections -Wl,--strip-all startup.o startup_c.o main.o -o main.elf

xtensa-hifi4-elf-objcopy -O binary main.elf main.bin

xtensa-hifi4-elf-objdump -D main.elf > main.asm

bin2c.exe main.bin main.txt

pause
