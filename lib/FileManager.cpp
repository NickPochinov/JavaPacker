#include <io.h>
#include <fstream>
#include <iostream>
#include "CodePage.h"
#include <codecvt>
#include <Shlobj.h>
#include "charset.h"
#include <filesystem>
#include <Windows.h>
#include "temps.h"
#include "FileManager.h"
using namespace std;
namespace fs = std::filesystem;

namespace jp {
	CData FileManager::binaryReadA(const char* path) {
		ifstream stream(path, ios::binary);
		if (!stream.is_open()) {
			return {};
		}
		auto fileSize = fs::file_size(path);
		CData buffer(fileSize);
		stream.read(buffer.data(), fileSize);
		stream.close();
		return buffer;
	}

	CWData FileManager::binaryReadW(const wchar_t* path) {
		wifstream stream(path, ios::binary);
		if (!stream.is_open()) {
			return {};
		}
		auto fileSize = fs::file_size(path);
		CWData buffer(fileSize);
		stream.read(buffer.data(), fileSize);
		stream.close();
		return buffer;
	}

	void FileManager::binaryWriteA(const char* path, const CData data, const bool append) {
		ofstream stream;
		if (append) {
			stream.open(path, ios::binary | ios::app);
			if (!stream.is_open()) {
				return;
			}
		}
		else {
			stream.open(path, ios::binary);
		}
		stream.write(data.data(), data.size());
		stream.close();
	}

	void FileManager::binaryWriteW(const wchar_t* path, const CWData data, const bool append) {
		wofstream stream;
		if (append) {
			stream.open(path, ios::binary | ios::app);
			if (!stream.is_open()) {
				return;
			}
		}
		else {
			stream.open(path, ios::binary);
		}
		stream.write(data.data(), data.size());
		stream.close();
	}

	void FileManager::fileCopyA(const char* oldPath, const char* newPath) {
		auto data = binaryReadA(oldPath);
		binaryWriteA(newPath, data);
	}

	void FileManager::fileCopyW(const wchar_t* oldPath, const wchar_t* newPath) {
		auto data = binaryReadW(oldPath);
		binaryWriteW(newPath, data);
	}
}
