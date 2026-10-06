@echo off
g++ add.cpp -o add -static -finput-charset=UTF-8 -fexec-charset=UTF-8 -I"C:\vcpkg\vcpkg\installed\x64-mingw-static\include" -L"C:\vcpkg\vcpkg\installed\x64-mingw-static\lib" lib/out/libcjp.a -lzip -lzs -lbz2 -lbcrypt
pause