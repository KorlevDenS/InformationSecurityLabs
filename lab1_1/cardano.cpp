#include "cardano.h"

#include <iostream>
#include <bits/ranges_algobase.h>
#include <random>
#include <vector>
#include <algorithm>
#include <bitset>
#include <cmath>
#include <fstream>

#include <chrono>
#include <string>

template <typename T> void deleteMatrix(T** matrix, int width);

static void rotateMatrix90(int** matrix, const int width) {
    for (int i = 0; i < width / 2; i++) {
        for (int j = i; j < width - i - 1; j++) {
            const int temp = matrix[i][j];
            matrix[i][j] = matrix[width - 1 - j][i];
            matrix[width - 1 - j][i] = matrix[width - 1 - i][width - 1 - j];
            matrix[width - 1 - i][width - 1 - j] = matrix[j][width - 1 - i];
            matrix[j][width - 1 - i] = temp;
        }
    }
}

static int** fillMatrix(const int width) {
    const auto matrix = new int*[width];
    int fillingData = 1;
    for (int i = 0; i < width; i++) {
        matrix[i] = new int[width];
        for (int j = 0; j < width; j++) {
            matrix[i][j] = fillingData;
            fillingData++;
        }
    }
    return matrix;
}

static void printMatrix(int** matrix, const int width) {
    int maxVal = 0;
    const int pos = width / 2 - 1;
    for (int i = 0; i < 4; i++) {
        if (matrix[pos][pos] != 0) {
            maxVal = matrix[pos][pos];
        } else if (matrix[pos][pos + 1] != 0) {
            maxVal = matrix[pos][pos + 1];
        } else if (matrix[pos + 1][pos] != 0) {
            maxVal = matrix[pos + 1][pos];
        } else {
            maxVal = matrix[pos + 1][pos + 1];
        }
    }
    const size_t maxLen = std::to_string(maxVal).length() + 1;
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < width; j++) {
            const size_t tab = maxLen - std::to_string(matrix[i][j]).length();
            std::cout << matrix[i][j] << " " << std::string(tab, ' ');
        }
        std::cout << std::endl << std::endl;
    }
}

template <typename T> static void deleteMatrix(T** matrix, const int width) {
    for (int i = 0; i < width; i++) {
        delete[] matrix[i];
    }
    delete[] matrix;
}

static int** copyMatrix (int** origMatrix, const int width) {
    const auto copyMatrix = new int*[width];
    for (int i = 0; i < width; i++) {
        copyMatrix[i] = new int[width];
    }
    for (int i = 0; i < width; i++) {
        std::ranges::copy_n(origMatrix[i], width, copyMatrix[i]);
    }
    return copyMatrix;
}

static int generateRandomNumber(const int end) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis{0, end};
    return dis(gen);
}

static std::pair<int, int> markCell(int const cell, int** quarter, const int quarterWidth) {
    int row = 0;
    int col = 0;
    [quarterWidth, quarter, cell, &row, &col]() -> void {
        for (int i = 0; i < quarterWidth; i++) {
            for (int j = 0; j < quarterWidth; j++) {
                if (quarter[i][j] == cell) {
                    quarter[i][j] = 0;
                    row = i;
                    col = j;
                    return;
                }
            }
        }
    }();
    return {row, col};
}

static void markNeighbours(const std::pair<int, int> &position, int** quarter, const int quarterWidth) {
    if (position.first > 0) {
        if (quarter[position.first - 1][position.second] > 0) {
            quarter[position.first - 1][position.second] *= -1;
        }
    }
    if (position.first < quarterWidth - 1) {
        if (quarter[position.first + 1][position.second] > 0) {
            quarter[position.first + 1][position.second] *= -1;
        }
    }
    if (position.second > 0) {
        if (quarter[position.first][position.second - 1] > 0) {
            quarter[position.first][position.second - 1] *= -1;
        }
    }
    if (position.second < quarterWidth - 1) {
        if (quarter[position.first][position.second + 1] > 0) {
            quarter[position.first][position.second + 1] *= -1;
        }
    }
}

