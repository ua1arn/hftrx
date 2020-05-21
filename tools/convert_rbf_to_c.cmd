set SRCPATH=..\fpga_q131\output_files
rem set SRCPATH=..\fpga_q181std\output_files

rbf2c.exe %SRCPATH%\t1_v3.rbf ..\rbf\rbfimage_v3.h
rbf2c.exe %SRCPATH%\t1_v3_pll.rbf ..\rbf\rbfimage_v3_pll.h
rbf2c.exe %SRCPATH%\t1_v4.rbf ..\rbf\rbfimage_v4.h
rbf2c.exe %SRCPATH%\t1_v4_pll.rbf ..\rbf\rbfimage_v4_pll.h
rbf2c.exe %SRCPATH%\t1_v5_2ch.rbf ..\rbf\rbfimage_v5_2ch.h
rbf2c.exe %SRCPATH%\t1_v5_2ch_pll.rbf ..\rbf\rbfimage_v5_2ch_pll.h
rbf2c.exe %SRCPATH%\t1_v6_2ch.rbf ..\rbf\rbfimage_v6_2ch.h
rbf2c.exe %SRCPATH%\t1_v7_1ch.rbf ..\rbf\rbfimage_v7_1ch.h
rbf2c.exe %SRCPATH%\t1_v7_2ch.rbf ..\rbf\rbfimage_v7_2ch.h
rbf2c.exe %SRCPATH%\t1_v7a_2ch.rbf ..\rbf\rbfimage_v7a_2ch.h
rbf2c.exe %SRCPATH%\t1_v7h_2ch.rbf ..\rbf\rbfimage_v7h_2ch.h
rbf2c.exe %SRCPATH%\t1_v7_oleg4z.rbf ..\rbf\rbfimage_oleg4z.h
rbf2c.exe %SRCPATH%\t1_v8t_96k.rbf ..\rbf\rbfimage_v8t_96k.h
rbf2c.exe %SRCPATH%\t1_v8t_192k.rbf ..\rbf\rbfimage_v8t_192k.h
rbf2c.exe %SRCPATH%\t1_v9a_2ch.rbf ..\rbf\rbfimage_v9a_2ch.h

pause
