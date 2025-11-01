#include <iostream>
#include <string>

using namespace std;

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cerr << "Usage: directory-analyzer <file> <directory>\n";
        return 1;
    }

    string file_name = argv[1];
    string directory_name = argv[2];

    cout << "File name: " << file_name << "\n";
    cout << "Directory name: " << directory_name << "\n";

    return 0;
}