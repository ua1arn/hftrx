REM Utility files can be downloaded from https://sourceforge.net/projects/dfu-util/files/latest/download
@echo Press any key for write BOOTLOADERs and APPLICATION sections of FLASH
@pause
set FSBLNAME="fsbl_533.stm32"
dfu-util --alt 1 --dfuse-address 0x70000000 --download %FSBLNAME%
dfu-util --alt 1 --dfuse-address 0x70040000 --download %FSBLNAME%
rem dfu-util.exe --device ffff:0738 --alt 0 --dfuse-address 0x70080000 --download "tc1_stm32mp157axx_app.stm32"
@pause

