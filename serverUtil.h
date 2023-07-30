/*
 ===============================================================================================================
Name        : serverUtil.h
Author      : Seyedsajad Hosseini
Version     : 1.0
Description : Contains functions prototypes for serverUtil.c
Assumption  : -
 ===============================================================================================================
 */

#ifndef SRVFILEUTIL_H
#define SRVFILEUTIL_H

#include <stdio.h>

int setupServer();
FILE *openPokemonFile();
char *searchPokemons(char *,  FILE *);

#endif
