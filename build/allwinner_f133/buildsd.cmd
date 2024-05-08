set OFILE=sdimage_f133.img
@echo zzz >%OFILE%
wtitefsbl.exe %OFILE% 0x0000 bootsector0.bin -w
wtitefsbl.exe %OFILE% 0x2000 fsbl.f133 -w
wtitefsbl.exe %OFILE% 0x40000 tc1_t113s3_app.f133 -w
pause
