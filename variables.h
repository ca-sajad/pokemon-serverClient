/*
 ===============================================================================================================
Name        : queryUtil.c
Author      : Seyedsajad Hosseini
Version     : 1.0
Description : Contains global variables
Assumption  : -
 ===============================================================================================================
 */

#ifndef VARIABLES_H
#define VARIABLES_H

#include "queryType.h"
#include <semaphore.h>

extern volatile QueryType queries;
extern pthread_mutex_t mutex1;
extern pthread_mutex_t mutex2;

#endif