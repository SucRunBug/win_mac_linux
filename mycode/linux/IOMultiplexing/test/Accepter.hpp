#pragma once

#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "Reactor.hpp"
#include "Callback.hpp"

using namespace std;
using namespace ns_reactor;
using namespace ns_sock;

void Accepter(Event &event)
{
    cout << "Accepter 回调方法被调用" << endl;
    //连接事件到来，在一个时间段，有很多的连接到来
    while (true)
    {
        struct sockaddr_in peer;
        socklen_t len = sizeof(peer);
        int sock = accept(event.sock_, (struct sockaddr *)&peer, &len);
        if (sock > 0)
        {
            // 0. 将sock设为非阻塞
            Sock::SetNonBlock(sock);
            // 1. 构建新与sock对应的Event对象
            Event ev;
            ev.sock_ = sock;
            ev.r_ = event.r_;
            ev.RegisterCallback(Receiver, Sender, Errorer);
            // 2. 将新的ev托管给epoll，必须得知道曾经的epoll对象！即Event中r_成员存在的价值
            (event.r_)->AddEvent(ev, EPOLLIN | EPOLLET);
        }
        else
        {
            if (errno == EINTR)
            {
                //当前的aceept调用，被信号中断，不代表底层没有新连接了。
                continue;
            }
            else if (errno == EAGAIN || errno == EWOULDBLOCK)
            {
                //底层没有连接了
                break;
            }
            else
            {
                cerr << "accept err: " << errno << endl;
                continue;
            }
        }
    }
}
