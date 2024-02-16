#ifndef __TCPCONNECTION_H__
#define __TCPCONNECTION_H__

#include "SocketIO.h"
#include "Socket.h"
#include "InetAddr.h"
#include <memory>
#include <functional>

using std::shared_ptr;
using std::function;

class EventLoop;

// 已经成功建立3次握手的类
class TcpConnection
:  public std::enable_shared_from_this<TcpConnection>
{
    using TcpConnectionPtr = shared_ptr<TcpConnection>;
    using TcpConnectionCallback = function<void(const TcpConnectionPtr &)>;

public:
    TcpConnection(int fd,EventLoop *loop);
    ~TcpConnection();
    void send(const string &msg);
    void sendInLoop(const string &msg);
    string receive();
    string toString();
    bool isClosed() const;

    //注册回调函数
    void setConnectionCb(const TcpConnectionCallback &cb);
    void setMessageCb(const TcpConnectionCallback &cb);
    void setCloseCb(const TcpConnectionCallback &cb);

    //执行回调函数
    void handleConnectionCb();
    void handleMessageCb();
    void handleCloseCb();

private:
    InetAddr getLocalAddr();
    InetAddr getPeerAddr();

private:
    EventLoop *_loop;
    Socket _sock;
    SocketIO _sockIO;
    InetAddr _localAddr;
    InetAddr _peerAddr;

    TcpConnectionCallback _onConnectionCb;
    TcpConnectionCallback _onMessageCb;
    TcpConnectionCallback _onCloseCb;
};

#endif