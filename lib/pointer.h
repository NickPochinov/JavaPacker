#pragma once
#include "Text.h"
namespace jp {

    namespace text {
        class pointer {
            private: Text text;

            public: pointer() = default;

            public: pointer(const Text& text) {
                this->text = text.page();
                for (char c : text) {
                    if (isdigit(static_cast<unsigned char>(c)) || c == '-') {
                        this->text += std::string(new char[]{c, '\0'});
                    }
                    else if (c == '.' || c == ',') {
                        this->text += std::string(".");
                    }
                }
            }

            public: pointer(const pointer& p) {
                text = p.text;
            }

            public: pointer operator=(const pointer& p) {
                this->text = p.text;
                return *this;
            }

            public: Text toText() {
                return Text(text);
            }

            public: int count() const {
                int count = 1;
                for (char c : text) {
                    if (c == '.') {
                        count++;
                    }
                }
                return count;
            }

            public: bool isUnsigned() const {
                if (text.empty()) {
                    return true;
                }
                return text.string()[0] != '-';
            }

            public: int number(int pos) const {
                std::string value = "";
                int point = 0;
                for (char c : text) {
                    if (c == '.') {
                        point++;
                        continue;
                    }
                    else if (point == pos) {
                        value += c;
                    }
                    else if (point > pos) {
                        break;
                    }
                }
                if (value.empty()) {
                    return 0;
                }
                return basic_charset::to_number(value.c_str());
            }

            public: bool operator==(const pointer& p) const {
                return text == p.text;
            }

            public: bool operator!=(const pointer& p) const {
                return text != p.text;
            }

            public: bool operator>(const pointer& p) const {
                return operator-(p).isUnsigned();
            }

            public: bool operator<(const pointer& p) const {
                return !operator-(p).isUnsigned();
            }

            public: bool operator>=(const pointer& p) const {
                return *this > p || *this == p;
            }

            public: bool operator<=(const pointer& p) const {
                return *this < p || *this == p;
            }

            public: pointer operator-(const pointer& p) const {
                int leftc = count();
                int rightc = p.count();
                pointer result;
                for (size_t i = 0; i < (rightc > leftc ? rightc : leftc); i++) {
                    result.text += std::to_string(number(i) - p.number(i));
                    result.text += std::string(".");
                }
                result.text--;
                return result;
            }

            public: pointer operator+(const pointer& p) const {
                int leftc = count();
                int rightc = p.count();
                pointer result;
                for (size_t i = 0; i < (rightc > leftc ? rightc : leftc); i++) {
                    result.text += std::to_string(number(i) + p.number(i));
                    result.text += std::string(".");
                }
                result.text--;
                return result;
            }

            public: void operator-=(const pointer& p) {
                int leftc = count();
                int rightc = p.count();
                for (size_t i = 0; i < (rightc > leftc ? rightc : leftc); i++) {
                    this->text += std::to_string(number(i) - p.number(i));
                    this->text += std::string(".");
                }
                this->text--;
            }

            public: void operator+=(const pointer& p) {
                int leftc = count();
                int rightc = p.count();
                for (size_t i = 0; i < (rightc > leftc ? rightc : leftc); i++) {
                    this->text += std::to_string(number(i) + p.number(i));
                    this->text += std::string(".");
                }
                this->text--;
            }
        };
    }
}