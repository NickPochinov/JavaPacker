#pragma once
#include "charset.h"
#include "iostream"
#include <string>
#include "Text.h"

namespace jp {
    namespace text {
        template <class _Type>
        class anystring {

            protected: _Type object;

            protected: Text text;

            protected: bool haveObject = false;

            protected: bool haveText = false;

            public: virtual _Type toObject() const = 0;

            public: virtual Text toText() const = 0;


            public: operator Text() const {
                return toText();
            }

            public: operator _Type() const {
                return toObject();
            }
        };
    }
}