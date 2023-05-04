#pragma once

#include <cstring>
#include <iostream>

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

class Sock {
public:
    static int Socket() {
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock == -1)
        {
            std::cerr << "ERROR: socket" << std::endl;
            exit(1);
        }
        int optVal = 1;
        socklen_t len = sizeof(optVal);
        if (-1 == setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optVal, len))
        {
            std::cerr << "ERROR: setsockopt" << std::endl;
            exit(1);
        }
        return sock;
    }
    static void Bind(int sock, u_int16_t port) {
        struct sockaddr_in local;
        memset(&local, 0, sizeof(local));
        local.sin_family = AF_INET;
        local.sin_port = htons(port);
        local.sin_addr.s_addr = INADDR_ANY;
        socklen_t len = sizeof(local);
        if (-1 == bind(sock, (const struct sockaddr*)&local, len))
        {
            std::cerr << "ERROR: bind" << std::endl;
            exit(1);
        }
    }
    static void Listen(int sock) 
    {
        if (-1 == listen(sock, 5))
        {
            std::cerr << "ERROR: listen" << std::endl;
            exit(1);
        }
    }
};