#include "lib/javapacker.h"
#include "nlohmann/json.hpp"
#include "zip.h"
#include "lib/pointer.h"
#include <cctype>
#include <filesystem>
using json = nlohmann::json;
namespace fs = std::filesystem;
using namespace nlohmann::literals;

#define SYS_MAX(a, b) (a > b ? a : b)

#define SYS_MIN(a, b) (a < b ? a : b)

#include <cstdlib>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif

void throwError(const char* error) {
#if defined(_WIN32) || defined(_WIN64)
    char* copyError = jp::text::basic_wcharset::convert(jp::text::basic_charset::convert(error, UTF8), SYS);
    char* normalCaption = jp::text::basic_wcharset::convert(jp::text::basic_charset::convert("Утилита Java Packer", UTF8), SYS);
    MessageBoxA(NULL, copyError, normalCaption, MB_OK | MB_ICONERROR);
#elif defined(__linux__)
    std::string cmd = "zenity --info --title=\"Утилита Java Packer\" --text=\"";
    cmd += error;
    cmd += "\"";
    system(cmd.c_str());
#elif defined(__APPLE__)
    std::string cmd = "osascript -e 'display dialog \"";
    cmd += error;
    cmd += "\" buttons {\"OK\"} default button \"OK\" with title \"Утилита Java Packer\"'";
    system(cmd.c_str());
#endif
    exit(1);
}

long getPos(fs::path directory, std::string name) {
    long m = 0;
    for (auto dir : fs::directory_iterator(directory)) {
        if (dir.is_regular_file()) {
            fs::path path = dir;
            if (path.has_stem()) {
                std::string sname = fs::path(name).stem().string();
                std::string s = path.stem().string();
                if (s.find(sname) == 0) {
                    if (s == sname) {
                        m = SYS_MAX(m, 1);
                    }
                    else if (s.size() > sname.size()) {
                        std::string sub = s.substr(sname.size());
                        bool isNumber = true;
                        for (char c : sub) {
                            if (!std::isdigit(c)) {
                                isNumber = false;
                                break;
                            }
                        }
                        if (isNumber) {
                            long index = std::atol(sub.c_str());
                            m = SYS_MAX(index + 1, m);
                        }
                    }
                }
            }
        }
    }
    return m;
}


#if defined(_WIN32) || defined(_WIN64)
fs::path getExe() {
    zip_error_t error;
    wchar_t path[MAX_PATH];
    GetModuleFileNameW(NULL, path, MAX_PATH);
    int err = 0;
    return fs::path(path);
}
#elif defined(__APPLE__)
#include <mach-o/dyld.h>
#include "javapacker.h"
fs::path getExe() {
    char path[PATH_MAX];
    uint32_t size = sizeof(path);
    if (_NSGetExecutablePath(path, &size) == 0)
        return fs::path(path);
    else
        return fs::path("");
}
#else
fs::path getExe() {
    return fs::canonical("/proc/self/exe");
}
#endif

fs::path getTmpPath() {
    #if defined(_WIN32) || defined(_WIN64)
    const char* name = "C:\\Windows\\Temp";
    #else
    const char* name = fs::temp_directory_path();
    #endif
    if (name) {
        return (fs::path(name));
    }
    return fs::path("");
}

std::string findEntryByKey(const jp::text::zipstring& filePath, const std::string& resource) {
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
        if (name.back() != '/') {
            name.push_back('/');
        }
        if (copyResource.size() <= name.size()) {
            if (name.substr(name.size() - copyResource.size()) == copyResource) {
                name.pop_back();
                return name;
            }
        }
    }
    zip_close(archive);
    return "";
}


