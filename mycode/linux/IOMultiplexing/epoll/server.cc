#include "epoll_server.hpp"

int main()
{
    EpollServer* es = new EpollServer();
    es->InitServer();
    es->Loop();
    return 0;
}