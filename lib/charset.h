#pragma once
#include <vector>
#include <windows.h>
#include "CodePage.h"
#include <typeinfo>

namespace jp {
	namespace text {
		char _numtoch(int val);

		int _chtonum(char val);

		int get_number_length(long long val);

		int get_length_double(long double value);

		char* num_to_string(long long value);

		char* float_to_string(long double value);

		wchar_t* num_to_wstring(long long value);

		wchar_t* float_to_wstring(long double value);

		class basic_charset {
		private: basic_charset();

		public: static char* cpush(const char* src, char elem);

		public: static char* strpush(const char* src, const char* dst);

		public: static void strcopy(char*& src, const char* dst);

		public: static int strlen(const char* str);

		public: static bool equals(const char* left, const char* right);

		public: static bool contains(const char* str, const char* word);

		public: static std::vector<const char*> split(const char* str);

		public: static std::vector<const char*> split_of(const char* str, char sep = ' ');

		public: static int hash(const char* str);

		public: static char* trim(const char* str);

		public: static char* remove(const char* str, int index);

		public: static char* substr(const char* str, int start, int end);

		public: static char* to_lower_case(const char* str);

		public: static char* to_upper_case(const char* str);

		public: static char* swapcase(const char* str);

		public: static int find_first(const char* str, char item);

		public: static int find_last(const char* str, char item);

		public: static int count(const char* str, char item);

		public: static bool isnumber(const char* str);

		public: static char* cadd(const char* str, char item, int index);

		public: static char* stradd(const char* str, const char* item, int index);

		public: static wchar_t* convert(const char* str, Cpage page);

		public: static char* convertCP(const char* str, Cpage oldCP, Cpage newCP);

		public: template <typename _Type> static char* to_string(_Type val) {
			if (typeid(_Type) == typeid(int) || typeid(_Type) == typeid(long) || typeid(_Type) == typeid(long long)) {
				return num_to_string(static_cast<long long>(val));
			}
			else if (typeid(_Type) == typeid(char)) {
				return basic_charset::cpush("", (char)val);
			}
			else if (typeid(_Type) == typeid(double) || typeid(_Type) == typeid(long double) || typeid(_Type) == typeid(float)) {
				return float_to_string(static_cast<long double>(val));
			}
			else {
				return const_cast<char*>(typeid(_Type).name());
			}
		}

		public: static long long to_number(const char* str);

		public: static long double to_float_number(const char* str);

		public: static char* join(std::vector<const char*> list, const char* start, const char* end);
		};

		class basic_wcharset {
		private: basic_wcharset();

		public: static wchar_t* cpush(const wchar_t* src, wchar_t elem);

		public: static wchar_t* strpush(const wchar_t* src, const wchar_t* dst);

		public: static void strcopy(wchar_t*& src, const wchar_t* dst);

		public: static int strlen(const wchar_t* str);

		public: static bool equals(const wchar_t* left, const wchar_t* right);

		public: static bool contains(const wchar_t* str, const wchar_t* word);

		public: static std::vector<const wchar_t*> split(const wchar_t* str);

		public: static std::vector<const wchar_t*> split_of(const wchar_t* str, wchar_t sep = L' ');

		public: static int hash(const wchar_t* str);

		public: static wchar_t* trim(const wchar_t* str);

		public: static wchar_t* remove(const wchar_t* str, int index);

		public: static wchar_t* substr(const wchar_t* str, int start, int end);

		public: static wchar_t* to_lower_case(const wchar_t* str);

		public: static wchar_t* to_upper_case(const wchar_t* str);

		public: static wchar_t* swapcase(const wchar_t* str);

		public: static int find_first(const wchar_t* str, wchar_t item);

		public: static int find_last(const wchar_t* str, wchar_t item);

		public: static int count(const wchar_t* str, wchar_t item);

		public: static bool isnumber(const wchar_t* str);

		public: static wchar_t* cadd(const wchar_t* str, wchar_t item, int index);

		public: static wchar_t* stradd(const wchar_t* str, const wchar_t* item, int index);

		public: static char* convert(const wchar_t* str, Cpage page);

		public: template <typename _Type> static wchar_t* to_string(_Type val) {
			if (typeid(_Type) == typeid(int) || typeid(_Type) == typeid(long) || typeid(_Type) == typeid(long long)) {
				return num_to_wstring(static_cast<long long>(val));
			}
			else if (typeid(_Type) == typeid(wchar_t)) {
				return basic_wcharset::cpush(L"", (wchar_t)val);
			}
			else if (typeid(_Type) == typeid(double) || typeid(_Type) == typeid(long double) || typeid(_Type) == typeid(float)) {
				return float_to_wstring(static_cast<long double>(val));
			}
			else {
				return basic_charset::convert(typeid(_Type).name(), ASCII);
			}
		}

		public: static long long to_number(const wchar_t* str);

		public: static long double to_float_number(const wchar_t* str);

		public: static wchar_t* join(std::vector<const wchar_t*> list, const wchar_t* start, const wchar_t* end);
		};
	}
}