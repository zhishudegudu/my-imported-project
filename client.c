#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define SERVER_IP "127.0.0.1"  // 服务器 IP（本地回环）

int main() {
    // 1. 创建套接字（TCP）
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // 2. 连接服务器
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // 将字符串 IP 转换为网络字节序
    if (inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr) <= 0) {
        perror("Invalid address");
        exit(EXIT_FAILURE);
    }

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }
    printf("Connected to server %s:%d\n", SERVER_IP, PORT);

    // 3. 发送并接收数据
    char* message = "Hello from client!";
    send(sock, message, strlen(message), 0);
    printf("Message sent\n");

    char buffer[BUFFER_SIZE] = {0};
    recv(sock, buffer, BUFFER_SIZE, 0);
    printf("Received from server: %s\n", buffer);

    // 4. 关闭连接
    close(sock);

    return 0;
}