#pragma once
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
#include "zipstring.h"
#include "reader.h"

namespace jp {
    namespace zip {

        void extractZip(const jp::text::zipstring& filePath, const std::string& extractPath);

        bool extractResource(const jp::text::zipstring& filePath, const std::string& resource, const std::string& extractPath);

        bool addFile(const jp::text::zipstring& filePath, const jp::RFile& file);

        bool addFiles(const jp::text::zipstring& filePath, const std::vector<jp::RFile>& files);

        bool addDirectory(const jp::text::zipstring& filePath, const std::string& directory);

        bool addDirectories(const jp::text::zipstring& filePath, const std::vector<std::string>& dirs);

        bool removeFile(const jp::text::zipstring& filePath, const std::string& resource);

        bool includeSFX(const std::string& filePath);

        void extractDirectory(const jp::text::zipstring& filePath, const std::string& directory, const std::string& extractPath);

        jp::RFile getResource(const jp::text::zipstring& filePath, const std::string& resource);

        bool removeDirectory(const jp::text::zipstring& filePath, const std::string& directory);

        std::vector<CData> getBuffers(const jp::text::zipstring& filePath);

        std::vector<std::string> getSubDirectories(const jp::text::zipstring& filePath, const std::string& directory);

        std::vector<jp::RFile> getSubFiles(const jp::text::zipstring& filePath, const std::string& directory);
    }
}
