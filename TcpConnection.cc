#include "TcpConnection.h"
#include "EventLoop.h"
#include <iostream>
#include <sstream>

using std::cout;
using std::endl;
using std::ostringstream;

TcpConnection::TcpConnection(int fd,EventLoop *loop)
: _loop(loop)
, _sock(fd)
, _sockIO(fd)
, _localAddr(getLocalAddr())
, _peerAddr(getPeerAddr())
{

}

TcpConnection::~TcpConnection()
{

}

void TcpConnection::send(const string &msg)
{
    _sockIO.writen(msg.c_str(),msg.size());
}

void TcpConnection::sendInLoop(const string &msg)
{
    if(_loop)
    {
        _loop->runInLoop(std::bind(&TcpConnection::send,this,msg)); //IO操作放到EventLoop中执行
    }
}

string TcpConnection::receive()
{
    char buf[65536] = {0};
    _sockIO.readLine(buf,sizeof(buf));
    return string(buf);
}

string TcpConnection::toString()
{
    ostringstream oss;
    oss << _localAddr.ip() <<":"
        << _localAddr.port() << "---->"
        << _peerAddr.ip() << ":"
        << _peerAddr.port();
    return oss.str();
}

bool TcpConnection::isClosed() const
{
    char buf[10] = {0};
    int ret = ::recv(_sock.fd(),buf,sizeof(buf),MSG_PEEK);
    return (0==ret);
}

InetAddr TcpConnection::getLocalAddr()
{
    struct sockaddr_in addr;
    socklen_t len=sizeof(addr);
    int ret = getsockname(_sock.fd(),(struct sockaddr*)&addr,&len);
    if(-1==ret)
    {
        perror("getsockname");
    }
    return InetAddr(addr);
}

InetAddr TcpConnection::getPeerAddr()
{
    struct sockaddr_in addr;
    socklen_t len=sizeof(addr);
    int ret = getpeername(_sock.fd(),(struct sockaddr*)&addr,&len);
    if(-1==ret)
    {
        perror("getpeername");
    }
    return InetAddr(addr);
}

void TcpConnection::setConnectionCb(const TcpConnectionCallback &cb)
{
    _onConnectionCb = std::move(cb);
}

void TcpConnection::setMessageCb(const TcpConnectionCallback &cb)
{
    _onMessageCb = std::move(cb);
}

void TcpConnection::setCloseCb(const TcpConnectionCallback &cb)
{
    _onCloseCb = std::move(cb);
}

void TcpConnection::handleConnectionCb()
{
    if(_onConnectionCb)
    {
        _onConnectionCb(shared_from_this());
    }
}

void TcpConnection::handleMessageCb()
{
    if(_onMessageCb)
    {
        _onMessageCb(shared_from_this());
    }
}

void TcpConnection::handleCloseCb()
{
    if(_onCloseCb)
    {
        _onCloseCb(shared_from_this());
    }
}