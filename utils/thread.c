#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "utils/log.h"
#include "utils/thread.h"

int OpenThread(pthread_t* ptThreadID, const void* pThreadHandler, void* pThreadParam)
{
    int returnValue;

    /*Create thread with default attributes*/

    returnValue = pthread_create(ptThreadID, (pthread_attr_t *) NULL, (void *) pThreadHandler, (void *) pThreadParam);
    if (returnValue != 0)
    {
        printf("ERROR in pthread_create\n");
    }
    return returnValue;
}/* end of OpenThread*/

