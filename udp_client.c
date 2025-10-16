#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define SERVER_IP "127.0.0.1"  // 服务器 IP（本地测试用）

int main() {
    int sock_fd;
    struct sockaddr_in server_addr;
    socklen_t server_len = sizeof(server_addr);
    char buffer[BUFFER_SIZE] = {0};

    // 1. 创建 UDP 套接字
    if ((sock_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // 2. 配置服务器地址（目标地址）
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    // 将字符串 IP 转换为网络字节序
    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
        perror("Invalid server IP");
        exit(EXIT_FAILURE);
    }

    // 3. 发送数据到服务器并接收回复
    while (1) {
        // 输入要发送的消息
        printf("Enter message to send (type 'exit' to quit): ");
        fgets(buffer, BUFFER_SIZE, stdin);
        // 移除 fgets 自动添加的换行符
        buffer[strcspn(buffer, "\n")] = '\0';

        // 发送数据到服务器
        sendto(
            sock_fd, 
            buffer, 
            strlen(buffer), 
            0, 
            (struct sockaddr*)&server_addr, 
            server_len
        );

        // 若发送 "exit"，则退出客户端
        if (strcmp(buffer, "exit") == 0) {
            printf("Client exiting...\n");
            break;
        }

        // 接收服务器回复
        ssize_t recv_len = recvfrom(
            sock_fd, 
            buffer, 
            BUFFER_SIZE, 
            0, 
            (struct sockaddr*)&server_addr, 
            &server_len
        );
        if (recv_len < 0) {
            perror("Receive failed");
            continue;
        }
        buffer[recv_len] = '\0';
        printf("Received from server: %s\n\n", buffer);

        // 清空缓冲区
        memset(buffer, 0, BUFFER_SIZE);
    }

    // 4. 关闭套接字
    close(sock_fd);
    return 0;
}