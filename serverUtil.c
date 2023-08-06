/*
 ===============================================================================================================
Name        : serverUtil.c
Author      : Seyedsajad Hosseini
Version     : 1.0
Description : Contains functions for server side of pokemon query project
Assumption  : Contains constants from constants.h
 ===============================================================================================================
 */

#include "serverUtil.h"
#include "constants.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>


/**
 * Create and returns a server socket.
 *
 * This function creates a server socket based on TCP protocol.
 *
 * @returns an int representing server socket.
 */
int setupServer(){

    int                 serverSocket;
    struct sockaddr_in  serverAddress;
    int                 status;

    // Create the server socket
    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket < 0) {
        printf("SERVER ERROR: Could not open socket.\n");
        exit(-1);
    }

    // Setup the server address
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddress.sin_port = htons((unsigned short) SERVER_PORT);

    // Bind the server socket
    status = bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
    if (status < 0) {
        printf("SERVER ERROR: Could not bind socket.\n");
        exit(-1);
    }

    // Set up the line-up to handle up to CLIENTS_SIZE number of clients in line
    status = listen(serverSocket, CLIENTS_QUEUE_SIZE);
    if (status < 0) {
        printf("SERVER ERROR: Could not listen on socket.\n");
        exit(-1);
    }

    return serverSocket;
}


/**
 * Open a file containing pokemon records
 *
 * This function prompts the user for name of a pokemon file.
 * If the file can be opened, a FILE pointer is returned. Otherwise,
 * the user is prompted again.
 *
 * @returns FILE pointer of pokemon records file.
 */
FILE *openPokemonFile() {

    FILE     *inFile;

    // Get pokemon file name
    char inFileName[FILE_NAME_SIZE];
    printf("Please enter the name of pokemon file: ");
    scanf("%s", inFileName);
    inFile = fopen(inFileName, "r");

    // Get file name from the user until it is valid
    while (!inFile) {
        printf("Pokemon file is not found. Please enter the name of the file again "
               "or \"q\" to exit\n");
        scanf("%s", inFileName);
        if (!strcmp(inFileName, "q"))
            break;
        inFile = fopen(inFileName, "r");
    }

    return inFile;
}


/**
 * Search Pokemon database for a "type 1" string
 *
 * This function reads the pokemon database file, finds pokemon records
 * that match a "type 1" pokemon search word, adds them to a string
 * and return the string.
 *
 * @param   searchWord a string to be searched in "type 1" field of pokemon records
 * @param   file       file containing pokemon records
 * @returns a dynamically allocated string containing the search results,
 *         or NULL if no matching records are found or there's an error
 *         during the search.
 */
char *searchPokemons(char *searchWord,  FILE *file) {

    char       *line = NULL;
    size_t     lineSize = 0;
    char       *lineCopy;
    char       *field;
    int        queryLinesCount = 0;
    size_t     queryTotalLength = 0;
    char       *response = NULL;       // a string concatenating all pokemon records containing searchWord

    // convert letters of the "type 1" search string to lowercase
    for (int i = 0; searchWord[i]; i++) {
        searchWord[i] = tolower(searchWord[i]);
    }

    // read the title row
    getline(&line, &lineSize, file);

    while (getline(&line, &lineSize, file) != -1) { // read one line

        // copy the line to a new variable, as the line is modified using strtok()
        lineCopy = malloc(strlen(line) + 1);
        strcpy(lineCopy, line);

        // read and discard id and name fields
        field = strtok(line, COMMA);
        field = strtok(NULL, COMMA);
        // read "type 1" field
        field = strtok(NULL, COMMA);

        // convert letters of the "type 1" field to lowercase
        for (int i = 0; field[i]; i++) {
            field[i] = tolower(field[i]);
        }

        // if search is successful, allocate memory for the whole line and save it
        if (strcmp(field, searchWord) == 0) {
            queryLinesCount++;
            queryTotalLength += strlen(lineCopy);
            // reallocate memory to add another pokemon record
            response = realloc(response, queryTotalLength);
            if (response == NULL) {
                printf("Memory allocation failed.\n");
                exit(EXIT_FAILURE);
            }
            // add pokemon record to the end of found records
            strcat(response, lineCopy);
        }

        free(lineCopy);
    }

    // reset the file position indicator for next read
    rewind(file);

    // free dynamically allocated memory
    free(line);

    return response;
}