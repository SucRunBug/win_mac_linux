#pragma once 

#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>

using namespace std;
using std::string;
using std::vector;

class Util
{
public:
    //根据协议进行分隔报文
    static void StringSplit(string &in, vector<string> *out, const string &sep)
    {
        while(true)
        {
            std::cout << in << std::endl;
            auto pos = in.find(sep);
            if(pos != string::npos)
            {
                string sub = in.substr(0, pos);
                out->push_back(sub);
                in.erase(0, pos + sep.size());
            }
            else 
            {
                //没有找到分隔符，是不符合协议的报文，直接丢弃
                break;
            }
        }
    }

    //业务处理：根据运算符进行计算
    static bool ParsePackage(std::string &package, int *x, int *y, char *op)
    {
        auto pos = package.find("+");
        if(pos == std::string::npos)
        {
            return false;
        }
        else 
        {
            *x = atoi(package.substr(0, pos).c_str());
            *y = atoi(package.substr(pos + 1).c_str());
            *op = '+';
            return true;
        }
    }

};
