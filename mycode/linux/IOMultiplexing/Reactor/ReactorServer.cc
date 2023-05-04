#include "Accepter.hpp"

int main() {
    Reactor* r = new Reactor();
    r->InitReactor();
    int listen_sock = Sock::Socket();
    Sock::setNonBlock(listen_sock);
    Sock::Bind(listen_sock, 8080);
    Sock::Listen(listen_sock);

    Event ev;
    ev.InitEvent(listen_sock, r);
    ev.RegisterCallback(Accepter, nullptr, nullptr);
    r->AddEvent(ev, EPOLLET | EPOLLIN);
    while(true) {
        r->Dispatcher();
    }
    return 0;
}