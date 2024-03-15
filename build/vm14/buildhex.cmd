stm32image -l 0x40000100 -e 0x40000100 -s tc1_vm14_app.bin -d tc1_vm14_app.stm32
@echo zzzz >progimage.bin
wtitefsbl.exe progimage.bin 0x00000000 tc1_vm14_boot0.bin -w
wtitefsbl.exe progimage.bin 0x00000800 tc1_vm14_boot.bin -w
wtitefsbl.exe progimage.bin 0x00020000 tc1_vm14_app.stm32 -w
bin2ihex  -l 0x80000000 -s progimage.bin >progimage.hex
pause
