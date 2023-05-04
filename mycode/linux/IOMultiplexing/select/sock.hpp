#pragma once

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <stdint.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

class Sock
{
public:
    static int Socket() {
        int sock = socket(AF_INET, SOCK_STREAM, 0);    // ipv4, TCP, defalut protocol
        if (sock == -1)
        {
            std::cerr << "socket error" << std::endl;
            exit(1);
        }
        int opt = 1;
        if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
        {
            std::cerr << "地址复用出错" << std::endl;
            exit(1);
        }
        return sock;
    }
    static void Bind(int sockfd, u_int16_t port)
    {
        struct sockaddr_in local;
        memset(&local, 0, sizeof(local));
        local.sin_family = AF_INET;
        local.sin_port = htons(port);
        local.sin_addr.s_addr = htonl(INADDR_ANY);
        
        if(bind(sockfd, (const struct sockaddr*)&local, sizeof(local)) == -1)
        {
            std::cerr << "bind error" << std::endl;
            exit(1);
        }
    }
    static void Listen(int sockfd)
    {
        if (listen(sockfd, 5) == -1)
        {
            std::cerr << "listen error" << std::endl;
            exit(1);
        }
    }
};
