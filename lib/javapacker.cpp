#include <string>
#include <zip.h>
#include <filesystem>
#include <vector>
#include "FileManager.h"
#include "Text.h"
#include "temps.h"
#include <functional>
#include <fstream>
#include "charset.h"
#include "reader.h"
#include "zipconf.h"
#include "zipstring.h"
#include "sfxdata.h"
#include "javapacker.h"
namespace fs = std::filesystem;

namespace jp {
    namespace zip {
        std::vector<CData> getBuffers(const jp::text::zipstring& filePath) {
            if (!filePath.isInit()) {
                return {};
            }
            std::vector<CData> buffers;
            zip_t* archive = filePath.toObject();
            zip_int64_t numEntries = zip_get_num_entries(archive, 0);
            for (zip_int64_t i = 0; i < numEntries; i++) {
                zip_stat_t stat;
                if (zip_stat_index(archive, i, 0, &stat) != 0) {
                    buffers.push_back({});
                    continue;
                }
                std::string name = stat.name;
                if (name.back() == '/') {
                    buffers.push_back({});
                    continue;
                }
                zip_file_t* fileInZip = zip_fopen_index(archive, i, 0);
                if (!fileInZip) {
                    buffers.push_back({});
                    continue;
                }
                buffers.emplace_back(stat.size);
                jp::CData& buffer = buffers.back();
                if (zip_fread(fileInZip, buffer.data(), stat.size) != stat.size) {
                    buffer.clear();
                }
                zip_fclose(fileInZip);
            }
            zip_close(archive);
            return buffers;
        }

        std::vector<std::string> getSubDirectories(const jp::text::zipstring& filePath, const std::string& directory) {
            if (!filePath.isInit()) {
                return {};
            }
            std::string copyDirectory = directory;
            if (copyDirectory.back() != '/') {
                copyDirectory += '/';
            }
            std::vector<std::string> result;
            zip_t* archive = filePath.toObject();
            zip_int64_t numEntries = zip_get_num_entries(archive, 0);
            for (int i = 0; i < numEntries; i++) {
                zip_stat_t stat;
                if (zip_stat_index(archive, i, 0, &stat) != 0) {
                    continue;
                }
                std::string entryName = stat.name;
                if (entryName.back() == '/') {
                    if (copyDirectory.size() < entryName.size()) {
                        if (entryName.substr(0, copyDirectory.size()) == copyDirectory) {
                            entryName.pop_back();
                            result.push_back(entryName);
                        }
                    }
                }
            }
            return result;
        }

        std::vector<jp::RFile> getSubFiles(const jp::text::zipstring& filePath, const std::string& directory) {
            if (!filePath.isInit()) {
                return {};
            }
            std::vector<jp::RFile> result;
            std::vector<CData> buffers = getBuffers(filePath);
            std::string copyDirectory = directory;
            if (copyDirectory.back() != '/') {
                copyDirectory += '/';
            }
            zip_t* archive = filePath.toObject();
            zip_int64_t numEntries = zip_get_num_entries(archive, 0);
            for (int i = 0; i < numEntries; i++) {
                zip_stat_t stat;
                if (zip_stat_index(archive, i, 0, &stat) != 0) {
                    continue;
                }
                std::string entryName = stat.name;
                if (entryName.back() != '/') {
                    if (copyDirectory.size() < entryName.size()) {
                        if (entryName.substr(0, copyDirectory.size()) == copyDirectory) {
                            entryName.pop_back();
                            result.push_back(RFile(entryName, buffers[i]));
                        }
                    }
                }
            }
            return result;
        }

