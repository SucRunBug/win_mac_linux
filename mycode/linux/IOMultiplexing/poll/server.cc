#include "poll_server.hpp"

int main()
{
    PollServer* ps = new PollServer();
    ps->InitServer();
    ps->Loop();
    return 0;
}