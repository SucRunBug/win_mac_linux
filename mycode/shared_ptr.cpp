#include <iostream>
#include <string>

#include <iostream>
#include <memory>

template<typename T>
class MyShared_ptr {
public:
    MyShared_ptr(T* ptr = nullptr) : m_ptr(ptr), m_counter(new int(1)) {}

    // 析构函数
    ~MyShared_ptr() {
        if (--(*m_counter) == 0) {
            delete m_counter;
            delete m_ptr;
            std::cout << "Resource freed" << std::endl;
        }
    }

    // 拷贝构造函数
    MyShared_ptr(const MyShared_ptr<T>& other) : m_ptr(other.m_ptr), m_counter(other.m_counter) {
        ++(*m_counter);
    }

    // 赋值运算符
    MyShared_ptr<T>& operator=(const MyShared_ptr<T>& other) {
        if (this != &other) {
            if (--(*m_counter) == 0) {
                delete m_counter;
                delete m_ptr;
            }
            m_ptr = other.m_ptr;
            m_counter = other.m_counter;
            ++(*m_counter);
        }
        return *this;
    }

    // 获取指针
    T* get() const { return m_ptr; }

    // 获取引用计数
    int count() const { return *m_counter; }

private:
    T* m_ptr;
    int* m_counter;
};

int main() {
    MyShared_ptr<int> ptr1(new int(10));
    std::cout << "ptr1 count = " << ptr1.count() << std::endl;

    MyShared_ptr<int> ptr2(ptr1);
    std::cout << "ptr1 count = " << ptr1.count() << std::endl;
    std::cout << "ptr2 count = " << ptr2.count() << std::endl;

    MyShared_ptr<int> ptr3(new int(20));
    std::cout << "ptr3 count = " << ptr3.count() << std::endl;
    ptr3 = ptr1;
    std::cout << "ptr1 count = " << ptr1.count() << std::endl;
    std::cout << "ptr3 count = " << ptr3.count() << std::endl;

    return 0;
}


// 动态规划之爬楼梯
int climbStairs(int n)
{
    if (n == 1)
    {
        return 1;
    }
    else if (n == 2)
    {
        return 2;
    }
    else // n > 2
    {
        int a = 1;
        int b = 2;
        int c;
        for (int i = 3; i <= n; ++i)
        {
            c = a + b;
            a = b;
            b = c;
        }
        return c;
    }
    return -1;
}

// int main()
// {
//     // std::cout << "hello world" << std::endl;
//     // std::cout << climbStairs(45) << std::endl;
//     return 0;
// }