static int selectCell(int** quarter, const int quarterWidth, std::vector<int>* chosen) {
    std::vector<int> positive_numbers;
    for (int i = 0; i < quarterWidth; i++) {
        for (int j = 0; j < quarterWidth; j++) {
            if (quarter[i][j] > 0 && std::ranges::find(*chosen, quarter[i][j]) == chosen->end()) {
                positive_numbers.push_back(quarter[i][j]);
            }
        }
    }
    if (positive_numbers.empty()) {
        return -1;
    }
    const int randomId = generateRandomNumber(static_cast<int>(positive_numbers.size()) - 1);
    const int selectedCell = positive_numbers[randomId];
    chosen->push_back(selectedCell);
    markNeighbours(markCell(selectedCell, quarter, quarterWidth), quarter, quarterWidth);
    return 0;
}

static int selectCellFromNeighborsToo(int** quarter, const int quarterWidth, std::vector<int>* chosen) {
    std::vector<int> available_numbers;
    for (int i = 0; i < quarterWidth; i++) {
        for (int j = 0; j < quarterWidth; j++) {
            if (quarter[i][j] != 0 && std::ranges::find(*chosen, abs(quarter[i][j])) == chosen->end()) {
                quarter[i][j] = abs(quarter[i][j]);
                available_numbers.push_back(quarter[i][j]);
            }
        }
    }
    if (available_numbers.empty()) {
        return -1;
    }
    const int randomId = generateRandomNumber(static_cast<int>(available_numbers.size()) - 1);
    const int selectedCell = available_numbers[randomId];
    chosen->push_back(selectedCell);
    markCell(selectedCell, quarter, quarterWidth);
    return 0;
}

static int createEncryptMatrix(int** codeMatrix, const int width) {
    const int quarterWidth = width / 2;
    const auto quarter1 = fillMatrix(quarterWidth);
    const auto quarter2 = copyMatrix(quarter1, quarterWidth);
    rotateMatrix90(quarter2, quarterWidth);
    const auto quarter3 = copyMatrix(quarter2, quarterWidth);
    rotateMatrix90(quarter3, quarterWidth);
    const auto quarter4 = copyMatrix(quarter3, quarterWidth);
    rotateMatrix90(quarter4, quarterWidth);
    std::vector<int> chosen;
    int** allQuarters[] = {quarter1, quarter2, quarter3, quarter4};
    auto rd = std::random_device {};
    auto rng = std::default_random_engine { rd() };
    std::ranges::shuffle(allQuarters, rng);
    int iterator = 0;
    int withoutMarkIterationCount = 0;
    int (*selectFunction)(int** quarter, int quarterWidth, std::vector<int>* chosen) = selectCell;
    while (true) {
        if (selectFunction(allQuarters[iterator], quarterWidth, &chosen) != 0) {
            if (chosen.size() == quarterWidth * quarterWidth) {
                break;
            }
            withoutMarkIterationCount++;
            if (withoutMarkIterationCount == 4) {
                selectFunction = selectCellFromNeighborsToo;
            }
        } else {
            withoutMarkIterationCount = 0;
        }
        iterator = (iterator + 1) % 4;
    }
    for (int i = 0; i < quarterWidth; i++) {
        const auto arr = new int[width];
        std::ranges::copy_n(quarter1[i], quarterWidth, arr);
        std::ranges::copy_n(quarter2[i], quarterWidth, arr + quarterWidth);
        codeMatrix[i] = arr;
    }
    for (int i = quarterWidth; i < width; i++) {
        const auto arr = new int[width];
        std::ranges::copy_n(quarter4[i - quarterWidth], quarterWidth, arr);
        std::ranges::copy_n(quarter3[i - quarterWidth], quarterWidth, arr + quarterWidth);
        codeMatrix[i] = arr;
    }
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < width; j++) {
            codeMatrix[i][j] = abs(codeMatrix[i][j]);
        }
    }
    return 0;
}

static void makeKeyFromMatrix(int** matrix, size_t* key, const int width) {
    for (int i = 0; i < width; i++) {
        std::string part;
        for (int j = 0; j < width; j++) {
            if (matrix[i][j] == 0) {
                part.append("1");
            } else {
                part.append("0");
            }
        }
        key[i] = std::stoull(part, nullptr, 2);
    }
}

static void makeMatrixFromKey(int** pattern, const size_t* key, const int width) {
    for (int i = 0; i < width; i++) {
        std::bitset<64> bits(key[i]);
        const auto matrixPart = new int[width];
        for (int j = width - 1; j >= 0; j--) {
            matrixPart[j] = bits.test(width - 1 - j);
        }
        pattern[i] = matrixPart;
    }
}

