#include <iostream>
#include <string>

size_t findLastSubstring(const std::string &str, const std::string &sub) {
    return str.rfind(sub);  // Finds last occurrence of 'sub' in 'str'
}

int main() {
    std::string text = "hello word, we are here work smart and hard";
    std::string target = "wo";

    size_t pos = findLastSubstring(text, target);

    if (pos != std::string::npos) {
        std::cout << "Last occurrence of '" << target << "' found at index: " << pos << std::endl;
    } else {
        std::cout << "'" << target << "' not found in the string." << std::endl;
    }

    return 0;
}