        void extractZip(const jp::text::zipstring& filePath, const std::string& extractPath) {
            if (!filePath.isInit()) {
                return;
            }
            zip_t* archive = filePath.toObject();
            zip_int64_t numEntries = zip_get_num_entries(archive, 0);
            fs::create_directories(extractPath);
            for (zip_int64_t i = 0; i < numEntries; i++) {
                zip_stat_t stat;
                if (zip_stat_index(archive, i, 0, &stat) != 0) {
                    continue;
                }
                std::string entryName = stat.name;
                fs::path fullPath = fs::absolute(extractPath) / entryName;
                if (entryName.back() == '/') {
                    fs::create_directories(fullPath);
                    continue;
                }
                fs::create_directories(fullPath.parent_path());
                zip_file_t* fileInZip = zip_fopen_index(archive, i, 0);
                if (!fileInZip) {
                    continue;
                }
                std::ofstream outFile(fullPath, std::ios::binary);
                if (!outFile.is_open()) {
                    zip_fclose(fileInZip);
                    continue;
                }
                jp::CData buffer(stat.size);
                zip_fread(fileInZip, buffer.data(), stat.size);
                outFile.write(buffer.data(), stat.size);
                outFile.close();
                zip_fclose(fileInZip);
            }

            zip_close(archive);
        }

        bool extractResource(const jp::text::zipstring& filePath, const std::string& resource, const std::string& extractPath) {
            if (!filePath.isInit()) {
                return false;
            }
            zip_t* archive = filePath.toObject();
            fs::create_directories(fs::absolute(extractPath).parent_path());
            zip_int64_t index = zip_name_locate(archive, resource.c_str(), 0);
            if (index >= 0) {
                zip_stat_t stat;
                if (zip_stat_index(archive, index, 0, &stat) != 0) {
                    zip_close(archive);
                    return false;
                }
                zip_file_t* fileInZip = zip_fopen_index(archive, index, 0);
                std::ofstream outFile(jp::text::Text(extractPath).convert(SYS), std::ios::binary);
                if (!outFile.is_open()) {
                    zip_fclose(fileInZip);
                    zip_close(archive);
                    return false;
                }
                jp::CData buffer(stat.size);
                zip_fread(fileInZip, buffer.data(), stat.size);
                outFile.write(buffer.data(), stat.size);
                outFile.close();
                zip_fclose(fileInZip);
                zip_close(archive);
                return true;
            }
            zip_close(archive);
            return false;
        }

        void extractDirectory(const jp::text::zipstring& filePath, const std::string& directory, const std::string& extractPath) {
            if (!filePath.isInit()) {
                return;
            }
            std::string copyDirectory = directory;
            if (copyDirectory.back() != '/') {
                copyDirectory += '/';
            }
            zip_t* archive = filePath.toObject();
            fs::create_directories(fs::absolute(extractPath));
            zip_uint64_t entries = zip_get_num_entries(archive, 0);
            for (int i = 0; i < entries; i++) {
                zip_stat_t stat;
                if (zip_stat_index(archive, i, 0, &stat) != 0) {
                    continue;
                }
                std::string name = stat.name;
                if (name.find(copyDirectory) != 0) {
                    continue;
                }
                std::string entryName = name.substr(copyDirectory.size());
                fs::path fullPath = fs::absolute(extractPath) / entryName;
                if (entryName.back() == '/') {
                    fs::create_directories(fullPath);
                    continue;
                }
                zip_file_t* fileInZip = zip_fopen_index(archive, i, 0);
                if (!fileInZip) {
                    continue;
                }
                CData buffer(stat.size);
                zip_fread(fileInZip, buffer.data(), buffer.size());
                ofstream outFile(jp::text::Text(fullPath).convert(SYS), std::ios::binary);
                outFile.write(buffer.data(), buffer.size());
                outFile.close();
                zip_fclose(fileInZip);
            }
            zip_close(archive);
        }

