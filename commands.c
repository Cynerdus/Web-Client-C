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
#include "requests.h"
#include "parson.h"

/*
    register command

    1.  Dynamic allocation of memory
            .username;
            .password;
            .json;
            .server_response.

    2.  Json construct
            .username;
            .password.

    3.  POST server request
            ."34.254.242.81:8080";
            ."/api/v1/tema/auth/register";
            ."application/json";
            .Json construct.
        Server response status => 201 / error
*/

void _register(int sockfd) {
    char *username, *password, *json, *server_response;

    username = (char *) malloc(USERNAME * sizeof(char));
    DIE(username == NULL, "malloc failed");

    password = (char *) malloc(USERNAME * sizeof(char));
    DIE(password == NULL, "malloc failed");

    json = (char *) malloc(LINELEN * sizeof(char));
    DIE(json == NULL, "malloc failed");

    server_response = (char *) malloc(BUFLEN * sizeof(char));
    DIE(server_response == NULL, "malloc failed");

    // - - - - - - - - - - - - - - - - - - - - - - - - - - -

    printf("username=");
    scanf("%s", username);
    printf("password=");
    scanf("%s", password);

    JSON_Value *root = json_value_init_object();
    JSON_Object *object = json_value_get_object(root);

    json_object_set_string(object, "username", username);
    json_object_set_string(object, "password", password);

    json = json_serialize_to_string_pretty(root);

    // - - - - - - - - - - - - - - - - - - - - - - - - - - -

    char *message =  compute_post_request(HOST_PORT, REGISTER_ROUTE,
                            PAYLOAD_TYPE, json, NULL, 0);

    send_to_server(sockfd, message);
    server_response = receive_from_server(sockfd);

    /* Extracting error messaje beforehand, in case if needed */
    char *error = basic_extract_json_response(server_response);

    server_response[12] = '\0'; // HTTP/1.1 xxx => 12 characters

    if (strcmp(server_response, CREATED_STATUS) == 0) {
        printf(OK_REGISTER_MESSAGE);
    } else {
        printf("%s\n", error);
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - - -

    free(username);
    free(password);
    free(json);
    free(server_response);
    free(message);
}

/*
    login command

    1.  Dynamic allocation of memory
            .username;
            .password;
            .json;
            .server_response.

    2.  Json construct
            .username;
            .password.

    3.  POST server request
            ."34.254.242.81:8080";
            ."/api/v1/tema/auth/login";
            ."application/json";
            .Json construct.
        Server response status => 200 / error
        Cookie extraction from server response
*/

char *_login(int sockfd) {
    char *username, *password, *json, *server_response, *cookie;

    username = (char *) malloc(USERNAME * sizeof(char));
    DIE(username == NULL, "malloc failed");

    password = (char *) malloc(USERNAME * sizeof(char));
    DIE(password == NULL, "malloc failed");

    json = (char *) malloc(LINELEN * sizeof(char));
    DIE(json == NULL, "malloc failed");

    server_response = (char *) malloc(BUFLEN * sizeof(char));
    DIE(server_response == NULL, "malloc failed");

    cookie = NULL;

    // - - - - - - - - - - - - - - - - - - - - - - - - - - -

    printf("username=");
    scanf("%s", username);
    printf("password=");
    scanf("%s", password);

    JSON_Value *root = json_value_init_object();
    JSON_Object *object = json_value_get_object(root);

    json_object_set_string(object, "username", username);
    json_object_set_string(object, "password", password);

    json = json_serialize_to_string_pretty(root);

    // - - - - - - - - - - - - - - - - - - - - - - - - - - -

    char *message = compute_post_request(HOST_PORT, LOGIN_ROUTE,
                            PAYLOAD_TYPE, json, NULL, 0);

    send_to_server(sockfd, message);
    server_response = receive_from_server(sockfd);

    /* Extracting error message beforehand, in case if needed */
    char *error = basic_extract_json_response(server_response);

    /* Cookie extraction from server response, using strstr and strtok */
    cookie = strstr(server_response, "connect");
    cookie = (strstr(server_response, "error") == NULL) ? strtok(cookie, ";") : NULL;

    server_response[12] = '\0'; // HTTP/1.1 xxx => 12 characters

    if (strcmp(server_response, OK_STATUS) == 0) {
        printf(OK_LOGIN_MESSAGE);
    } else {
        printf("%s\n", error);
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - - -

    free(server_response);
    free(username);
    free(password);
    free(json);
    free(message);

    return cookie == NULL ? "" : cookie;
}

/*
    enter_library command

    1.  Dynamic allocation of memory
            .server_response;
            .cookies matrix.

    2.  Cookies matrix assemble
            .position 0 => Cookie.

    3.  GET server request
            ."34.254.242.81:8080";
            ."/api/v1/tema/library/access";
            .Cookie matrix;
            .Cookies count.
        Server response status => 200 / error
        JWT extraction from server response
*/

char *_enter_library(int sockfd, char *cookie) {
    char *token, *server_response, **cookies;

    server_response = (char *) malloc(BUFLEN * sizeof(char));
    DIE(server_response == NULL, "malloc failed");

    // - - - - - - - - - - - - - - - - - - - - - - - - - - -

    cookies = (char **) malloc(sizeof(char *));
    cookies[0] = (char *) malloc(LINELEN * sizeof(char));
    strcpy(cookies[0], cookie);

    // - - - - - - - - - - - - - - - - - - - - - - - - - - -

    char *message =  compute_get_request(HOST_PORT, LIBRARY_ROUTE,
                            NULL, cookies, 1);

    send_to_server(sockfd, message);
    server_response = receive_from_server(sockfd);

    /* Token extraction from server response, using the provided function*/
    token = basic_extract_json_response(server_response) + 10;
    token[strlen(token) - 2] = '\0';

    /* Extracting error message beforehand, in case if needed */
    char *error = basic_extract_json_response(server_response);

    server_response[12] = '\0'; // HTTP/1.1 xxx => 12 characters

    if (strcmp(server_response, OK_STATUS) == 0) {
        printf(OK_ENTER_LIBRARY);
    } else {
        printf("%s\n", error);
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - - -

    free(server_response);
    free(cookies[0]);
    free(cookies);
    free(message);

    return token;
}

/*
    get_books command

    1.  Dynamic allocation of memory
            .server_response;
            .cookies matrix.

    2.  Cookies matrix assemble
            .position 0 => JWT;
            .position 1 => Cookie.

    3.  GET server request
            ."34.254.242.81:8080";
            ."/api/v1/tema/library/books";
            .Cookie matrix;
            .Cookie + Token count.
        Server response status => 200 / error
        Books list extraction from server response
*/

void _get_books(int sockfd, char *cookie, char *jwt) {
    char *server_response, **cookies;

    server_response = (char *) malloc(BUFLEN * sizeof(char));
    DIE(server_response == NULL, "malloc failed");

    // - - - - - - - - - - - - - - - - - - - - - - - - - - -

    cookies = (char **) malloc(2 * sizeof(char *));
    cookies[0] = (char *) malloc(LINELEN * sizeof(char));
    cookies[1] = (char *) malloc(LINELEN * sizeof(char));
    
    if (jwt != NULL) {
        strcpy(cookies[0], BEARER);
        strcat(cookies[0], jwt);
    }

    if (cookie != NULL) {
        strcpy(cookies[1], cookie);
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - - -

    char *message =  compute_get_request(HOST_PORT, BOOKS_ROUTE,
                            NULL, cookies, 2);

    send_to_server(sockfd, message);
    server_response = receive_from_server(sockfd);

    /* Extracting error message beforehand, in case if needed */
    char *error = basic_extract_json_response(server_response);

    /* Extracting books list, using strstr */
    char *books = strstr(server_response, "[");

    server_response[12] = '\0'; // HTTP/1.1 xxx => 12 characters

    if (strcmp(server_response, OK_STATUS) == 0) {
        printf("%s", OK_GET_BOOKS);

        /* Printing the list with endline in between items */
        int n = strlen(books);
        for (int i = 0; i < n; i++) {
            printf("%c", books[i]);
            if (i < n - 2 && books[i] == ',' && books[i + 1] == '{') {
                printf("\n");
            }
        }
        printf("\n");

    } else {
        printf("%s\n", error);
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - - -

    free(server_response);
    free(cookies[0]);
    free(cookies[1]);
    free(cookies);
    free(message);
}

/*
    get_book command

    1.  Dynamic allocation of memory
            .id;
            .server_response;
            .cookies matrix;
            .route.

    2.  Cookies matrix assemble
            .position 0 => JWT;
            .position 1 => Cookie.

    3.  GET server request
            ."34.254.242.81:8080";
            ."/api/v1/tema/library/books/:bookId";
            .Cookie matrix;
            .Cookie + Token count.
        Server response status => 200 / error
        Book info extraction from server response
*/

void _get_book(int sockfd, char *cookie, char *jwt) {
    char *server_response, **cookies, *id;

    id = (char *) malloc(USERNAME * sizeof(char));
    DIE(id == NULL, "malloc failed");

    server_response = (char *) malloc(BUFLEN * sizeof(char));
    DIE(server_response == NULL, "malloc failed");

    printf("id=");
    scanf("%s", id);

    char *route = (char *) malloc(USERNAME * sizeof(char));
    strcpy(route, BOOK_ROUTE);
    strcat(route, id);
    route[strlen(route)] = '\0';

    // - - - - - - - - - - - - - - - - - - - - - - - - - - -

    cookies = (char **) malloc(2 * sizeof(char *));
    cookies[0] = (char *) malloc(LINELEN * sizeof(char));
    cookies[1] = (char *) malloc(LINELEN * sizeof(char));
    
    if (jwt != NULL) {
        strcpy(cookies[0], BEARER);
        strcat(cookies[0], jwt);
    }

    if (cookie != NULL) {
        strcpy(cookies[1], cookie);
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - - -

    char *message =  compute_get_request(HOST_PORT, route,
                            NULL, cookies, 2);

    send_to_server(sockfd, message);
    server_response = receive_from_server(sockfd);

    /* Extracting error message beforehand, in case if needed */
    char *error = basic_extract_json_response(server_response);

    /* Extracting book info, using strstr */
    char *book = strstr(server_response, "{");

    server_response[12] = '\0'; // HTTP/1.1 xxx => 12 characters

    if (strcmp(server_response, OK_STATUS) == 0) {
        printf("%s", OK_GET_BOOK);

        int n = strlen(book);
        for (int i = 0; i < n; i++) {
            printf("%c", book[i]);
            if (book[i] == ',') {
                printf("\n");
            }
        }
        printf("\n");

    } else {
        printf("%s\n", error);
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - - -

    free(id);
    free(server_response);
    free(cookies[0]);
    free(cookies[1]);
    free(cookies);
    free(message);
}

/*
    add_book command

    1.  Dynamic allocation of memory
            .title;
            .author;
            .genre;
            .publisher;
            .page_count;
            .json;
            .server_response;
            .cookies matrix.

    2.  Cookies matrix assemble
            .position 0 => JWT;
            .position 1 => Cookie.

    3.  Json construct
            .title;
            .author;
            .genre;
            .publisher;
            .page_count.

    4.  POST server request
            ."34.254.242.81:8080";
            ."/api/v1/tema/library/books";
            ."application/json";
            .Json construct;
            .Cookie matrix;
            .Cookie + Token count.
        Server response status => 200 / error
*/

void _add_book(int sockfd, char *cookie, char *jwt) {
    char *title, *author, *genre, *publisher, *page_count;
    char *json, *server_response, **cookies;

    title = (char *) malloc(USERNAME * sizeof(char));
    DIE(title == NULL, "malloc failed");

    author = (char *) malloc(USERNAME * sizeof(char));
    DIE(author == NULL, "malloc failed");

    genre = (char *) malloc(USERNAME * sizeof(char));
    DIE(genre == NULL, "malloc failed");

    publisher = (char *) malloc(USERNAME * sizeof(char));
    DIE(publisher == NULL, "malloc failed");

    page_count = (char *) malloc(USERNAME * sizeof(char));
    DIE(page_count == NULL, "malloc failed");

    json = (char *) malloc(LINELEN * sizeof(char));
    DIE(json == NULL, "malloc failed");

    server_response = (char *) malloc(BUFLEN * sizeof(char));
    DIE(server_response == NULL, "malloc failed");

    // - - - - - - - - - - - - - - - - - - - - - - - - - - -

    cookies = (char **) malloc(2 * sizeof(char *));
    cookies[0] = (char *) malloc(LINELEN * sizeof(char));
    cookies[1] = (char *) malloc(LINELEN * sizeof(char));
    
    if (jwt != NULL) {
        strcpy(cookies[0], BEARER);
        strcat(cookies[0], jwt);
    }

    if (cookie != NULL) {
        strcpy(cookies[1], cookie);
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - - -

    getchar();
    printf("title=");
    fgets(title, USERNAME, stdin);
    printf("author=");
    fgets(author, USERNAME, stdin);
    printf("genre=");
    fgets(genre, USERNAME, stdin);
    printf("publisher=");
    fgets(publisher, USERNAME, stdin);
    printf("page_count=");
    fgets(page_count, USERNAME, stdin);

    title[strlen(title) - 1] = '\0';
    author[strlen(author) - 1] = '\0';
    genre[strlen(genre) - 1] = '\0';
    page_count[strlen(page_count) - 1] = '\0';
    publisher[strlen(publisher) - 1] = '\0';

    JSON_Value *root = json_value_init_object();
    JSON_Object *object = json_value_get_object(root);

    json_object_set_string(object, "title", title);
    json_object_set_string(object, "author", author);
    json_object_set_string(object, "genre", genre);
    json_object_set_string(object, "publisher", publisher);
    json_object_set_string(object, "page_count", page_count);

    json = json_serialize_to_string_pretty(root);

    // - - - - - - - - - - - - - - - - - - - - - - - - - - -

    char *message =  compute_post_request(HOST_PORT, BOOKS_ROUTE,
                            PAYLOAD_TYPE, json, cookies, 2);

    send_to_server(sockfd, message);
    server_response = receive_from_server(sockfd);

    /* Extracting error message beforehand, in case if needed */
    char *error = basic_extract_json_response(server_response);

    server_response[12] = '\0'; // HTTP/1.1 xxx => 12 characters

    if (strcmp(server_response, OK_STATUS) == 0) {
        printf(OK_ADD_BOOK);
    } else {
        printf("%s\n", error);
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - - -

    free(title);
    free(author);
    free(genre);
    free(publisher);
    free(page_count);
    free(json);
    free(server_response);
    free(message);
}

/*
    delete_book command

    1.  Dynamic allocation of memory
            .id;
            .server_response;
            .cookies matrix.

    2.  Cookies matrix assemble
            .position 0 => JWT;
            .position 1 => Cookie.

    4.  DELETE server request
            ."34.254.242.81:8080";
            ."/api/v1/tema/library/books";
            .Cookie matrix;
            .Cookie + Token count.
        Server response status => 200 / error
*/

void _delete_book(int sockfd, char *cookie, char *jwt) {
    char *server_response, **cookies, *id;

    id = (char *) malloc(USERNAME * sizeof(char));
    DIE(id == NULL, "malloc failed");

    server_response = (char *) malloc(BUFLEN * sizeof(char));
    DIE(server_response == NULL, "malloc failed");

    printf("id=");
    scanf("%s", id);

    char *route = (char *) malloc(USERNAME * sizeof(char));
    strcpy(route, BOOK_ROUTE);
    strcat(route, id);
    route[strlen(route)] = '\0';

    // - - - - - - - - - - - - - - - - - - - - - - - - - - -

    cookies = (char **) malloc(2 * sizeof(char *));
    cookies[0] = (char *) malloc(LINELEN * sizeof(char));
    cookies[1] = (char *) malloc(LINELEN * sizeof(char));
    
    if (jwt != NULL) {
        strcpy(cookies[0], BEARER);
        strcat(cookies[0], jwt);
    }

    if (cookie != NULL) {
        strcpy(cookies[1], cookie);
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - - -

    char *message =  compute_delete_request(HOST_PORT, route,
                            NULL, cookies, 2);

    send_to_server(sockfd, message);
    server_response = receive_from_server(sockfd);

    /* Extracting error message beforehand, in case if needed */
    char *error = basic_extract_json_response(server_response);

    server_response[12] = '\0'; // HTTP/1.1 xxx => 12 characters

    if (strcmp(server_response, OK_STATUS) == 0) {
        printf(OK_DELETE_BOOK);
    } else {
        printf("%s\n", error);
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - - -

    free(id);
    free(server_response);
    free(cookies[0]);
    free(cookies[1]);
    free(cookies);
    free(message);
}

/*
    logout command

    1.  Dynamic allocation of memory
            .server_response;
            .cookies matrix.

    2.  Cookies matrix assemble
            .position 0 => Cookie.

    3.  GET server request
            ."34.254.242.81:8080";
            ."/api/v1/tema/auth/logout";
            .Cookie matrix;
            .Cookie count.
        Server response status => 200 / error
*/

void _logout(int sockfd, char *cookie) {
    char *server_response, **cookies;

    server_response = (char *) malloc(BUFLEN * sizeof(char));
    DIE(server_response == NULL, "malloc failed");

    // - - - - - - - - - - - - - - - - - - - - - - - - - - -

    cookies = (char **) malloc(sizeof(char *));
    cookies[0] = (char *) malloc(LINELEN * sizeof(char));

    if (cookie != NULL) {
        strcpy(cookies[0], cookie);
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - - -

    char *message = compute_get_request(HOST_PORT, LOGOUT_ROUTE,
                            NULL, cookies, 1);

    send_to_server(sockfd, message);
    server_response = receive_from_server(sockfd);

    /* Extracting error message beforehand, in case if needed */
    char *error = basic_extract_json_response(server_response);

    server_response[12] = '\0'; // HTTP/1.1 xxx => 12 characters

    if (strcmp(server_response, OK_STATUS) == 0) {
        printf(OK_LOGOUT_MESSAGE);
    } else {
        printf("%s\n", error);
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - - -

    free(server_response);
    free(message);
    free(cookies[0]);
    free(cookies);
}

void process_command(char *command, int sockfd, char **cookie, char **jwt) {
    if (!strcmp(command, EXIT)) {
        return;
    }

    if (!strcmp(command, REGISTER)) {
        _register(sockfd);
    }

    if (!strcmp(command, LOGIN)) {
        strcpy(*cookie, _login(sockfd));
    }

    if (!strcmp(command, ENTER_LIBRARY)) {
        strcpy(*jwt, _enter_library(sockfd, *cookie));
    }

    if (!strcmp(command, GET_BOOKS)) {
        _get_books(sockfd, *cookie, *jwt);
    }

    if (!strcmp(command, GET_BOOK)) {
        _get_book(sockfd, *cookie, *jwt);
    }

    if (!strcmp(command, ADD_BOOK)) {
         _add_book(sockfd, *cookie, *jwt);
    }

    if (!strcmp(command, DELETE_BOOK)) {
        _delete_book(sockfd, *cookie, *jwt);
    }

    /* reseting the Cookie and JWT after logout */
    if (!strcmp(command, LOGOUT)) {
        _logout(sockfd, *cookie);
        strcpy(*cookie, "");
        strcpy(*jwt, "");
    }

}