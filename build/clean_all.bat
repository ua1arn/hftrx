@echo off

for /D %%d in (*) do (
    cd %%d
    make clean
    cd ..
)
