// Copyright 2023 - Popescu Iulia Cleopatra - 323CA
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include "helpers.h"
#include "macros.h"
#include "commands.h"

int main(int argc, char *argv) {

    char *command = (char *) malloc(LINELEN * sizeof(char));
    DIE(command == NULL, "malloc failed");

    char *cookie = (char *) malloc(LINELEN * sizeof(char));
    DIE(cookie == NULL, "malloc failed");

    char *jwt = (char *) malloc(LINELEN * sizeof(char));
    DIE(jwt == NULL, "malloc failed");

    do {
        int sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);

        scanf("%s", command);
        process_command(command, sockfd, &cookie, &jwt);

        close_connection(sockfd);

    } while(strcmp(command, EXIT) != 0);

    free(command);
    free(cookie);
    free(jwt);

    return 0;
}