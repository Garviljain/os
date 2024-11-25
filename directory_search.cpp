
#include <iostream>
#include <filesystem>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <ctime>

namespace fs = std::filesystem;

void printAttributes(const fs::directory_entry& entry) {
    struct stat fileStat;
    if (stat(entry.path().c_str(), &fileStat) < 0) {
        std::cerr << "Error fetching attributes for: " << entry.path() << '\n';
        return;
    }

    // Determine file type
    std::string type = (fs::is_directory(entry) ? "Directory" : "File");

    // Permissions
    std::string permissions;
    permissions += (fileStat.st_mode & S_IRUSR) ? "r" : "-";
    permissions += (fileStat.st_mode & S_IWUSR) ? "w" : "-";
    permissions += (fileStat.st_mode & S_IXUSR) ? "x" : "-";
    permissions += (fileStat.st_mode & S_IRGRP) ? "r" : "-";
    permissions += (fileStat.st_mode & S_IWGRP) ? "w" : "-";
    permissions += (fileStat.st_mode & S_IXGRP) ? "x" : "-";
    permissions += (fileStat.st_mode & S_IROTH) ? "r" : "-";
    permissions += (fileStat.st_mode & S_IWOTH) ? "w" : "-";
    permissions += (fileStat.st_mode & S_IXOTH) ? "x" : "-";

    // Owner and group
    struct passwd* owner = getpwuid(fileStat.st_uid);
    struct group* group = getgrgid(fileStat.st_gid);
    std::string ownerName = owner ? owner->pw_name : "Unknown";
    std::string groupName = group ? group->gr_name : "Unknown";

    // Timestamps
    char creationTime[20], accessTime[20], modificationTime[20];
    strftime(creationTime, 20, "%Y-%m-%d %H:%M:%S", localtime(&fileStat.st_ctime));
    strftime(accessTime, 20, "%Y-%m-%d %H:%M:%S", localtime(&fileStat.st_atime));
    strftime(modificationTime, 20, "%Y-%m-%d %H:%M:%S", localtime(&fileStat.st_mtime));

    // Print attributes
    std::cout << "Type: " << type << '\n';
    std::cout << "Permissions: " << permissions << '\n';
    std::cout << "Owner: " << ownerName << '\n';
    std::cout << "Group: " << groupName << '\n';
    std::cout << "Creation Time: " << creationTime << '\n';
    std::cout << "Access Time: " << accessTime << '\n';
    std::cout << "Modification Time: " << modificationTime << '\n';
    std::cout << "---------------------------------------\n";
}

void searchDirectory(const fs::path& dirPath, const std::string& nameToSearch) {
    try {
        for (const auto& entry : fs::recursive_directory_iterator(dirPath)) {
            if (entry.path().filename() == nameToSearch) {
                std::cout << "Found: " << entry.path() << '\n';
                printAttributes(entry);
            }
        }
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Filesystem error: " << e.what() << '\n';
    }
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <directory> <name_to_search>\n";
        return 1;
    }

    fs::path dirPath = argv[1];
    std::string nameToSearch = argv[2];

    if (!fs::exists(dirPath) || !fs::is_directory(dirPath)) {
        std::cerr << "Invalid directory path: " << dirPath << '\n';
        return 1;
    }

    searchDirectory(dirPath, nameToSearch);
    return 0;
}

