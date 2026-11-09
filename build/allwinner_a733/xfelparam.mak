
#BOORPARAM=bootparam_radxa_a7z_a733_lpddr4.mak
#DDRINITBLOB=ddrinit_radxa_a7z_a733_lpddr4.bin
#LIBDRAM=libdram.a

#BOORPARAM=bootparam_orangepi_4_pro.mak
#DDRINITBLOB=ddrinit_orangepi_4_pro.bin
#LIBDRAM=libdram.a

BOORPARAM=bootparam_helperboard_a733_lpddr5.mak
DDRINITBLOB=ddrinit_helperboard_a733_lpddr5.bin
#LIBDRAM=libdram-lpddr5.a
LIBDRAM=libdram.a

-include ./$(BOORPARAM)
