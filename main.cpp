#include <bitset>
#include <iostream>
#include <fstream>

#include "lab1_1/cardano.h"


int main() {

    const std::string weakFileName = R"(C:\Users\mvideo\CLionProjects\InformationSecurityLabs\files\WeakFile.txt)";
    const std::string strongFileName = R"(C:\Users\mvideo\CLionProjects\InformationSecurityLabs\files\StrongFile.txt)";
    const std::string weekAgainFileName = R"(C:\Users\mvideo\CLionProjects\InformationSecurityLabs\files\WeekAgain.txt)";

    constexpr int width = 8;

    const auto key = new size_t[width];
    if (const int status = generateKey(key, width); status != 0) {
        return status;
    }
    for (int i = 0; i < width; i++) {
        std::cout << key[i] << " ";
    }
    std::cout << std::endl << std::endl;
    showKeyAsGrid(key, width);
    encrypt(key, width, weakFileName, strongFileName);
    decrypt(key, width, weekAgainFileName, strongFileName);

    return 0;
}
