SET fn=boot.tmp
copy boottemplate.bin /b %fn%
wtitefsbl.exe %fn% 0x00100 tc1_alwnrt113s3_boot.bin -w
checksum.exe %fn% %fn%.v3s
pause

