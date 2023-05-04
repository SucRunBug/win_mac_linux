#pragma once

#include <asm-generic/socket.h>
#include <cstring>
#include <iostream> 
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <fcntl.h>

class Sock {
public:
    static int Socket() {
        int fd = socket(AF_INET, SOCK_STREAM, 0);
        if (fd < 0) {
            std::cerr << "ERROR: socket" << std::endl;
            exit(1); 
        }
        int optVal = 1;
        setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &optVal, sizeof(optVal));
        return fd;
    }
    static void Bind(int fd, u_int16_t port) {
        struct sockaddr_in local;
        memset(&local, 0, sizeof(local));
        local.sin_family = AF_INET;
        local.sin_port = htons(port);
        local.sin_addr.s_addr = INADDR_ANY;
        socklen_t len = sizeof(local);
        
        if (bind(fd, (const struct sockaddr*)&local, len) == -1) {
            std::cerr << "ERROR: bind" << std::endl;
            exit(1);
        }
    }
    static void Listen(int fd) {
        if (listen(fd, 5) == -1) {
            std::cerr << "ERROR: listen" << std::endl;
            exit(1);
        }
    }
    static void setNonBlock(int fd) {
        int flag = fcntl(fd, F_GETFL, 0);
        if (flag == -1) {
            std::cerr << "ERROR: fcntl F_GETFL" << std::endl;
            exit(1);
        }
        if (-1 == fcntl(fd, F_SETFL, flag | O_NONBLOCK)) {
            std::cerr << "ERROR: fcntl F_SETFL" << std::endl;
            exit(1);
        }
    }
};