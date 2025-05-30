@rem see https://www.gyan.dev/ffmpeg/builds/ for binaries
@rem ffmpeg -i tvtest1.png -c:v rawvideo -pix_fmt nv12 -qscale:v 0 -an -y picture.yuv
@rem ffmpeg -i picture.png -c:v rawvideo -pix_fmt nv12 -qscale:v 0 -an -y picture.yuv
@rem ffmpeg -i red_on_white.png -c:v rawvideo -pix_fmt nv12 -qscale:v 0 -an -y picture.yuv
ffmpeg -i ueitm720.bmp -c:v rawvideo -pix_fmt nv12 -qscale:v 0 -an -y picture.yuv
..\..\tools\bin2c.exe picture.yuv picture.h
..\..\tools\bin2c.exe 1920x1080.png 1920x1080.h
@pause
