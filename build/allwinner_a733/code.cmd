arm-none-eabi-gcc -Ofast -mcpu=cortex-a55 -mfloat-abi=hard -mfpu=neon-vfpv4 --specs=nosys.specs -I "../../CMSIS_6/CMSIS/Core/Include" -I "../../CMSIS_6/CMSIS/Core/Include/A-PROFILE"  -D"NDEBUG"=1 -D"CPUSTYLE_A733"=1 -c code.c
arm-none-eabi-objdump -d code.o >code.txt

arm-none-eabi-gcc -o defs_a55_32bit.txt -Ofast -mcpu=cortex-a55 -mfloat-abi=hard -mfpu=neon-vfpv4 --specs=nosys.specs -dM -E - < NUL
aarch64-none-elf-gcc -o defs_a55_64bit.txt -Ofast -mcpu=cortex-a55+fp+simd --specs=nosys.specs -dM -E - < NUL

arm-none-eabi-gcc -o defs_a53_32bit.txt -Ofast -mcpu=cortex-a53 -mfloat-abi=hard -mfpu=neon-vfpv4 --specs=nosys.specs -dM -E - < NUL
aarch64-none-elf-gcc -o defs_a53_64bit.txt -Ofast -mcpu=cortex-a53+fp+simd --specs=nosys.specs -dM -E - < NUL
