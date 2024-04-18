@echo zzz >sdimaget507.img
wtitefsbl.exe sdimaget507.img 0x0000 bootsector0.bin -w
wtitefsbl.exe sdimaget507.img 0x2000 fsbl.alw32 -w
wtitefsbl.exe sdimaget507.img 0x40000 tc1_awt507_app.alw32 -w
pause
