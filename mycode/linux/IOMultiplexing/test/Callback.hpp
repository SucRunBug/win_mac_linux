#pragma once

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include "Reactor.hpp"
#include "Util.hpp"
using namespace std;
using namespace ns_reactor;


static int RecverHelper(int sock, std::string *out)
{
    while(true)
    {
        char buffer[1024];
        ssize_t s = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if(s > 0)   //读取成功，但不一定就把数据读完了
        {
            buffer[s] = 0;
            (*out) += buffer;
        }
        else if(s == 0) //对端关闭连接
        {
            return -1;
        }
        else //信号中断or底层无数据or读取错误
        {
            if(errno == EINTR)
            {
                continue;
            }
            else if(errno == EAGAIN || errno == EWOULDBLOCK)
            {
                return 0;   //本轮读取完毕
            }
            else 
            {
                return -1;
            }
        }
    }
}

//网络就绪事件与事件派发，和网络数据的读取进行解耦
void Receiver(Event &event)
{
    std::cout << "Callback.hpp: call Receiver..." << std::endl;
    //如何保证本轮已经读完了？
    if(-1 == RecverHelper(event.sock_, &(event.inbuffer_))) //不做业务处理，只负责读取
    {
        //本轮读取出错，sock被关闭或者sock读取出错
        if(event.error_callback_)
        {
            event.error_callback_(event);
        }
        std::cout << "Callback.hpp: Receiver err" << std::endl;
        return;
    }

    //数据处理：定制协议以确定报文之间的分隔，解决粘包问题
    //分隔符X：1+1X2+2X3+3X ...
    //所有数据都已经在inbuffer中
    std::cout << "数据处理" << std::endl;
    std::vector<std::string> packages;
    Util::StringSplit(event.inbuffer_, &packages, "X");   //1. 将inbuffer中的报文拆开
    std::cout << "将inbuffer中的报文拆开" << std::endl;
    for(auto &package: packages)
    {
        int x = 0;
        int y = 0;
        char op = 0;
        if(!Util::ParsePackage(package, &x, &y, &op))  //2. 解析报文（反序列化）
        {
            continue;   //非法报文，忽略
        }
        //3. 业务处理
        int result = 0;
        switch (op)
        {
        case '+':
            result = x + y;
            break;
        
        default:
            break;
        }
        //4. 返回报文（序列化）
        std::string response = package;
        response += "=";
        response += std::to_string(result);
        response += "X";
        //5. 发送的核心：不是是读好了就立即发送，而是将数据放在发送缓冲区，并把事件的发送状态设为就绪
        std::cout << "将数据放在发送缓冲区" << std::endl;
        event.outbuffer_ += response;
        (event.r_)->EnableReadWrite(event.sock_, true, true);   //多路转接中，EPOLLIN是常设的，EPOLLOUT是可选的
    }
}

int SendHelper(int sock, std::string &send_string)
{
    //当前能够发送数据，但不一定能全部发送出去
    size_t total = 0;  //当前发送的总数
    const char *start = send_string.c_str();    //发送缓冲区的起始位置
    size_t size = send_string.size();  //发送缓冲区大小
    while(true)
    {
        ssize_t s = send(sock, start + total, size - total, 0);
        if(s > 0)
        {
            total += s;
            if(total == size)   //发送缓冲区足够大，本轮全部发完
            {
                send_string.clear();
                return 1;
            }
        }
        else 
        {
            if(errno == EINTR)
            {
                continue;   //被信号中断
            }
            else if(errno == EAGAIN || errno == EWOULDBLOCK)
            {
                send_string.erase(0, total);
                return 0;   //发送缓冲区被打满，本轮发送结束，但数据未被发完
            }
            else 
            {
                return -1;  //发送出错
            }
        }
    }
}

void Sender(Event &event)
{
    std::cout << "Callback.hpp: call Sender..." << std::endl;
    //发送outbuffer中的内容
    int ret = SendHelper(event.sock_, event.outbuffer_);
    if(ret == 1)
    {
        (event.r_)->EnableReadWrite(event.sock_, true, false);
    }
    else if(ret == 0)
    {
        (event.r_)->EnableReadWrite(event.sock_, true, true);   //还需要再发数据
    }
    else 
    {
        if(event.error_callback_)
        {
            event.error_callback_(event);
        }
    }
}

void Errorer(Event &event)
{
    std::cout << "Callback.hpp: call Errorer..." << std::endl;
    (event.r_)->DelEvent(event.sock_);
}
