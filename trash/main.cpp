#include <iostream>
#include <vector>
#include <string>
#include <cstdlib> // For exit()

using namespace std;

int defineTypeMethod(const string firstline) {
    vector<string> words;
    size_t start;
    size_t i = 0;
    while (i < firstline.length()) {
        start = i;
        while (i < firstline.length() && firstline[i] != ' ') {
            if (firstline[i] == '\t') {
                cout << "Bad Request" << endl;
                exit(1);
            }
            i++;
        }
        if (firstline[i] == ' ') {
            if (firstline[i + 1] == ' ' || firstline[i + 1] == '\t') {
                cout << "Bad Request" << endl;
                exit(1);
            }
        }
            std::cout << " i  = "<< i <<std::endl;
            std::cout << " start  = "<< start <<std::endl;
        words.push_back(firstline.substr(start, i - start));
        i++; // To skip the space after the word
    }

    cout << "Method: " << words[0] << endl;
    cout << "Path: " << words[1] << endl;
    cout << "Version: " << words[2] << endl;

    // Check method type (GET, POST, DELETE, etc.)
    if (words[0] == "GET") {
        return 1;
    } else if (words[0] == "POST") {
        return 2;
    } else if (words[0] == "DELETE") {
        return 3;
    }

    cout << "Bad Request" << endl;
    return 0;
}

int main() {
    // Example input request line
    string requestLine = "GET /~moorthy/Courses/os98/Pgms/socket.html HTTP/1.1";
    // Call defineTypeMethod with the request line
    int result = defineTypeMethod(requestLine);
    // You can check the result here and do additional processing if needed
    cout << "Method Type Result: " << result << endl;

    return 0;
}
