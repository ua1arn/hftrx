@rem bootable image should be linked to 0x00020000 base address
cfimager -raw -offset 0x2000 -f fsbl.alw32 -d _CHANGE_LETTER_E
