#pragma once
#include <vector>
#include "charset.h"
#include "reader.h"
#include <string>

namespace jp {
    typedef std::vector<char> CData;
    typedef std::vector<wchar_t> CWData;

    class RData {
        private: RData() = default;

        public: static CData parse(const char* arg) {
            CData data;
            for (size_t i = 0; i < strlen(arg); i++) {
                char c = arg[i];
                data.push_back(c);
            }
            return data;
        }

        public: static CWData parse(const wchar_t* arg) {
            CWData data;
            for (size_t i = 0; i < wcslen(arg); i++) {
                wchar_t c = arg[i];
                data.push_back(c);
            }
            return data;
        }
    };

    class RFile {
        private: std::string path = "";
        private: CData content;
        public: RFile(const std::string& path, const CData& content) {
            this->path = path;
            this->content = content;
        }

        public: RFile() = default;

        public: std::string getPath() const {
            return path;
        }

        public: CData getContent() const {
            return content;
        }

        public: ~RFile() {
        }

        public: RFile(const RFile& c) {
            this->path = c.path;
            this->content = c.content;
        }

        public: RFile(const RFile&& c) {
            this->path = c.path;
            this->content = c.content;
        }

        public: RFile operator=(const RFile& c) {
            RFile file = RFile();
            file.path = c.path;
            file.content = c.content;
            return file;
        }

        public: bool isEmpty() const {
            return content.empty() && path.empty();
        }
    };

    namespace zip {
        const int ZIP_SIGNATURE = 0x04034b50;
    }
}