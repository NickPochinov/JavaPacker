#include "lib/FileManager.h"
#include "lib/Text.h"
#include "lib/javapacker.h"

int main() {

    jp::RFile file = {"unpacker.exe", jp::FileManager::binaryReadA("unpacker.exe")};
    jp::zip::includeSFX("jpacker.exe");
    jp::zip::addFile({"jpacker.exe"}, file);
    std::cout << "finish" << std::endl;
    return 0;
}