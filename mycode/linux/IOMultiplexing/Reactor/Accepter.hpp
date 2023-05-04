#pragma once  

#include "Reactor.hpp"
#include "Sock.hpp"
#include "Callback.hpp"
#include <asm-generic/errno-base.h>
#include <asm-generic/errno.h>
#include <cerrno>
#include <iostream>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

void Accepter(Event& event) {
    while (true) {
        struct sockaddr_in peer;
        socklen_t len = sizeof(peer);
        int new_fd = accept(event.GetSock(), (struct sockaddr*)&peer, &len);
        if (new_fd == -1) {
            if (errno == EINTR) {   // 信号中断
                continue;
            } else if (errno == EAGAIN || errno == EWOULDBLOCK) {
                break;  // 底层没有链接了
            } else {
                std::cerr << "ERROR: accept" << std::endl;
                continue;
            }
        } else if (new_fd >= 0) {
            // 新链接到来，设置非阻塞，建立其对应的事件
            Sock::setNonBlock(new_fd);
            Event new_ev;
            new_ev.InitEvent(new_fd, event.GetReactorPointer());
            new_ev.RegisterCallback(Receiver, Sender, ErrorHandler);
            new_ev.GetReactorPointer()->AddEvent(new_ev, EPOLLET | EPOLLIN);
        }
    }
}