/*
 ===============================================================================================================
Name        : clientUtil.h
Author      : Seyedsajad Hosseini
Version     : 1.0
Description : Contains functions prototypes for clientUtil.c
              Defines structs used in client side files.
              Declares a global mutex.
Assumption  : Contains constants from constants.h
 ===============================================================================================================
 */

#ifndef CLTFILEUTIL_H
#define CLTFILEUTIL_H

#include <stdio.h>
#include <pthread.h>
#include "constants.h"

// Global variable
extern pthread_mutex_t mutex;

// Define structures
typedef struct {
    FILE *filePtr;
    char fileName[FILE_NAME_SIZE];
} FileType;

typedef struct {
    int         queryCount;
    int         queryLinesCount;
    char        **queryLines;
    int         savedFileCount;
    char        **savedFilesNames;
    FileType    *outFile;
} QueryType;

typedef struct {
    char        searchWord[SEARCH_WORD_SIZE];
    int         clientSocket;
    QueryType   *queries;
} ThreadDataType;

// Function Prototypes
void printMenu();
void closeFile(FILE *);
FileType openFileToWrite();
int writeQueriesToFile(QueryType *queries);
void freeMemory(QueryType *);
void saveFileName(QueryType *, char *);
void printFileNames(QueryType);
int connectToServer();
void saveQueryResponse(QueryType *queries, int clientSocket);
void *searchThreadFunc(void *);
void *saveThreadFunc(void *);

#endif