static void fillMatrixFromText(int** pattern, char** matrix, const int width, std::ifstream& file) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < width; j++) {
            for (int k = 0; k < width; k++) {
                if (pattern[j][k] == 1) {
                    if (char in; file.get(in)) {
                        matrix[j][k] = in;
                    } else {
                        matrix[j][k] = 0;
                    }
                }
            }
        }
        rotateMatrix90(pattern, width);
    }
}

static void fillCodeFromMatrix(char** matrix, const int width, std::ofstream& file) {
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < width; j++) {
            file.put(matrix[i][j]);
        }
    }
}

static void fillMatrixFromCode(char** matrix, const int width, std::ifstream& file) {
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < width; j++) {
            if (!file.get(matrix[i][j])) {
                matrix[i][j] = '?';
            }
        }
    }
}

static void fillTextFromMatrix(int** pattern, char** matrix, const int width, std::ofstream& file) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < width; j++) {
            for (int k = 0; k < width; k++) {
                if (pattern[j][k] == 1) {
                    if (matrix[j][k] != 0) {
                        file.put(matrix[j][k]);
                    }
                }
            }
        }
        rotateMatrix90(pattern, width);
    }
}

/// Generates random valid key of Cardano grill.
/// @param key pointer to array with code
/// @param width width of grill and of key too
/// @param showMatrix print decimal generated grill (optional)
/// @return -1 if width is non-positive or odd
/// @return -2 if width > 64 (result will cause size_t overflow)
/// @return 0 if done
int generateKey(size_t* key, const int width, const bool showMatrix) {
    if (width > 64) {
        return -2;
    }
    if (width < 2 || width % 2 != 0) {
        return -1;
    }
    const auto encryptMatrix = new int*[width];
    createEncryptMatrix(encryptMatrix, width);
    makeKeyFromMatrix(encryptMatrix, key, width);
    if (showMatrix) {
        printMatrix(encryptMatrix, width);
    }
    deleteMatrix(encryptMatrix, width);
    return 0;
}

/// Encrypts text file with Cardano grill. Fills missing characters with char of 0.
/// @param key pointer to array with code
/// @param width width of grill and of key too
/// @param weakFilePath input text
/// @param strongFilePath encrypt output
/// @return -1 if input file is not available
/// @return -2 if output file is not available
/// @return 0 if done
int encrypt(const size_t* key, const int width, const std::string& weakFilePath, const std::string& strongFilePath) {
    std::ifstream weakFile{weakFilePath};
    std::ofstream strongFile{strongFilePath};
    if (!weakFile.is_open()) {
        return -1;
    }
    if (!strongFile.is_open()) {
        return -2;
    }
    const auto pattern = new int*[width];
    makeMatrixFromKey(pattern, key, width);
    while (weakFile.peek() != EOF) {
        auto matrix = new char*[width];
        for (int j = 0; j < width; j++) {
            matrix[j] = new char[width];
        }
        fillMatrixFromText(pattern, matrix, width, weakFile);
        fillCodeFromMatrix(matrix, width, strongFile);
        deleteMatrix(matrix, width);
    }
    deleteMatrix(pattern, width);
    return 0;
}

/// Decrypts encrypted with Cardano grill file. Chars of 0 are ignored.
/// @param key pointer to array with code
/// @param width width of grill and of key too
/// @param weakFilePath decrypt output
/// @param strongFilePath encrypt input
/// @return -1 if output file is not available
/// @return -2 if input file is not available
/// @return 0 if done
int decrypt(const size_t* key, int width, const std::string& weakFilePath, const std::string& strongFilePath) {
    std::ofstream weakFile{weakFilePath};
    std::ifstream strongFile{strongFilePath};
    if (!weakFile.is_open()) {
        return -1;
    }
    if (!strongFile.is_open()) {
        return -2;
    }
    const auto pattern = new int*[width];
    makeMatrixFromKey(pattern, key, width);
    while (strongFile.peek() != EOF) {
        auto matrix = new char*[width];
        for (int j = 0; j < width; j++) {
            matrix[j] = new char[width];
        }
        fillMatrixFromCode(matrix, width, strongFile);
        fillTextFromMatrix(pattern, matrix, width, weakFile);
        deleteMatrix(matrix, width);
    }
    deleteMatrix(pattern, width);
    return 0;
}

/// Shows key as binary Cardano grill
/// @param key pointer to array with code
/// @param width width of grill and of key too
void showKeyAsGrid(const size_t* key, const int width) {
    const auto pattern = new int*[width];
    makeMatrixFromKey(pattern, key, width);
    printMatrix(pattern, width);
    deleteMatrix(pattern, width);
}