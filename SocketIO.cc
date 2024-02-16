#include "SocketIO.h"
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>

SocketIO::SocketIO(int fd)
    : _fd(fd)
{
}

SocketIO::~SocketIO()
{
    close(_fd);
}

int SocketIO::readn(char *buf, int len)
{
    int left = len;
    char *pStr = buf;
    int ret = 0;
    while (left > 0)
    {
        ret = read(_fd, pStr, left);
        if (-1 == ret && errno == EINTR)
        {
            continue;
        }
        else if (-1 == ret)
        {
            perror("readn");
            return ret;
        }
        else if (0 == ret)
        {
            break;
        }
        else
        {
            pStr += ret;
            left -= ret;
        }
    }
    return len - left;
}

int SocketIO::readLine(char *buf, int len)
{
    int left = len - 1;
    char *pStr = buf;
    int ret = 0, total = 0;
    while (left > 0)
    {
        ret = recv(_fd, pStr, left, MSG_PEEK);
        if (-1 == ret && errno == EINTR)
        {
            continue;
        }
        else if (-1 == ret)
        {
            perror("readLine");
            return ret;
        }
        else if (0 == ret)
        {
            break;
        }
        else
        {
            for (int idx = 0; idx < ret; ++idx)
            {
                if (pStr[idx] == '\n')
                {
                    int sz = idx + 1;
                    readn(pStr, sz);
                    pStr += sz;
                    *pStr = '\0';
                    return total + sz;
                }
            }
            readn(pStr, ret);
            total += ret;
            pStr += ret;
            left -= ret;
        }
    }
    *pStr = '\0';
    return total - left;
}

int SocketIO::writen(const char *buf, int len)
{
    int left = len;
    const char *pStr = buf;
    int ret = 0;
    while (left > 0)
    {
        ret = write(_fd, pStr, left);
        if (-1 == ret && errno == EINTR)
        {
            continue;
        }
        else if (-1 == ret)
        {
            perror("writen");
            return ret;
        }
        else if (0 == ret)
        {
            break;
        }
        else
        {
            pStr += ret;
            left -= ret;
        }
    }
    return len - left;
}