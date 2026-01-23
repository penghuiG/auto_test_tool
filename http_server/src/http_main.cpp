#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <filesystem>
#include <map>
#include "post.h"
namespace fs = std::filesystem;

const int PORT = 8080;
const std::string WEB_ROOT = "/home/cx/version_base_test/http_server/public";
const std::string SERVER_NAME = "SimpleCppHttpServer/1.0";

// MIME 类型映射
std::map<std::string, std::string> mime_types = {
    {".html", "text/html"},
    {".htm", "text/html"},
    {".css", "text/css"},
    {".js", "application/javascript"},
    {".jpg", "image/jpeg"},
    {".jpeg", "image/jpeg"},
    {".png", "image/png"},
    {".gif", "image/gif"},
    {".txt", "text/plain"},
    {".pdf", "application/pdf"},
    {"", "application/octet-stream"}
};

// 解析 HTTP 请求
std::vector<std::string> parse_request(const std::string& request) {
    std::vector<std::string> lines;
    std::istringstream stream(request);
    std::string line;
    
    while (std::getline(stream, line)) {
        if (line.back() == '\r') {
            line.pop_back();
        }
        if (!line.empty()) {
            lines.push_back(line);
        }
    }
    return lines;
}

// 获取文件 MIME 类型
std::string get_mime_type(const std::string& path) {
    size_t dot_pos = path.find_last_of(".");
    if (dot_pos != std::string::npos) {
        std::string ext = path.substr(dot_pos);
        auto it = mime_types.find(ext);
        if (it != mime_types.end()) {
            return it->second;
        }
    }
    return "application/octet-stream";
}

void head_handler(int client_socket, std::string path) {
    std::string full_path = WEB_ROOT + path;
    std::ifstream file(full_path, std::ios::binary | std::ios::ate);
    
    if (!file.is_open()) {
        // 文件不存在，返回 404
        std::string response = 
            "HTTP/1.1 404 Not Found\r\n"
            "Content-Type: text/html\r\n"
            "Connection: close\r\n\r\n";
        send(client_socket, response.c_str(), response.size(), 0);
        close(client_socket);
        return;
    }
    
    // 获取文件大小
    size_t file_size = file.tellg();
    file.close();
    
    // 构建 HTTP 响应头（不包含消息体）
    std::string mime_type = get_mime_type(full_path);
    std::string response = 
        "HTTP/1.1 200 OK\r\n"
        "Server: " + SERVER_NAME + "\r\n"
        "Content-Type: " + mime_type + "\r\n"
        "Content-Length: " + std::to_string(file_size) + "\r\n"
        "Accept-Ranges: bytes\r\n"
        "Connection: close\r\n\r\n"; // 注意：这里只有头部，没有消息体
    
    // 只发送头部，不发送文件内容
    send(client_socket, response.c_str(), response.size(), 0);
    close(client_socket);
}

