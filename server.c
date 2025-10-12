#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>   // Linux 套接字头文件
#include <netinet/in.h>   // 定义 sockaddr_in 结构体
#include <arpa/inet.h>    // 提供 inet_ntoa 等函数
#include <unistd.h>       // 提供 close 函数

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    // 1. 创建套接字（TCP）
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // 2. 绑定 IP 和端口
    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));  // 清空结构体
    address.sin_family = AF_INET;         // IPv4
    address.sin_addr.s_addr = INADDR_ANY; // 绑定所有本地 IP
    address.sin_port = htons(PORT);       // 端口转换为网络字节序

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // 3. 监听连接（最大等待队列长度为 5）
    if (listen(server_fd, 5) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }
    printf("Server listening on port %d...\n", PORT);

    // 4. 接受客户端连接
    int new_socket = accept(server_fd, NULL, NULL);
    if (new_socket < 0) {
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }
    printf("Client connected\n");

    // 5. 接收并发送数据
    char buffer[BUFFER_SIZE] = {0};
    recv(new_socket, buffer, BUFFER_SIZE, 0);
    printf("Received from client: %s\n", buffer);

    char* response = "Hello from server!";
    send(new_socket, response, strlen(response), 0);
    printf("Response sent\n");

    // 6. 关闭连接
    close(new_socket);
    close(server_fd);

    return 0;
}