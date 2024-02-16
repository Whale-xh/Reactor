#include "MutexLock.h"
#include <stdio.h>
#include <stdlib.h>

MutexLock::MutexLock()
{
    int ret = pthread_mutex_init(&_mutex,nullptr);
    if(ret)
    {
        perror("pthread_mutex_init");
        exit(EXIT_FAILURE);
    }
}

MutexLock::~MutexLock()
{
    int ret = pthread_mutex_destroy(&_mutex);
    if(ret)
    {
        perror("pthread_mutex_destroy");
        exit(EXIT_FAILURE);
    }
}

void MutexLock::lock()
{
    int ret = pthread_mutex_lock(&_mutex);
    if(ret)
    {
        perror("pthread_mutex_lock");
        exit(EXIT_FAILURE);
    }
}

void MutexLock::unlock()
{
    int ret = pthread_mutex_unlock(&_mutex);
    if(ret)
    {
        perror("pthread_mutex_unlock");
        exit(EXIT_FAILURE);
    }
}

pthread_mutex_t *MutexLock::getMutexLockPtr()
{
    return &_mutex;
}