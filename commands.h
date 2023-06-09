#ifndef _COMMANDS_
#define _COMMANDS_

void _register(int sockfd);

char *_login(int sockfd);

char *_enter_library(int sockfd, char *cookie);

void _get_books(int sockfd, char *cookie, char *jwt);

void _get_book(int sockfd, char *cookie, char *jwt);

void _add_book(int sockfd, char *cookie, char *jwt);

void _delete_book(int sockfd, char *cookie, char *jwt);

void _logout(int sockfd, char *cookie);

void process_command(char *command, int sockfd, char **cookie, char **jwt);

#endif