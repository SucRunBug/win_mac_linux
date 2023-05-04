#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <sys/epoll.h>
#include <unistd.h>

using namespace std;
using std::cerr;
using std::cin;
using std::cout;
using std::endl;
using std::string;
using std::unordered_map;

namespace ns_reactor
{
  class Event;
  class Reactor;

  typedef void (*callback_t)(Event &); //将返回值为void，参数为Event的函数重命名为 callback_t的函数指针
  //
  class Event
  {
  public:
    int sock_;   //特定的一个文件描述符
    Reactor *r_; //指向该Event对应的epoll

    string inbuffer_;  //对应的sock，私有的读取缓冲区
    string outbuffer_; //对应的sock，私有的发送缓冲区

    callback_t recv_callback_;  //对应的sock，读回调
    callback_t send_callback_;  //对应的sock，写回调
    callback_t error_callback_; //对应的sock，异常回调
  public:
    Event() : sock_(-1), r_(nullptr)
    {
      recv_callback_ = nullptr;
      send_callback_ = nullptr;
      error_callback_ = nullptr;
    }
    //接收函数指针，注册对应的回调函数
    void RegisterCallback(callback_t _recv, callback_t _send, callback_t _error)
    {
      recv_callback_ = _recv;
      send_callback_ = _send;
      error_callback_ = _error;
    }
    ~Event() {}
  };

  class Reactor
  {
  private:
    int epfd_;
    unordered_map<int, Event> events_; // sock: Event
  public:
    Reactor() : epfd_(-1)
    {
    }
    void InitReactor()
    {
      epfd_ = epoll_create(128);
      if (epfd_ < 0)
      {
        cerr << "epoll_create err" << endl;
        exit(1);
      }
    }
    void AddEvent(const Event &ev, uint32_t events)
    {
      // 1. 将ev中的sock添加到epoll中，默认我们认为，所有添加到事件默认全部都要关心read事件
      // 2. 将ev本身添加到unordered_map
      struct epoll_event epoll_ev;
      epoll_ev.events = events;
      epoll_ev.data.fd = ev.sock_;

      if (epoll_ctl(epfd_, EPOLL_CTL_ADD, ev.sock_, &epoll_ev) < 0)
      {
        cerr << "epoll_ctl add event err: " << ev.sock_ << endl;
        return;
      }
      else
      {
        events_.insert({ev.sock_, ev});
      }
      cout << "add event success, sock: " << ev.sock_ << endl;
    }
    void DelEvent(int sock) 
    {
      cout << "client quit or err, DEL all resources" << endl;
      auto it = events_.find(sock);
      if(it == events_.end())
      {
        return;
      }
      epoll_ctl(epfd_, EPOLL_CTL_DEL, sock, nullptr);
      events_.erase(sock);
      close(sock);
    }
    void EnableReadWrite(int sock, bool readable, bool writeable)
    {
      struct epoll_event ev;
      ev.data.fd = sock;
      ev.events = EPOLLET | (readable ? EPOLLIN : 0) | (writeable ? EPOLLOUT : 0);
      if(0 == epoll_ctl(epfd_, EPOLL_CTL_MOD, sock, &ev))
      {
        cout << "Reactor.hpp: EnableRW MOD" << sock << " 上关心的事件success" << endl; 
      }
    }
    bool IsExist(int sock)
    {
      auto it = events_.find(sock);
      return it == events_.end() ? false : true;
    }
    //对就绪事件进行事件派发
    void Dispatcher(int timeout)
    {
#define NUM 128
      struct epoll_event revs[NUM];
      int num = epoll_wait(epfd_, revs, NUM, 0);
      for (int i = 0; i < num; i++)
      {
        //就绪事件派发
        int sock = revs[i].data.fd;
        uint32_t events = revs[i].events;
        cout << "sock: " << sock << " 这个fd上有数据" << endl;

        //将所有的异常全部交给读写处理，读写出现异常时交给error_handler
        if (events & EPOLLERR)
        {
          events |= (EPOLLIN | EPOLLOUT);
        }
        if (events & EPOLLHUP)  //对端关闭连接
        {
          events |= (EPOLLIN | EPOLLOUT);
        }
        if (IsExist(sock) && (events & EPOLLIN))
        {
          if (events_[sock].recv_callback_)
          {
            events_[sock].recv_callback_(events_[sock]);
          }
        }
        if (IsExist(sock) && (events & EPOLLOUT))
        {
          if (events_[sock].send_callback_)
          {
            events_[sock].send_callback_(events_[sock]);
          }
        }
      }
    }
    ~Reactor()
    {
      if (epfd_ >= 0)
        close(epfd_);
    }
  };
}
