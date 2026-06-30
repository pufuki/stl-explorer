#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <cstdint>

namespace stl_explorer {
namespace utils {

// String utilities
std::string formatAddress(const void* ptr);
std::string formatBytes(size_t bytes);
std::string formatHex(unsigned int value);
std::string padCenter(const std::string& text, size_t width);
std::string padLeft(const std::string& text, size_t width);
std::string padRight(const std::string& text, size_t width);
std::vector<std::string> wrapText(const std::string& text, size_t max_width);

// Time complexity utilities
std::string complexityToString(const std::string& complexity);

// Input utilities
int getValidInt(const std::string& input, int min, int max, int default_val);

// Math utilities
template<typename T>
T clamp(T value, T min, T max) {
    return value < min ? min : (value > max ? max : value);
}

// Random utilities for quiz
int randomInt(int min, int max);
std::string shuffleString(const std::string& str);
std::vector<int> parseCommaSeparated(const std::string& input);

} // namespace utils
} // namespace stl_explorer
