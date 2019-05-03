cd at91sam7s
make MAKE_DEFINES="-DCTLSTYLE_V7=1 -DNIGHTBUILD=1"
cd ..
cd at91sam9xe
make MAKE_DEFINES="-DCTLSTYLE_V7=1 -DNIGHTBUILD=1"
cd ..
cd atmega32a_8
make MAKE_DEFINES="-DCTLSTYLE_SW2018XVR=1 -DNIGHTBUILD=1"
cd ..
cd atmega644p_8
make MAKE_DEFINES="-DCTLSTYLE_SW2018XVR=1 -DNIGHTBUILD=1"
cd ..
cd atsam3s4b
make MAKE_DEFINES="-DCTLSTYLE_V8B=1 -DNIGHTBUILD=1"
cd ..
cd atsam4sa16c
make MAKE_DEFINES="-DCTLSTYLE_V8B=1 -DNIGHTBUILD=1"
cd ..
cd atsams70q20
make MAKE_DEFINES="-DCTLSTYLE_V8B=1 -DNIGHTBUILD=1"
cd ..
cd atxmega128a4u
make MAKE_DEFINES="-DCTLSTYLE_V1X=1 -DNIGHTBUILD=1"
cd ..
cd mk20dx256vlh7
make MAKE_DEFINES="-DCTLSTYLE_V1T=1 -DNIGHTBUILD=1"
cd ..
cd r7s721020
make MAKE_DEFINES="-DCTLSTYLE_STORCH_V8=1 -DNIGHTBUILD=1"
cd ..
cd stm32f030f4p
make MAKE_DEFINES="-DCTLSTYLE_V1E=1 -DNIGHTBUILD=1"
cd ..
cd stm32f051c6t
make MAKE_DEFINES="-DCTLSTYLE_V1F=1 -DNIGHTBUILD=1"
cd ..
cd stm32f101rb
make MAKE_DEFINES="-DCTLSTYLE_V1B=1 -DNIGHTBUILD=1"
cd ..
cd stm32f103c8
make MAKE_DEFINES="-DCTLSTYLE_RA4YBO_V3=1 -DNIGHTBUILD=1"
cd ..
cd stm32f303vc
make MAKE_DEFINES="-DCTLSTYLE_RA4YBO_V3=1 -DNIGHTBUILD=1"
cd ..
cd stm32f401rb
make MAKE_DEFINES="-DCTLSTYLE_V1K=1 -DNIGHTBUILD=1"
cd ..
cd stm32f407vg
make MAKE_DEFINES="-DCTLSTYLE_V2D=1 -DNIGHTBUILD=1"
cd ..
cd stm32f429zi
make MAKE_DEFINES="-DCTLSTYLE_V2D=1 -DNIGHTBUILD=1"
cd ..
cd stm32f446ze
make MAKE_DEFINES="-DCTLSTYLE_V3D=1 -DNIGHTBUILD=1"
cd ..
cd stm32f723ze
make MAKE_DEFINES="-DCTLSTYLE_STORCH_V3=1 -DNIGHTBUILD=1"
cd ..
cd stm32f746zg
make MAKE_DEFINES="-DCTLSTYLE_STORCH_V3=1 -DNIGHTBUILD=1"
cd ..
cd stm32f767zi
make MAKE_DEFINES="-DCTLSTYLE_STORCH_V5=1 -DNIGHTBUILD=1"
cd ..
cd stm32f769ii
make MAKE_DEFINES="-DCTLSTYLE_STORCH_V5=1 -DNIGHTBUILD=1"
cd ..
cd stm32h743zi
make MAKE_DEFINES="-DCTLSTYLE_STORCH_V3=1 -DNIGHTBUILD=1"
cd ..
cd stm32l051k6t
make MAKE_DEFINES="-DCTLSTYLE_V1G=1 -DNIGHTBUILD=1"
cd ..

@echo off
ECHO Checking build results...
ECHO.