// 处理 HTTP 请求
void handle_request(int client_socket) {
    char buffer[4096];
    ssize_t bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
    
    if (bytes_received <= 0) {
        close(client_socket);
        return;
    }
    
    std::string request(buffer, bytes_received);
    auto lines = parse_request(request);
    if (lines.empty()) {
        close(client_socket);
        return;
    }
    // 解析请求行
    std::istringstream request_line(lines[0]);
    std::string method, path, protocol;
    request_line >> method >> path >> protocol;
    std::cout << "method: "<< method << std::endl;
    std::cout << "path: "<< path << std::endl;

    if(method == "HEAD") {
        head_handler(client_socket,path);
        return;
    }
    if(method == "POST") {
        try {
            // 创建处理器，指定保存目录
            // PostHandler handler("./uploaded_files");
            PostHandler handler(CONFIG_PATH_ROOT);
            
            handler.post_handler(client_socket, "/api/upload");
            
            std::cout << "POST处理器初始化完成" << std::endl;
            
        } catch (const std::exception& e) {
            std::cerr << "初始化失败: " << e.what() << std::endl;
            return ;
        }
    }
    // 只处理 GET 请求
    if (method != "GET") {
        std::string response = 
            "HTTP/1.1 405 Method Not Allowed\r\n"
            "Content-Type: text/html\r\n"
            "Connection: close\r\n\r\n"
            "<html><body><h1>405 Method Not Allowed,哈哈哈</h1></body></html>";
        send(client_socket, response.c_str(), response.size(), 0);
        close(client_socket);
        return;
    }
    
    // 安全处理路径
    if (path == "/") {
        path = "/index.html";
    }
    std::string full_path = WEB_ROOT + path;
    std::cout << "full path : " << full_path << std::endl;
    // 防止目录遍历攻击
    if (full_path.find("..") != std::string::npos) {
        std::string response = 
            "HTTP/1.1 403 Forbidden\r\n"
            "Content-Type: text/html\r\n"
            "Connection: close\r\n\r\n"
            "<html><body><h1>403 Forbidden</h1></body></html>";
        send(client_socket, response.c_str(), response.size(), 0);
        close(client_socket);
        return;
    }
    
    // 检查文件是否存在
    if (!fs::exists(full_path) || !fs::is_regular_file(full_path)) {
        std::string response = 
            "HTTP/1.1 404 Not Found\r\n"
            "Content-Type: text/html\r\n"
            "Connection: close\r\n\r\n"
            "<html><body><h1>404 Not Found</h1></body></html>";
        send(client_socket, response.c_str(), response.size(), 0);
        close(client_socket);
        return;
    }
    
    // 读取文件内容
    std::ifstream file(full_path, std::ios::binary);
    if (!file.is_open()) {
        std::string response = 
            "HTTP/1.1 500 Internal Server Error\r\n"
            "Content-Type: text/html\r\n"
            "Connection: close\r\n\r\n"
            "<html><body><h1>500 Internal Server Error</h1></body></html>";
        send(client_socket, response.c_str(), response.size(), 0);
        close(client_socket);
        return;
    }
    
    // 获取文件大小
    file.seekg(0, std::ios::end);
    size_t file_size = file.tellg();
    file.seekg(0, std::ios::beg);
    
    // 读取文件内容
    std::vector<char> content(file_size);
    file.read(content.data(), file_size);
    file.close();
    
    // 构建 HTTP 响应
    std::string mime_type = get_mime_type(full_path);
    std::string response = 
        "HTTP/1.1 200 OK\r\n"
        "Server: " + SERVER_NAME + "\r\n"
        "Content-Type: " + mime_type + "\r\n"
        "Content-Length: " + std::to_string(file_size) + "\r\n"
        "Connection: close\r\n\r\n";
    
    send(client_socket, response.c_str(), response.size(), 0);
    
    send(client_socket, content.data(), file_size, 0);
    
    close(client_socket);
}

int main() {    
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket creation failed");
        return 1;
    }
    
    // 设置 SO_REUSEADDR
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt failed");
        close(server_fd);
        return 1;
    }
    
    // 绑定地址
    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    
    if (bind(server_fd, (sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind failed");
        close(server_fd);
        return 1;
    }
    
    // 监听
    if (listen(server_fd, 10) < 0) {
        perror("listen failed");
        close(server_fd);
        return 1;
    }
    
    std::cout << "Server running on port " << PORT << std::endl;
    std::cout << "Serving files from: " << fs::absolute(WEB_ROOT) << std::endl;
    std::cout << "Press Ctrl+C to stop..." << std::endl;
    
    while (true) {
        sockaddr_in client_addr{};
        socklen_t addr_len = sizeof(client_addr);
        
        int client_socket = accept(server_fd, (sockaddr*)&client_addr, &addr_len);
        if (client_socket < 0) {
            perror("accept failed");
            continue;
        }
        
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
        
        std::cout << "Client connected: " << client_ip << ":" << ntohs(client_addr.sin_port) << std::endl;
        
        handle_request(client_socket);
    }
    
    close(server_fd);
    return 0;
}