del *.o
del *.exe

::pause

gcc -Ofast -DNDEBUG -DUSE_JPWL -fomit-frame-pointer -ftree-vectorize -fno-math-errno -fmax-errors=1 -c rs.c
gcc -Ofast -DNDEBUG -DUSE_JPWL -fomit-frame-pointer -ftree-vectorize -fno-math-errno -fmax-errors=1 -c main.c

gcc -Ofast -o main.exe main.o rs.o -static-libstdc++ -static-libgcc -Wl,--strip-all -Wl,--gc-sections

pause

main.exe

::main.exe > out.txt

::pause

exit
