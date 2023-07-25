/*
 ===============================================================================================================
Name        : assignment4.c
Author      : Seyedsajad Hosseini
Version     : 1.0
Description : The program prompts the user to enter the name a Pokemon.csv file. Then the user has 3 options:
                search for a Pokemons with a "type 1" that user enters, save the result of the search in a file
                that user determines, or exit the program.
                The program uses multithreading to provide a seamless user experience.
Assumption  : The order of fields in the Pokemon file is as the ones provided in the assignment specifications.
 ===============================================================================================================
 */

// includes
#include "fileUtil.h"
#include "queryUtil.h"
#include "variables.h"

#include <pthread.h>
#include <stdlib.h>

// function prototype
void printMenu();


int main() {

  FILE *inFile = openPokemonFile();

  // initialize a QueryType variable
  queries.inFile = inFile;
  queries.queryCount = 0;
  queries.queryLinesCount = 0;
  queries.savedFileCount = 0;

  // initializing mutexes
  if (pthread_mutex_init(&mutex1, NULL) != 0 | pthread_mutex_init(&mutex2, NULL) != 0) {
    perror("pthread_mutex_init");
    exit(1);
  }

  // declaring threads and local variables
  FileType outFile;
  pthread_t thread1, thread2;
  int result1 = 1, result2 = 1;
  char option = 'a';

  while (option != 'c') {
    printMenu();
    scanf(" %c", &option);      // receives user input
      int c;
      while ((c = getchar()) != '\n' && c != EOF){}

    switch (option) {

        case 'a':   // searching the Pokemon file for a "type 1" string
          printf("Please enter a type 1 property to search for: ");
          scanf("%s", queries.searchWord);
          // create a thread for executing a query
          result1 = pthread_create(&thread1, NULL, searchPokemons, NULL);
          if (result1 != 0) {
            printf("Error creating thread.\n");
            return 1;
          }
          break;

        case 'b':   // saving the results of previous queries
          if (queries.queryLinesCount > 0) {
            // ask the user for the name of the output file
            outFile = openOutFile();
            queries.outFile = outFile.filePtr;
              // create a thread for saving queries to a file
            result2 = pthread_create(&thread2, NULL, saveQueries, NULL);
            if (result2 != 0) {
              printf("Error creating thread.\n");
              return 1;
            }
            saveFileName(outFile.fileName);
          } else {
            printf("No queries have been saved.\n");
          }
          break;

        case 'c':   // exiting the program
            // waiting for threads to finish
          if(result1 == 0)
            pthread_join(thread1, NULL);
          if(result2 == 0)
            pthread_join(thread2, NULL);
          // printing a summary of queries and saved files
          printf("Number of successful queries: %d\n", queries.queryCount);
          printFileNames();
          // cleaning up
          freeMemory();
          closeFile(inFile);
          break;
    }
  }

  return 0;
}

/* prints the program options */
void printMenu() {
  printf("Choose one of the following options (type the letter):\n");
  printf("\ta.Type search\n");
  printf("\tb.Save results\n");
  printf("\tc.Exit the program\n");
}
