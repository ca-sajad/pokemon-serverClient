/*
 ===============================================================================================================
Name        : queryType.h
Author      : Seyedsajad Hosseini
Version     : 1.0
Description : Defines a QueryType struct for saving query related data
Assumption  : -
 ===============================================================================================================
 */


#ifndef QUERYTYPE_H
#define QUERYTYPE_H

#include <stdio.h>

typedef struct {
    int queryCount;
    int queryLinesCount;
    int savedFileCount;
    char searchWord[20];
    char **queryLines;
    char **savedFilesNames;
    FILE *inFile;
    FILE *outFile;
} QueryType;

#endif