xtensa-hifi4-elf-gcc -Os -c "c0.S"
xtensa-hifi4-elf-gcc -I d:/user/svn/hftrx/CMSIS_5/CMSIS/Core_A/Include -mauto-litpools -Os -omit-frame -c "t0.c"
xtensa-hifi4-elf-gcc -nostartfiles -nodefaultlibs -T"hifi4.ld"  -Wl,-Map=t0.map,--cref -o"t0.elf" "c0.o" "t0.o"

xtensa-hifi4-elf-objcopy -O binary -R .noinit -R .bss -R .vram -R .heap "t0.elf"  "t0.bin"

bin2c.exe "t0.bin" "t0.txt"


pause
