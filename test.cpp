#include <iostream>
#include <unistd.h>
#include <cerrno>
#include <cstring>

int main() {
    if (unlink("./test/test.txt_") == -1) {
        std::cerr << "Error deleting file: " << std::strerror(errno) << std::endl;
    } else {
        std::cout << "File deleted successfully" << std::endl;
    }
    return 0;
}