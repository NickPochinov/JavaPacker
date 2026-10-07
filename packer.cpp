#include "lib/FileManager.h"
#include "lib/Text.h"
#include "lib/charset.h"
#include "lib/javapacker.h"
#include "lib/temps.h"
#include "lib/zipstring.h"
#include "nlohmann/json.hpp"
#include "icoreader.h"
#include "info.h"
#include "zip.h"
#include <filesystem>
using json = nlohmann::json;
namespace fs = std::filesystem;
using namespace nlohmann::literals;
#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif

jp::RFile loadExecutable(char os) {
    switch (os) {
        case (0):
            return jp::zip::getResource({""}, "unpacker.exe");
            break;
        case (1):
            return jp::zip::getResource({""}, "unpacker.elf");
            break;
        case (2):
            return jp::zip::getResource({""}, "unpacker.out");
            break;
        default:
            break;
    }
    return jp::RFile();
}

int getIndex(std::vector<std::string> svec, std::string value) {
    for (int i = 0; i < svec.size(); i++) {
        std::string data = svec[i];
        if (data == value) {
            return i;
        }
    }
    return -1;
}

bool contains(std::vector<std::string> svec, std::string value) {
    return getIndex(svec, value) != -1;
}

std::string findDirectoryByKey(const jp::text::zipstring& filePath, const std::string& resource) {
    if (!filePath.isInit()) {
        return "";
    }
    std::string copyResource = resource;
    if (copyResource.back() != '/') {
        copyResource += '/';
    }
    zip_t* archive = filePath.toObject();
    zip_int64_t entries = zip_get_num_entries(archive, 0);
    for (int i = 0; i < entries; i++) {
        std::string name = zip_get_name(archive, i, 0);
        if (name.rfind(copyResource) == name.size() - copyResource.size()) {
            std::string p = name.substr(0, name.size() - copyResource.size());
            if (!p.empty() && (p.back() == '/' || p.back() == '\\')) {
                p.pop_back();
            }
            zip_close(archive);
            return p;
        }
    }
    zip_close(archive);
    return "";
}


std::string getJdkVersion(const jp::text::zipstring& filePath) {
    std::string javaDir = findDirectoryByKey(filePath, "bin");
    std::string prefix = "JAVA_VERSION";
    std::string release = javaDir + "/release";
    jp::RFile content = jp::zip::getResource(filePath, release);
    std::vector<std::string> lines;
    std::string line = "";
    for (char c : content.getContent()) {
        if (c != '\n' && c != '=' && c != '\"' && c != '\'' && c != '\r') {
            line += c;
        }
        else if (c != '\"' && c != '\'' && c != '\r') {
            lines.push_back(line);
            line.clear();
        }
    }
    if (!line.empty()) {
        lines.push_back(line);
    }
    int versionIndex = getIndex(lines, prefix);
    if (versionIndex == lines.size() - 1 || versionIndex == -1) {
        return "";
    }
    return lines[versionIndex + 1];
}

std::string toSystemString(std::string utfChars) {
#if defined(_WIN32) || defined(_WIN64)
    return jp::text::basic_wcharset::convert(jp::text::basic_charset::convert(utfChars.c_str(), UTF8), SYS);
#else
    return utfChars;
#endif
}

