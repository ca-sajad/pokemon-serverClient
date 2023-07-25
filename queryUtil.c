/*
 ===============================================================================================================
Name        : queryUtil.c
Author      : Seyedsajad Hosseini
Version     : 1.0
Description : Contains functions for working on user queries.
Assumption  : -
 ===============================================================================================================
 */

// includes
#include "fileUtil.h"
#include "queryType.h"
#include <ctype.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// constants
#define COMMA ","

// global variables
volatile QueryType queries;
pthread_mutex_t mutex1;
pthread_mutex_t mutex2;

/* prints name of the files used for saving queries */
void printFileNames() {
    if (queries.savedFileCount > 0) {
        printf("The following files were saved: \n");
    for (int i = 0; i < queries.savedFileCount; ++i) {
        printf("\t%s\n", queries.savedFilesNames[i]);
    }
    } else {
        printf("No File was saved\n");
    }
}

/* free dynamically allocated memory */
void freeMemory() {
    // free lines of queries
    for (int i = 0; i < queries.queryLinesCount; ++i) {
        free(queries.queryLines[i]);
    }
    free(queries.queryLines);
    // free file names
    for (int i = 0; i < queries.savedFileCount; ++i) {
        free(queries.savedFilesNames[i]);
    }
    free(queries.savedFilesNames);
}

/* closes a file, prints error if unsuccessful */
void *saveQueries() {
    // lock mutex
    if (pthread_mutex_lock(&mutex2) != 0) {
        perror("pthread_mutex_lock");
        exit(1);
    }
    // write saved query lines to the file
    for (int i = 0; i < queries.queryLinesCount; i++) {
        size_t len = strlen(queries.queryLines[i]);
        fwrite(queries.queryLines[i], sizeof(char), len, queries.outFile);
    }
    closeFile(queries.outFile);
    // unlock mutex
    if (pthread_mutex_unlock(&mutex2) != 0) {
        perror("pthread_mutex_unlock");
        exit(1);
    }

    return 0;
}

/* save a file name */
void saveFileName(char *fileName) {
    // if file name already exists, quit
    for (int i = 0; i < queries.savedFileCount; ++i) {
        if (strcmp(queries.savedFilesNames[i], fileName) == 0)
          return;
    }

    queries.savedFileCount++;
    // allocate memory for a new file name and save it
    queries.savedFilesNames =
      realloc(queries.savedFilesNames, queries.savedFileCount * sizeof(char *));
    queries.savedFilesNames[queries.savedFileCount - 1] =
      malloc((strlen(fileName) + 1) * sizeof(char));
    strcpy(queries.savedFilesNames[queries.savedFileCount - 1], fileName);
}

/* search Pokemon database for a "type 1" string */
void *searchPokemons() {
    // convert letters of the "type 1" search string to lowercase
    char *searchWord = strdup(queries.searchWord);
    for (int i = 0; searchWord[i]; i++) {
        searchWord[i] = tolower(searchWord[i]);
    }

    // lock mutex
    if (pthread_mutex_lock(&mutex1) != 0) {
        perror("pthread_mutex_lock");
        exit(1);
    }

    char *line = NULL;
    size_t lineSize = 0;
    char *field;
    // read the title row
    getline(&line, &lineSize, queries.inFile);

    while (getline(&line, &lineSize, queries.inFile) != -1) { // read one line

        // copy the line to a new variable, because line is modified using strtok()
        char *lineCopy = malloc(strlen(line) + 1);
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
          queries.queryLinesCount++;
          queries.queryLines =
              realloc(queries.queryLines, queries.queryLinesCount * sizeof(char *));
          queries.queryLines[queries.queryLinesCount - 1] =
              malloc((strlen(lineCopy) + 1) * sizeof(char));
          strcpy(queries.queryLines[queries.queryLinesCount - 1], lineCopy);
        }

        free(lineCopy);
    }

    // increment query counter
    queries.queryCount++;
    // free dynamically allocated memory
    free(line);
    free(searchWord);

    // reset the file position indicator to read the file again
    rewind(queries.inFile);

    // unlock mutex
    if (pthread_mutex_unlock(&mutex1) != 0) {
        perror("pthread_mutex_unlock");
        exit(1);
    }

    return 0;
}