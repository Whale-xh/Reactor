#include "ThreadPool.h"

ThreadPool::ThreadPool(size_t threadNum,size_t queSize)
: _threadNum(threadNum)
, _queSize(queSize)
, _taskQue(_queSize)
, _isExit(false)
{
    _threads.reserve(_threadNum);
}

ThreadPool::~ThreadPool()
{
    if(!_isExit)
    {
        stop();
        _isExit = true;
    }
}

//线程池开始执行的时候，其实就是工作线程已经开启
void ThreadPool::start()
{
    for(int idx = 0; idx < _threadNum; ++idx)
    {
        unique_ptr<Thread> up(new Thread(std::bind(&ThreadPool::threadFunc,this)));
        _threads.push_back(std::move(up));
    }

    for(auto &th: _threads)
    {
        th->start();   //创建线程id，并运行所有工作线程
    }
}

void ThreadPool::addTask(Task &&task)
{
    if(task)
    {
        _taskQue.push(std::move(task));
    }
}

Task ThreadPool::getTask()
{
    return _taskQue.pop();
}

//线程池的退出就是线程池中的工作线程退出
void ThreadPool::stop()
{
    //只要任务队列中有数据，线程池中的工作线程就不能退出
    while(!_taskQue.empty());
    _isExit = true;
    _taskQue.wakeup();
    for(auto &th: _threads)
    {
        th->join();
    }
}

//线程池中封装的任务
void ThreadPool::threadFunc()
{
    //只要线程池没有退出，就可以一直获取任务，并执行相应的process
    while(!_isExit)
    {
        Task taskcb = getTask();
        if(taskcb)
        {
            taskcb();
        }
    }
}