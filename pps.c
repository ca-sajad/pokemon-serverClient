/*
 ============================================================================================
Name        : pps.c
Author      : Seyedsajad Hosseini
Version     : 1.0
Description : Sets up the server responsible for searching a pokemon database.
Assumption  : Contains constants from constants.h
 ============================================================================================
 */

#include "serverUtil.h"
#include "constants.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>


/**
 * Start the server, listens to clients, and responds.
 *
 * This function creates a server socket based on TCP protocol,
 * waits for clients to connect, and responds to their query.
 * If the query results in a string of pokemon records, the string
 * is sent to the client. Otherwise, a NOT_FOUND message is sent.
 *
 */
int main(){

    int                 serverSocket, clientSocket;
    struct sockaddr_in  clientAddr;
    int                 addrSize, bytesRcv;
    char                searchWord[SEARCH_WORD_SIZE];
    char                *response;

    // Set up the server
    serverSocket = setupServer();
    if (serverSocket < 0)
        exit(EXIT_FAILURE);

    // Open pokemon file to search queries
    FILE *inFile = openPokemonFile();
    if (!inFile)
        exit(EXIT_FAILURE);

    // Wait for clients
    while (1) {
        addrSize = sizeof(clientAddr);
        clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &addrSize);
        if (clientSocket < 0) {
            printf("SERVER ERROR: Could accept incoming client connection.\n");
            exit(EXIT_FAILURE);
        }

        // Go into infinite loop to talk to client
        while (1) {
            // Get the message from the client
            bytesRcv = recv(clientSocket, searchWord, sizeof(searchWord), 0);
            searchWord[bytesRcv] = '\0';

            // Disconnect gracefully if the client sends "disconnect"
            if (strcmp(searchWord, DISCONNECT) == 0)
                break;

            // Search pokemon file to find query lines
            response = searchPokemons(searchWord, inFile);
            // Send the query lines to the client
            if (response)
                send(clientSocket, response, strlen(response), 0);
            // Send a special message if no pokemon is found
            else
                send(clientSocket, NOT_FOUND, strlen(NOT_FOUND), 0);
        }
    }

}

