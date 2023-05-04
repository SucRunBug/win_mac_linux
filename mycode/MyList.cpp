#include <iostream>
// #include <list>
#include <assert.h>
#include <algorithm>
using namespace std;
// 模拟实现std::list
// 包含node节点类和迭代器类
template <class T>
struct __list_node
{
    __list_node(T data = T()) :_data(data), _next(nullptr), _prev(nullptr)
    {}
    T _data;
    __list_node<T>* _next;
    __list_node<T>* _prev;
};

template <class T, class Ref, class Ptr>
struct __list_iterator
{
    __list_iterator(__list_node<T>* node): _node(node)
    {}
    Ref operator->()
    {
        return _node->_data;
    }
    Ptr operator*()
    {
        return &(_node->_data);
    }
    bool operator==(const __list_iterator<T, Ref, Ptr> it) const 
    {
        return this->_node == it._node;
    }
    bool operator!=(const __list_iterator<T, Ref, Ptr> it) const
    {
        return this->_node != it._node;
    }
    __list_iterator<T, Ref, Ptr>& operator++()
    {
        _node = _node->_next;
        return *this;
    }
    __list_iterator<T, Ref, Ptr> operator++(int)
    {
        __list_iterator<T, Ref, Ptr> ret(*this);
        _node = _node->_next;
        return ret;
    }

    __list_iterator<T, Ref, Ptr>& operator--()
    {
        _node = _node->_prev;
        return *this;
    }
    __list_iterator<T, Ref, Ptr> operator--(int)
    {
        __list_iterator<T, Ref, Ptr> ret(*this);
        _node = _node->_prev;
        return ret;
    }

    __list_node<T>* _node;
};

template <class T>
class MyList
{
public:
    MyList()
    {
        _head = new __list_node<T>;
        _head->_next = _head;
        _head->_prev = _head;
    }
    // 通过迭代器区间初始化
    template<class InputIterator>
    MyList(InputIterator begin, InputIterator end)
    {
        _head = new __list_node<T>;
        _head->_next = _head;
        _head->_prev = _head;
        while (begin != end)
        {
            push_back(*begin);
            begin++;
        }
    }
    __list_iterator<T, T&, T*> begin()
    {
        return __list_iterator<T, T&, T*>(_head->_next);
    }
    __list_iterator<T, T&, T*> end()
    {
        return __list_iterator<T, T&, T*>(_head);
    }
    const __list_iterator<T, const T&, const T*> begin() const
    {
        return __list_iterator<T, const T&, const T*>(_head->_next);
    }
    const __list_iterator<T, const T&, const T*> end() const
    {
        return __list_iterator<T, const T&, const T*>(_head);
    }
    __list_iterator<T, T&, T*> insert(__list_iterator<T, T&, T*> it, const T& val)
    {
        __list_node<T>* node = new __list_node<T>(val);
        __list_node<T>* pos = it._node;
        __list_node<T>* prev = pos->_prev;
        prev->_next = node;
        node->_prev = prev;
        node->_next = pos;
        pos->_prev = node;
        return __list_iterator<T, T&, T*>(node);
    }
    void push_back(const T& val)
    {
        insert(end(), val);
    }
    __list_iterator<T, T&, T*> erase(__list_iterator<T, T&, T*> it)
    {
        assert(it != end());
        __list_node<T>* node = it._node;
        __list_node<T>* prev = node->_prev;
        __list_node<T>* next = node->_next;
        prev->_next = next;
        next->_prev = prev;
        delete node;
        return __list_iterator<T, T&, T*>(next);
    }
    MyList<T> operator=(MyList<T>& l)
    {
        swap(_head, l._head);
        return (*this);
    }
    void clear()
    {
        auto it = begin();
        while (it != end())
        {
            it = erase(it);
        }
    }
    ~MyList()
    {
        clear();
    }    
private:
    __list_node<T>* _head;
};

int main()
{   
    MyList<int> l1;
    l1.push_back(1);
    l1.push_back(2);
    l1.push_back(3);
    for (auto e: l1)
    {
        std::cout << *e << " ";
    }
    std::cout << endl;
    
    return 0;
}