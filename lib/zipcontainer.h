#pragma once
#include <zip.h>
#include "sfxdata.h"
#include "charset.h"
#include <fstream>
#include <string>
#include <filesystem>
#include "Text.h"
namespace fs = std::filesystem;

namespace jp {
    namespace zip {
        class ZipContainer {
            private: text::Text filePath = {""};

            public: ZipContainer() = default;

            public: ZipContainer(const text::Text& filePath) {
                this->filePath = filePath;
            }

            public: ZipContainer(const ZipContainer& c) {
                filePath = c.filePath;
            }

            public: ZipContainer& operator=(const ZipContainer& c) {
                filePath = c.filePath;
                return *this;
            }

            public: ZipContainer(ZipContainer&& c) {
                filePath = c.filePath;
            }

            public: ZipContainer& operator=(ZipContainer&& c) {
                filePath = c.filePath;
                return *this;
            }

            public: ~ZipContainer() {
            }

            public: text::Text getPath() const {
                return filePath;
            }

            public: zip_t* getZip(const int flags = ZIP_RDONLY) const {
                if (filePath.string().empty()) {
                    return zip::getExecutable();
                }
                std::string path = filePath;
            #if defined(_WIN32) || defined(_WIN64)
                path = text::basic_wcharset::convert(text::basic_charset::convert(path.c_str(), UTF8), SYS);
            #endif
                std::streampos pos = zip::findZipSignature(path);
                zip_source_t* src;
                zip_error_t error;
                #if defined(_WIN32) || defined(_WIN64)
                src = zip_source_win32a_create(path.c_str(), pos, -1, &error);
                #else
                src = zip_source_file_create(path.c_str(), pos, -1, &error);
                #endif
                if (!src) {
                    return nullptr;
                }
                zip_t* archive = zip_open_from_source(src, flags, &error);
                if (!archive) {
                    return nullptr;
                }
                return archive;
            }

            public: std::streampos getOffset() const {
                return zip::findZipSignature(filePath);
            }

            public: operator std::string() const {
                return getPath();
            }

            public: operator zip_t*() const {
                return getZip();
            }
        };
    }
}