int main(int argc, char* argv[]) {
#if defined(_WIN32) || defined(_WIN64)
    ShowWindow(GetConsoleWindow(), SW_HIDE);
#endif
    jp::RFile file = jp::zip::getResource(jp::text::Text(""), "javapacker/config.json");
    if (file.isEmpty()) {
        throwError("Конфигурации не существует");
        return 1;
    }
    if (getTmpPath().empty()) {
        throwError("Невозможно получить путь к папке");
        return 1;
    }
    jp::CData data = file.getContent();
    json j = json::parse(data.begin(), data.end());
    if (!j.contains("jdk_version") || !j.contains("manifest_path")) {
        throwError("Конфигурация повреждена или имеет неправильную структуру");
        return 1;
    }
    std::string jdk_version = j["jdk_version"];
    std::string manifest_path = j["manifest_path"];
    jp::text::pointer minJdk = jp::text::Text(std::string(j["min_jdk"]));
    jp::text::pointer maxJdk = jp::text::Text(std::string(j["max_jdk"]));

    bool haveJdk = j["have_jdk"] == "true" ? true : false;
    fs::path processDir = (getTmpPath() / ".jp" / "tmp");
    if (!fs::exists(processDir)) {
        fs::create_directories(processDir);
    }
    fs::path jdks = getTmpPath() / ".jp" / "jdk";
    fs::path jdkDir;
    if (haveJdk) {
        jdkDir = (jdks / jdk_version);
        if (!fs::exists(jdkDir)) {
            fs::create_directories(jdkDir);
        }
        fs::path zipPath = (jdkDir / "jdk.zip");
        if (!fs::exists(zipPath)) {
            bool haveJdk = jp::zip::extractResource(jp::text::Text(""), "javapacker/jdk/" + jdk_version + "/jdk.zip", zipPath.string());
            if (!haveJdk) {
                throwError("Jdk не существует");
                return 1;
            }
        }
        fs::path binFolder = jdkDir / "bin";
        if (!fs::exists(binFolder)) {
            std::string binDirectory = findEntryByKey(zipPath.string(), "bin");
            if (binDirectory.empty()) {
                throwError("Jdk повреждён или имеет неправильную структуру");
                return 1;
            }
            std::string jdkDirectory = fs::path(binDirectory).parent_path().string();
            for (std::string sub : jp::zip::getSubDirectories(zipPath.string(), jdkDirectory)) {
                jp::zip::extractDirectory(zipPath.string(), sub, (jdkDir / fs::path(sub.substr(jdkDirectory.size() + 1)).make_preferred()).string());
            }
        }
    }
    else {
        if (!fs::exists(jdks)) {
            throwError("Не удалось найти подходящий jdk для запуска программы");
            return -1;
        }
        for (auto dir : fs::directory_iterator(jdks)) {
            if (dir.is_directory()) {
                jp::text::pointer d = jp::text::Text(dir.path().filename().string());
                if (maxJdk.toText() == std::string("-1")) {
                    if (d >= minJdk) {
                        jdkDir = dir;
                        break;
                    }
                }
                else {
                    if (d >= minJdk && d <= maxJdk) {
                        jdkDir = dir;
                        break;
                    }
                }
            }
        }
    }
    if (jdkDir.empty()) {
        if (!fs::exists(jdks)) {
            throwError("Не удалось найти подходящий jdk для запуска программы");
            return -1;
        }
    }
    long position = getPos(processDir, "jp-process.jar.tmp");
    std::string exeFile = (processDir / "jp-process").string() + (position != 0 ? to_string(position) : "") + ".jar.tmp";
    
    bool extractJar = jp::zip::extractResource({""}, "javapacker/jar/" + manifest_path, exeFile);

    if (!extractJar) {
        throwError("Невозможно запустить файл");
        return 1;
    }

    std::string jrePath = (jdkDir / "bin" / "java").string();

#if defined(_WIN32) || defined(_WIN64)
    jrePath += ".exe";
#endif

    std::string cmd = "\"" + jrePath + "\" ";
    std::string args = "-jar \"" + exeFile + "\" ";
    for (int i = 1; i < argc; i++) {
        args += "\"";
        for (int j = 0; j < strlen(argv[i]); j++) {
            if (argv[i][j] != '\"' || argv[i][j] != '\'') {
                args += argv[i][j];
            }
            else if (argv[i][j] == '\"') {
                args += "\\\"";
            }
            else {
                args += "\\\'";
            }
        }
        args += "\" ";
    }
    args.pop_back();
    cmd += args;

    unsigned long result;

#if defined(_WIN32) || defined(_WIN64)
    std::wstring wfile = jp::text::basic_charset::convert(jrePath.c_str(), UTF8);
    std::wstring wargs = wfile + L" " + jp::text::basic_charset::convert(args.c_str(), UTF8);

    STARTUPINFOW si = {sizeof(STARTUPINFOW)};
    PROCESS_INFORMATION pi;
    DWORD written;

    if (!CreateProcessW(wfile.c_str(), const_cast<wchar_t*>(wargs.c_str()), 0, 0, false, 0, 0, 0, &si, &pi)) {
        fs::remove(exeFile);
        throwError("Не удалось запустить файл");
        return 1;
    }
    WaitForSingleObject(pi.hProcess, INFINITE);
    GetExitCodeProcess(pi.hProcess, &result);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
#else
    result = std::system(cmd.c_str());
#endif

    fs::remove(exeFile);
    return result;
}