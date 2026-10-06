#pragma once
#include <io.h>
#include <fstream>
#include <iostream>
#include "CodePage.h"
#include <vector>
#include <codecvt>
#include <Shlobj.h>
#include <Windows.h>
#include "temps.h"
using namespace std;

namespace jp {
    class FileManager {
        private: FileManager();

        public: static void binaryWriteW(const wchar_t* path, const jp::CWData data, const bool append = false);

        public: static jp::CWData binaryReadW(const wchar_t* path);

        public: static void binaryWriteA(const char* path, const jp::CData data, const bool append = false);

        public: static jp::CData binaryReadA(const char* path);

        public: static void fileCopyA(const char* oldPath, const char* newPath);

        public: static void fileCopyW(const wchar_t* oldPath, const wchar_t* newPath);
    };
}