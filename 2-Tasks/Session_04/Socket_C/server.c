#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

void* receive(void* arg) {
    int connfd = *(int*)arg;

    char buffer[1024];
    while (1) {
        memset(buffer, 0, sizeof(buffer));
        int n = recv(connfd, buffer, sizeof(buffer), 0);
        if (n <= 0) {
            break;
        }
        printf("Client: %s\n", buffer);
    }

    pthread_exit(NULL);
}

int main() {
    int listenfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(8080);

    if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0) {
        printf("Failed to bind the socket.\n");
        exit(1);
    }

    if (listen(listenfd, 5) != 0) {
        printf("Failed to start listening.\n");
        exit(1);
    }

    int connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);

    pthread_t thread;
    if (pthread_create(&thread, NULL, receive, &connfd) != 0) {
        printf("Failed to create a thread.\n");
        exit(1);
    }

    char buffer[1024];
    while (1) {
        memset(buffer, 0, sizeof(buffer));
        fgets(buffer, sizeof(buffer), stdin);
        send(connfd, buffer, sizeof(buffer), 0);
        if (strcmp(buffer, "exit\n") == 0) {
            break;
        }
    }

    close(listenfd);
    close(connfd);

    pthread_join(thread, NULL);

    return 0;
}