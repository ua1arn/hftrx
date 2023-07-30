@rem bootable image should be linked to 0x00020000 base address
@rem http://nskhuman.ru/allwinner/card/cardmode.php?ysclid=lkq00i1n2061987481
@set TARGET=E

cfimager -raw -offset 0x2000 -f boot0_sdcard_sun50iw9p1.bin -d %TARGET%
cfimager -raw -offset 0x1004000 -f bl31.bin -d %TARGET%


rem boot0_nand_sun50iw9p1.bin
rem boot0_sdcard_sun50iw9p1.bin
rem boot0_spinor_sun50iw9p1.bin
rem fes1_sun50iw9p1.bin
rem sboot_sun50iw9p1.bin

