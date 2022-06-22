program_flash -f boot.bin -verify -offset 0x00000 -flash_type qspi-x4-single -fsbl fsbl_Y7Z020.elf
program_flash -f boot.bin -verify -offset 0x40000 -flash_type qspi-x4-single -fsbl fsbl_Y7Z020.elf
