#include "sock.hpp"
#include "Reactor.hpp"
#include "Accepter.hpp"

using namespace std;
using namespace ns_sock;
using namespace ns_reactor;

//单进程基于ET非阻塞设计的一个Reactor模式
//Linux特别常用 半同步半异步的Reactor 反应堆模式
//（检测事件就绪 + 对数据的读写） + 对数据的分析处理->交给其他模块

int main()
{
    //1. 创建epoll对象
    Reactor* R = new Reactor();
    R->InitReactor();

    //2. 创建网络套接字
    int listen_sock = Sock::Socket();   
    Sock::SetNonBlock(listen_sock); //为了支持ET模式，需要把文件描述符设置为非阻塞
    Sock::Bind(listen_sock, 8081);
    Sock::Listen(listen_sock);

    //3. 创建Event对象
    Event ev;
    ev.sock_ = listen_sock;
    ev.r_ = R;
    //Accepter: 链接管理器
    ev.RegisterCallback(Accepter, nullptr, nullptr);    //listen_sock只需要监听读就绪事件

    //4. 将ev注册进epoll中
    R->AddEvent(ev, EPOLLIN | EPOLLET); //ET模式

    //5. 进入事件派发逻辑，服务器启动(
    int timeout = 1000;
    while(true)
    {
        R->Dispatcher(timeout);
    }
    return 0;
}