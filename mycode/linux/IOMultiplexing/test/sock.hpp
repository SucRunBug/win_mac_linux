#pragma once 

#include <iostream>
#include <string>
#include <cstdlib>
#include <unistd.h>
#include <strings.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>

using namespace std;

namespace ns_sock
{
  const int g_backlog = 5;
  class Sock
  {
    public:
      static int Socket()
      {
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        int opt = 1;
        setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
        return sock;
      }
      static void Bind(const int &sock, const u_int16_t &port)
      {
        struct sockaddr_in local;
        bzero(&local, sizeof(local));
        local.sin_family = AF_INET;
        local.sin_port = htons(port);
        local.sin_addr.s_addr = INADDR_ANY;

        if(bind(sock, (struct sockaddr*)&local, sizeof(local)) < 0)
        {
          std::cerr << "bind err" << std::endl;
          exit(1);
        }
      }
      static void Listen(const int &sock)
      {
        listen(sock, g_backlog);
      }
      static void SetNonBlock(const int &sock)
      {
        int fl = fcntl(sock, F_GETFL);
        fcntl(sock, F_SETFL, fl | O_NONBLOCK);
      }
  };
}
