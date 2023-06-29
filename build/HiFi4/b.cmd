set CARGS = -Wa,--longcalls -static -O2  -Wall -mtext-section-literals  -fno-inline-functions -ffunction-sections -fdata-sections  -mlongcalls -std=c99 -Wextra -Wa,--no-generate-flix -mtarget-align -Wno-unused-variable
xtensa-hifi4-elf-gcc  -x assembler-with-cpp -Os -c "c0.S"
rem xtensa-hifi4-elf-gcc -I "../../CMSIS_5/CMSIS/Core_A/Include" -mauto-litpools -Os -%CARGS% -c "t0.c"
xtensa-hifi4-elf-gcc -I "../../CMSIS_5/CMSIS/Core_A/Include" -Os %CARGS% -c "t0.c"
xtensa-hifi4-elf-gcc -nostartfiles -nodefaultlibs -T"hifi4.ld"  -Wl,-Map=t0.map,--cref -o"t0.elf" "c0.o" "t0.o"

xtensa-hifi4-elf-objcopy -O binary -R .noinit -R .bss -R .vram -R .heap "t0.elf"  "t0.bin"

bin2c.exe "t0.bin" "t0.txt"


pause
