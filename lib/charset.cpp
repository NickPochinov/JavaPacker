#include <vector>
#include <windows.h>
#include "CodePage.h"
#include <typeinfo>
#include <stdlib.h>
#include <cmath>
#include <algorithm>
#include "charset.h"
#include <functional>
using namespace std;

namespace jp {
	namespace text {
		bool basic_charset::equals(const char* left, const char* right) {
			int length = strlen(left);
			int length2 = strlen(right);
			if (length != length2) {
				return false;
			}
			if (strcmp(left, "") == 0 && strcmp(right, "") == 0) {
				return true;
			}
			else if (strcmp(left, "") == 0 || strcmp(right, "") == 0) {
				return false;
			}
			for (int i = 0; i < length; i++) {
				if (left[i] != right[i]) {
					return false;
				}
			}
			return true;
		}
		bool basic_wcharset::equals(const wchar_t* left, const wchar_t* right) {
			int length = wcslen(left);
			int length2 = wcslen(right);
			if (length != length2) {
				return false;
			}
			if (wcscmp(left, L"") == 0 && wcscmp(right, L"") == 0) {
				return true;
			}
			else if (wcscmp(left, L"") == 0 || wcscmp(right, L"") == 0) {
				return false;
			}
			for (int i = 0; i < length; i++) {
				if (left[i] != right[i]) {
					return false;
				}
			}
			return true;
		}
		int basic_wcharset::strlen(const wchar_t* str) {
			int length = 0;
			while (str[length] != L'\0') {
				length++;
			}
			return length;
		}
		int basic_charset::strlen(const char* str) {
			int length = 0;
			while (str[length] != '\0') {
				length++;
			}
			return length;
		}
		bool basic_charset::contains(const char* str, const char* word) {
			int len1 = strlen(str);
			int len2 = strlen(word);
			for (int i = 0; i < len1; i++) {
				if (str[i] == word[0] && i <= len1 - len2) {
					bool cont = true;
					for (int j = 0; j < len2; j++) {
						if (str[i + j] != word[j]) {
							cont = false;
							break;
						}
					}
					if (cont) {
						return true;
					}
				}
			}
			return false;
		}
		bool basic_wcharset::contains(const wchar_t* str, const wchar_t* word) {
			int len1 = wcslen(str);
			int len2 = wcslen(word);
			for (int i = 0; i < len1; i++) {
				if (str[i] == word[0] && i <= len1 - len2) {
					bool cont = true;
					for (int j = 0; j < len2; j++) {
						if (str[i + j] != word[j]) {
							cont = false;
							break;
						}
					}
					if (cont) {
						return true;
					}
				}
			}
			return false;
		}
		vector<const char*> basic_charset::split(const char* str) {
			vector<const char*> result;
			for (int i = 0; i < strlen(str); i++) {
				result.push_back(new const char[1] {str[i]});
			}
			return result;
		}
		vector<const wchar_t*> basic_wcharset::split(const wchar_t* str) {
			vector<const wchar_t*> result;
			for (int i = 0; i < wcslen(str); i++) {
				result.push_back(new const wchar_t[1] {str[i]});
			}
			return result;
		}
		char* basic_charset::cpush(const char* src, char elem) {
			char* new_ch = new char[strlen(src) + 2];
			for (int j = 0; j < strlen(src); j++) {
				*(new_ch + j) = *(src + j);
			}
			*(new_ch + strlen(src)) = elem;
			*(new_ch + strlen(src) + 1) = '\0';
			return new_ch;
		}
		char* basic_charset::strpush(const char* src, const char* dst) {
			char* new_ch = new char[strlen(src) + strlen(dst) + 1];
			for (int j = 0; j < strlen(src); j++) {
				*(new_ch + j) = *(src + j);
			}
			for (int j = 0; j < strlen(dst); j++) {
				*(new_ch + strlen(src) + j) = *(dst + j);
			}
			*(new_ch + strlen(src) + strlen(dst)) = '\0';
			return new_ch;
		}
		wchar_t* basic_wcharset::cpush(const wchar_t* src, wchar_t elem) {
			wchar_t* new_ch = new wchar_t[wcslen(src) + 2];
			for (int j = 0; j < wcslen(src); j++) {
				*(new_ch + j) = *(src + j);
			}
			*(new_ch + wcslen(src)) = elem;
			*(new_ch + wcslen(src) + 1) = L'\0';
			return new_ch;
		}
		wchar_t* basic_wcharset::strpush(const wchar_t* src, const wchar_t* dst) {
			wchar_t* new_ch = new wchar_t[wcslen(src) + wcslen(dst) + 1];
			for (int j = 0; j < wcslen(src); j++) {
				*(new_ch + j) = *(src + j);
			}
			for (int j = 0; j < wcslen(dst); j++) {
				*(new_ch + wcslen(src) + j) = *(dst + j);
			}
			*(new_ch + wcslen(src) + wcslen(dst)) = L'\0';
			return new_ch;
		}
		void basic_charset::strcopy(char*& src, const char* dst) {
			int length = strlen(dst);
			src = new char[length + 1];
			for (int i = 0; i < length; i++) {
				src[i] = dst[i];
			}
			src[length] = '\0';
		}
		void basic_wcharset::strcopy(wchar_t*& src, const wchar_t* dst) {
			int length = wcslen(dst);
			src = new wchar_t[length + 1];
			for (int i = 0; i < length; i++) {
				src[i] = dst[i];
			}
			src[length] = L'\0';
		}
		vector<const char*> basic_charset::split_of(const char* str, char sep) {
			vector<char*> result;
			char* s = nullptr;
			for (int i = 0; i < strlen(str); i++) {
				if (str[i] != sep) {
					if (!s) {
						s = const_cast<char*>("");
					}
					s = basic_charset::cpush(s, str[i]);
				}
				else if (s) {
					result.push_back(const_cast<char*>(""));
					basic_charset::strcopy(result[result.size() - 1], s);
					free(s);
					s = nullptr;
				}
			}
			if (s) {
				result.push_back(const_cast<char*>(""));
				basic_charset::strcopy(result[result.size() - 1], s);
				free(s);
				s = nullptr;
			}
			vector<const char*> result2;
			for (auto arg : result) {
				result2.push_back(arg);
			}
			return result2;
		}
		vector<const wchar_t*> basic_wcharset::split_of(const wchar_t* str, wchar_t sep) {
			vector<wchar_t*> result;
			wchar_t* s = nullptr;
			for (int i = 0; i < wcslen(str); i++) {
				if (str[i] != sep) {
					if (!s) {
						s = const_cast<wchar_t*>(L"");
					}
					s = basic_wcharset::cpush(s, str[i]);
				}
				else if (s) {
					result.push_back(const_cast<wchar_t*>(L""));
					basic_wcharset::strcopy(result[result.size() - 1], s);
					free(s);
					s = nullptr;
				}
			}
			if (s) {
				result.push_back(const_cast<wchar_t*>(L""));
				basic_wcharset::strcopy(result[result.size() - 1], s);
				free(s);
				s = nullptr;
			}
			vector<const wchar_t*> result2;
			for (auto arg : result) {
				result2.push_back(arg);
			}
			return result2;
		}
		char* basic_charset::join(vector<const char*> list, const char* start = "", const char* end = " ") {
			char* result = const_cast<char*>("");
			for (int i = 0; i < list.size(); i++) {
				result = strpush(result, start);
				result = strpush(result, list[i]);
				result = strpush(result, end);
			}
			return result;
		}
		wchar_t* basic_wcharset::join(vector<const wchar_t*> list, const wchar_t* start, const wchar_t* end) {
			wchar_t* result = const_cast<wchar_t*>(L"");
			for (int i = 0; i < list.size(); i++) {
				result = strpush(result, start);
				result = strpush(result, list[i]);
				if (i != list.size() - 1) {
					result = strpush(result, end);
				}
			}
			return result;
		}
		int basic_charset::hash(const char* str) {
			std::hash<const char*> h = std::hash<const char*>();
			return h(str);
		}
		int basic_wcharset::hash(const wchar_t* str) {
			std::hash<const wchar_t*> h = std::hash<const wchar_t*>();
			return h(str);
		}
		char* basic_charset::remove(const char* str, int index) {
			int length = strlen(str);
			if (index >= 0 && index < length) {
				char* s = new char[length];
				int r = 0;
				for (int i = 0; i < length; i++) {
					if (i != index) {
						s[i - r] = str[i];
					}
					else {
						r = 1;
					}
				}
				s[length - 1] = '\0';
				return s;
			}
			else {
				return const_cast<char*>(str);
			}
		}
		wchar_t* basic_wcharset::remove(const wchar_t* str, int index) {
			int length = wcslen(str);
			if (index >= 0 && index < length) {
				wchar_t* s = new wchar_t[length];
				int r = 0;
				for (int i = 0; i < length; i++) {
					if (i != index) {
						s[i - r] = str[i];
					}
					else {
						r = 1;
					}
				}
				s[length - 1] = L'\0';
				return s;
			}
			else {
				return const_cast<wchar_t*>(str);
			}
		}
		char* basic_charset::trim(const char* str) {
			char* trim_s = const_cast<char*>("");
			basic_charset::strcopy(trim_s, str);
			while (trim_s[0] == ' ') {
				trim_s = basic_charset::remove(trim_s, 0);
			}
			while (trim_s[strlen(trim_s)] == ' ') {
				trim_s = basic_charset::remove(trim_s, strlen(trim_s) - 1);
			}
			return trim_s;
		}
		wchar_t* basic_wcharset::trim(const wchar_t* str) {
			wchar_t* trim_s = const_cast<wchar_t*>(L"");
			basic_wcharset::strcopy(trim_s, str);
			while (trim_s[0] == L' ') {
				trim_s = basic_wcharset::remove(trim_s, 0);
			}
			while (trim_s[strlen(trim_s)] == L' ') {
				trim_s = basic_wcharset::remove(trim_s, strlen(trim_s) - 1);
			}
			return trim_s;
		}
		char* basic_charset::substr(const char* str, int start, int end) {
			int length = strlen(str);
			if (start >= 0 && start < length && end >= 0 && end < length) {
				char* s = new char[end - start + 2];
				for (int i = start; i <= end; i++) {
					s[i - start] = str[i];
				}
				s[end - start + 1] = '\0';
				return s;
			}
			else {
				return const_cast<char*>(str);
			}
		}
		wchar_t* basic_wcharset::substr(const wchar_t* str, int start, int end) {
			int length = wcslen(str);
			if (start >= 0 && start < length && end >= 0 && end < length) {
				wchar_t* s = new wchar_t[end - start + 2];
				for (int i = start; i <= end; i++) {
					s[i - start] = str[i];
				}
				s[end - start + 1] = L'\0';
				return s;
			}
			else {
				return const_cast<wchar_t*>(str);
			}
		}
		bool basic_charset::isnumber(const char* str) {
			int length = strlen(str);
			for (int i = 0; i < length; i++) {
				if (i == 0 && str[i] == '-') {
					continue;
				}
				if (!isdigit(str[i])) {
					return false;
				}
			}
			return true;
		}
		bool basic_wcharset::isnumber(const wchar_t* str) {
			int length = wcslen(str);
			for (int i = 0; i < length; i++) {
				if (i == 0 && str[i] == L'-') {
					continue;
				}
				if (!iswdigit(str[i])) {
					return false;
				}
			}
			return true;
		}
		int basic_charset::count(const char* str, char item) {
			int count = 0;
			int length = strlen(str);
			for (int i = 0; i < length; i++) {
				if (str[i] == item) {
					count++;
				}
			}
			return count;
		}
		int basic_wcharset::count(const wchar_t* str, wchar_t item) {
			int count = 0;
			int length = wcslen(str);
			for (int i = 0; i < length; i++) {
				if (str[i] == item) {
					count++;
				}
			}
			return count;
		}
		int basic_charset::find_first(const char* str, char item) {
			int length = strlen(str);
			int i = 0;
			while (str[i] != item && i < length) {
				i++;
			}
			if (i == length) {
				return -1;
			}
			return i;
		}
		int basic_charset::find_last(const char* str, char item) {
			int length = strlen(str);
			int i = length - 1;
			while (str[i] != item && i >= 0) {
				i--;
			}
			return i;
		}
		int basic_wcharset::find_first(const wchar_t* str, wchar_t item) {
			int length = wcslen(str);
			int i = 0;
			while (str[i] != item && i < length) {
				i++;
			}
			if (i == length) {
				return -1;
			}
			return i;
		}
		int basic_wcharset::find_last(const wchar_t* str, wchar_t item) {
			int length = wcslen(str);
			int i = length - 1;
			while (str[i] != item && i >= 0) {
				i--;
			}
			return i;
		}
		char* basic_charset::to_lower_case(const char* str) {
			int length = strlen(str);
			char* s = new char[length + 1];
			for (int i = 0; i < length; i++) {
				s[i] = tolower(str[i]);
			}
			s[length] = '\0';
			return s;
		}
		char* basic_charset::to_upper_case(const char* str) {
			int length = strlen(str);
			char* s = new char[length + 1];
			for (int i = 0; i < length; i++) {
				s[i] = toupper(str[i]);
			}
			s[length] = '\0';
			return s;
		}
		char* basic_charset::swapcase(const char* str) {
			int length = strlen(str);
			char* s = new char[length + 1];
			for (int i = 0; i < length; i++) {
				if (islower(str[i])) {
					s[i] = toupper(str[i]);
				}
				else {
					s[i] = tolower(str[i]);
				}
			}
			s[length] = '\0';
			return s;
		}
		wchar_t* basic_wcharset::to_lower_case(const wchar_t* str) {
			int length = strlen(str);
			wchar_t* s = new wchar_t[length + 1];
			for (int i = 0; i < length; i++) {
				s[i] = towlower(str[i]);
			}
			s[length] = L'\0';
			return s;
		}
		wchar_t* basic_wcharset::to_upper_case(const wchar_t* str) {
			int length = wcslen(str);
			wchar_t* s = new wchar_t[length + 1];
			for (int i = 0; i < length; i++) {
				s[i] = towupper(str[i]);
			}
			s[length] = L'\0';
			return s;
		}
		wchar_t* basic_wcharset::swapcase(const wchar_t* str) {
			int length = wcslen(str);
			wchar_t* s = new wchar_t[length + 1];
			for (int i = 0; i < length; i++) {
				if (iswlower(str[i])) {
					s[i] = towupper(str[i]);
				}
				else {
					s[i] = towlower(str[i]);
				}
			}
			s[length] = L'\0';
			return s;
		}
		char _numtoch(int val) {
			return (char)((int)'0' + val);
		}
		int get_number_length(long long val) {
			int length = 1;
			get:
			if (val / 10 != 0) {
				val /= 10;
				length++;
				goto get;
			}
			return length;
		}
		int get_length_double(long double value) {
			int length = 0;
			get:
			if ((long long)value != value) {
				length++;
				value *= 10;
				goto get;
			}
			return length;
		}
		char* basic_charset::cadd(const char* str, char item, int index) {
			char* new_ch = new char[strlen(str) + 2];
			for (int j = 0; j < index; j++) {
				*(new_ch + j) = *(str + j);
			}
			*(new_ch + index) = item;
			for (int j = index; j < strlen(str); j++) {
				*(new_ch + j + 1) = *(str + j);
			}
			*(new_ch + strlen(str) + 1) = '\0';
			return new_ch;
		}
		wchar_t* basic_wcharset::cadd(const wchar_t* str, wchar_t item, int index) {
			wchar_t* new_ch = new wchar_t[wcslen(str) + 2];
			for (int j = 0; j < index; j++) {
				*(new_ch + j) = *(str + j);
			}
			*(new_ch + index) = item;
			for (int j = index; j < wcslen(str); j++) {
				*(new_ch + j + 1) = *(str + j);
			}
			*(new_ch + wcslen(str) + 1) = L'\0';
			return new_ch;
		}
		char* basic_charset::stradd(const char* str, const char* item, int index) {
			char* new_ch = new char[strlen(str) + strlen(item) + 1];
			for (int j = 0; j < index; j++) {
				*(new_ch + j) = *(str + j);
			}
			for (int j = 0; j < strlen(item); j++) {
				*(new_ch + j + index) = *(item + j);
			}
			for (int j = index; j < strlen(str); j++) {
				*(new_ch + j + 1 + strlen(item)) = *(str + j);
			}
			*(new_ch + strlen(str) + strlen(item)) = '\0';
			return new_ch;
		}
		wchar_t* basic_wcharset::stradd(const wchar_t* str, const wchar_t* item, int index) {
			wchar_t* new_ch = new wchar_t[strlen(str) + strlen(item) + 1];
			for (int j = 0; j < index; j++) {
				*(new_ch + j) = *(str + j);
			}
			for (int j = 0; j < strlen(item); j++) {
				*(new_ch + j + index) = *(item + j);
			}
			for (int j = index; j < strlen(str); j++) {
				*(new_ch + j + 1 + strlen(item)) = *(str + j);
			}
			*(new_ch + strlen(str) + strlen(item)) = L'\0';
			return new_ch;
		}
		char* num_to_string(long long value) {
			double origin = value;
			value = abs(value);
			int length = get_number_length(value);
			char* result = new char[2] {'n', '\0'};
			for (int i = 0; i < length; i++) {
				if (*result != 'n') {
					result = basic_charset::cadd(result, _numtoch(value % 10), 0);
				}
				else {
					*result = _numtoch(value % 10);
				}
				value /= 10;
			}
			if (origin < 0) {
				result = basic_charset::cadd(result, '-', 0);
			}
			return result;
		}
		char* float_to_string(long double value) {
			double origin = value;
			value = abs(value);
			char* result = new char[2] {'n', '\0'};
			int length = get_number_length(value);
			for (int i = 0; i < get_length_double(value); i++) {
				value *= 10;
				length++;
			}
			int l = get_number_length(value);
			for (int i = 0; i < length; i++) {
				if (i == length - l + 1) {
					result = basic_charset::cadd(result, '.', 0);
				}
				if (*result != 'n') {
					result = basic_charset::cadd(result, _numtoch((int)value % 10), 0);
				}
				else {
					*result = _numtoch((int)value % 10);
				}
				value /= 10;
			}
			if (origin < 0) {
				result = basic_charset::cadd(result, '-', 0);
			}
			return result;
		}
		wchar_t* num_to_wstring(long long value) {
			double origin = value;
			value = abs(value);
			int length = get_number_length(value);
			wchar_t* result = new wchar_t[2] {L'n', L'\0'};
			for (int i = 0; i < length; i++) {
				if (*result != L'n') {
					result = basic_wcharset::cadd(result, _numtoch(value % 10), 0);
				}
				else {
					*result = _numtoch(value % 10);
				}
				value /= 10;
			}
			if (origin < 0) {
				result = basic_wcharset::cadd(result, L'-', 0);
			}
			return result;
		}
		wchar_t* float_to_wstring(long double value) {
			double origin = value;
			value = abs(value);
			wchar_t* result = new wchar_t[2] {L'n', L'\0'};
			int length = get_number_length(value);
			for (int i = 0; i < get_length_double(value); i++) {
				value *= 10;
				length++;
			}
			int l = get_number_length(value);
			for (int i = 0; i < length; i++) {
				if (i == length - l + 1) {
					result = basic_wcharset::cadd(result, L'.', 0);
				}
				if (*result != L'n') {
					result = basic_wcharset::cadd(result, _numtoch((int)value % 10), 0);
				}
				else {
					*result = _numtoch((int)value % 10);
				}
				value /= 10;
			}
			if (origin < 0) {
				result = basic_wcharset::cadd(result, L'-', 0);
			}
			return result;
		}
		int _chtonum(char val) {
			return (int)val - (int)'0';
		}
		long long basic_charset::to_number(const char* str) {
			if (!isnumber(str)) {
				return 0;
			}
			long long result = 0;
			bool otr = false;
			if (str[0] == '-') {
				str = basic_charset::remove(str, 0);
				otr = true;
			}
			for (int i = strlen(str) - 1; i >= 0; i--) {
				long long itog = _chtonum(str[i]);
				itog *= pow(10, strlen(str) - 1 - i);
				result += itog;
			}
			if (otr) {
				result *= -1;
			}
			return result;
		}
		long long basic_wcharset::to_number(const wchar_t* str) {
			if (!isnumber(str)) {
				return 0;
			}
			long long result = 0;
			bool otr = false;
			if (str[0] == L'-') {
				str = basic_wcharset::remove(str, 0);
				otr = true;
			}
			for (int i = strlen(str) - 1; i >= 0; i--) {
				long long itog = _chtonum(str[i]);
				itog *= pow(10, strlen(str) - 1 - i);
				result += itog;
			}
			if (otr) {
				result *= -1;
			}
			return result;
		}
		long double basic_charset::to_float_number(const char* str) {
			long double result = 0;
			bool otr = false;
			if (str[0] == '-') {
				str = basic_charset::remove(str, 0);
				otr = true;
			}
			bool doub = false;
			int index_doub = basic_charset::find_first(str, '.');
			for (int i = 0; i < strlen(str); i++) {
				if (isdigit(str[i])) {
					if (!doub) {
						long long itog = _chtonum(str[i]);
						itog *= pow(10, strlen(str) - 1 - i);
						result += itog;
					}
					else {
						long double itog = _chtonum(str[i]);
						if (itog != 0) {
							itog *= pow(10, index_doub - i);
							result += itog;
						}
					}
				}
				else if (str[i] == '.') {
					if (!doub) {
						doub = true;
					}
					else {
						return 0;
					}
				}
				else {
					return 0;
				}
			}
			if (otr) {
				result *= -1;
			}
			return result;
		}
		long double basic_wcharset::to_float_number(const wchar_t* str) {
			long double result = 0;
			bool otr = false;
			if (str[0] == L'-') {
				str = basic_wcharset::remove(str, 0);
				otr = true;
			}
			bool doub = false;
			int index_doub = basic_wcharset::find_first(str, L'.');
			for (int i = 0; i < strlen(str); i++) {
				if (iswdigit(str[i])) {
					if (!doub) {
						long long itog = _chtonum(str[i]);
						itog *= pow(10, strlen(str) - 1 - i);
						result += itog;
					}
					else {
						long double itog = _chtonum(str[i]);
						if (itog != 0) {
							itog *= pow(10, index_doub - i);
							result += itog;
						}
					}
				}
				else if (str[i] == L'.') {
					if (!doub) {
						doub = true;
					}
					else {
						return 0;
					}
				}
				else {
					return 0;
				}
			}
			if (otr) {
				result *= -1;
			}
			return result;
		}
		wchar_t* basic_charset::convert(const char* str, Cpage page) {
			int encoding = 1;
			if (page == Cpage::UTF8) {
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
		char* basic_wcharset::convert(const wchar_t* str, Cpage page) {
			int encoding = 1;
			if (page == Cpage::UTF8) {
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

		char* basic_charset::convertCP(const char* str, Cpage oldCP, Cpage newCP) {
			return basic_wcharset::convert(convert(str, oldCP), newCP);
		}
	}
}