#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <vector>
#include <sys/socket.h>
#include <unistd.h>         // close() 函数和read/write系列函数

#define CONFIG_PATH_ROOT "/home/cx/auto_test_test/conf"

namespace fs = std::filesystem;

class PostHandler {
private:
    std::string save_directory_;
    
public:
    PostHandler(const std::string& save_directory = "./uploads") 
        : save_directory_(save_directory) {
        initializeSaveDirectory();
    }
    
    void post_handler(int client_socket, const std::string& path);
    
private:
    void initializeSaveDirectory();
    std::string generateUniqueFilename(const std::string& original_name = "");
    bool savePostDataToFile(const std::string& data, const std::string& filename);
    std::string getTimestamp();
    std::string extractFilenameFromPath(const std::string& path);
    std::string extractContentType(const std::string& request_headers);
};