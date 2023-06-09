#ifndef _MACROS_
#define _MACROS_

#define DIE(assertion, call_description)    \
    do {                                    \
        if (assertion) {                    \
            fprintf(stderr, "(%s, %d): ",    \
                    __FILE__, __LINE__);    \
            perror(call_description);        \
            exit(EXIT_FAILURE);                \
        }                                    \
    } while(0)

#define HOST "34.254.242.81"
#define PORT 8080
#define HOST_PORT "34.254.242.81:8080"
#define USERNAME 60

#define EXIT "exit"
#define REGISTER "register"
#define LOGIN "login"
#define ENTER_LIBRARY "enter_library"
#define GET_BOOKS "get_books"
#define GET_BOOK "get_book"
#define ADD_BOOK "add_book"
#define DELETE_BOOK "delete_book"
#define LOGOUT "logout"

#define REGISTER_ROUTE "/api/v1/tema/auth/register"
#define LOGIN_ROUTE "/api/v1/tema/auth/login"
#define LIBRARY_ROUTE "/api/v1/tema/library/access"
#define BOOKS_ROUTE "/api/v1/tema/library/books"
#define BOOK_ROUTE "/api/v1/tema/library/books/"
#define LOGOUT_ROUTE "/api/v1/tema/auth/logout"

#define PAYLOAD_TYPE "application/json"
#define BEARER "Authorization: Bearer "

#define CREATED_STATUS "HTTP/1.1 201"
#define OK_STATUS "HTTP/1.1 200"
#define OK_REGISTER_MESSAGE "201 - Created account.\n"
#define OK_LOGIN_MESSAGE "200 - Welcome!\n"
#define OK_ENTER_LIBRARY "200 - You have entered the online library!\n"
#define OK_GET_BOOKS "200 - Books list:\n"
#define OK_GET_BOOK "200 - Requested book:\n"
#define OK_ADD_BOOK "200 - Added book.\n"
#define OK_DELETE_BOOK "200 - Deleted book.\n"
#define OK_LOGOUT_MESSAGE "200 - You have logged out.\n"

#endif