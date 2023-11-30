@rem bootable image should be linked to 0x00020000 base address
cfimager -raw -offset 0x2000 -f fsbl.alw32 -d _CHANGE_LETTER_E
cfimager -raw -offset 0x40000 -f tc1_awt507_app.alw32 -d _CHANGE_LETTER_E
