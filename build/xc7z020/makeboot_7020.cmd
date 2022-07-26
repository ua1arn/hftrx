call bootgen -arch zynq -image app_7020.bif -o boot.bin -w
call program_flash -f boot.bin -flash_type qspi-x4-single -fsbl fsbl_Y7Z020.elf
