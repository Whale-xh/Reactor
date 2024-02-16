#ifndef __INETADDR_H__
#define __INETADDR_H__

#include <arpa/inet.h>
#include <string>

using std::string;

class InetAddr
{
public:
    InetAddr(const string &ip,unsigned short port);
    InetAddr(const struct sockaddr_in &addr);
    ~InetAddr();
    string ip() const;
    unsigned short port() const;
    const sockaddr_in *getInetAddrPtr() const;
    
private:
    struct sockaddr_in _addr;
};

#endif