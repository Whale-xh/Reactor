#include "Acceptor.h"
#include <stdlib.h>

Acceptor::Acceptor(const string &ip,unsigned short port)
: _listenSock()
, _servAddr(ip,port)
{

}

Acceptor::~Acceptor()
{

}

void Acceptor::ready()
{
    setReuseAddr();
    SetReusePort();
    bind();
    listen();
}

void Acceptor::setReuseAddr()
{
    int on = 1;
    int ret = setsockopt(_listenSock.fd(),SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));
    if(ret)
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }    
}

void Acceptor::SetReusePort()
{
    int on=1;
    int ret = setsockopt(_listenSock.fd(),SOL_SOCKET,SO_REUSEPORT,&on,sizeof(on));
    if(ret)
    {
        perror("setsocket");
        exit(EXIT_FAILURE);
    }
}

void Acceptor::bind()
{
    int ret = ::bind(_listenSock.fd(),(struct sockaddr *)_servAddr.getInetAddrPtr(),sizeof(struct sockaddr));
    if(-1 == ret)
    {
        perror("bind");
        exit(EXIT_FAILURE);
    }
}

void Acceptor::listen()
{
    int ret = ::listen(_listenSock.fd(),128);
    if(-1==ret)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
}

int Acceptor::accept()
{   
    int connfd=::accept(_listenSock.fd(),nullptr,nullptr);
    if(-1==connfd)
    {
        perror("accept");
        return connfd;
    }
    return connfd;
}

int Acceptor::fd() const
{
    return _listenSock.fd();
}