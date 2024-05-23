set OFILE=sdimage_t113.img
@echo zzz >%OFILE%
wtitefsbl.exe %OFILE% 0x0000 bootsector0.bin -w
wtitefsbl.exe %OFILE% 0x2000 fsbl.alw32 -w
wtitefsbl.exe %OFILE% 0x40000 tc1_t113s3_app.alw32 -w
pause
