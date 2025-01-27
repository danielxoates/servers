#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include "server.h"

struct Server server_Constructor(int domain, int port, int service, int protocol, int backlog, u_long interface, void (*launch)(struct Server *server)) {

    struct Server server;

    server.domain = domain;
    server.service = service;
    server.protocol = protocol;
    server.backlog = backlog;
    server.port = port;

    server.address.sin_family = domain;
    server.address.sin_port = htons(port);
    server.address.sin_addr.s_addr = htonl(interface);

    server.socket = socket(domain, service, protocol);
    if (socket < 0) {
        perror("Failed to initialize/connect to socket...\n");
        exit(EXIT_FAILURE);
    }
    if(bind(server.socket, (struct sockaddr*)&server.address, sizeof(server.address)) < 0){
        perror("Failed to bind socket... \n");
        exit(EXIT_FAILURE);
    }
    if(listen(server.socket, server.backlog) < 0){
        perror("Failed to listen on socket... \n");
        exit(EXIT_FAILURE);
    }

    char ip_str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(server.address.sin_addr), ip_str, INET_ADDRSTRLEN);
    printf("Server is running on IP: %s, Port: %d\n", ip_str, ntohs(server.address.sin_port));

    server.launch = launch;
    return server;

}

void launch(struct Server *server){
    char buffer[BUFFER_SIZE];
    while(1) {
        printf("Waiting for connection");
        int addr_len = sizeof(server->address);
        int new_socket = accept(server->socket, (struct sockaddr *)&server->address, (socklen_t*)&addr_len);
        ssize_t bytesRead = read(new_socket, buffer, BUFFER_SIZE -1);
        if (bytesRead >= 0){
            buffer[bytesRead] = '\0';
            puts(buffer);
        } else {
            perror("Error reading buffer...\n");
        }
        FILE *fptr;
        fptr = fopen("../index.html", "r"); 
        if (!fptr) {
            perror("Error opening file...\n");
            exit(EXIT_FAILURE);
        }
        fseek(fptr, 0, SEEK_END);
        long length = ftell(fptr);
        fseek(fptr,0, SEEK_SET);

        char* bfr = malloc(length + 1);
        if(!bfr) {
            perror("Error allocating memory...\n");
            fclose(fptr);
        }
        fread(bfr, 1, length, fptr);
        bfr[length] = '\0';
        fclose(fptr);

        char *response = bfr;
        write(new_socket, response, strlen(response));
        close(new_socket);
    }
}

int main(int argc, char const *argv[])
{
    /* code */
    struct Server server = server_Constructor(AF_INET, 80, SOCK_STREAM, 0, 10, INADDR_ANY, launch);
    launch(&server);
    return 0;
}
