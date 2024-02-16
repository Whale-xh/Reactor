#ifndef __ACCEPTOR_H__
#define __ACCEPTOR_H__

#include "Socket.h"
#include "InetAddr.h"
#include <string>

using std::string;

class Acceptor
{
public:
    Acceptor(const string &ip, unsigned short port);
    ~Acceptor();
    void ready();
    void setReuseAddr();
    void SetReusePort();
    void bind();
    void listen();
    int accept();  //通信fd
    int fd() const; //listenfd

private:
    Socket _listenSock;
    InetAddr _servAddr;    
};

#endif