#include <iostream>
#include <string>
#include <filesystem>
#include <sys/stat.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <pwd.h>

namespace fs = std::filesystem;

std::string permissions_to_string(fs::perms p) {
    std::string result;

    result += (p & fs::perms::owner_read)  != fs::perms::none ? 'r' : '-';
    result += (p & fs::perms::owner_write) != fs::perms::none ? 'w' : '-';
    result += (p & fs::perms::owner_exec)  != fs::perms::none ? 'x' : '-';

    result += (p & fs::perms::group_read)  != fs::perms::none ? 'r' : '-';
    result += (p & fs::perms::group_write) != fs::perms::none ? 'w' : '-';
    result += (p & fs::perms::group_exec)  != fs::perms::none ? 'x' : '-';

    result += (p & fs::perms::others_read)  != fs::perms::none ? 'r' : '-';
    result += (p & fs::perms::others_write) != fs::perms::none ? 'w' : '-';
    result += (p & fs::perms::others_exec)  != fs::perms::none ? 'x' : '-';

    return result;
}

unsigned long long get_inode_number(const std::string& path) {
    struct stat st{};

    if (lstat(path.c_str(), &st) == -1) {
        throw std::runtime_error(
            std::string("lstat() failed for '") + path + "': " + std::strerror(errno)
        );
    }

    return static_cast<unsigned long long>(st.st_ino);
}

std::string get_file_type(const std::string& path) {
    struct stat st{};

    if (lstat(path.c_str(), &st) == -1) {
        throw std::runtime_error(
            std::string("lstat() failed for '") + path + "': " + std::strerror(errno)
        );
    }

    if (S_ISREG(st.st_mode))  return "regular file";
    if (S_ISDIR(st.st_mode))  return "directory";
    if (S_ISLNK(st.st_mode))  return "symlink";
    if (S_ISCHR(st.st_mode))  return "character device";
    if (S_ISBLK(st.st_mode))  return "block device";
    if (S_ISFIFO(st.st_mode)) return "fifo";
    if (S_ISSOCK(st.st_mode)) return "socket";

    return "unknown";
}

std::string get_owner_name(const std::string& path) {
    struct stat st{};

    if (lstat(path.c_str(), &st) == -1) {
        throw std::runtime_error(
            std::string("lstat() failed for '") + path + "': " + std::strerror(errno)
        );
    }

    uid_t owner_uid = st.st_uid;

    struct passwd* pwd = getpwuid(owner_uid);
    if (pwd != nullptr) {
        // pwd->pw_name to właściciel jako string
        return std::string(pwd->pw_name);
    }

    // jeśli system nie ma nazwy usera (np. dziwny FS / network FS) -> zwracamy UID jako string
    return std::to_string(owner_uid);
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: directory_analyzer <file_name> <directory_name>\n";
        return 1;
    }

    std::string file_name = argv[1];
    std::string directory_name = argv[2];

    fs::path file_path = fs::absolute(directory_name + "/" + file_name);

    fs::file_status status = fs::status(file_path);
    fs::perms permissions = status.permissions();

    std::cout << "1) Absolute file path: " << file_path << "\n";

    std::string type = get_file_type(file_path);
    std::cout << "2) File type: " << type << "\n";

    std::cout << "3) Permissions to this file: " << permissions_to_string(permissions) << "\n";

    std::string owner = get_owner_name(file_name);
    std::cout << "4) Owner: " << owner << "\n";

    try {
        unsigned long long inode = get_inode_number(file_path);
        std::cout << "5) I-Node value: " << inode << std::endl;
    } catch (const std::exception& e) {
        std::cerr << e.what() << "\n";
        return 2;
    }

    return 0;
}
