#include <iostream>
#include <string>
#include <filesystem>
#include <sys/stat.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <pwd.h>

namespace fs = std::filesystem;

inline struct stat stat_or_throw(const std::string& path, bool follow_symlinks = false) {
    struct stat st{};
    int rc = follow_symlinks ? ::stat(path.c_str(), &st)
                             : ::lstat(path.c_str(), &st);

    if (rc == -1) {
        throw std::runtime_error(
            std::string(follow_symlinks ? "stat() failed for '" : "lstat() failed for '")
            + path + "': " + std::strerror(errno)
        );
    }

    return st;
}

std::string permissions_to_string(mode_t mode) {
    std::string r;
    r += (mode & S_IRUSR) ? 'r' : '-';
    r += (mode & S_IWUSR) ? 'w' : '-';
    r += (mode & S_IXUSR) ? 'x' : '-';

    r += (mode & S_IRGRP) ? 'r' : '-';
    r += (mode & S_IWGRP) ? 'w' : '-';
    r += (mode & S_IXGRP) ? 'x' : '-';

    r += (mode & S_IROTH) ? 'r' : '-';
    r += (mode & S_IWOTH) ? 'w' : '-';
    r += (mode & S_IXOTH) ? 'x' : '-';
    return r;
}

unsigned long long get_inode_number(const struct stat& st) {
    return static_cast<unsigned long long>(st.st_ino);
}

std::string get_file_type(const struct stat& st) {
    if (S_ISREG(st.st_mode))  return "regular file";
    if (S_ISDIR(st.st_mode))  return "directory";
    if (S_ISLNK(st.st_mode))  return "symlink";
    if (S_ISCHR(st.st_mode))  return "character device";
    if (S_ISBLK(st.st_mode))  return "block device";
    if (S_ISFIFO(st.st_mode)) return "fifo";
    if (S_ISSOCK(st.st_mode)) return "socket";
    return "unknown";
}

std::string get_owner_name(const struct stat& st) {
    uid_t uid = st.st_uid;

    if (struct passwd* pwd = ::getpwuid(uid)) {
        return std::string(pwd->pw_name);
    }

    // fallback to UID
    return std::to_string(uid);
}

std::string get_permissions_string(const struct stat& st) {
    return permissions_to_string(st.st_mode);
}

unsigned long long get_hardlink_count(const struct stat& st) {
    return static_cast<unsigned long long>(st.st_nlink);
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: directory_analyzer <file_name> <directory_name>\n";
        return 1;
    }

    const std::string file_name = argv[1];
    const std::string directory_name = argv[2];

    fs::path file_path = fs::absolute(fs::path(directory_name) / file_name);
    const std::string path_str = file_path.string();

    struct stat st = stat_or_throw(path_str);

    std::cout << "1) Absolute path:   " << file_path << std::endl;
    std::cout << "2) File type:       " << get_file_type(st) << std::endl;
    std::cout << "3) Permissions:     " << get_permissions_string(st) << std::endl;
    std::cout << "4) Owner:           " << get_owner_name(st) << std::endl;
    std::cout << "5) I-Node:          " << get_inode_number(st) << std::endl;
    std::cout << "6) Hard link count: " << get_hardlink_count(st) << "\n";

    return 0;
}
