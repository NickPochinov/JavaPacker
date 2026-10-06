#pragma once
#include "CodePage.h"
#include "anystring.h"
#include "charset.h"
#include "zip.h"
#include "sfxdata.h"
#include "Text.h"
#include "zipcontainer.h"

namespace jp {
    namespace text {
        class zipstring : public anystring<zip::ZipContainer> {
            public: zip::ZipContainer toObject() const override {
                if (haveObject) {
                    return object;
                }
                return zip::ZipContainer(text);
            }

            public: Text toText() const override {
                if (haveText) {
                    return text;
                }
                else {
                    return object.getPath();
                }
            }

            public: std::streampos getOffset() const {
                return toObject().getOffset();
            }

            public: bool isInit(const int flags = ZIP_RDONLY) const {
                return toObject().getZip(flags);
            }

            public: zipstring(const zip::ZipContainer& object) {
                this->object = object;
                haveObject = true;
            }

            public: zipstring(const Text& text) {
                this->text = text;
                haveText = true;
            }

            public: zipstring(const std::string& iter, const Cpage page = UTF8) {
                text = {iter, page};
                haveText = true;
            }

            public: zipstring(const std::wstring& iter, const Cpage page = UTF8) {
                text = {iter, page};
                haveText = true;
            }

            public: zipstring(const zipstring& c) {
                if (c.haveObject) {
                    this->object = c.object;
                }
                else if (c.haveText) {
                    this->text = c.text;
                }
                haveObject = c.haveObject;
                haveText = c.haveText;
            }

            public: zipstring& operator=(const zipstring& c) {
                if (c.haveObject) {
                    this->object = c.object;
                }
                else if (c.haveText) {
                    this->text = c.text;
                }
                haveObject = c.haveObject;
                haveText = c.haveText;
                return *this;
            }

            public: ~zipstring() {
            }

            public: void operator+=(const zipstring& object) {
                this->object = toText() + object.toText();
                haveObject = true;
                haveText = false;
            }

            public: zipstring operator+(const zipstring& object) const {
                zipstring copy = *this;
                copy += object;
                return copy;
            }
        };
    }
}