int main(int argc, char* argv[]) {
#if defined(_WIN32) || defined(_WIN64)
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
#endif
    if (argc == 1) {
        std::cout << "Ошибка: Неизвестная команда. Напишите --help для просмотра всех подкоманд" << std::endl;
        return 1;
    }
    std::vector<std::string> args;
    for (int i = 1; i < argc; i++) {
        std::string s = argv[i];
        args.push_back(argv[i]);
    }
    char os = 0;
    if (contains(args, "--help")) {
        std::cout << "-----------------------------" << std::endl;
        std::cout << "Подкоманды javapacker:" << std::endl;
        std::cout << "--version - отображает версию Java Packer в консоли." << std::endl;
        std::cout << "-file \"Путь к файлу\" - упаковывает указанный файл (.jar) в файл - обёртку." << std::endl;
        std::cout << "-windows - создаёт файл - обёртку для систем Windows (.exe)." << std::endl;
        std::cout << "-linux - создаёт файл - обёртку для систем Linux (.elf)." << std::endl;
        std::cout << "-mac - создаёт файл - обёртку для систем MacOS (.out)." << std::endl;
        std::cout << "-jdk \"Путь к архиву\" - встраивает среду выполнения (jdk) в файл - обёртку." << std::endl;
        std::cout << "-out \"Путь к файлу (без формата)\" - указывает путь к будующему файлу - обёртке." << std::endl;
        std::cout << "-nojdk - поиск среды выполнения при запуске обёртки - (используется по умолчанию)." << std::endl;
        std::cout << "-minjdk \"Версия jdk\" - минимальная версия jdk, которую можно использовать для запуска обёртки - (обязательный параметр при -nojdk)." << std::endl;
        std::cout << "-maxjdk \"Версия jdk\" - максимальная версия jdk, которую можно использовать для запуска обёртки - (параметр при -nojdk)." << std::endl;
        std::cout << "-icon \"Путь к файлу\" - добавляет иконку обёртке - (Работает только на Windows)." << std::endl;
        std::cout << "-----------------------------" << std::endl;
        std::cout << "Обязательные подкоманды: " << std::endl;
        std::cout << "-file, -out" << std::endl;
        std::cout << "-----------------------------" << std::endl;
        std::cout << "Пример для Windows: " << std::endl;
        std::cout << "cpacker -windows -file \"C:\\MyProgram\\out\\file.jar\" -jdk \"C:\\Users\\User\\Downloads\\jdk.zip\" -out \"C:\\MyProgram\\program\"" << std::endl;
        std::cout << "-----------------------------" << std::endl;
        return 0;
    }

    if (contains(args, "--version") || contains(args, "--ver")) {
        std::cout << "-----------------------------" << std::endl;
        std::cout << "Версия Java Packer: " << PACKER_VER << std::endl;
        std::cout << "-----------------------------" << std::endl;
        return 0;
    }

    if (!contains(args, "-file")) {
        std::cout << "Ошибка: Неправильный синтаксис команды. Напишите --help для просмотра всех подкоманд." << std::endl;
        return 1;
    }

    bool haveJdk = true;
    std::string minJdk = "-1";
    std::string maxJdk = "-1";

    if (!contains(args, "-jdk")) {
        haveJdk = false;
        if (!contains(args, "-minjdk")) {
            std::cout << "Ошибка: Неправильный синтаксис команды. Напишите --help для просмотра всех подкоманд." << std::endl;
            return 1;
        }
    }

    jp::RFile file = loadExecutable(0);

    if (contains(args, "-linux")) {
        file = loadExecutable(1);
        os = 1;
    }
    else if (contains(args, "-mac")) {
        file = loadExecutable(2);
        os = 2;
    }

    int fileIndex = getIndex(args, "-file");
    int jdkIndex = getIndex(args, "-jdk");
    int outIndex = getIndex(args, "-out");

    if (contains(args, "-icon")) {
        if (getIndex(args, "-icon") == args.size() - 1) {
            std::cout << "Ошибка: Неправильный синтаксис команды. Напишите --help для просмотра всех подкоманд" << std::endl;
            return 1;
        }
    }

    if (fileIndex == args.size() - 1 || jdkIndex == args.size() - 1 || outIndex == args.size() - 1 || fileIndex == -1 || outIndex == -1) {
        std::cout << "Ошибка: Неправильный синтаксис команды. Напишите --help для просмотра всех подкоманд" << std::endl;
        return 1;
    }

    if (!fs::exists(args[fileIndex + 1])) {
        std::cout << "Ошибка: Путь к файлу \"" << args[fileIndex + 1] << "\" не существует. Напишите --help для просмотра всех подкоманд" << std::endl;
        return 1;
    }

    if (haveJdk) {
        if (jdkIndex == args.size() - 1) {
            std::cout << "Ошибка: Неправильный синтаксис команды. Напишите --help для просмотра всех подкоманд" << std::endl;
            return 1;
        }
        if (!fs::exists(args[jdkIndex + 1])) {
            std::cout << "Ошибка: Путь к файлу \"" << args[jdkIndex + 1] << "\" не существует. Напишите --help для просмотра всех подкоманд" << std::endl;
            return 1;
        }
    }

    jp::RFile jarFile = {"javapacker/jar/main.jar", jp::FileManager::binaryReadA(args[fileIndex + 1].c_str())};

    std::string out = args[outIndex + 1];

    if (jarFile.isEmpty()) {
        std::cout << "Ошибка: Не удалось загрузить файл" << std::endl;
        return 1;
    }

    std::string jdkVersion = "-1";

    if (haveJdk) {
        fs::path jdkTmpPath = fs::absolute(fs::path("jdk"));
        if (!fs::exists(jdkTmpPath)) {
            fs::create_directory(jdkTmpPath);
        }
        jdkVersion = getJdkVersion({args[jdkIndex + 1].c_str()});
        std::string newPath = (jdkTmpPath / jdkVersion).string() + ".tmp";
        if (jdkVersion.empty()) {
            std::cout << "Ошибка: Не удалось загрузить jdk" << std::endl;
            return 1;
        }
        if (!fs::exists(newPath)) {
            std::string jdkPath = args[jdkIndex + 1];
            jp::FileManager::fileCopyA(jdkPath.c_str(), newPath.c_str());
            std::string root = findDirectoryByKey({newPath}, "bin");
            std::cout << "Вывод: Подготовка jdk..." << std::endl;
            jp::zip::removeDirectory({newPath}, root + "/jmods");
            jp::zip::removeFile({newPath}, root + "/lib/src.zip");
            jp::zip::removeFile({newPath}, root + "/lib/ct.sym");
            std::cout << "Вывод: Jdk успешно настроен" << std::endl;
        }
    }
    else {
        int minJdkIndex = getIndex(args, "-minjdk");
        int maxJdkIndex = getIndex(args, "-maxjdk");
        minJdk = args[minJdkIndex + 1];
        if (maxJdkIndex != -1) {
            maxJdk = args[maxJdkIndex + 1];
        }
    }

    json j = {{"jdk_version", jdkVersion}, {"manifest_path", "main.jar"}, {"min_jdk", minJdk}, {"max_jdk", maxJdk}, {"have_jdk", haveJdk ? "true" : "false"}};

    std::string jstr = j.dump();

    jp::RFile config = {"javapacker/config.json", {jstr.begin(), jstr.end()}};

    std::vector<jp::RFile> files = {config, jarFile};

    if (haveJdk) {
        std::string newPath = (fs::path("jdk") / jdkVersion).string() + ".tmp";
        jp::RFile jdk = {("javapacker/jdk/" + jdkVersion + "/jdk.zip"), jp::FileManager::binaryReadA(newPath.c_str())};
        files.push_back(jdk);
    }

    std::string outFile = out + fs::path(file.getPath()).extension().string();
    bool ext = jp::zip::extractResource({""}, file.getPath(), outFile);
    if (!ext) {
        std::cout << "Ошибка: Не удалось создать файл \"" + outFile + "\"" << std::endl;
        return 1;
    }
    if (os == 0 && contains(args, "-icon")) {
        HANDLE updater = BeginUpdateResourceA(outFile.c_str(), true);
        if (!updater) {
            std::cout << "Предупреждение: Не удалось начать добавление иконки" << std::endl;
        }
        else {
            jp::CData data = jp::FileManager::binaryReadA(args[getIndex(args, "-icon") + 1].c_str());
            if (data.size() < sizeof(ICONDIR) + sizeof(ICONDIRENTRY)) {
                std::cout << "Предупреждение: Неизвестный ico файл" << std::endl;
                EndUpdateResourceA(updater, false);
            }
            else {
                ICONDIR* dir = reinterpret_cast<ICONDIR*>(data.data());
                ICONDIRENTRY* dirEntry = reinterpret_cast<ICONDIRENTRY*>(data.data() + sizeof(ICONDIR));
                if (!dir && !dirEntry) {
                    std::cout << "Предупреждение: Неизвестный ico файл" << std::endl;
                    EndUpdateResourceA(updater, false);
                }
                else {
                    bool update = UpdateResourceA(updater, MAKEINTRESOURCEA(3), MAKEINTRESOURCEA(101), MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), data.data() + dirEntry->dwImageOffset, dirEntry->dwBytesInRes);
                    if (!update) {
                        std::cout << "Предупреждение: Не удалось добавить иконку" << std::endl;
                        EndUpdateResourceA(updater, false);
                    }
                    else {
                        jp::CData grp(sizeof(GroupHeader) + sizeof(GRPICONDIRENTRY));
                        GroupHeader gh = {0, 1, 1};
                        GRPICONDIRENTRY* ge = reinterpret_cast<GRPICONDIRENTRY*>(dirEntry);
                        ge->nID = 101;
                        memcpy(grp.data(), &gh, sizeof(GroupHeader));
                        memcpy(grp.data() + sizeof(GroupHeader), ge, sizeof(GRPICONDIRENTRY));
                        UpdateResourceA(updater, MAKEINTRESOURCEA(14), MAKEINTRESOURCEA(1), MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), grp.data(), grp.size());
                        EndUpdateResourceA(updater, false);
                    }
                }
            }
        }
    }
    jp::zip::includeSFX(outFile);
    jp::zip::addDirectories({outFile}, {"javapacker", "javapacker/jar", "javapacker/jdk", "javapacker/jdk/" + jdkVersion});
    jp::zip::addFiles({outFile}, files);
    std::cout << "Вывод: Создан файл: " + outFile << std::endl;
    std::cout << "Результат: Сборка завершена успешно" << std::endl;
    return 0;
}