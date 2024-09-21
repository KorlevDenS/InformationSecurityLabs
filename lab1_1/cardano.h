#ifndef CARDANO_H
#define CARDANO_H
#include <string>

int encrypt(const size_t* key, int width, const std::string& weakFilePath, const std::string& strongFilePath);
int decrypt(const size_t* key, int width, const std::string& weakFilePath, const std::string& strongFilePath);
int generateKey(size_t* key, int width, bool showMatrix = false);
void showKeyAsGrid(const size_t* key, int width);

#endif //CARDANO_H
