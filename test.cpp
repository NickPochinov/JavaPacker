#include <windows.h>
#include <iostream>

enum Cpage {CP1251 = 1251, CP1252 = 1252, UTF8 = 8, UTF16 = 16, ASCII = 1, SYS = 0};

wchar_t* convert1(const char* str, Cpage page) {
	int encoding = 1;
	if (page == Cpage::UTF8 || page == Cpage::UTF16) {
		encoding = CP_UTF8;
	}
	else if (page == Cpage::SYS) {
		encoding = CP_ACP;
	}
	else {
		encoding = page;
	}
	if (encoding != 1) {
		int length = MultiByteToWideChar(encoding, 0, str, -1, 0, 0);
		wchar_t* s = new wchar_t[length];
		MultiByteToWideChar(encoding, 0, str, -1, s, length);
		return s;
	}
	else {
		return const_cast<wchar_t*>(reinterpret_cast<const wchar_t*>(str));
	}
}
char* convert2(const wchar_t* str, Cpage page) {
	int encoding = 1;
	if (page == Cpage::UTF8 || page == Cpage::UTF16) {
		encoding = CP_UTF8;
	}
	else if (page == Cpage::SYS) {
		encoding = CP_ACP;
	}
	else {
		encoding = page;
	}
	if (encoding != 1) {
		int length = WideCharToMultiByte(encoding, 0, str, -1, 0, 0, 0, 0);
		char* s = new char[length];
		WideCharToMultiByte(encoding, 0, str, -1, s, length, 0, 0);
		return s;
	}
	else {
		return const_cast<char*>(reinterpret_cast<const char*>(str));
	}
}

int main() {

    bool result = SetConsoleCP(1251);
    std::cout << result << std::endl;
    std::cout << GetConsoleCP() << std::endl;
    std::string utf8 = "Привет";
    std::string ansi = convert2(convert1(utf8.c_str(), UTF8), SYS);
    std::cout << "ANSI bytes: ";
    for (unsigned char c : ansi) {
        printf("%02X ", c);
    }
    std::cout << std::endl;
    return 0;
}