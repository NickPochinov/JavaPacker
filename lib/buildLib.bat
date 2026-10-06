@echo off
g++ -c *.cpp
md out
for %%i in (*.o) do ar rcs out/libcjp.a %%i
pause