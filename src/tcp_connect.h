#pragma once

#include <iostream>
#include <string>
#include <stdio.h>
#include <cstring>       // void *memset(void *s, int ch, size_t n);
#include <sys/types.h>   // 数据类型定义
#include <sys/socket.h>  // 提供socket函数及数据结构sockaddr
#include <arpa/inet.h>   // 提供IP地址转换函数，htonl()、htons()...
#include <netinet/in.h>  // 定义数据结构sockaddr_in
#include <ctype.h>       // 小写转大写
#include <unistd.h>      // close()、read()、write()、recv()、send()...
#include <thread>		 // c++11 thread类
#include <curl/curl.h>
enum connect_retStatus{
    CONNECT_OK,
    CONNECT_ERR

};
#define SERVER_PORT 6666
extern const int flag; // 0表示读写处于阻塞模式
extern const int port;
extern const int buffer_size;

void requestHandling(const int client_sockfd, const struct sockaddr_in& client_addr);
void recive_handle(const int sockefd);
int connect_to_server(const char* serverIp, int serverPort); 
bool check_connection(int socket_fd);
void monitor_and_reconnect();
