#include <iostream>
#include <string>
#include <filesystem>
#include <vector>
#include <stdexcept>
#include <sys/stat.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <pwd.h>
#include <dirent.h>

namespace fs = std::filesystem;

inline struct stat stat_or_throw(const std::string& path) {
    struct stat st{};
    int rc = ::lstat(path.c_str(), &st);

    if (rc == -1) {
        std::cerr << "File could not be found (lstat failed)" << std::endl;
        exit(1);
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

inline bool is_dot_or_dotdot(const char* name) {
    return (name[0] == '.' && (name[1] == '\0' || (name[1] == '.' && name[2] == '\0')));
}

std::vector<std::string> find_other_hardlinks_in_dir(
    const std::string& directory,
    const struct stat& target_st,
    const std::string& target_abs_path
) {
    std::vector<std::string> results;

    DIR* dir = ::opendir(directory.c_str());

    if (!dir) {
        std::cerr << "opendir() failed for '" + directory + "'" << std::endl;
        exit(1);
    }

    const std::string dir_abs = fs::absolute(directory).string();

    for (dirent* ent = ::readdir(dir); ent != nullptr; ent = ::readdir(dir)) {
        if (is_dot_or_dotdot(ent->d_name)) continue;

        fs::path candidate = fs::path(dir_abs) / ent->d_name;
        std::string candidate_abs = fs::absolute(candidate).string();

        struct stat st{};

        if (::lstat(candidate_abs.c_str(), &st) == -1) {
            std::cerr << "Warning: lstat() failed for '" << candidate_abs
                      << "': " << std::strerror(errno) << "\n";
            continue;
        }

        if (st.st_dev == target_st.st_dev && st.st_ino == target_st.st_ino) {
            if (candidate_abs != target_abs_path) {
                results.push_back(candidate_abs);
            }
        }
    }

    ::closedir(dir);
    return results;
}

void print_other_hardlinks(const std::vector<std::string>& links) {
    if (links.empty()) {
        std::cout << "7) Other hard links in directory: (none)\n";
        return;
    }

    std::cout << "7) Other hard links in directory:\n";

    for (const auto& p : links) {
        std::cout << "   - " << p << "\n";
    }
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: directory_analyzer <file_name> <directory_name>\n";
        return 1;
    }

    const std::string file_path = argv[1];
    const std::string directory_path = argv[2];
    const std::string file_abs_path = fs::absolute(file_path).string();

    struct stat st = stat_or_throw(file_abs_path);

    std::cout << "1) Absolute path:   " << file_abs_path << std::endl;
    std::cout << "2) File type:       " << get_file_type(st) << std::endl;
    std::cout << "3) Permissions:     " << get_permissions_string(st) << std::endl;
    std::cout << "4) Owner:           " << get_owner_name(st) << std::endl;
    std::cout << "5) I-Node:          " << get_inode_number(st) << std::endl;
    std::cout << "6) Hard link count: " << get_hardlink_count(st) << std::endl;

    std::vector<std::string> links = find_other_hardlinks_in_dir(directory_path, st, file_abs_path);
    print_other_hardlinks(links);

    return 0;
}
