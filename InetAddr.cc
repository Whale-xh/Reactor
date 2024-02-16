#include "InetAddr.h"
#include <string.h>

InetAddr::InetAddr(const string &ip,unsigned short port)
{
    ::bzero(&_addr,sizeof(_addr));
    _addr.sin_addr.s_addr = inet_addr(ip.c_str());
    _addr.sin_family = AF_INET;
    _addr.sin_port = htons(port);
}

InetAddr::InetAddr(const struct sockaddr_in &addr)
:_addr(addr)
{

}

InetAddr::~InetAddr()
{

}

string InetAddr::ip() const
{
    return string(inet_ntoa(_addr.sin_addr));
}

unsigned short InetAddr::port() const
{
    return ntohs(_addr.sin_port);
}

const sockaddr_in *InetAddr::getInetAddrPtr() const
{
    return &_addr;
}