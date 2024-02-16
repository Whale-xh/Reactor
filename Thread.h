#ifndef __THREAD_H__
#define __THREAD_H__

#include <pthread.h>
#include <functional>

using std::bind;
using std::function;

using ThreadCallback = function<void()>;

class Thread
{
public:
    Thread(ThreadCallback &&cb);
    ~Thread();
    void start();
    void join();

private:
    static void *threadFunc(void *arg);

private:
    pthread_t _thid;
    bool _isRunning;
    ThreadCallback _cb; // 实现的任务
};

#endif