        bool addFile(const jp::text::zipstring& filePath, const jp::RFile& file) {
            auto buffers = getBuffers(filePath);
            if (!filePath.isInit()) {
                return false;
            }
            zip_t* oldArchive = filePath.toObject();
            jp::text::zipstring s = (filePath + text::Text(".tmp"));
            std::string tmpPath = s.toText().string();
            int err = 0;
            zip_t* newArchive = zip_open(tmpPath.c_str(), ZIP_CREATE | ZIP_TRUNCATE, &err);
            if (!newArchive) {
                zip_close(oldArchive);
                return false;
            }
            zip_int64_t count = zip_get_num_entries(oldArchive, 0);
            for (zip_int64_t i = 0; i < count; i++) {
                std::string name = zip_get_name(oldArchive, i, 0);
                if (name.back() == '/') {
                    zip_dir_add(newArchive, name.c_str(), 0);
                    continue;
                }
                jp::CData& buffer = buffers[i];
                zip_source_t* src = zip_source_buffer(newArchive, buffer.data(), buffer.size(), 0);
                zip_file_add(newArchive, name.c_str(), src, ZIP_FL_OVERWRITE);
            }
            jp::CData content = file.getContent();
            zip_source_t* src = zip_source_buffer(newArchive, content.data(), content.size(), 0);
            if (!src) {
                zip_close(newArchive);
                zip_close(oldArchive);
                remove(tmpPath.c_str());
                return false;
            }
            zip_file_add(newArchive, file.getPath().c_str(), src, ZIP_FL_OVERWRITE);
            zip_close(newArchive);
            zip_close(oldArchive);
            std::ifstream oldFile(filePath.toText().string(), std::ios::binary);
            if (!oldFile) {
                remove(tmpPath.c_str());
                return false;
            }
            jp::CData stub((size_t)filePath.getOffset());
            oldFile.read(stub.data(), stub.size());
            oldFile.close();
            remove(filePath.toText().string().c_str());
            std::ofstream newFile(filePath.toText().string(), std::ios::binary);
            if (!newFile) {
                remove(tmpPath.c_str());
                return false;
            }
            newFile.write(stub.data(), stub.size());
            std::ifstream tmpArchive(tmpPath, std::ios::binary);
            newFile << tmpArchive.rdbuf();
            tmpArchive.close();
            newFile.close();
            remove(tmpPath.c_str());
            return true;
        }

        bool addFiles(const jp::text::zipstring& filePath, const std::vector<jp::RFile>& files) {
            auto buffers = getBuffers(filePath);
            if (!filePath.isInit()) {
                return false;
            }
            zip_t* oldArchive = filePath.toObject();
            std::string tmpPath = (filePath + text::Text(".tmp")).toText().string();
            int err = 0;
            zip_t* newArchive = zip_open(tmpPath.c_str(), ZIP_CREATE | ZIP_TRUNCATE, &err);
            if (!newArchive) {
                zip_close(oldArchive);
                return false;
            }
            zip_int64_t count = zip_get_num_entries(oldArchive, 0);
            for (zip_int64_t i = 0; i < count; i++) {
                std::string name = zip_get_name(oldArchive, i, 0);
                if (name.back() == '/') {
                    zip_dir_add(newArchive, name.c_str(), 0);
                    continue;
                }
                jp::CData& buffer = buffers[i];
                zip_source_t* src = zip_source_buffer(newArchive, buffer.data(), buffer.size(), 0);
                zip_file_add(newArchive, name.c_str(), src, ZIP_FL_OVERWRITE);
            }
            std::vector<CData> storedBuffers;
            for (jp::RFile file : files) {
                jp::CData content = file.getContent();
                storedBuffers.emplace_back(content.size());
                jp::CData& buffer = storedBuffers.back();
                memcpy(buffer.data(), content.data(), content.size());
                zip_source_t* src = zip_source_buffer(newArchive, buffer.data(), buffer.size(), 0);
                if (src) {
                    if (zip_file_add(newArchive, file.getPath().c_str(), src, ZIP_FL_OVERWRITE) < 0) {
                        zip_source_free(src);
                    }
                }
            }
            zip_close(newArchive);
            zip_close(oldArchive);
            std::ifstream oldFile(filePath.toText().string(), std::ios::binary);
            if (!oldFile) {
                remove(tmpPath.c_str());
                return false;
            }
            jp::CData stub((size_t)filePath.getOffset());
            oldFile.read(stub.data(), stub.size());
            oldFile.close();
            remove(filePath.toText().string().c_str());
            std::ofstream newFile((filePath.toText().string()), std::ios::binary);
            if (!newFile) {
                remove(tmpPath.c_str());
                return false;
            }
            newFile.write(stub.data(), stub.size());
            std::ifstream tmpArchive(tmpPath, std::ios::binary);
            newFile << tmpArchive.rdbuf();
            tmpArchive.close();
            newFile.close();
            remove(tmpPath.c_str());
            return true;
        }

