#define PORT 8088
#define BUFFER_SIZE 1024
#define MAX_REQUEST_SIZE 1024
#define DEFAULT_DIR "/var/www/"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "fileExists.h"
#include "connectionHandler.h"
#include "extractMethodAndURL.h"

int main() {
    setvbuf (stdout, NULL, _IONBF, 0);
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    // Create socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        printf("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    // Bind the socket to localhost port 8088
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) {
        printf("Bind failed");
        exit(EXIT_FAILURE);
    }
    // Listen for incoming connections
    if (listen(server_fd, 3) < 0) {
        printf("Listen failed");
        exit(EXIT_FAILURE);
    }
    // Accept incoming connections and create a thread for each
    while ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))) {
        pthread_t thread_id;
        int *new_sock = malloc(sizeof(int));
        *new_sock = new_socket;

        if (pthread_create(&thread_id, NULL, connection_handler, (void*)new_sock) < 0) {
            printf("Thread creation failed");
            exit(EXIT_FAILURE);
        }
        // Detach the thread to avoid memory leaks
        pthread_detach(thread_id);
    }
    if (new_socket<0) {
        printf("Accept failed");
        exit(EXIT_FAILURE);
    }
    return 0;
}