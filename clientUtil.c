/*
 ===============================================================================================================
Name        : clientUtil.c
Author      : Seyedsajad Hosseini
Version     : 1.0
Description : Contains client side functions.
Assumption  : Contains constants from constants.h
 ===============================================================================================================
 */

// Includes
#include "clientUtil.h"
#include "constants.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <netinet/in.h>
#include <arpa/inet.h>


/**
 * Print a menu for the client
 *
 * This function prints to the console a menu of options for the client.
 *
 */
void printMenu() {
    printf("Choose one of the following options (type the letter):\n");
    printf("\ta.Type search\n");
    printf("\tb.Save results\n");
    printf("\tc.Exit the program\n");
}


/**
 * Close a file
 *
 * This function closes a file. If unsuccessful, prints a message and exits.
 *
 * @param   file a pointer to the FILE to be closed
 */
void closeFile(FILE *file) {
    if (fclose(file) == EOF) {
        printf("ERROR: Could not close file \n");
        exit(EXIT_FAILURE);
    }
}


/**
 * Open a file using the name entered in console
 *
 * This function prompts the user for a file name, opens the
 * file for appending data, and returns FileType containing
 * a FILE pointer to the opened file and the file name
 *
 * @returns a FILEType struct containing a FILE pointer to the opened file
 */
FileType openFileToWrite() {
    FileType    outFile;
    FILE        *filePtr;
    char        fileName[FILE_NAME_SIZE];

    // Prompt the user for file name
    printf("Please enter a file name for saving queries: ");
    scanf("%s", fileName);
    filePtr = fopen(fileName, "w");

    // Get file name from the user until it is valid
    while (!filePtr) {
        printf("Unable to create the new file. Please enter the name of the file again.\n");
        scanf("%s", fileName);
        filePtr = fopen(fileName, "w");
    }

    outFile.filePtr = filePtr;
    strcpy(outFile.fileName, fileName);

    return outFile;
}


/**
 * Print the names of files used for saving query responses
 *
 * This function loops through savedFilesNames attribute of a
 * QueryType variable and prints them to the console. If no
 * file was saved during the process, a message is shown.
 *
 * @param   queries a QueryTpe variable containing names of saved file
 */
void printFileNames(QueryType queries) {
    if (queries.savedFileCount > 0) {
        printf("The following files were saved: \n");
        for (int i = 0; i < queries.savedFileCount; ++i) {
            printf("\t%s\n", queries.savedFilesNames[i]);
        }
    } else {
        printf("No File was saved\n");
    }
}


/**
 * Free dynamically allocated memory of a QueryTpe variable
 *
 * This function frees dynamically allocated memory for a QueryType
 * variable. This include queryLines and savedFilesNames attributes.
 *
 * @param   queries a QueryTpe variable
 */
void freeMemory(QueryType *queries) {
    // free lines of queries
    for (int i = 0; i < queries->queryLinesCount; ++i) {
        free(queries->queryLines[i]);
    }
    free(queries->queryLines);
    // free file names
    for (int i = 0; i < queries->savedFileCount; ++i) {
        free(queries->savedFilesNames[i]);
    }
    free(queries->savedFilesNames);
}

/**
 * Write saved query lines to file
 *
 * This function writes saved query records to a file and closes the file.
 * File pointer and query records are read from a QueryType variable.
 * As this variable is shared between threads, mutex is used to synchronize
 * access. If mutex locking or unlocking process fails, the program is exited.
 *
 * @param   queries a QueryTpe variable
 */
int writeQueriesToFile(QueryType *queries) {

    // lock mutex
    if (pthread_mutex_lock(&mutex) != 0) {
        perror("pthread_mutex_lock");
        exit(EXIT_FAILURE);
    }

    // write saved query lines to the file
    for (int i = 0; i < queries->queryLinesCount; i++) {
        size_t len = strlen(queries->queryLines[i]);
        fwrite(queries->queryLines[i], sizeof(char), len, queries->outFile->filePtr);
    }

    closeFile(queries->outFile->filePtr);

    // unlock mutex
    if (pthread_mutex_unlock(&mutex) != 0) {
        perror("pthread_mutex_unlock");
        exit(EXIT_FAILURE);
    }

    return 0;
}

/**
 * Add a file name to the list of file names in a QueryType variable
 *
 * This function checks if a file name is not among saved file names of
 * a QueryType variable. If it is, returns to the calling function.
 * Otherwise, allocates memory for a new file name and adds it to the
 * list of saved names in the QueryType variable.
 *
 * @param   queries a QueryTpe variable
 * @param   fileName a string to be added to the savedFileNames of queries
 */
