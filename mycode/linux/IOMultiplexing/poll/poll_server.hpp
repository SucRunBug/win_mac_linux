#pragma once

#include "sock.hpp"
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/poll.h>
#include <unistd.h>

static const int num = 1024;    // 文件描述符最大接受数

class PollServer
{
public:
    PollServer(int port = 8080) :_port(port), _listen_sock(-1) {
        for (int i = 0; i < num; ++i)
        {
            _rfds[i].fd = -1;
            _rfds[i].events = 0;
            _rfds[i].revents = 0;
        }
    }
    void InitServer() {
        _listen_sock = Sock::Socket();
        Sock::Bind(_listen_sock, _port);
        Sock::Listen(_listen_sock);
        _rfds[0].fd = _listen_sock;
        _rfds[0].events = POLLIN;
    }
    void Loop() {
        while (true) {
            int n = poll(_rfds, num, -1);  // 阻塞
            if (n == 0)
            {
                std::cout << "INFO: Timeout" << std::endl;
            }
            else if (n == -1) {
                std::cout << "ERROR: poll" << std::endl;
            }
            else {
                HandlerEvents(_rfds);
            }
        }
    }
    ~PollServer() {
        if (_listen_sock >= 0)
        {
            close(_listen_sock);
        }
    }
private:
    void HandlerEvents(struct pollfd _rfds[]) {
        for (int i = 0; i < num; ++i)
        {
            if (_rfds[i].fd == -1)
            {
                continue;
            }
            if (_rfds[i].revents == POLLIN)
            {
                if (_rfds[i].fd == _listen_sock)
                {
                    // 新链接到来
                    struct sockaddr_in peer;
                    socklen_t len = sizeof(peer);
                    int new_fd = accept(_listen_sock, (struct sockaddr*)&peer, &len);
                    if (new_fd == -1)
                    {
                        std::cerr << "ERROR: accept" << std::endl;
                    }
                    int j;
                    for (j = 1; j < num; ++j)
                    {
                        if (_rfds[j].fd == -1)
                        {
                            _rfds[j].fd = new_fd;
                            _rfds[j].events = POLLIN;
                            _rfds[j].revents = 0;
                            std::cout << "DEBUG: 协议是" << peer.sin_family << " 端口是" << peer.sin_port << " 地址是" << peer.sin_addr.s_addr << std::endl;
                            break;
                        }
                    }
                    if (j == num)
                    {
                        std::cout << "INFO: _rfds have been full" << std::endl; 
                        close(_rfds[j].fd);
                    }
                }
                else if (_rfds[i].fd != _listen_sock)
                {
                    // 已建立链接发送数据
                    char buffer[1024];
                    ssize_t n = recv(_rfds[i].fd, buffer, sizeof(buffer), 0);
                    if (n == -1)
                    {
                        std::cerr << "ERROR: recv" << std::endl;
                    }
                    else if (n == 0)
                    {
                        std::cout << "INFO: The peer have been closed connection" << std::endl;
                        std::cout << "INFO: The socket is " << _rfds[i].fd << std::endl;
                        if (_rfds[i].fd >= 0)
                        {
                            close(_rfds[i].fd);
                        }
                        _rfds[i].fd = -1;
                        _rfds[i].events = 0;
                        _rfds[i].revents = 0;
                    }
                    else {
                        buffer[n] = '\0';
                        std::cout << "INFO: The peer sent a message: " << buffer << std::endl;
                    }
                }
            }
        }
    }
private:
    u_int16_t _port;
    int _listen_sock;
    struct pollfd _rfds[num];
};