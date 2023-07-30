/*
 ============================================================================================
Name        : constants.h
Author      : Seyedsajad Hosseini
Version     : 1.0
Description : Defines constants for client and server functions.
Assumption  : -
 ============================================================================================
 */

#ifndef CONSTANTS_H
#define CONSTANTS_H

#define xstr(s) str(s)
#define str(s) #s

#define SEARCH_WORD_SIZE        20
#define FILE_NAME_SIZE          200

#define SERVER_IP               "127.0.0.1"
#define SERVER_PORT             6000
#define MAX_BUFFER_SIZE         1024
#define CLIENTS_QUEUE_SIZE      5
#define DISCONNECT              "disconnect"
#define NOT_FOUND               "-1"

#define COMMA                   ","

#endif