void saveFileName(QueryType *queries, char *fileName) {
    // if file name already exists, quit
    for (int i = 0; i < queries->savedFileCount; ++i) {
        if (strcmp(queries->savedFilesNames[i], fileName) == 0)
            return;
    }

    queries->savedFileCount++;
    // allocate memory for a new file name and save it
    queries->savedFilesNames = realloc(queries->savedFilesNames,
                                       queries->savedFileCount * sizeof(char *));
    queries->savedFilesNames[queries->savedFileCount - 1] =
            malloc((strlen(fileName) + 1) * sizeof(char));
    strcpy(queries->savedFilesNames[queries->savedFileCount - 1], fileName);
}

/**
 * Connect to a server socket using TCP protocol
 *
 * This function creates a socket for a client, and connects to the server.
 * If socket creation or connection is unsuccessful, program is exited.
 * Otherwise, the integer representation of socket is returned.
 *
 * @returns an int value representing client socket file descriptor
 */
int connectToServer(){
    int                 clientSocket;
    struct sockaddr_in  serverAddress;
    int                 status;

    // Create socket
    clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket < 0) {
        printf("Unable to establish connection to the PPS!\n");
        exit(EXIT_FAILURE);
    }

    // Setup address
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr(SERVER_IP);
    serverAddress.sin_port = htons((unsigned short) SERVER_PORT);

    // Connect to server
    status = connect(clientSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress));
    if (status < 0) {
        printf("Unable to establish connection to the PPS!\n");
        exit(EXIT_FAILURE);
    }

    return clientSocket;
}

/**
 * Save query response returned by server in the queryLines attribute of
 * a QueryType variable
 *
 * This function enters a loop to receive data from server.
 * If the received string is equal to a pre-defined message, the loop is
 * broken. Otherwise, the loop continues receiving strings of size
 * (MAX_BUFFER_SIZE - 1) from the server and saving them in queryLines
 * attribute of a QueryType variable. When the length of received string
 * is smaller than the full size, the server is done sending data and
 * the loop is broken.
 *
 * @param   queries a QueryType variable to save query response
 * @param   clientSocket client socket address used to receive data from server
 */
void saveQueryResponse(QueryType *queries, int clientSocket){

    int          bytesRcv;
    char         response[MAX_BUFFER_SIZE];

    // increment queryCount
    queries->queryCount++;
    // Get response from server
    while(1) {
        bytesRcv = recv(clientSocket, response, MAX_BUFFER_SIZE - 1, 0);
        if (strcmp(response, NOT_FOUND) == 0)
            break;
        if (bytesRcv > 0) {
            // null-terminate string
            response[bytesRcv] = '\0';
            // Dynamically allocate memory for the received data and add it to queryLines
            queries->queryLinesCount++;
            queries->queryLines = realloc(queries->queryLines, queries->queryLinesCount * sizeof(char *));
            queries->queryLines[queries->queryLinesCount - 1] = strdup(response);
        }
        // break the loop if there is no more response to get from server
        if (bytesRcv < (MAX_BUFFER_SIZE - 1))
            break;
    }
}


/**
 * Search thread function to send query and save server response using saveQueryResponse()
 *
 * This function receives a ThreadDataType variable and sends its searchWord
 * to the server. Then, puts the saveQueryResponse() function in a mutex
 * to synchronize access to queries attribute of the ThreadDataType variable.
 * If mutex locking or unlocking process fails, the program is exited.
 *
 * @param   p a pointer to a ThreadDataType variable
 * @returns an integer to show function success
 */
void *searchThreadFunc(void *p){

    ThreadDataType *threadData = (ThreadDataType *) p;

    // Send search string to server
    send(threadData->clientSocket, threadData->searchWord, strlen(threadData->searchWord), 0);

    // Do not wait for response when disconnecting
    if (strcmp(threadData->searchWord, DISCONNECT) == 0)
        return 0;

    // lock mutex
    if (pthread_mutex_lock(&mutex) != 0) {
        perror("pthread_mutex_lock");
        exit(EXIT_FAILURE);
    }

    // Receive and save server response
    saveQueryResponse(threadData->queries, threadData->clientSocket);

    // lock mutex
    if (pthread_mutex_unlock(&mutex) != 0) {
        perror("pthread_mutex_lock");
        exit(EXIT_FAILURE);
    }

    return 0;
}


/**
 * Save thread function to write queries to file and save file name using
 * writeQueriesToFile() and saveFileName() functions
 *
 * This function receives a QueryType variable and calls writeQueriesToFile()
 * and saveFileName().
 *
 * @param   p a pointer to a QueryType variable
 * @returns an integer to show function success
 */
void *saveThreadFunc(void *p) {

    QueryType           *queries;

    queries = (QueryType *) p;

    // Write Pokemon records to file
    writeQueriesToFile(queries);
    // Save file name
    saveFileName(queries, queries->outFile->fileName);

    return 0;
}
