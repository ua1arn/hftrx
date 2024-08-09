set SRCPATH=..\fpga_q131\output_files
set SRCPATHLONG=..\fpga_q131_long\output_files
rem set SRCPATH=..\fpga_q181std\output_files

@rem STM32F/STM32H

rbf2c.exe %SRCPATH%\t1_v5_2ch.rbf ..\rbf\rbfimage_v5_2ch.h
rbf2c.exe %SRCPATH%\t1_v6_2ch.rbf ..\rbf\rbfimage_v6_2ch.h
rbf2c.exe %SRCPATH%\t1_v7_1ch.rbf ..\rbf\rbfimage_v7_1ch.h
rbf2c.exe %SRCPATH%\t1_v7_2ch.rbf ..\rbf\rbfimage_v7_2ch.h
rbf2c.exe %SRCPATH%\t1_v7a_2ch.rbf ..\rbf\rbfimage_v7a_2ch.h
rbf2c.exe %SRCPATH%\t1_v7h_2ch.rbf ..\rbf\rbfimage_v7h_2ch.h
rbf2c.exe %SRCPATH%\t1_v7_oleg4z.rbf ..\rbf\rbfimage_oleg4z.h

@rem Renesas

rbf2c.exe %SRCPATH%\t1_v8t_96k.rbf ..\rbf\rbfimage_v8t_96k.h
rbf2c.exe %SRCPATH%\t1_v8t_192k.rbf ..\rbf\rbfimage_v8t_192k.h

@rem STM32MP157

rbf2c.exe %SRCPATH%\t1_v9a_2ch.rbf ..\rbf\rbfimage_v9a_2ch.h
rbf2c.exe %SRCPATH%\t1_v9c_2ch.rbf ..\rbf\rbfimage_v9c_2ch.h

@rem Allwinner T113-s3, T507-H

rbf2c.exe %SRCPATH%\t1_v9d_2ch.rbf ..\rbf\rbfimage_v9d_2ch.h

rbf2c.exe %SRCPATHLONG%\t1_v9m_velo.rbf ..\rbf\rbfimage_v9m_velo.h

@rem Allwinner T507-H

rbf2c.exe %SRCPATHLONG%\t1_veloci_v0.rbf ..\rbf\rbfimage_veloci_v0.h
rbf2c.exe %SRCPATHLONG%\t1_veloci_v2.rbf ..\rbf\rbfimage_veloci_v2.h

pause
