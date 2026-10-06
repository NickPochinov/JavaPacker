#include <string>
#include <zip.h>
#include <filesystem>
#include <vector>
#include "FileManager.h"
#include "temps.h"
#include <functional>
#include <fstream>
#include "sfxdata.h"
#include "charset.h"
#include "reader.h"
namespace jp {
    namespace zip {
        std::streampos findZipSignature(const std::string filePath) {
            std::ifstream file(filePath, std::ios::binary);
            const size_t bufferSize = 1024 * 1024;
            jp::CData buffer(bufferSize);
            std::streampos currentPos = 0;
            while (file.read(buffer.data(), bufferSize) || file.gcount() > 0) {
                size_t bytesRead = file.gcount();
                for (size_t i = 0; i < bytesRead - 3; i++) {
                    if (*(uint32_t*)(buffer.data() + i) == jp::zip::ZIP_SIGNATURE) {
                        auto pos = currentPos + static_cast<std::streampos>(i);
                        zip_error_t error;
                        zip_source_t* src;
                    #if defined(_WIN32) || defined(_WIN64)
                        src = zip_source_win32a_create(filePath.c_str(), pos, -1, &error);
                    #else
                        src = zip_source_file_create(filePath.c_str(), pos, -1, &error);
                    #endif
                        zip_t* archive = zip_open_from_source(src, 0, &error);
                        if (!archive) {
                            continue;
                        }
                        else {
                            zip_int64_t entries = zip_get_num_entries(archive, 0);
                            if (entries == -1) {
                                zip_close(archive);
                                continue;
                            }
                        }
                        zip_close(archive);
                        return pos;
                    }
                }
                currentPos += static_cast<std::streampos>(bytesRead);
            }
            return -1;
        }

        #if defined(_WIN32) || defined(_WIN64)
        #include <windows.h>
        zip_t* getExecutable() {
            zip_error_t error;
            char path[MAX_PATH];
            GetModuleFileNameA(NULL, path, MAX_PATH);
            int err = 0;
            zip_source_t* src = zip_source_win32a_create(path, findZipSignature(path), -1, &error);
            auto zip = zip_open_from_source(src, ZIP_RDONLY, &error);
            return zip;
        }
        #elif defined(__APPLE__)
        #include <mach-o/dyld.h>
        zip_t* getExecutable() {
            char path[PATH_MAX];
            uint32_t size = sizeof(path);
            if (_NSGetExecutablePath(path, &size) == 0)
                return nullptr;
            else {
                zip_source_t* src = zip_source_file_create(path, findZipSignature(path), -1, &error);
                return zip_open_from_source(src, ZIP_RDONLY, &error);
            }
        }
        #else
        zip_t* getExecutable() {
            fs::path path = fs::canonical("/proc/self/exe");
            int err = 0;
            zip_error_t error;
            zip_source_t* src = zip_source_file_create(path, findZipSignature(path), -1, &error);
            return zip_open_from_source(src, ZIP_RDONLY, &error);
        }
        #endif
    }
}