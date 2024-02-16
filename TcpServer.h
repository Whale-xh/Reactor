#ifndef __TCPSERVER_H__
#define __TCPSERVER_H__

#include "EventLoop.h"
#include "Acceptor.h"
#include "TcpConnection.h"
#include <iostream>
#include <unistd.h>

using std::cout;
using std::endl;

class TcpServer
{
public:
    TcpServer(const string &ip,unsigned short port)
    : _acceptor(ip,port)
    ,_loop(_acceptor)
    {

    }

    void start()
    {
        _acceptor.ready();
        _loop.loop();
    }

    void stop()
    {
        _loop.unloop();
    }

    void setAllCallback(TcpConnectionCallback &&onConnect,TcpConnectionCallback &&onMessage,TcpConnectionCallback &&onClose)
    {
        _loop.setConnectionCb(std::move(onConnect));
        _loop.setMessageCb(std::move(onMessage));
        _loop.setCloseCb(std::move(onClose));
    }
    
private:
    Acceptor _acceptor;
    EventLoop _loop;
};

#endif