        bool addDirectory(const jp::text::zipstring& filePath, const std::string& dir) {
            auto buffers = getBuffers(filePath);
            if (!filePath.isInit()) {
                return false;
            }
            std::string copyDirectory = dir;
            if (copyDirectory.back() != '/') {
                copyDirectory += '/';
            }
            zip_t* oldArchive = filePath.toObject();
            std::string tmpPath = (filePath + text::Text(".tmp")).toText().string();
            int err = 0;
            zip_t* newArchive = zip_open(tmpPath.c_str(), ZIP_CREATE | ZIP_TRUNCATE, &err);
            if (!newArchive) {
                zip_close(oldArchive);
                return false;
            }
            zip_int64_t count = zip_get_num_entries(oldArchive, 0);
            for (zip_int64_t i = 0; i < count; i++) {
                std::string name = zip_get_name(oldArchive, i, 0);
                if (name.back() == '/') {
                    zip_dir_add(newArchive, name.c_str(), 0);
                    continue;
                }
                jp::CData& buffer = buffers[i];
                zip_source_t* src = zip_source_buffer(newArchive, buffer.data(), buffer.size(), 0);
                zip_file_add(newArchive, name.c_str(), src, ZIP_FL_OVERWRITE);
            }
            zip_dir_add(newArchive, copyDirectory.c_str(), 0);
            zip_close(newArchive);
            zip_close(oldArchive);
            std::ifstream oldFile(filePath.toText().string(), std::ios::binary);
            if (!oldFile) {
                remove(tmpPath.c_str());
                return false;
            }
            jp::CData stub((size_t)filePath.getOffset());
            oldFile.read(stub.data(), stub.size());
            oldFile.close();
            remove(filePath.toText().string().c_str());
            std::ofstream newFile((filePath.toText().string()), std::ios::binary);
            if (!newFile) {
                remove(tmpPath.c_str());
                return false;
            }
            newFile.write(stub.data(), stub.size());
            std::ifstream tmpArchive(tmpPath, std::ios::binary);
            newFile << tmpArchive.rdbuf();
            tmpArchive.close();
            newFile.close();
            remove(tmpPath.c_str());
            return true;
        }

