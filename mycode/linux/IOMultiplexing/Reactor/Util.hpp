#pragma once

#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

class Util {
public:
    // 根据分隔符切分字符串
    static void SplitString(std::string& in, std::vector<std::string>* out, std::string& separator);  
    // 报文解析及计算
    template<class T>
    static bool ParseMessage(std::string& message, T* x, T* y, char* op);
};

void Util::SplitString(std::string &in, std::vector<std::string> *out, std::string &separator) {
    while (true) {
        std::cout << "DEBUG: SplitString- " << in << std::endl;     // 1+1sep2-2sep3*3
        auto pos = in.find(separator);
        if (pos != std::string::npos) {
            out->push_back(in.substr(0, pos));
            in.erase(0, pos + separator.size());
        } else {
            break;  // 找不到分隔符，分隔完毕或不符合协议
        }
    }
}

template<class T>
bool Util::ParseMessage(std::string &message, T* x, T* y, char *op) {
    // message 1+1
    auto pos = message.find('+');
    *op = '+';
    if (pos == std::string::npos) {
        pos = message.find('-');
        *op = '-';
    } 
    if (pos == std::string::npos) {
        pos = message.find('*');
        *op = '*';
    }
    if (pos == std::string::npos) {
        pos = message.find('/');
        *op = '/';
    }
    if (pos == std::string::npos) {
        pos = message.find('%');
        *op = '%';
    }
    if (pos == std::string::npos) {
        // 暂时不符合协议，停止处理
        std::cerr << "ERROR: Util::ParseMessage" << std::endl;
        return false;
    }
    *x = atoi(message.substr(0, pos).c_str());
    *y = atoi(message.substr(pos + 1).c_str());
    return true;
}