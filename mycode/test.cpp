// my basic_string 模拟实现
#include <iostream>
#include <string.h>
#include <algorithm>
#include <assert.h>
using namespace std;

class myString
{
public:
    typedef char* iterator;
    typedef const char* const_iterator;
    myString(const char* str = ""): _size(strlen(str)), _capacity(_size)
    {
        _str = new char[_capacity + 1];
        strcpy(_str, str);
    }
    myString(myString& s): _str(nullptr)
    {
        myString tmp(s._str);
        this->swap(tmp);
    }
    iterator begin()
    {
        return _str;
    }
    const_iterator begin() const
    {
        return _str;
    }
    iterator end()
    {
        return _str + _size;
    }
    const_iterator end() const
    {
        return _str + _size;
    }
    void swap(myString& s)
    {
        ::swap(this->_str, s._str);     
        ::swap(this->_size, s._size);
        ::swap(this->_capacity, s._capacity);
    }
    char operator[](size_t pos)
    {
        assert(pos >= 0 && pos < _size);
        return _str[pos];
    }
    const char operator[](size_t pos) const
    {
        assert(pos >= 0 && pos < _size);
        return _str[pos];
    }
    myString& operator=(myString& s)
    {
        this->swap(s);
        return (*this);
    }
    void reserve(size_t n)
    {
        if (n > _capacity)
        {
            size_t newCapacity = n;
            char* newStr = new char[newCapacity + 1];
            strcpy(newStr, _str);
            delete[] _str;
            _str = newStr;
            _capacity = newCapacity;
        }
    }
    void resize(size_t n, char ch = '\0')
    {
        if (n < _size)
        {
            _str[n] = '\0';
            _size = n;
        }
        else if (n < _capacity)
        {
            for (size_t i = _size; i < n; ++i)
            {
                _str[i] = ch;
            }
            _size = n;
            _str[_size] = '\0';
        }
        else 
        {
            reserve(n);
            for (size_t i = _size; i < n; ++i)
            {
                _str[i] = ch;
            }
            _size = n;
            _str[_size] = '\0';
        }
    }
    void insert(size_t pos, const char* s)
    {}
    void insert(size_t pos, const myString& s)
    {}
    ~myString()
    {
        delete _str;
        _str = nullptr;
    }
private:
    char* _str; // 字符串起始位置
    size_t _size;      // 有效字符数
    size_t _capacity;   // 有效容量
};

int main()
{

    return 0;
}