        bool removeDirectory(const jp::text::zipstring& filePath, const std::string& dir) {
            auto buffers = getBuffers(filePath);
            if (!filePath.isInit()) {
                return false;
            }
            std::string copyDirectory = dir;
            if (copyDirectory.back() != '/') {
                copyDirectory += '/';
            }
            zip_t* oldArchive = filePath.toObject();
            std::string tmpPath = (filePath + text::Text(".tmp")).toText().string();
            int err = 0;
            zip_t* newArchive = zip_open(tmpPath.c_str(), ZIP_CREATE | ZIP_TRUNCATE, &err);
            if (!newArchive) {
                zip_close(oldArchive);
                return false;
            }
            zip_int64_t count = zip_get_num_entries(oldArchive, 0);
            for (zip_int64_t i = 0; i < count; i++) {
                std::string name = zip_get_name(oldArchive, i, 0);
                if (name.find(copyDirectory) == 0) {
                    continue;
                }
                if (name.back() == '/') {
                    zip_dir_add(newArchive, name.c_str(), 0);
                    continue;
                }
                jp::CData& buffer = buffers[i];
                zip_source_t* src = zip_source_buffer(newArchive, buffer.data(), buffer.size(), 0);
                zip_file_add(newArchive, name.c_str(), src, ZIP_FL_OVERWRITE);
            }
            zip_close(newArchive);
            zip_close(oldArchive);
            std::ifstream oldFile(filePath.toText().string(), std::ios::binary);
            if (!oldFile) {
                remove(tmpPath.c_str());
                return false;
            }
            jp::CData stub((size_t)filePath.getOffset());
            oldFile.read(stub.data(), stub.size());
            oldFile.close();
            remove(filePath.toText().string().c_str());
            std::ofstream newFile((filePath.toText().string()), std::ios::binary);
            if (!newFile) {
                remove(tmpPath.c_str());
                return false;
            }
            newFile.write(stub.data(), stub.size());
            std::ifstream tmpArchive(tmpPath, std::ios::binary);
            newFile << tmpArchive.rdbuf();
            tmpArchive.close();
            newFile.close();
            remove(tmpPath.c_str());
            return true;
        }

        bool addDirectories(const jp::text::zipstring& filePath, const std::vector<std::string>& dirs) {
            auto buffers = getBuffers(filePath);
            if (!filePath.isInit()) {
                return false;
            }
            std::vector<std::string> copyDirectories;
            for (std::string directory : dirs) {
                std::string copyDirectory = directory;
                if (copyDirectory.back() != '/') {
                    copyDirectory += '/';
                }
                copyDirectories.push_back(copyDirectory);
            }
            zip_t* oldArchive = filePath.toObject();
            std::string tmpPath = (filePath + text::Text(".tmp")).toText().string();
            int err = 0;
            zip_t* newArchive = zip_open(tmpPath.c_str(), ZIP_CREATE | ZIP_TRUNCATE, &err);
            if (!newArchive) {
                zip_close(oldArchive);
                return false;
            }
            zip_int64_t count = zip_get_num_entries(oldArchive, 0);
            for (zip_int64_t i = 0; i < count; i++) {
                std::string name = zip_get_name(oldArchive, i, 0);
                if (name.back() == '/') {
                    zip_dir_add(newArchive, name.c_str(), 0);
                    continue;
                }
                jp::CData& buffer = buffers[i];
                zip_source_t* src = zip_source_buffer(newArchive, buffer.data(), buffer.size(), 0);
                zip_file_add(newArchive, name.c_str(), src, ZIP_FL_OVERWRITE);
            }
            for (auto dir : copyDirectories) {
                zip_dir_add(newArchive, dir.c_str(), 0);
            }
            zip_close(newArchive);
            zip_close(oldArchive);
            std::ifstream oldFile(filePath.toText().string(), std::ios::binary);
            if (!oldFile) {
                remove(tmpPath.c_str());
                return false;
            }
            jp::CData stub((size_t)filePath.getOffset());
            oldFile.read(stub.data(), stub.size());
            oldFile.close();
            remove(filePath.toText().string().c_str());
            std::ofstream newFile((filePath.toText().string()), std::ios::binary);
            if (!newFile) {
                remove(tmpPath.c_str());
                return false;
            }
            newFile.write(stub.data(), stub.size());
            std::ifstream tmpArchive(tmpPath, std::ios::binary);
            newFile << tmpArchive.rdbuf();
            tmpArchive.close();
            newFile.close();
            remove(tmpPath.c_str());
            return true;
        }

