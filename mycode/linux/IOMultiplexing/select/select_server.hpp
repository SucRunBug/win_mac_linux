#pragma once
#include "sock.hpp"

#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

const static int num = sizeof(fd_set) * 8;
const u_int16_t defalut_port = 8080;

class SelectServer{
public:
    SelectServer(u_int16_t port = defalut_port): _port(port), _listen_sock(-1)
    {
        for (int i = 0; i < num; ++i)
        {
            _fd_arry[i] = -1;    // 初始化存放套接字文件描述符的数组
        }
    }
    void InitSelectServer();    // 负责创建套接字，绑定端口和监听
    void Loop();    // 负责阻塞等待套接字
    void HandlerEvent(fd_set& rdfs);    // 负责处理事件   
    ~SelectServer(){
        // 负责关闭打开的套接字
        if (_listen_sock >= 0)
        {
            close(_listen_sock);
        }
    }
private:
    int _listen_sock;   // 监听套接字
    u_int16_t _port;    // 端口号
    int _fd_arry[num];
};

void SelectServer::InitSelectServer()
{
    _listen_sock = Sock::Socket();
    Sock::Bind(_listen_sock, _port);
    Sock::Listen(_listen_sock);
    _fd_arry[0] = _listen_sock;
}

void SelectServer::Loop()
{
    fd_set rfds;
    while (true)
    {
        FD_ZERO(&rfds);     // 清空可读描述符集合
        struct timeval tv;
        tv.tv_sec = 3;
        tv.tv_usec = 0;     // 设置超时时间为3秒
        int max_fd = -1;    // 存储最大的文件描述符
        for (int i = 0; i < num; ++i)
        {
            if (_fd_arry[i] == -1)
            {
                continue;
            }
            if (_fd_arry[i] > max_fd)
            {
                max_fd = _fd_arry[i];
            }
            FD_SET(_fd_arry[i], &rfds);     // 添加获取到的文件描述符
        }
        int ret = select(max_fd + 1, &rfds, nullptr, nullptr, &tv);
        if (ret == -1)
        {
            // error
            std::cerr << "select error" << std::endl;
            exit(1);
        }
        else if (ret == 0)
        {
            // timeout
            std::cout << "select timeout" << std::endl;
        }
        else 
        {
            HandlerEvent(rfds);
        }
    }
    
}

void SelectServer::HandlerEvent(fd_set& rdfs)
{
    for (int i = 0; i < num; ++i)   // 遍历存储文件描述符数组
    {
        if (_fd_arry[i] == -1)
        {
            continue;
        }
        if (FD_ISSET(_fd_arry[i], &rdfs) == true)   // 检测是否存在某个fd已经在rdfs中准备就绪了
        {
            // 某个文件符准备就绪，如果是监听套接字，就说明有新链接；否则，是其他已建立链接的新数据。
            if (_fd_arry[i] == _listen_sock)
            {
                // 新链接到来
                struct sockaddr_in peer;   // 存储对端链接的数据
                socklen_t len = sizeof(peer);
                int new_fd = accept(_listen_sock, (struct sockaddr*)&peer, &len);
                if (new_fd == -1)
                {
                    std::cerr << "accept error" << std::endl;
                }
                else {
                    int j;
                    for (j = 1; j < num; ++j)
                    {
                        if (_fd_arry[j] == -1)
                        {
                            // std::cout << "Debug: _fd_arry[i] is " << _fd_arry[i] << std::endl;
                            break;
                        }
                    }
                    if (j == num)
                    {
                        std::cout << "无法再建立更多链接" << std::endl;
                        close(_fd_arry[j]);
                    }
                    else {
                        // 存放新链接文件描述符
                        _fd_arry[j] = new_fd;
                        std::cout << "Debug: The new sock: "<< new_fd << " have been stored" << std::endl;
                    }
                }
            }
            else if (_fd_arry[i] != _listen_sock)
            {
                // 已建立链接发送数据
                char buffer[1024];
                ssize_t n = recv(_fd_arry[i], buffer, sizeof(buffer), 0);
                if (-1 == n) {
                    std::cerr << "recv error" << std::endl;
                    exit(1);
                }
                else if (0 == n) {
                    std::cout << "The peer have been closed connection." << std::endl;
                    std::cout << "The peer sock: " << _fd_arry[i] << std::endl;
                    close(_fd_arry[i]);
                    _fd_arry[i] = -1;
                }
                else {
                    buffer[n] = '\0';
                    std::cout << "Get message from peer: " << buffer << std::endl;
                }
            }
        }
    }
}