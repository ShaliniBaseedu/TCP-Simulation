// TCP Transmitter (Client)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_IP   "10.1.201.151"   // Change to receiver's IP if on different machine
#define PORT        8080
#define BUFFER_SIZE 1024

int main() {
    int sock_fd;
    struct sockaddr_in server_addr;
    char send_buf[BUFFER_SIZE];
    char recv_buf[BUFFER_SIZE];

    // 1. Create socket
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // 2. Set server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port   = htons(PORT);

    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
        perror("Invalid address / Address not supported");
        close(sock_fd);
        exit(EXIT_FAILURE);
    }

    // 3. Connect to the receiver (server)
    if (connect(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection to receiver failed");
        close(sock_fd);
        exit(EXIT_FAILURE);
    }

    printf("Connected to Receiver at %s:%d\n", SERVER_IP, PORT);
    printf("Type a message and press Enter (type 'exit' to quit):\n");

    // 4. Send data in a loop
    while (1) {
        printf(">> ");
        fflush(stdout);

        if (fgets(send_buf, BUFFER_SIZE, stdin) == NULL)
            break;

        // Remove trailing newline
        send_buf[strcspn(send_buf, "\n")] = '\0';

        if (strcmp(send_buf, "exit") == 0) {
            printf("Closing connection.\n");
            break;
        }

        // Send message
        int bytes_sent = send(sock_fd, send_buf, strlen(send_buf), 0);
        if (bytes_sent < 0) {
            perror("Send failed");
            break;
        }
        printf("Sent [%d bytes]: %s\n", bytes_sent, send_buf);

        // Receive echo from server
        int bytes_received = recv(sock_fd, recv_buf, BUFFER_SIZE - 1, 0);
        if (bytes_received <= 0) {
            printf("Receiver disconnected or error.\n");
            break;
        }
        recv_buf[bytes_received] = '\0';
        printf("Echo from Receiver: %s\n", recv_buf);

        memset(send_buf, 0, BUFFER_SIZE);
        memset(recv_buf, 0, BUFFER_SIZE);
    }

    // 5. Close socket
    close(sock_fd);

    return 0;
}
