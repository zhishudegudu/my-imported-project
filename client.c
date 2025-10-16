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
    printf("Type 'exit' to quit\n");

    // 3. 持续发送并接收数据
    char buffer[BUFFER_SIZE] = {0};
    while (1) {
        // 输入要发送的消息
        printf("Enter message: ");
        fgets(buffer, BUFFER_SIZE, stdin);
        // 移除 fgets 自动添加的换行符
        buffer[strcspn(buffer, "\n")] = '\0';

        // 发送数据
        send(sock, buffer, strlen(buffer), 0);

        // 检查是否为退出命令
        if (strcmp(buffer, "exit") == 0) {
            printf("Client exiting...\n");
            break;
        }

        // 接收服务器回复
        memset(buffer, 0, BUFFER_SIZE);
        int valread = recv(sock, buffer, BUFFER_SIZE, 0);
        if (valread < 0) {
            perror("Receive failed");
            break;
        } else if (valread == 0) {
            printf("Server disconnected\n");
            break;
        }
        printf("Received from server: %s\n", buffer);
    }

    // 4. 关闭连接
    close(sock);

    return 0;
}