/*
 ===============================================================================================================
Name        : fileUtil.h
Author      : Seyedsajad Hosseini
Version     : 1.0
Description : Contains functions prototypes for fileUtil.c
              Defines a FileType struct
Assumption  : -
 ===============================================================================================================
 */

#ifndef CLTFILEUTIL_H
#define CLTFILEUTIL_H

#include <stdio.h>

typedef struct {
  FILE *filePtr;
  char fileName[100];
} FileType;

void closeFile(FILE *);
FILE *openPokemonFile();
FileType openOutFile();

#endif
