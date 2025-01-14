arm-none-eabi-gcc -Ofast -mcpu=cortex-a53 -mfloat-abi=hard -mfpu=neon-vfpv4 --specs=nosys.specs -I "../../CMSIS_6/CMSIS/Core/Include" -I "../../CMSIS_6/CMSIS/Core/Include/A-PROFILE"  -D"NDEBUG"=1 -D"CPUSTYLE_T507"=1 -D"CPUSTYLE_CA53"=1 -c code.c
arm-none-eabi-objdump -d code.o >code.txt

