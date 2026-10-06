#pragma once
#include <string>
#include <filesystem>
#include "CodePage.h"
#include "charset.h"
namespace fs = std::filesystem;
namespace jp {
    namespace text {
        class Text {
            private: std::string text;
            private: std::wstring wtext;
            private: Cpage encoding = UTF8;
            public: Text() = default;

            public: Text(const std::string& iter, const Cpage page = UTF8) {
                text = iter;
                wtext = basic_charset::convert(iter.c_str(), UTF8);
                encoding = page;
            }

            public: Text(const std::wstring& iter, const Cpage page = UTF8) {
                text = basic_wcharset::convert(iter.c_str(), page);
                wtext = iter;
                encoding = page;
            }

            public: Text(const Cpage page) {
                encoding = page;
            }

            public: Text(const Text& c) {
                this->text = c.text;
                this->encoding = c.encoding;
            }

            public: Text operator=(const Text& c) {
                this->text = c.text;
                this->encoding = c.encoding;
                return *this;
            }

            public: Text operator=(const Cpage page) {
                this->encoding = page;
                return *this;
            }

            public: int size() const {
                return text.size();
            }

            public: Text operator+(const Text& right) const {
                std::string s = right.string();
                Text text = Text(string() + basic_charset::convertCP(s.c_str(), right.encoding, this->encoding), this->encoding);
                return text;
            }

            public: void operator+=(const Text& right) {
                std::string s = right.string();
                this->text = string() + basic_charset::convertCP(s.c_str(), right.encoding, this->encoding);
                this->wtext = basic_charset::convert(this->text.c_str(), UTF8);
            }

            public: Text operator/(const Text& right) const {
                Text text = *this;
                std::string t = string();
                #if defined(_WIN32) || defined(_WIN64)
                t += '\\';
                #else
                t += '/';
                #endif
                t += right;
                text.text = t;
                return text;
            }

            public: void operator/=(const Text& right) {
                std::string t = string();
                #if defined(_WIN32) || defined(_WIN64)
                t += '\\';
                #else
                t += '/';
                #endif
                t += right;
                text = t;
            }

            public: bool operator==(const Text& right) const {
                return text == right.text && encoding == right.encoding;
            }

            public: bool operator!=(const Text& right) const {
                return text != right.text || encoding != right.encoding;
            }

            public: void pop_back() {
                this->operator--();
            }

            public: Text subtext(int start, int end = -1) {
                if (end == -1) {
                    return {text.substr(start), encoding};
                }
                else {
                    return {text.substr(start, end - start), encoding};
                }
            }

            public: void operator--() {
                text.pop_back();
                wtext = basic_charset::convert(text.c_str(), UTF8);
            }
            
            public: void clear() {
                text.clear();
                wtext.clear();
            }

            public: bool empty() const {
                return text.empty();
            }

            public: void operator--(int) {
                if (!text.empty()) {
                    text.pop_back();
                    wtext = basic_charset::convert(text.c_str(), UTF8);
                }
            }

            public: std::string string() const {
                return std::string(text);
            }

            public: std::wstring wstring() const {
                return std::wstring(wtext);
            }

            public: fs::path path() const {
                return fs::path(basic_charset::convertCP(text.c_str(), encoding, UTF8));
            }

            public: Cpage page() const {
                return encoding;
            }

            public: operator fs::path() const {
                return path();
            }

            public: operator std::string() const {
                return string();
            }

            public: operator std::wstring() const {
                return wstring();
            }

            public: std::string::const_iterator begin() const {
                return text.begin();
            }

            public: std::string::const_iterator end() const {
                return text.end();
            }

            public: Text convert(const Cpage page) const {
                return {basic_charset::convertCP(text.c_str(), encoding, page), page};
            }

        };
    }
}