SET MPROC=at91sam7s
if exist %MPROC%/tc1_%MPROC%.hex ( ECHO Testing %MPROC% OK ) else ( ECHO Testing %MPROC% ERROR )
SET MPROC=at91sam9xe
if exist %MPROC%/tc1_%MPROC%.hex ( ECHO Testing %MPROC% OK ) else ( ECHO Testing %MPROC% ERROR )
SET MPROC=atmega32a_8
if exist %MPROC%/tc1_atmega32_8.hex ( ECHO Testing %MPROC% OK ) else ( ECHO Testing %MPROC% ERROR )
SET MPROC=atmega644p_8
if exist %MPROC%/tc1_atmega644_8.hex ( ECHO Testing %MPROC% OK ) else ( ECHO Testing %MPROC% ERROR )
SET MPROC=atsam3s4b
if exist %MPROC%/tc1_%MPROC%.hex ( ECHO Testing %MPROC% OK ) else ( ECHO Testing %MPROC% ERROR )
SET MPROC=atsam4sa16c
if exist %MPROC%/tc1_%MPROC%.hex ( ECHO Testing %MPROC% OK ) else ( ECHO Testing %MPROC% ERROR )
SET MPROC=atsams70q20
if exist %MPROC%/tc1_%MPROC%.hex ( ECHO Testing %MPROC% OK ) else ( ECHO Testing %MPROC% ERROR )
SET MPROC=atxmega128a4u
if exist %MPROC%/tc1_%MPROC%.hex ( ECHO Testing %MPROC% OK ) else ( ECHO Testing %MPROC% ERROR )
SET MPROC=mk20dx256vlh7
if exist %MPROC%/tc1_%MPROC%.hex ( ECHO Testing %MPROC% OK ) else ( ECHO Testing %MPROC% ERROR )
SET MPROC=r7s721020
if exist %MPROC%/tc1_%MPROC%_app.bin ( ECHO Testing %MPROC% OK ) else ( ECHO Testing %MPROC% ERROR )
SET MPROC=stm32f030f4p
if exist %MPROC%/tc1_%MPROC%_rom.hex ( ECHO Testing %MPROC% OK ) else ( ECHO Testing %MPROC% ERROR )
SET MPROC=stm32f051c6t
if exist %MPROC%/tc1_%MPROC%_rom.hex ( ECHO Testing %MPROC% OK ) else ( ECHO Testing %MPROC% ERROR )
SET MPROC=stm32f101rb
if exist %MPROC%/tc1_%MPROC%_rom.hex ( ECHO Testing %MPROC% OK ) else ( ECHO Testing %MPROC% ERROR )
SET MPROC=stm32f103c8
if exist %MPROC%/tc1_%MPROC%_rom.hex ( ECHO Testing %MPROC% OK ) else ( ECHO Testing %MPROC% ERROR )
SET MPROC=stm32f303vc
if exist %MPROC%/tc1_%MPROC%_rom.hex ( ECHO Testing %MPROC% OK ) else ( ECHO Testing %MPROC% ERROR )
SET MPROC=stm32f401rb
if exist %MPROC%/tc1_%MPROC%_rom.hex ( ECHO Testing %MPROC% OK ) else ( ECHO Testing %MPROC% ERROR )
SET MPROC=stm32f407vg
if exist %MPROC%/tc1_%MPROC%_rom.hex ( ECHO Testing %MPROC% OK ) else ( ECHO Testing %MPROC% ERROR )
SET MPROC=stm32f429zi
if exist %MPROC%/tc1_%MPROC%_rom.hex ( ECHO Testing %MPROC% OK ) else ( ECHO Testing %MPROC% ERROR )
SET MPROC=stm32f446ze
if exist %MPROC%/tc1_%MPROC%_rom.hex ( ECHO Testing %MPROC% OK ) else ( ECHO Testing %MPROC% ERROR )
SET MPROC=stm32f723ze
if exist %MPROC%/tc1_%MPROC%_rom.hex ( ECHO Testing %MPROC% OK ) else ( ECHO Testing %MPROC% ERROR )
SET MPROC=stm32f746zg
if exist %MPROC%/tc1_%MPROC%_rom.hex ( ECHO Testing %MPROC% OK ) else ( ECHO Testing %MPROC% ERROR )
SET MPROC=stm32f767zi
if exist %MPROC%/tc1_%MPROC%_rom.hex ( ECHO Testing %MPROC% OK ) else ( ECHO Testing %MPROC% ERROR )
SET MPROC=stm32f769ii
if exist %MPROC%/tc1_%MPROC%_rom.hex ( ECHO Testing %MPROC% OK ) else ( ECHO Testing %MPROC% ERROR )
SET MPROC=stm32h743zi
if exist %MPROC%/tc1_%MPROC%_rom.hex ( ECHO Testing %MPROC% OK ) else ( ECHO Testing %MPROC% ERROR )
SET MPROC=stm32l051k6t
if exist %MPROC%/tc1_%MPROC%_rom.hex ( ECHO Testing %MPROC% OK ) else ( ECHO Testing %MPROC% ERROR )

pause
