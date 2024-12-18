#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "craftLine.h"

void sendMessage(int socket_fd, char* message) {
    if (strcmp(message, "exit") == 0) {
        printf("Exiting client program.\n");
        close(socket_fd);
        exit(EXIT_SUCCESS);
    } else if (send(socket_fd, message, strlen(message), 0) < 0) {;
        close(socket_fd);
        perror("Send failed");
        exit(EXIT_FAILURE);
    }
}

char* readMessage(int socket_fd) {
    int bufferSize = 50;
    char* inputBuffer = malloc(bufferSize);

    char c;
    int i = 0;
    while (1) {
        read(socket_fd, &c, 1);
        if (c == '\0') {
            inputBuffer[i] = c;
            break;
        } else {
            inputBuffer[i++] = c;
        }

        if (i >= bufferSize) {
            bufferSize += 25;
            inputBuffer = realloc(inputBuffer, bufferSize);
        }
    }

    return inputBuffer;
}

int connectServer(char* serverIpAddress) {
    int socket_fd;
    struct sockaddr_in serverAddress;

    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);
    if (inet_pton(AF_INET, serverIpAddress, &serverAddress.sin_addr) <= 0) {
        perror("Invalid address or Address not supported");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }

    if (connect(socket_fd, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) {
        perror("Connection failed");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }

    printf("Connected to the server at %s:%d\n", serverIpAddress, 8080);
    return socket_fd;
}

int main(int argc, char** argv) {
    int client_fd;
    if (argc != 2) {
        perror("Incorrect number of arguments");
        exit(EXIT_FAILURE);
    } else {
        client_fd = connectServer(argv[1]);
    }

    while (1) {
        char* messageBuffer;
        messageBuffer = craftLine("snapClient => ");
        sendMessage(client_fd, messageBuffer);
        messageBuffer = readMessage(client_fd);
        printf("Server response: %s\n", messageBuffer);
    }
}