/*
 ===============================================================================================================
Name        : fileUtil.c
Author      : Seyedsajad Hosseini
Version     : 1.0
Description : Contains functions for opening and closing files needed for assignment3.c
Assumption  : -
 ===============================================================================================================
 */

// includes
#include "fileUtil.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* closes a file, prints error if unsuccessful */
void closeFile(FILE *file) {
  if (fclose(file) == EOF) {
    printf("ERROR: Could not close file \n");
    exit(1);
  }
}

/* prompts the user for Pokemon database and opens the file for reading
 returns a FILE pointer to the opened file
 */
FILE *openPokemonFile() {
//  char inFileName[100];
//  printf("Please enter the name of pokemon file: ");
//  scanf("%s", inFileName);

  char *inFileName = "pokemon.csv";

  FILE *inFile = fopen(inFileName, "r");
  while (!inFile) {
    printf("Pokemon file is not found. Please enter the name of the file again "
           "or \"q\" to exit\n");
    scanf("%s", inFileName);
    if (!strcmp(inFileName, "q"))
      return 0;
    inFile = fopen(inFileName, "r");
  }

  return inFile;
}

/* prompts the user for a file name and opens the file for appending data
 returns a FileType containing a FILE pointer to the opened file and file name
 */
FileType openOutFile() {
  printf("Please enter a file name for saving queries: ");
  char fileName[100];
  scanf("%s", fileName);

  FILE *filePtr = fopen(fileName, "a");
  while (!filePtr) {
    printf("Unable to create the new file. Please enter the name of the "
           "file again.\n");
    scanf("%s", fileName);
    filePtr = fopen(fileName, "r");
  }

  FileType outFile;
  outFile.filePtr = filePtr;
  strcpy(outFile.fileName, fileName);

  return outFile;
}
