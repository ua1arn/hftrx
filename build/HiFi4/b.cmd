set CARGS= -Os -Wall  -std=c99 -Wextra -Wno-unused-variable -ffunction-sections -fdata-sections
	
xtensa-hifi4-elf-gcc  -x assembler-with-cpp %CARGS% -c "c0.S"
rem xtensa-hifi4-elf-gcc -I "../../CMSIS_5/CMSIS/Core_A/Include" -mauto-litpools -%CARGS% -c "t0.c"
xtensa-hifi4-elf-gcc -I "../../CMSIS_5/CMSIS/Core_A/Include" %CARGS% -c "t0.c"
xtensa-hifi4-elf-gcc %CARGS%  -nostartfiles -nodefaultlibs -T"hifi4.ld"  -Wl,-Map=t0.map,--cref -o"t0.elf" "c0.o" "t0.o"

xtensa-hifi4-elf-objcopy -O binary -R .noinit -R .bss -R .vram -R .heap "t0.elf"  "t0.bin"

bin2c.exe "t0.bin" "t0.txt"


pause
