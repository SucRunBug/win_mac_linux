# STL之 stack & queque

## 一、stack

stack是STL库中的一个容器适配器（container adapter），它基于其他STL容器（如vector、deque、list等）实现。stack提供了一种LIFO（Last-In-First-Out）的数据结构，仅允许在栈的顶部进行添加和删除元素的操作，因此它只提供了少量的操作函数，如push、pop、top、empty等。stack的底层容器可以通过模板参数进行指定，如果未指定，则默认使用deque作为底层容器。

总之，栈不是容器，栈是容器适配器。不是直接实现，封装其他容器，包装转换实现出来。

### 1.1 基本用法

STL（标准模板库）是C++中的一个重要的库，其中stack是STL中的一个数据结构，它是一个后进先出（LIFO）的容器，类似于一个栈。

使用stack需要包含头文件`<stack>`，并创建一个stack对象，如下所示：

```cpp
#include <stack>
using namespace std;

stack<int> myStack;
```

这里创建了一个stack对象myStack，它可以存储int类型的数据。

可以使用push()函数将数据压入栈中，如下所示：

```cpp
myStack.push(10);
myStack.push(20);
myStack.push(30);
```

这里将10、20、30三个数依次压入栈中。

可以使用top()函数获取栈顶元素，如下所示：

```cpp
cout << myStack.top() << endl;
```

这里输出30，因为30是栈顶元素。

可以使用pop()函数将栈顶元素弹出栈，如下所示：

```cpp
myStack.pop();
```

这里将30弹出栈，栈中剩下的元素依次为20、10。

可以使用empty()函数判断栈是否为空，如下所示：

```cpp
if (myStack.empty()) {
    cout << "Stack is empty." << endl;
} else {
    cout << "Stack is not empty." << endl;
}
```

这里输出“Stack is not empty.”，因为栈中还有元素。

stack还有其他一些常用的函数，比如size()函数可以获取栈的大小，swap()函数可以交换两个栈的内容等。

### 1.2 stack 代码练习

#### 1.2.1 最小栈

题目链接：[最小栈](https://leetcode.cn/problems/min-stack)

设计一个支持 push ，pop ，top 操作，并能在常数时间内检索到最小元素的栈。

实现 MinStack 类:

- MinStack() 初始化堆栈对象。
- void push(int val) 将元素val推入堆栈。
- void pop() 删除堆栈顶部的元素。
- int top() 获取堆栈顶部的元素。
- int getMin() 获取堆栈中的最小元素。

思路分析：栈1存放所有数字，栈2存放历史最小数字。

栈2入栈规则：栈1入栈时，栈2为空，或者栈1入栈数字比栈2栈顶小或相等。

栈2出栈规则：栈1出栈前，栈2的栈顶与栈1点栈顶相等。 

```cpp
class MinStack {
public:
    MinStack() {

    }
    
    void push(int val) {
        _st.push(val);
        if (_st_min.empty() == true || val <= _st_min.top())
        {
            _st_min.push(val);
        }
    }
    
    void pop() {
        if (_st.top() == _st_min.top())
        {
            _st_min.pop();
        }
        _st.pop();
        
    }
    
    int top() {
        return _st.top();
    }
    
    int getMin() {
        return _st_min.top();
    }
private:
    stack<int> _st;
    stack<int> _st_min;
};

/**
 * Your MinStack object will be instantiated and called as such:
 * MinStack* obj = new MinStack();
 * obj->push(val);
 * obj->pop();
 * int param_3 = obj->top();
 * int param_4 = obj->getMin();
 */
```

#### 1.2.2 栈的压入和弹出序列

题目链接：[栈的压入和弹出序列](https://www.nowcoder.com/practice/d77d11405cc7470d82554cb392585106?tpId=13&tqId=11174&ru=/exam/oj)

