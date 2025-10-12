aarch64-none-elf-gcc -Ofast -mcpu=cortex-a53 --specs=nosys.specs -I "../../inc"  -I "../../CMSIS_6/CMSIS/Core/Include" -I "../../CMSIS_6/CMSIS/Core/Include/A-PROFILE"  -D"NDEBUG"=1 -D"CPUSTYLE_T507"=1 -D"CPUSTYLE_CA53"=1 -c code.c
aarch64-none-elf-objdump -d code.o >code.txt
pause


