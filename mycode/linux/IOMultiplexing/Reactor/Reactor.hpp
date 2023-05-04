#pragma once 

#include <iostream>
#include <string>
#include <unordered_map>

#include <sys/epoll.h>
#include <unistd.h>

class Event;
class Reactor;

typedef void (*CallbackFunc)(Event&); // 重命名回调函数指针CallbackFunc，参数类型为Event&，返回值为void

class Event {   // 存储在epoll模型中的事件
public:
    Event():_sock(-1), _reactor_pointer(nullptr) {
        _recv_callback = nullptr;
        _send_callback = nullptr;
        _error_callback = nullptr;
    }
    int GetSock() {
        return _sock;
    }
    Reactor* GetReactorPointer() {
        return _reactor_pointer;
    }
    std::string GetInputBuffer() {
        return _in_buffer;
    }
    std::string GetOutputBuffer() {
        return _out_buffer;
    }
    void ClearInputBuffer() {
        _in_buffer.clear();
    }
    void ClearOutputBuffer() {
        _out_buffer.clear();
    }
    void InitEvent(int sock, Reactor* r) {
        _sock = sock;
        _reactor_pointer = r;
    }
    void RegisterCallback(CallbackFunc recv, CallbackFunc send, CallbackFunc error);    // 负责接受函数指针，注册回调函数
    void InputBuffer(std::string str);   // 将接收到的报文填入输入缓冲区
    void OutputBuffer(std::string str);  // 将要发送的报文填入输出缓冲区
    ~Event() {}

private:
    int _sock;  // 文件描述符
    Reactor* _reactor_pointer;  // 指向该事件所在的epoll模型
    std::string _in_buffer;     // 输入缓冲区
    std::string _out_buffer;    // 输出缓冲区
public:
    CallbackFunc _recv_callback;    // 读回调
    CallbackFunc _send_callback;    // 写回调
    CallbackFunc _error_callback;   // 异常回调
};

void Event::RegisterCallback(CallbackFunc recv, CallbackFunc send, CallbackFunc error) {
    _recv_callback = recv;
    _send_callback = send;
    _error_callback = error;
}

void Event::InputBuffer(std::string str) {
    _in_buffer += str;
}

void Event::OutputBuffer(std::string str) {
    _out_buffer += str;
}

class Reactor {     // 描述一个epoll模型及其关注的事件
public:
    Reactor():_epoll_fd(-1) {}
    void InitReactor();     // 创建epoll句柄
    void AddEvent(Event& classEventBeAdded, int eventsBeSeen);     // 用于添加某个关注的事件到epoll模型中
    void DelEvent(int sock);    // 通过文件描述符删除epoll中对应的链接和关注的事件
    void EnableReadWrite(int fd, bool readable, bool writable);     // 为某个链接添加关注事件的属性
    void Dispatcher();      // 等待链接事件就绪，并执行回调
    ~Reactor() {
        if (_epoll_fd >= 0) {
            close(_epoll_fd);
        }
    }
private:
    int _epoll_fd;
    std::unordered_map<int, Event> _fd2events;     // 存储文件描述符到事件类的映射
};

void Reactor::InitReactor() {
    int fd = epoll_create(128);
    if (fd == -1) {
        std::cerr << "ERORR: epoll_create" << std::endl;
        exit(1);
    }
    _epoll_fd = fd;
}

void Reactor::AddEvent(Event &classEventBeAdded, int eventsBeSeen) {
    struct epoll_event ep_ev;
    int fd = classEventBeAdded.GetSock();
    ep_ev.data.fd = fd;
    ep_ev.events = eventsBeSeen;
    if(-1 == epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, ep_ev.data.fd, &ep_ev)) {
        std::cerr << "ERROR: EPOLL_CTL_ADD" << std::endl;
        return;
    } else {
        _fd2events.insert({fd, classEventBeAdded});  // 添加事件成功后，建立链接的文件描述符和事件的映射，用于后续快速读取?
    }
    std::cout << "DEBUG: AddEvent success, sock " << ep_ev.data.fd << " have been add" << std::endl; 
}

void Reactor::DelEvent(int sock) {
    // 通过文件描述符找到map中映射的事件
    auto it = _fd2events.find(sock);
    if (it == _fd2events.end()) {
        // 没有该链接
        std::cerr << "ERROR: No such connetion- " << sock << std::endl;
        return;
    }
    epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, sock, nullptr);
    _fd2events.erase(sock);
    if (sock >= 0) {
        close(sock);
    }
    std::cout << "INFO: The peer have been closed connection. sock- " << sock << std::endl;
}

void Reactor::EnableReadWrite(int fd, bool readable, bool writable) {
    struct epoll_event ep_ev;
    ep_ev.data.fd = fd;
    ep_ev.events = EPOLLET;     // 设置边缘触发
    if (readable == true) {
        ep_ev.events |= EPOLLIN;
    }
    if (writable == true) {
        ep_ev.events |= EPOLLOUT;
    }
    int ret = epoll_ctl(_epoll_fd, EPOLL_CTL_MOD, fd, &ep_ev);
    if (ret == 0) {     // 设置成功
        std::cout << "INFO: EPOLL_CTL_MOD(EPOLLET|EPOLLIN|EPOLLOUT) success in " << __LINE__ << std::endl;
    } else if (ret == -1) {     // 设置错误
        std::cerr << "ERORR: EPOLL_CTL_MOD in " << __LINE__ << std::endl;
        exit(1);
    }
}

void Reactor::Dispatcher() {
    struct epoll_event revs[128];
    int n = epoll_wait(_epoll_fd, revs, 128, 0);    // 立即返回
    for (int i = 0; i < n; ++i) {
        int sock = revs[i].data.fd;
        int events = revs[i].events;
        std::cout << "INFO: This sock-" << sock << " has data" << std::endl;

        if (events & EPOLLERR) {    // 若出现异常，设置关注读写，并交给“错误回调函数”处理
            events |= EPOLLIN;
            events |= EPOLLOUT;
        }
        if (events & EPOLLHUP) {    // 对端关闭链接
            events |= EPOLLIN;
            events |= EPOLLOUT;
        }
        // 查找epoll中是否存在该链接
        auto it = _fd2events.find(sock);
        bool isExist = false;
        if (it != _fd2events.end()) {
            isExist = true;
        } else if (it == _fd2events.end()) {
            isExist = false;
        }
        // 根据关注事件执行回调
        if (isExist == true) {
            if (events & EPOLLIN) {
                if (_fd2events[sock]._recv_callback != nullptr) {
                    _fd2events[sock]._recv_callback(_fd2events[sock]);
                }
            }
            if (events & EPOLLOUT) {
                if (_fd2events[sock]._send_callback != nullptr) {
                    _fd2events[sock]._send_callback(_fd2events[sock]);
                }
            }
        }
    }
}