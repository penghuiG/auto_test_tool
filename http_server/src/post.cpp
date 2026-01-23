#include "post.h"

// 初始化保存目录
void PostHandler::initializeSaveDirectory() {
    try {
        if (!fs::exists(save_directory_)) {
            fs::create_directories(save_directory_);
            std::cout << "创建保存目录: " << save_directory_ << std::endl;
        }
        
        // 检查目录是否可写
        std::ofstream test_file(save_directory_ + "/.write_test");
        if (test_file.is_open()) {
            test_file.close();
            fs::remove(save_directory_ + "/.write_test");
        } else {
            throw std::runtime_error("保存目录不可写: " + save_directory_);
        }
    } catch (const std::exception& e) {
        std::cerr << "初始化保存目录失败: " << e.what() << std::endl;
        throw;
    }
}

// 生成唯一文件名
std::string PostHandler::generateUniqueFilename(const std::string& original_name) {
    std::string base_name = original_name.empty() ? "upload" : original_name;
    
    // 移除路径部分，只保留文件名
    size_t last_slash = base_name.find_last_of("/\\");
    if (last_slash != std::string::npos) {
        base_name = base_name.substr(last_slash + 1);
    }
    
    // 如果没有扩展名，添加默认的.txt
    if (base_name.find('.') == std::string::npos) {
        base_name += ".txt";
    }
    
    // 生成带时间戳的唯一文件名
    std::string timestamp = getTimestamp();
    size_t dot_pos = base_name.find_last_of('.');
    
    if (dot_pos != std::string::npos) {
        std::string name_without_ext = base_name.substr(0, dot_pos);
        std::string extension = base_name.substr(dot_pos);
        return name_without_ext + "_" + timestamp + extension;
    } else {
        return base_name + "_" + timestamp;
    }
}

// 获取时间戳字符串
std::string PostHandler::getTimestamp() {
    auto now = std::time(nullptr);
    auto tm = *std::localtime(&now);
    
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y%m%d_%H%M%S");
    return oss.str();
}

// 从路径中提取文件名
std::string PostHandler::extractFilenameFromPath(const std::string& path) {
    size_t last_slash = path.find_last_of("/\\");
    if (last_slash != std::string::npos && last_slash < path.length() - 1) {
        return path.substr(last_slash + 1);
    }
    return "";
}

// 提取Content-Type（简化版本）
std::string PostHandler::extractContentType(const std::string& request_headers) {
    std::istringstream iss(request_headers);
    std::string line;
    
    while (std::getline(iss, line)) {
        if (line.length() > 13 && 
            line.substr(0, 13).compare("Content-Type:") == 0) {
            return line.substr(14);
        }
    }
    return "application/octet-stream";
}

void PostHandler::post_handler(int client_socket, const std::string& path) {
    const int BUFFER_SIZE = 4096;
    char buffer[BUFFER_SIZE];
    std::string request_data;
    std::string headers;
    std::string body;
    bool headers_received = false;
    long content_length = 0;
    
    std::cout << "开始处理POST请求，路径: " << path << std::endl;
    
    try {
        // 读取HTTP请求头
        while (!headers_received) {
            ssize_t bytes_read = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
            if (bytes_read <= 0) {
                throw std::runtime_error("接收数据失败或连接关闭");
            }
            
            buffer[bytes_read] = '\0';
            request_data.append(buffer, bytes_read);
            
            // 检查是否收到完整的头部（以\r\n\r\n结尾）
            size_t header_end = request_data.find("\r\n\r\n");
            if (header_end != std::string::npos) {
                headers = request_data.substr(0, header_end);
                body = request_data.substr(header_end + 4); // +4 跳过 \r\n\r\n
                headers_received = true;
                
                // 解析Content-Length
                std::istringstream iss(headers);
                std::string line;
                while (std::getline(iss, line)) {
                    if (line.length() > 15 && 
                        line.substr(0, 15).compare("Content-Length:") == 0) {
                        content_length = std::stol(line.substr(16));
                        break;
                    }
                }
                
                std::cout << "Content-Length: " << content_length << std::endl;
                std::cout << "Headers received, body length so far: " << body.length() << std::endl;
                break;
            }
        }
        
        // 如果还有body数据需要读取
        if (content_length > static_cast<long>(body.length())) {
            long remaining = content_length - body.length();
            
            while (remaining > 0) {
                ssize_t bytes_read = recv(client_socket, buffer, 
                                        std::min(BUFFER_SIZE, static_cast<int>(remaining)), 0);
                if (bytes_read <= 0) {
                    throw std::runtime_error("读取body数据时连接中断");
                }
                
                buffer[bytes_read] = '\0';
                body.append(buffer, bytes_read);
                remaining -= bytes_read;
            }
        }
        
        std::cout << "完整接收POST数据，总长度: " << body.length() << std::endl;
        
        // 生成文件名并保存数据
        std::string filename = generateUniqueFilename(extractFilenameFromPath(path));
        std::string full_path = save_directory_ + "/" + filename;
        
        if (savePostDataToFile(body, full_path)) {
            // 发送成功响应
            std::string success_response = 
                "HTTP/1.1 200 OK\r\n"
                "Content-Type: application/json\r\n"
                "Connection: close\r\n"
                "\r\n"
                "{\"status\":\"success\",\"message\":\"File saved successfully\",\"filename\":\"" + 
                filename + "\"}";
            
            send(client_socket, success_response.c_str(), success_response.length(), 0);
            std::cout << "文件保存成功: " << full_path << std::endl;
        } else {
            throw std::runtime_error("文件保存失败");
        }
        
    } catch (const std::exception& e) {
        std::cerr << "处理POST请求错误: " << e.what() << std::endl;
        
        // 发送错误响应
        std::string error_response = 
            "HTTP/1.1 500 Internal Server Error\r\n"
            "Content-Type: application/json\r\n"
            "Connection: close\r\n"
            "\r\n"
            "{\"status\":\"error\",\"message\":\"Failed to process request: " + 
            std::string(e.what()) + "\"}";
        
        send(client_socket, error_response.c_str(), error_response.length(), 0);
    }
}

// 保存POST数据到文件
bool PostHandler::savePostDataToFile(const std::string& data, const std::string& filename) {
    try {
        std::ofstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "无法打开文件进行写入: " << filename << std::endl;
            return false;
        }
        
        file.write(data.c_str(), data.length());
        file.close();
        
        // 验证文件是否真的写入了数据
        if (fs::file_size(filename) == data.length()) {
            return true;
        } else {
            std::cerr << "文件写入不完整: " << filename << std::endl;
            fs::remove(filename); // 删除不完整的文件
            return false;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "保存文件时发生异常: " << e.what() << std::endl;
        return false;
    }
}