#ifndef __SOCKET_H__
#define __SOCKET_H__

#include "NoCopyable.h"

class Socket
: NoCopyable 
{
public:
    Socket();
    explicit Socket(int fd);
    ~Socket();
    int fd() const;
    
private:
    int _fd;
};

#endif