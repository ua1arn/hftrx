SET fn=image_test_M.img
copy image.raw.backup /b %fn%
wtitefsbl.exe %fn% 0x04400 fsbl.stm32 -w
wtitefsbl.exe %fn% 0x44400 fsbl.stm32 -w
wtitefsbl.exe %fn% 0x84400 tc1_stm32mp157axx_app.stm32 -w
pause

