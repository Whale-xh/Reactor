#ifndef __CONDITION_H__
#define __CONDITION_H__

#include "NoCopyable.h"
#include <pthread.h>

class MutexLock;

class Condition
: NoCopyable
{
public:
    Condition(MutexLock &mutex);
    ~Condition();
    void wait();
    void notify();
    void notifyAll();
public:
    pthread_cond_t _cond;
    MutexLock &_mutex;
};

#endif