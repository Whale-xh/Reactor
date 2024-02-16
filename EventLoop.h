#ifndef __EVENTLOOP_H__
#define __EVENTLOOP_H__

#include "MutexLock.h"
#include "Acceptor.h"
#include "TcpConnection.h"
#include <vector>
#include <map>
#include <functional>

using std::function;
using std::map;
using std::vector;
using std::shared_ptr;

using TcpConnectionPtr = shared_ptr<TcpConnection>;
using TcpConnectionCallback = function<void(const TcpConnectionPtr &)>;
using Functor = function<void()>;

class EventLoop
{
public:
    EventLoop(Acceptor &acceptor);
    ~EventLoop();
    void loop();
    void unloop();
    void runInLoop(Functor &&cb);
    void wakeup();
    void handleRead();
    void doPendingFunctors();
    void setConnectionCb(TcpConnectionCallback &&cb);
    void setMessageCb(TcpConnectionCallback &&cb);
    void setCloseCb(TcpConnectionCallback &&cb);

private:
    void waitEpollFd();
    void handleNewConnection();
    void handleMessage(int fd);
    int createEpollFd();
    void addEpollReadFd(int fd);
    void delEpollReadFd(int fd);
    int createEventFd();

private:
    int _epfd;
    int _evtfd;
    Acceptor &_acceptor;
    bool _isLooping;
    vector<struct epoll_event> _eventList;
    map<int,TcpConnectionPtr> _conns;
    
    TcpConnectionCallback _onConnectionCb;
    TcpConnectionCallback _onMessageCb;
    TcpConnectionCallback _onCloseCb;
    vector<Functor> _pendingsCb;
    MutexLock _mutex;

};

#endif