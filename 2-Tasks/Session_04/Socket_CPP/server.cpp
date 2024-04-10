#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <thread>

void receive(int connfd) {
    char buffer[1024];
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int n = recv(connfd, buffer, sizeof(buffer), 0);
        if (n <= 0) {
            break;
        }
        std::cout << "Client: " << buffer << std::endl;
    }
}

int main() {
    int listenfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(8080);

    bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

    listen(listenfd, 5);

    int connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);

    std::thread t(receive, connfd);

    char buffer[1024];
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        std::cin.getline(buffer, sizeof(buffer));
        send(connfd, buffer, sizeof(buffer), 0);
        if (std::string(buffer) == "exit") {
            break;
        }
    }

    close(listenfd);
    close(connfd);

    t.join();

    return 0;
}
