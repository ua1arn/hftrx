@rem see https://www.gyan.dev/ffmpeg/builds/ for binaries
ffmpeg -i picture.png -c:v rawvideo -pix_fmt nv12 -qscale:v 0 -an -y picture.yuv
ffmpeg -i red_on_white.png -c:v rawvideo -pix_fmt nv12 -qscale:v 0 -an -y picture.yuv
bin2c.exe picture.yuv picture.h
