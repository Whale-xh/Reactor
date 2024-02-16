#include "EventLoop.h"
#include <unistd.h>
#include <sys/eventfd.h>
#include <sys/epoll.h>
#include <iostream>

using std::cout;
using std::endl;

EventLoop::EventLoop(Acceptor &acceptor)
: _epfd(createEpollFd())
, _evtfd(createEventFd())
, _acceptor(acceptor)
, _isLooping(false)
, _eventList(1024)
{
    addEpollReadFd(_acceptor.fd()); //listenfd加入监听
    addEpollReadFd(_evtfd);  //eventfd加入监听
}

EventLoop::~EventLoop()
{
    if(_epfd)
    {
        close(_epfd);
    }
    if(_evtfd)
    {
        close(_evtfd);
    }
}

void EventLoop::loop()
{
    _isLooping=true;
    while(_isLooping)
    {
        waitEpollFd();
    }
}

void EventLoop::unloop()
{
    _isLooping=false;
}

// using Functor = function<void()>;
void EventLoop::runInLoop(Functor &&cb)
{
    {
        MutexLockGuard autoLock(_mutex);
        _pendingsCb.push_back(std::move(cb));
    }
    wakeup();
}

void EventLoop::doPendingFunctors()
{
    vector<Functor> tmp;
    {
        MutexLockGuard autoLock(_mutex);
        tmp.swap(_pendingsCb);
    }
    for(auto &cb:tmp)
    {
        cb();
    }
}

void EventLoop::wakeup()
{
    uint64_t one = 1;
    int ret = ::write(_evtfd,&one,sizeof(one));
    if(ret != sizeof(one))
    {
        perror("EventLoop::wakeup : write");
        exit(EXIT_FAILURE);
    }
}

void EventLoop::handleRead()
{
    uint64_t one = 1;
    int ret = ::read(_evtfd,&one,sizeof(one));
    if(ret != sizeof(one))
    {
        perror("EventLoop::handleRead : read");
        exit(EXIT_FAILURE);
    }
}

void EventLoop::waitEpollFd()
{
    int nready = -1;
    do
    {
        nready = ::epoll_wait(_epfd,&*_eventList.begin(),_eventList.size(),2000);
    }while(-1 == nready && errno ==EINTR);
    if(-1 == nready)
    {
        perror("EventLoop::waitEpollFd : epoll_wait");
        exit(EXIT_FAILURE);
    }
    else if(0 ==nready)
    {
        cout << ">>epoll_wait timeout" << endl;
    }
    else
    {
        if(nready == (int)_eventList.size())
        {
            _eventList.resize(2 * nready);
        }

        for(int idx=0;idx<nready;++idx)
        {
            int fd = _eventList[idx].data.fd;
            if(fd == _acceptor.fd()) //有新的连接请求
            {
                if(_eventList[idx].events & EPOLLIN)
                {
                    handleNewConnection();
                }
            }
            else if(fd ==_evtfd)
            {
                handleRead();
                doPendingFunctors();
            }
            else
            {
                if(_eventList[idx].events & EPOLLIN)
                {
                    handleMessage(fd);
                }
            }
        }
    }
}

void EventLoop::handleNewConnection()
{
    int peerfd = _acceptor.accept();
    addEpollReadFd(peerfd);
    TcpConnectionPtr con(new TcpConnection(peerfd,this));
    con->setConnectionCb(_onConnectionCb);
    con->setMessageCb(_onMessageCb);
    con->setCloseCb(_onCloseCb);

    _conns.insert(std::make_pair(peerfd,con));
    con->handleConnectionCb();
}

void EventLoop::handleMessage(int fd)
{
    auto iter = _conns.find(fd);
    if(iter == _conns.end())
    {
        cout << "该连接不存在" << endl;
    }
    else
    {
        bool flag = iter->second->isClosed();
        if(flag)
        {
            //连接断开
            iter->second->handleCloseCb();
            delEpollReadFd(fd);
            _conns.erase(iter);
        }
        else
        {
            //连接正常
            iter->second->handleMessageCb();
        }
    }
}

int EventLoop::createEpollFd()
{
    int fd = epoll_create(1);
    if(-1 == fd)
    {
        perror("epoll_create");
        return -1;
    }
    return fd;
}

void EventLoop::addEpollReadFd(int fd)
{
    struct epoll_event evt;
    evt.data.fd = fd;
    evt.events = EPOLLIN;

    int ret = ::epoll_ctl(_epfd,EPOLL_CTL_ADD,fd,&evt);
    if(-1 == ret)
    {
        perror("epoll_ctl_add");
        exit(EXIT_FAILURE);
    }
}

void EventLoop::delEpollReadFd(int fd)
{
    struct epoll_event evt;
    evt.data.fd = fd;
    evt.events = EPOLLIN;

    int ret = ::epoll_ctl(_epfd,EPOLL_CTL_DEL,fd,&evt);
    if(-1 == ret)
    {
        perror("epoll_ctl_del");
        exit(EXIT_FAILURE);
    }
}

int EventLoop::createEventFd()
{
    int fd = eventfd(0,0);
    if(-1 == fd)
    {
        perror("eventfd");
        return fd;
    }
    return fd;
}

void EventLoop::setConnectionCb(TcpConnectionCallback &&cb)
{
    _onConnectionCb = std::move(cb);
}

void EventLoop::setMessageCb(TcpConnectionCallback &&cb)
{
    _onMessageCb = std::move(cb);
}

void EventLoop::setCloseCb(TcpConnectionCallback &&cb)
{
    _onCloseCb = std::move(cb);
}