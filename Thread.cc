#include "Thread.h"
#include <stdio.h>
#include <stdlib.h>

Thread::Thread(ThreadCallback &&cb)
: _thid(0)
, _isRunning(false)
, _cb(std::move(cb))
{

}

Thread::~Thread()
{
    if(_thid)
    {
        int ret = pthread_detach(_thid);
        if(ret)
        {
            perror("pthread_detach");
            exit(EXIT_FAILURE);
        }
    }
}

void Thread::start()
{
    int ret = pthread_create(&_thid,nullptr,threadFunc,this);
    if(ret)
    {
        perror("pthread_create");
        exit(EXIT_FAILURE);
    }
    _isRunning = true;
}

void Thread::join()
{
    if(_isRunning)
    {
        int ret = pthread_join(_thid,nullptr);
        if(ret)
        {
            perror("pthread_join");
            exit(EXIT_FAILURE);
        }
        _isRunning = false;
    }
}

void *Thread::threadFunc(void *arg)
{
    Thread *pth = static_cast<Thread*>(arg);
    if(pth)
    {
        pth->_cb();  //回调函数的执行
    }
    pthread_exit(nullptr);
}