#pragma once
#include "sock.hpp"
#include <asm-generic/errno-base.h>
#include <asm-generic/errno.h>
#include <cerrno>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <string>

class EpollServer {
public:
    EpollServer(int port = 8080): _listen_sock(-1), _epoll_fd(-1), _port(port) {}
    void InitServer();
    void Loop();
    ~EpollServer() {
        if (_listen_sock >= 0)
        {
            close(_listen_sock);
        }
        if (_epoll_fd >= 0)
        {
            close(_epoll_fd);
        }
    }
private:
    void HandlerEvent(struct epoll_event* ep_revs, int num);
private:
    int _listen_sock;
    int _epoll_fd;
    u_int16_t _port;
};

void EpollServer::InitServer() {
    _listen_sock = Sock::Socket();
    Sock::Bind(_listen_sock, _port);
    Sock::Listen(_listen_sock);
    _epoll_fd = epoll_create(128);
    if (_epoll_fd == -1)
    {
        std::cerr << "ERROR: epoll_create" << std::endl;
        exit(1);
    }
}

void EpollServer::Loop() {
    // 首先将监听套接字加入epoll模型中
    struct epoll_event ep_ev;
    ep_ev.events = EPOLLIN;
    ep_ev.data.fd = _listen_sock;
    epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, _listen_sock, &ep_ev);
    // 定义epoll_event数组用于存储新链接
    int num = 10;   // 接收数量需要小于epoll_create参数值
    struct epoll_event ep_revs[num];
    while (true) {
        int n = epoll_wait(_epoll_fd, ep_revs, num, -1);    // 阻塞等待
        if (n == 0)
        {
            std::cout << "INFO: Timeout" << std::endl; 
        }
        else if (n < 0)
        {
            std::cerr << "ERROR: epoll_wait" << std::endl;
        }
        else {
            HandlerEvent(ep_revs, n);
        }
    }
}

void EpollServer::HandlerEvent(struct epoll_event* ep_revs, int num) {
    for (int i = 0; i < num; ++i)
    {
        int fd = ep_revs[i].data.fd;
        if (ep_revs[i].events == EPOLLIN)
        {
            if (fd == _listen_sock) {
                struct sockaddr_in peer;
                socklen_t len = sizeof(peer);
                int new_fd = accept(_listen_sock, (struct sockaddr*)&peer, &len);
                if (new_fd < 0)
                {
                    std::cerr << "ERROR: accept" << std::endl;
                    continue;
                }
                else if (new_fd >= 0)
                {
                    struct epoll_event new_ep_ev;
                    new_ep_ev.data.fd = new_fd;
                    new_ep_ev.events = EPOLLIN;
                    epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, new_fd, &new_ep_ev);
                }
            }
            else if (fd != _listen_sock) {
                char buffer[1024];
                ssize_t n = recv(fd, buffer, sizeof(buffer) - 1, 0);
                if (n == 0)
                {
                    std::cout << "INFO: The peer have been closed connection" << std::endl;
                    if (fd >= 0) {
                        close(fd);
                    }
                    epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, fd, nullptr);
                }
                else if (n < 0) {
                    std::cerr << "ERROR: recv" << std::endl;
                    if (fd >= 0) {
                        close(fd);
                    }
                    epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, fd, nullptr);
                    exit(1);
                }
                else {
                    buffer[n] = '\0';
                    std::cout << "INFO: The peer have been said: " << std::endl;
                    std::cout << buffer << std::endl; 
                    // 接收对端消息后，可以设置该链接关注EPOLLOUT，从而向该链接发送消息
                    struct epoll_event mod_ep_ev;
                    mod_ep_ev.data.fd = fd;
                    mod_ep_ev.events = EPOLLIN | EPOLLOUT;
                    epoll_ctl(_epoll_fd, EPOLL_CTL_MOD, fd, &mod_ep_ev);
                }
            }
        }
        if (ep_revs[i].events & EPOLLOUT)
        {
            std::string message = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: 123\n\n<!DOCTYPE html>\n<html>\n<head>\n<title>My Web Page</title>\n</head>\n<body>\n<h1>Hello, World!</h1>\n<p>This is my first web page.</p>\n</body>\n</html>\n";
            int ret = send(ep_revs[i].data.fd, message.c_str(), message.length(), 0);
            if (ret < 0)
            {
                if (errno == EAGAIN || errno == EWOULDBLOCK) {
                    std::cout << "INFO: Send buffer is full" << std::endl;
                    continue;
                }
                else {
                    std::cerr << "ERROR: send" << std::endl;
                    epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, ep_revs[i].data.fd, nullptr);
                    close(ep_revs[i].data.fd);
                    continue;
                }
            } 
            else {
                std::cout << "DEBUG: send success." << std::endl;
            }
            // 再取消关注EPOLLOUT（关注EPOLLIN是常态，关注EPOLLOUT是按需）
            struct epoll_event del_ep_ev;
            del_ep_ev.data.fd = ep_revs[i].data.fd;
            del_ep_ev.events = EPOLLIN;
            epoll_ctl(_epoll_fd, EPOLL_CTL_MOD, ep_revs[i].data.fd, &del_ep_ev);
        }
    }
}