#pragma once
#include <string>
#include <zip.h>
#include <filesystem>
#include <vector>
#include "FileManager.h"
#include "temps.h"
#include <functional>
#include <fstream>
#include "charset.h"
#include "reader.h"

namespace jp {
    namespace zip {
        std::streampos findZipSignature(const std::string filePath);
        zip_t* getExecutable();
    }
}