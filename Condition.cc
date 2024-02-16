#include "Condition.h"
#include "MutexLock.h"
#include <stdio.h>
#include <stdlib.h>

Condition::Condition(MutexLock &mutex)
:_mutex(mutex)
{
    int ret = pthread_cond_init(&_cond,nullptr);
    if(ret)
    {
        perror("pthread_cond_init");
        exit(EXIT_FAILURE);
    }
}

Condition::~Condition()
{
    int ret = pthread_cond_destroy(&_cond);
    if(ret)
    {
        perror("pthread_cond_destroy");
        exit(EXIT_FAILURE);
    }
}

void Condition::wait()
{
    int ret = pthread_cond_wait(&_cond,_mutex.getMutexLockPtr());
    if(ret)
    {
        perror("pthread_cond_wait");
        exit(EXIT_FAILURE);
    }
}

void Condition::notify()
{
    pthread_cond_signal(&_cond);
}

void Condition::notifyAll()
{
    pthread_cond_broadcast(&_cond);
}