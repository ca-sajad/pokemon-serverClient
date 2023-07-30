/*
 ============================================================================================
Name        : pqc.c
Author      : Seyedsajad Hosseini
Version     : 1.0
Description : Sets up the client socket and provides functionality for the user options.
              The user can search for a "type 1" word in pokemon database, save the results
              of the search, or exit the program.
              To provide a seamless experience, a multithreading approach is used.
Assumption  : Contains constants from constants.h
 ============================================================================================
 */


#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <pthread.h>
#include "clientUtil.h"
#include "constants.h"


/**
 * Open the client socket, send queries, receive responses, and
 * save data to file
 *
 * This function creates a client socket based on TCP protocol,
 * prompts the user for an option between sending a search word
 * to the server, saving the results of the sent queries to a file
 * determined by the user, and exiting the program.
 * The function employs threads to improve user experience.
 */
int main() {

    // Declare variables
    char                searchWord[SEARCH_WORD_SIZE];
    char                option = 'd';
    int                 clientSocket;
    FileType            outFile;
    pthread_t           thread1, thread2;
    int                 threadResult1 = 1, threadResult2 = 1;
    ThreadDataType      threadData;
    QueryType           queries;

    // initialize a QueryType variable
    queries.queryCount = 0;
    queries.queryLines = NULL;
    queries.queryLinesCount = 0;
    queries.savedFilesNames = NULL;
    queries.outFile = NULL;
    queries.savedFileCount = 0;

    // Initialize mutexe
    if (pthread_mutex_init(&mutex1, NULL) != 0) {
        perror("pthread_mutex_init");
        exit(EXIT_FAILURE);
    }

    while (option != 'c') {
        printMenu();
        scanf(" %c", &option);      // receives user input
        int c;
        while ((c = getchar()) != '\n' && c != EOF) {}

        switch (option) {

            // Search the Pokemon file for a "type 1" string
            case 'a':
                printf("Please enter a type 1 property to search for: ");
                scanf("%19s", searchWord);
                while ((c = getchar()) != '\n' && c != EOF) {}

                // Set up socket if this is the first query
                if (queries.queryCount == 0) {
                    clientSocket = connectToServer();
                    threadData.clientSocket = clientSocket;
                    threadData.queries = &queries;
                }
                strcpy(threadData.searchWord, searchWord);

                if (threadResult1 == 0)
                    pthread_join(thread1, NULL);

                // Create the thread for sending the search string to the server and saving result
                threadResult1 = pthread_create(&thread1, NULL, thread1Function, &threadData);
                if (threadResult1 != 0) {
                    printf("Error creating thread.\n");
                    exit(EXIT_FAILURE);
                }

                break;

            // Save the results of previous queries
            case 'b':
                if (queries.queryLinesCount > 0) {
                    if (threadResult2 == 0)
                        pthread_join(thread2, NULL);

                    // Open a file to write data
                    outFile = openOutFile();
                    queries.outFile = &outFile;

                    // Create the thread for sending the search string to the server and saving result
                    threadResult2 = pthread_create(&thread2, NULL, thread2Function, &queries);
                    if (threadResult2 != 0) {
                        printf("Error creating thread.\n");
                        exit(EXIT_FAILURE);
                    }
                } else {
                    printf("No results to save\n");
                }
                break;

            // Disconnect from server and exit the program
            case 'c':
                // Wait for threads to finish their tasks
                if (threadResult1 == 0)
                    pthread_join(thread1, NULL);
                if (threadResult2 == 0)
                    pthread_join(thread2, NULL);
                // Disconnect from server
                send(clientSocket, DISCONNECT, strlen(DISCONNECT), 0);
                close(clientSocket);
                // printing a summary of queries and saved files
                printf("Number of successful queries: %d\n", queries.queryCount);
                printFileNames(queries);
                // cleaning up
                freeMemory(&queries);
                break;
        }
    }

    return EXIT_SUCCESS;
}

