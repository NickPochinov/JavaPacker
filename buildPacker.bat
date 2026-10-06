@echo off
g++ packer.cpp -o jpacker -finput-charset=UTF-8 -fexec-charset=UTF-8 -static -I"C:\vcpkg\vcpkg\installed\x64-mingw-static\include" -L"C:\vcpkg\vcpkg\installed\x64-mingw-static\lib" lib/out/libcjp.a -lzip -lzs -lbz2 -lbcrypt
pause