        bool removeFile(const jp::text::zipstring& filePath, const std::string& resource) {
            auto buffers = getBuffers(filePath);
            if (!filePath.isInit()) {
                return false;
            }
            zip_t* oldArchive = filePath.toObject();
            std::string tmpPath = (filePath + text::Text(".tmp")).toText().string();
            int err = 0;
            zip_t* newArchive = zip_open(tmpPath.c_str(), ZIP_CREATE | ZIP_TRUNCATE, &err);
            if (!newArchive) {
                zip_close(oldArchive);
                return false;
            }
            zip_int64_t index = zip_name_locate(oldArchive, resource.c_str(), 0);
            zip_int64_t count = zip_get_num_entries(oldArchive, 0);
            for (zip_int64_t i = 0; i < count; i++) {
                if (i == index) {
                    continue;
                }
                std::string name = zip_get_name(oldArchive, i, 0);
                if (name.back() == '/') {
                    zip_dir_add(newArchive, name.c_str(), 0);
                    continue;
                }
                jp::CData& buffer = buffers[i];
                zip_source_t* src = zip_source_buffer(newArchive, buffer.data(), buffer.size(), 0);
                zip_file_add(newArchive, name.c_str(), src, ZIP_FL_OVERWRITE);
            }
            zip_close(newArchive);
            zip_close(oldArchive);
            std::ifstream oldFile(filePath.toText().string(), std::ios::binary);
            if (!oldFile) {
                remove(tmpPath.c_str());
                return false;
            }
            jp::CData stub((size_t)filePath.getOffset());
            oldFile.read(stub.data(), stub.size());
            oldFile.close();
            remove(filePath.toText().string().c_str());
            std::ofstream newFile((filePath.toText().string()), std::ios::binary);
            if (!newFile) {
                remove(tmpPath.c_str());
                return false;
            }
            newFile.write(stub.data(), stub.size());
            std::ifstream tmpArchive(tmpPath, std::ios::binary);
            newFile << tmpArchive.rdbuf();
            tmpArchive.close();
            newFile.close();
            remove(tmpPath.c_str());
            return true;
        }

        bool includeSFX(const std::string& filePath) {
            zip_error_t error;
            zip_t* archive;
            std::streampos pos = findZipSignature(filePath);
            zip_source_t* src = zip_source_file_create(filePath.c_str(), pos, -1, &error);
            archive = zip_open_from_source(src, 0, &error);
            zip_int64_t entries = zip_get_num_entries(archive, 0);
            if (archive) {
                zip_int64_t entries = zip_get_num_entries(archive, 0);
                if (entries != -1) {
                    return false;
                }
                zip_close(archive);
            }
            std::hash<std::string> hashFile;
            long code = hashFile(filePath);
            code = abs(code);
            std::string filename = "temp" + to_string(code) + ".tmp";
            fs::path p = fs::absolute(filePath).parent_path();
            p /= filename;
            int err = 0;
            zip_t* zipfile = zip_open(p.string().c_str(), ZIP_CREATE | ZIP_TRUNCATE, &err);
            zip_source_t* source = zip_source_buffer(zipfile, "Created by M1ster_sl1me", 24, 0);
            zip_file_add(zipfile, "info.txt", source, 0);
            zip_close(zipfile);
            jp::CData data2 = FileManager::binaryReadA(p.string().c_str());
            remove(p);
            FileManager::binaryWriteA(filePath.c_str(), data2, true);
            return true;
        }
        jp::RFile getResource(const jp::text::zipstring& filePath, const std::string& resource) {
            if (!filePath.isInit()) {
                return jp::RFile();
            }
            zip_t* archive = filePath.toObject();
            zip_int64_t index = zip_name_locate(archive, resource.c_str(), 0);
            if (index >= 0) {
                zip_stat_t stat;
                if (zip_stat_index(archive, index, 0, &stat) != 0) {
                    zip_close(archive);
                    return jp::RFile();
                }
                zip_file_t* fileInZip = zip_fopen_index(archive, index, 0);
                CData buffer(stat.size);
                zip_fread(fileInZip, buffer.data(), buffer.size());
                zip_fclose(fileInZip);
                zip_close(archive);
                return jp::RFile(resource, buffer);
            }
            zip_close(archive);
            return jp::RFile();
        }
    }
}
