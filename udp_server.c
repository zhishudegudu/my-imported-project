#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int server_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);  // 用于存储客户端地址长度
    char buffer[BUFFER_SIZE] = {0};

    // 1. 创建 UDP 套接字（SOCK_DGRAM 表示 UDP 协议）
    if ((server_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // 2. 配置服务器地址（绑定 IP 和端口）
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;         // IPv4
    server_addr.sin_addr.s_addr = INADDR_ANY; // 绑定所有本地 IP
    server_addr.sin_port = htons(PORT);       // 端口转换为网络字节序

    // 3. 绑定套接字到指定地址和端口
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }
    printf("UDP server listening on port %d...\n", PORT);

    // 4. 循环接收客户端数据并回复
    while (1) {
        // 接收客户端数据（同时获取客户端地址）
        ssize_t recv_len = recvfrom(
            server_fd, 
            buffer, 
            BUFFER_SIZE, 
            0, 
            (struct sockaddr*)&client_addr, 
            &client_len
        );
        if (recv_len < 0) {
            perror("Receive failed");
            continue;
        }
        buffer[recv_len] = '\0';  // 手动添加字符串结束符
        printf("Received from %s:%d: %s\n", 
               inet_ntoa(client_addr.sin_addr),  // 客户端 IP
               ntohs(client_addr.sin_port),      // 客户端端口
               buffer);

        // 回复客户端（使用接收到的客户端地址）
        char* response = "Message received by UDP server!";
        sendto(
            server_fd, 
            response, 
            strlen(response), 
            0, 
            (struct sockaddr*)&client_addr, 
            client_len
        );
        printf("Response sent to client\n\n");

        // 若收到 "exit"，则退出服务器
        if (strcmp(buffer, "exit") == 0) {
            printf("Server exiting...\n");
            break;
        }

        // 清空缓冲区
        memset(buffer, 0, BUFFER_SIZE);
    }

    // 5. 关闭套接字
    close(server_fd);
    return 0;
}