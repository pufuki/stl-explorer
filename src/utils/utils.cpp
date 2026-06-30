#include "utils/utils.hpp"
#include <random>

namespace stl_explorer {
namespace utils {

std::string formatAddress(const void* ptr) {
    std::ostringstream os;
    os << "0x" << std::hex << reinterpret_cast<uintptr_t>(ptr);
    return os.str();
}

std::string padCenter(const std::string& text, size_t width) {
    if (text.length() >= width) return text;
    size_t left_pad = (width - text.length()) / 2;
    size_t right_pad = width - text.length() - left_pad;
    return std::string(left_pad, ' ') + text + std::string(right_pad, ' ');
}

int getValidInt(const std::string& input, int min, int max, int default_val) {
    try {
        int val = std::stoi(input);
        return clamp(val, min, max);
    } catch (...) {
        return default_val;
    }
}

int randomInt(int min, int max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(min, max);
    return dis(gen);
}

std::vector<int> parseCommaSeparated(const std::string& input) {
    std::vector<int> res;
    if (input.empty()) return res;
    std::stringstream ss(input);
    std::string item;
    while (std::getline(ss, item, ',')) {
        try {
            size_t first = item.find_first_not_of(" \t");
            if (first != std::string::npos) {
                size_t last = item.find_last_not_of(" \t");
                std::string trimmed = item.substr(first, (last - first + 1));
                res.push_back(std::stoi(trimmed));
            }
        } catch(...) {}
    }
    return res;
}

} // namespace utils
} // namespace stl_explorer
