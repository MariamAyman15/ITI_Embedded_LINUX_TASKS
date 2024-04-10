#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

void* receive(void* arg) {
    int sockfd = *(int*)arg;

    char buffer[1024];
    while (1) {
        memset(buffer, 0, sizeof(buffer));
        int n = recv(sockfd, buffer, sizeof(buffer), 0);
        if (n <= 0) {
            break;
        }
        printf("Server: %s\n", buffer);
    }

    pthread_exit(NULL);
}

int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // replace with the IP address of the server
    servaddr.sin_port = htons(8080); // replace with the port number of the server

    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0) {
        printf("Failed to connect to the server.\n");
        exit(1);
    }

    pthread_t thread;
    if (pthread_create(&thread, NULL, receive, &sockfd) != 0) {
        printf("Failed to create a thread.\n");
        exit(1);
    }

    char buffer[1024];
    while (1) {
        memset(buffer, 0, sizeof(buffer));
        fgets(buffer, sizeof(buffer), stdin);
        send(sockfd, buffer, sizeof(buffer), 0);
        if (strcmp(buffer, "exit\n") == 0) {
            break;
        }
    }

    close(sockfd);

    pthread_join(thread, NULL);

    return 0;
}