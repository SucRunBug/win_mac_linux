#pragma once  

#include "Reactor.hpp"
#include "Util.hpp"
#include <asm-generic/errno-base.h>
#include <asm-generic/errno.h>
#include <cerrno>
#include <cstdio>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <vector>

void Receiver(Event& event) {
    int fd = event.GetSock();
    // 循环读取数据
    while (true) {
        char buffer[1024];
        ssize_t n = recv(fd, buffer, sizeof(buffer) - 1, 0);
        // errno = 0;  // 将全局变量errno清零，以便后续出错正确判断出错误类型
        std::cout << "DEBUG: recv return value n- " << n << std::endl;
        if (n == -1) {
            perror("recv failed");
            if (errno == EINTR) {   // 表示调用被中断，可能是由于接收到信号而导致的
                continue;
            } else if (errno == EWOULDBLOCK || errno == EAGAIN) {   // 表示接收缓冲区中没有数据可用，需要等待一段时间后再次调用 recv 函数
                break;  // 本轮读取结束
            } else if (errno == ENOTCONN) {
                break;  // 套接字未连接
            } else if (errno == ECONNRESET) {
                break;  // 链接关闭
            } else if (errno == EINVAL) {
                break;  // 接收缓冲区为0
            } else if (errno == EBADF) {
                break;  // 套接字关闭
            } else if (errno == ETIMEDOUT) {
                break;  // 超时
            } else if (errno == ENOMEM) {
                break;  // 系统内存不够
            } else if (errno == ENETDOWN || errno == ENETUNREACH) {
                break;  // 网络错误
            } else if (errno == EMSGSIZE) { 
                break;  // 大于缓冲区大小
            } else {
                std::cerr << "ERROR: recv" << std::endl;
                if (event._error_callback != nullptr) {     // 不直接退出程序，而是交给错误处理回调函数
                    event._error_callback(event);
                }
                return;
            }
        } else if (n == 0) {
            std::cout << "INFO: The peer have been closed connection" << std::endl;
            if (event._error_callback != nullptr) {
                event._error_callback(event);
            }
            return;
        } else if (n > 0) {
            buffer[n] = '\0';
            event.InputBuffer(buffer);
            std::cout << "DEBUG: recv buffer is " << event.GetInputBuffer() << std::endl;
        }
    }
    // 报文分隔+解析
    std::vector<std::string> data;
    std::string input_buffer = event.GetInputBuffer();
    event.ClearInputBuffer();
    std::string separator = "X";
    Util::SplitString(input_buffer, &data, separator);
    for (auto &e: data) {
        // e 1+1
        double x, y;
        char op;
        if(false == Util::ParseMessage(e, &x, &y, &op)) {
            continue;   // 忽略非法报文
        }
        double result;
        if (op == '+') {
            result = x + y;
        } else if (op == '-') {
            result = x - y;
        } else if (op == '*') {
            result = x * y;
        } else if (op == '/') {
            result = x / y * 1.0;
        } else if (op == '%') {
            result = (int)x % (int)y;
        } else {
            // op error
            continue;
        }
        // 计算完成后构建发送报文
        std::string sent_message;
        sent_message = e;
        sent_message += '=';
        sent_message += std::to_string(result);
        sent_message += 'X';
        event.OutputBuffer(sent_message);
        event.GetReactorPointer()->EnableReadWrite(event.GetSock(), true, true);
    }
}

void Sender(Event& event) {
    int fd = event.GetSock();
    std::string output_buffer = event.GetOutputBuffer();
    const char* start = output_buffer.c_str();  // 发送缓冲区起始位置
    size_t total = output_buffer.size();    // 总共要发的数量
    size_t have_been_sent = 0;  // 已经被发送的数量
    while (true) {
        ssize_t n = send(fd, start + have_been_sent, total - have_been_sent, 0);    // 阻塞发送
        if (n == -1) {
            if (errno == EINTR) {
                continue;
            } else {
                std::cerr << "ERROR: send" << std::endl;
                if (event._error_callback != nullptr) {
                    event._error_callback(event);
                    return;
                }
            }
        } else if (n == 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // 发送缓冲区满了，本轮发送结束，存在未发送数据
                event.ClearOutputBuffer();
                event.GetReactorPointer()->EnableReadWrite(fd, true, true);
                return;
            }
        } else if (n > 0) {
            have_been_sent += n;
            if (have_been_sent == total) {  // 缓冲区足够大，本轮发送完毕
                std::cout << "DEBUG: Output buffer is " << output_buffer << std::endl;
                output_buffer.clear();  // 清空发送缓冲区
                event.ClearOutputBuffer();
                event.GetReactorPointer()->EnableReadWrite(fd, true, false);
                return;
            }
        }
    }
}

void ErrorHandler(Event& event) {
    event.GetReactorPointer()->DelEvent(event.GetSock());
}