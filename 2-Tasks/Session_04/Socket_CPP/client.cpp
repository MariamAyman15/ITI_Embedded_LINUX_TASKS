#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <thread>

void receive(int sockfd) {
    char buffer[1024];
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int n = recv(sockfd, buffer, sizeof(buffer), 0);
        if (n <= 0) {
            break;
        }
        std::cout << "Server: " << buffer << std::endl;
    }
}

int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // replace with the IP address of the server
    servaddr.sin_port = htons(8080); // replace with the port number of the server

    connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

    std::thread t(receive, sockfd);

    char buffer[1024];
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        std::cin.getline(buffer, sizeof(buffer));
        send(sockfd, buffer, sizeof(buffer), 0);
        if (std::string(buffer) == "exit") {
            break;
        }
    }

    close(sockfd);

    t.join();

    return 0;
}
