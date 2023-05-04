# 深浅拷贝——利用模拟实现basic_string深入理解

## 一、深浅拷贝的基本概念

*深拷贝和浅拷贝都是指在对象复制时，复制对象的内存空间的方式。*

### 1.1 深浅拷贝的不同之处

浅拷贝是指**将一个对象的所有成员变量都直接拷贝给另一个对象**，包括指针成员变量，**两个对象共享同一块内存空间**。

深拷贝是指**在对象复制时，分配一块新的内存空间，将原对象的成员变量复制到新的内存空间中**。

### 1.2 适用场景及优缺点

浅拷贝通常用于**类的复制操作**，可以在一定程度上提高性能，但是如果一个对象的成员变量指向的是堆内存空间，那么当其中一个对象释放了这块内存空间后，另一个对象仍然指向这块内存空间，容易出现问题。

浅拷贝适用于对象的成员变量是基本数据类型，或指向栈空间的指针的情况。因为这些数据类型的内存是在栈上分配的，所以可以直接拷贝，共享同一块内存空间也不会带来问题。

深拷贝可以避免两个对象共享同一块内存空间带来的问题，但是也会增加内存空间的消耗，以及复制成员变量的时间消耗。

深拷贝适用于**对象的成员变量是指向堆内存空间的指针，或者成员变量是对象**的情况。因为这些情况下，如果采用浅拷贝的方式，两个对象共享同一块内存空间，可能会导致一个对象释放了内存空间，另一个对象还在使用这块内存空间，造成不可预料的错误。

## 二、basic_string的模拟实现

###  2.1 中规中矩的传统写法

```cpp
#include<iostream>;
#include<string.h>;

using namespace std;

//深浅拷贝——利用模拟实现basic_string深入理解
namespace yfy
{
	class string
	{
	public:
		string(const char* str)
			:_str(new char[strlen(str) + 1])	//若此处不在堆上开空间，而是将str作为参数赋值给_str，就是浅拷贝，其后果是无法修改常量字符串
		{
			strcpy(_str, str);
		}

		string(const string& s)
			:_str(new char[strlen(s._str) + 1])		//自定义拷贝构造函数，深拷贝————开辟同样大小空间，并将数据拷贝
		{
			strcpy(_str, s._str);
		}

		~string()
		{
			delete[] _str;
			_str = nullptr;
		}

		char& operator[](size_t pos)
		{
			return _str[pos];
		}

		string& operator=(const string& s)		//不管左值右值谁长，直接将左值释放，开辟和右值同样大小空间，再拷贝右值
		{
			if (this != &s)		//防止自己给自己赋值
			{
				char* tmp = new char[strlen(s._str) + 1];	//如果先释放_str，new失败了会导致赔了夫人又折兵，new如果失败会抛异常，所以先不要释放_str，new成功后再释放
				delete[] _str;									
				_str = tmp;
				strcpy(_str, s._str);
			}
			return *this;
		}
	private:
		char* _str;
	};

	void test_string1()
	{
		string s1("hello");
		s1[0] = 'x';
		string s2(s1);//若是编译器默认实现的拷贝构造，则为浅拷贝————导致：1、同一块空间析构两次 2、一个对象修改值，另一个随之改变
		string s3("hello world");
		s1 = s3;
	}
}

int main()
{
	yfy::test_string1();
	return 0;
}
```

### 2.2 富有"资本主义"的现代写法

```cpp
//拷贝构造
		string(const string& s)
			:_str(nullptr)			//若不置空，tmp作为临时变量，在生命结束时调用析构，此时tmp为一个随机值，会出问题
		{
			string tmp(s._str);		//调用构造函数为tmp开空间
			swap(_str, tmp._str);
		}

//赋值
		string& operator=(string s)
		{
			swap(_str, s._str);
			return *this;
		}
```

### 3.3 basic_string模拟汇总

#### 3.3.1 代码

```cpp
#include<iostream>
#include<string.h>
#include<assert.h>

using namespace std;

//深浅拷贝——利用模拟实现basic_string深入理解
namespace yfy
{
	class string
	{
	public:
		typedef char* iterator;			//string 中的迭代器就是原生指针
		typedef const char* const_iterator;

		iterator begin()
		{
			return _str;
		}

		iterator end()
		{
			return _str + _size;
		}

		const_iterator begin() const
		{
			return _str;
		}

		const_iterator end() const
		{
			return _str + _size;
		}

		/*string()						//默认构造函数
			:_str(new char[1])
			, _size(0)
			, _capacity(0)
		{
			*_str = '\0';//初始化
		}
		*/

		string(const char* str = "")	//给一个空串作为缺省值即可替代上面的默认构造函数
			:_size(strlen(str))
			,_capacity(_size)
		{
			_str = new char[_capacity + 1];
			strcpy(_str, str);
		}

		void swap(string& s)
		{
			::swap(_str, s._str);		//swap前加作用域限定符代表调用全局域中的swap函数，若不加，编译器无法区分
			::swap(_size, s._size);
			::swap(_capacity, s._capacity);
		}
		
		string(const string& s)
			:_str(nullptr)			//若不置空，tmp作为临时变量，在生命结束时调用析构，此时tmp为一个随机值，会出问题
		{
			string tmp(s._str);		//调用构造函数为tmp开空间
			this->swap(tmp);
		}

		~string()
		{
			delete[] _str;
			_str = nullptr;
		}

		//可读可写
		char& operator[](size_t pos)
		{
			assert(pos < _size);
			return _str[pos];
		}

		//只读
		const char& operator[](size_t pos) const
		{
			assert(pos < _size);
			return _str[pos];
		}

		string& operator=(string s)
		{
			this->swap(s);
			return *this;
		}

		void reserve(size_t n)		//重新开辟空间
		{
			if (n > _capacity)
			{
				char* tmp = new char[n + 1];
				strcpy(tmp, _str);
				delete[] _str;
				_str = tmp;
				_capacity = n;
			}
		}

		void resize(size_t n, char ch = '\0')
		{
			if (n <= _size)
			{
				_size = n;
				_str[_size] = '\0';
			}
			else
			{
				if (n > _capacity)
				{
					reserve(n);
				}
				for (size_t i = _size; i < n; i++)
				{
					_str[i] = ch;
				}
				_size = n;
				_str[_size] = '\0';
			}
		}

		void push_back(char ch)
		{
			//if (_size >= _capacity)
			//{
			//	size_t newcapactiy = _capacity == 0 ? 4 : _capacity * 2;
			//	reserve(newcapactiy);
			//}
			//_str[_size] = ch;
			//++_size;
			//_str[_size] = '\0';

			insert(_size, ch);
		}

		void append(const char* str)
		{
			//size_t len = strlen(str);
			//if (_size + len > _capacity)
			//{
			//	reserve(_size + len);
			//}
			//strcpy(_str + _size, str);
			//_size += len;

			insert(_size, str);
 		}

		string& operator+=(char ch)
		{
			push_back(ch);
			return *this;
		}

		string& operator+=(const char* str)
		{
			append(str);
			return *this;
		}

		string& operator+=(const string& s)
		{
			*this += s._str;
			return *this;
		}

		size_t size() const
		{
			return _size;
		}

		size_t capacity() const
		{
			return _capacity;
		}

		string& insert(size_t pos, char ch)
		{
			assert(pos <= _size);
			if (_size == _capacity)
			{
				size_t newcapacity = _capacity == 0 ? 4 : _capacity * 2;
				reserve(newcapacity);
			}
			int end = _size;						//进行头插时，此处的end会变为负数，在与pos比较时会整型提升到无符号数，从而导致死循环
			while (end >= (int)pos)						//解决方案：将比较时的pos强转为int或控制end不能为负数
			{
				_str[end + 1] = _str[end];
				end--;
			}
			_str[pos] = ch;
			_size++;
			return *this;
		}

		string& insert(size_t pos, const char* str)
		{
			assert(pos <= _size);
			size_t len = strlen(str);
			if (len == 0)				//插入空串
			{
				return *this;
			}
			if (_size + len > _capacity)
			{
				reserve(_size + len);
			}
			size_t end = _size + len;
			while (end >= pos + len)			//字符移位
			{
				_str[end] = _str[end - len];
				end--;
			}											
			for (int i = 0; i < len; i++)		//字符填充
			{
				_str[pos + i] = str[i];
			}
			_size += len;
			return* this;
		}

		string& erase(size_t pos, size_t len = npos)
		{
			assert(pos < _size);
			if (len == npos || pos + len >= _size)			//删除的长度大于剩余的长度
			{
				_str[pos] = '\0';
				_size = pos;
			}
			else                                            //删除的长度小于剩余长度，需要把遗留字符移花接木 
			{
				strcpy(_str + pos, _str + pos + len);
				_size -= len;
			}
			return *this;
		}

		const char* c_str()
		{
			return _str;
		}

		void clear()
		{
			_str[0] = '\0';
			_size = 0;
		}

		size_t find(char ch, size_t pos = 0)
		{
			for (size_t i = pos; i < _size; i++)
			{
				if (_str[i] == ch)
				{
					return i;
				}
			}
			return npos;
		}

		size_t find(const char* sub, size_t pos = 0)
		{
			const char* p = strstr(_str + pos, sub);
			if (p == nullptr)
			{
				return npos;
			}
			else
			{
				return p - _str;
			}
		}

	private:
		char* _str;
		size_t _size;
		size_t _capacity;

		static const size_t npos;
	};

	const size_t string::npos = -1;

	ostream& operator<<(ostream& out, const string& s)
	{
		for (size_t i = 0; i < s.size(); i++)
		{
			out << s[i];
		}
		return out;
	}

	istream& operator>>(istream& in, string& s)
	{
		s.clear();
		char ch;
		//in >> ch;                    //如此写会自动忽略换行和空格，导致死循环
		ch = in.get();
		while (ch != ' ' && ch != '\n')
		{
			s += ch;
			ch = in.get();
		}
		return in;
	}

	istream& getline(istream& in, string& s)
	{
		s.clear();
		char ch;
		ch = in.get();
		while (ch != '\n')			//区别在于读到空格不结束
		{
			s += ch;
			ch = in.get();
		}
		return in;
	}

	void print(const string& s1)					//输入时用于清空字符串
	{
		for (int i = 0; i < s1.size(); i++)
		{
			cout << s1[i] << " ";					//s1不可修改，所以[]需要有两个版本
		}
		cout << endl;

		string::const_iterator it = s1.begin();			//s1不可修改，所以定义const迭代器和重载begin和end
		while (it != s1.end())
		{
			cout << *it << " ";
			it++;
		}
		cout << endl;
	}

	bool operator>(const string& s1, const string& s2)
	{
		size_t i1 = 0, i2 = 0;
		while (i1 < s1.size() && i2 < s2.size())
		{
			if (s1[i1] > s2[i2])
			{
				return true;
			}
			else if (s1[i1] < s2[i2])
			{
				return false;
			}
			else
			{
				i1++;
				i2++;
			}
		}
		//abc abc	false
		//abcd abc	true
		//abc abcd	false
		if (i1 == s1.size())
		{
			return false;
		}
		else
		{
			return true;
		}
	}

	bool operator==(const string& s1, const string& s2)
	{
		size_t i1 = 0, i2 = 0;
		while (i1 != s1.size() && i2 != s2.size())
		{
			if (s1[i1] != s2[i2])
			{
				return false;
			}
			else
			{
				i1++;
				i2++;
			}
		}
		if (i1 == s1.size() && i2 == s2.size())
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	bool operator!=(const string& s1, const string& s2)
	{
		return !(s1 == s2);
	}

	bool operator>=(const string& s1, const string& s2)
	{
		return (s1 > s2 || s1 == s2);
	}
	
	bool operator<(const string& s1, const string& s2)
	{
		return !(s1 >= s2);
	}

	bool operator<=(const string& s1, const string& s2)
	{
		return !(s1 > s2);
	}

	string operator+(const string& s1, const char* str)
	{
		string ret = s1;										//存在深拷贝对象，尽量少用
		ret += str;
		return ret;
	}

	void test_string1()
	{
		string s1("hello");
		s1[0] = 'x';
		string s2(s1);//若是编译器默认实现的拷贝构造，则为浅拷贝————导致：1、同一块空间析构两次 2、一个对象修改值，另一个随之改变
		string s3("hello world");
		s1 = s3;
		s1.push_back(' ');
		s1.append("by");
		s1 += ' ';
		s1 += "cplusplus";
	}

	void test_string2()
	{
		string s1;
		string s2(s1);
		//s1 += 'x';
		s1 += "hello";
		s1.resize(2);
		s1.resize(8, 'x');
	}

	void test_string3()
	{
		string s1("hello world");
		//遍历字符串————三种方式：普通for循环、迭代器、范围for
		for (int i = 0; i < s1.size(); i++)
		{
			cout << s1[i] << " ";
		}
		cout << endl;

		string::iterator it = s1.begin();			//指定iterator在string类域中，而不在全局中
		while (it != s1.end())
		{
			cout << *it << " ";
			it++;
		}
		cout << endl;

		for (auto e : s1)			//语法上支持迭代器、迭代器命名规范即可使用范围for--本质上是替换为迭代器
		{
			cout << e << " ";
		}
		cout << endl;

		print(s1);

	}

	void test_string4()
	{
		string s1("hello world");
		s1.insert(5, 'x');
		s1.push_back('!');
		s1.insert(0, ' ');
		s1.insert(0, "hehe");
		s1.erase(0, 5);
		cout << s1.c_str() << endl;
	}

	void test_string5()
	{
		string s1("hello world");
		s1.resize(20);
		s1[18] = 'x';
		cout << s1 << endl;						//区别在于输出了_size个字符
		cout << s1.c_str() << endl;				//遇到\0就终止

		string s2("hehe");
		cin >> s2;
		cout << s2 << endl;
	}
	void test_string6()
	{
		string s1;
		//cin >> s1;			//输入带空格的字符
		//cout << s1 << endl;
		getline(cin, s1);
		cout << s1 << endl;
	}
}

int main()
{
	//yfy::test_string1();
	//yfy::test_string2();
	//yfy::test_string3();
	//yfy::test_string4();
	//yfy::test_string5();
	yfy::test_string6();
	return 0;
}
```

#### 3.3.2 初始化列表

在初始化列表中，除了给成员变量赋值，还可以在括号中使用new为其**开辟空间**。

关于**初始化顺序**：如果在构造函数的初始化列表中对**多个成员变量进行初始化**，类的成员变量的初始化顺序是**由它们在类定义中声明的顺序决定的**，而不是由它们在构造函数的初始化列表中出现的顺序决定的。

例如，考虑以下示例类定义：

```cpp
class Example {
public:
    Example(int a, int b, int c) : _a(a), _b(b), _c(c) {}
private:
    int _a;
    int _b;
    int _c;
};
```

在上面的代码中，类`Example`有三个成员变量`_a`、`_b`和`_c`，它们在类定义中的声明顺序是`_a`、`_b`、`_c`。在`Example`的构造函数中，它们的初始化顺序将按照声明的顺序执行，而不是按照它们在初始化列表中出现的顺序执行。例如，以下构造函数的初始化列表：

```c++
Example(int a, int b, int c) : _c(c), _a(a), _b(b) {}
```

虽然在初始化列表中的顺序是`_c`、`_a`、`_b`，但它们的初始化顺序将按照它们在类定义中的声明顺序来执行，即先执行`_a`，然后执行`_b`，最后执行`_c`。

因此，为了避免出现不可预测的结果，建议在初始化列表中的顺序与成员变量在类定义中的声明顺序保持一致。

## 三、关于string对象的大小

在 Visual Studio 2019 中，当 `basic_string` 的长度小于等于 15 个字符时，标准库会将字符串的字符数组存储在 `basic_string` 对象内部的一个名为 `buf` 的字符数组中，而不是动态分配内存。这个 `buf` 数组的大小是 16 个字节，其中包括 15 个字符和一个用于存储字符串结尾的 null 字符的位置。

因此，如果你在 VS2019 的 32 位环境下创建一个 `basic_string` 对象，它的大小可能是 28 个字节。其中，16 个字节用于存储 `buf` 数组，4 个字节用于存储指向 `buf` 数组的指针，4 个字节用于存储 `size_`，4 个字节用于存储 `capacity_`。但需要注意的是，这个大小可能因为编译器版本、编译器选项等因素而有所不同。



## 四、 浅拷贝缺陷的解决方案

下面我们再来回顾一下浅拷贝：

C++中浅拷贝是指当对象被复制时，只复制指向数据的指针，而不是数据本身。这意味着当一个对象的内容发生更改时，所有指向该对象的指针都会受到影响，从而导致潜在的内存泄漏和程序错误，例如对象的生命周期结束时，多次调用析构函数。

### 4.0 浅拷贝改深拷贝

浅拷贝的缺陷示例：

```cpp
class MyClass {
public:
  MyClass(int size) {
    m_data = new int[size];
    m_size = size;
  }
  ~MyClass() {
    delete[] m_data;
  }
private:
  int* m_data;
  int m_size;
};

MyClass obj1(10);
MyClass obj2 = obj1;
```

在这个例子中，当`obj2`被复制为`obj1`时，`m_data`指针也被复制，这意味着两个对象都指向相同的内存地址。如果一个对象更改了其内部数据，另一个对象也会受到影响。

要解决这个问题，可以使用深拷贝来复制对象。深拷贝不仅复制指针，而且复制指针所指向的数据本身，因此每个对象都有其自己的内存空间。

以下是深拷贝的示例实现：

```cpp
class MyClass {
public:
  MyClass(int size) {
    m_data = new int[size];
    m_size = size;
  }
  ~MyClass() {
    delete[] m_data;
  }
  // 拷贝构造函数
  MyClass(const MyClass& other) {
    m_size = other.m_size;
    m_data = new int[m_size];
    memcpy(m_data, other.m_data, m_size * sizeof(int));
  }
  // 赋值操作符
  MyClass& operator=(const MyClass& other) {
    if (this != &other) {
      delete[] m_data;
      m_size = other.m_size;
      m_data = new int[m_size];
      memcpy(m_data, other.m_data, m_size * sizeof(int));
    }
    return *this;
  }
private:
  int* m_data;
  int m_size;
};

MyClass obj1(10);
MyClass obj2 = obj1; // 使用拷贝构造函数进行深拷贝
MyClass obj3(5);
obj3 = obj1; // 使用赋值操作符进行深拷贝
```



### 4.1 引用计数

引用计数是一种内存管理技术，它通过跟踪对象被引用的次数来决定何时释放内存。当对象被创建时，引用计数被初始化为1。每当对象被复制时，引用计数也会增加1。当对象不再被使用时，引用计数会减少1。当引用计数为0时，内存被释放。

```cpp
class MyClass {
public:
  MyClass(int size) {
    m_data = new int[size];
    m_size = size;
    m_refCount = new int(1);
  }
  ~MyClass() {
    if (--(*m_refCount) == 0) {
      delete[] m_data;
      delete m_refCount;
    }
  }
  // 拷贝构造函数
  MyClass(const MyClass& other) {
    m_data = other.m_data;
    m_size = other.m_size;
    m_refCount = other.m_refCount;
    ++(*m_refCount);
  }
  // 赋值操作符
  MyClass& operator=(const MyClass& other) {
    if (this != &other) {
      if (--(*m_refCount) == 0) {
        delete[] m_data;
        delete m_refCount;
      }
      m_data = other.m_data;
      m_size = other.m_size;
      m_refCount = other.m_refCount;
      ++(*m_refCount);
    }
    return *this;
  }
private:
  int* m_data;
  int m_size;
  int* m_refCount;
};

MyClass obj1(10);
MyClass obj2 = obj1;
MyClass obj3(5);
obj3 = obj1;
```

在这个例子中，引用计数被实现为一个指向整数的指针，它被存储在对象中。每当对象被复制时，引用计数都会增加1。当对象不再被使用时，引用计数会减少1。当引用计数为0时，内存被释放。这种方法可以避免浅拷贝的问题，并且在对象被复制时不需要进行大量的数据复制。但是，引用计数需要额外的内存空间来存储计数器，而且在多线程环境下，需要进行额外的同步和保护。



*引用计数指针不能直接写成int类型，因为这样会导致内存管理出现问题。*

*如果我们将引用计数实现为一个普通的int类型，那么当对象被复制时，每个副本都将拥有一个独立的引用计数。这意味着，如果其中一个副本被销毁，它的引用计数会减少，但是其它副本的引用计数并不会减少，这会导致内存泄漏。*

*使用一个指向int的指针可以解决这个问题。每个副本都可以共享同一个引用计数，这样当其中一个副本被销毁时，它的引用计数会减少，而其它副本的引用计数也会随之减少，从而避免了内存泄漏的问题。*

### 4.2 写时拷贝

写时拷贝（Copy on Write，简称COW）是一种内存管理技术，它允许多个对象共享同一块内存，在对象被修改时才进行复制，从而减少内存的消耗。

在COW中，当一个对象被拷贝时，只有指向原始对象的引用被复制，而不是整个对象。在拷贝对象被修改时，COW会将原始对象复制到一个新的内存地址，并且将引用指向新的内存地址。这个过程是透明的，使用者无需关心内部实现。

COW实现：

```cpp
#include <iostream>
#include <string.h>

class MyString {
public:
  MyString(const char* str) {
    m_data = new char[strlen(str) + 1];
    strcpy(m_data, str);
    m_refCount = new int(1);
  }
  ~MyString() {
    if (--(*m_refCount) == 0) {
      delete[] m_data;
      delete m_refCount;
    }
  }
  // 拷贝构造函数
  MyString(const MyString& other) {
    m_data = other.m_data;
    m_refCount = other.m_refCount;
    ++(*m_refCount);
  }
  // 赋值操作符
  MyString& operator=(const MyString& other) {
    if (this != &other) {
      if (--(*m_refCount) == 0) {
        delete[] m_data;
        delete m_refCount;
      }
      m_data = other.m_data;
      m_refCount = other.m_refCount;
      ++(*m_refCount);
    }
    return *this;
  }
  // 修改字符串时进行复制
  void ModifyString() {
    if (*m_refCount > 1) {
      char* newData = new char[strlen(m_data) + 1];
      strcpy(newData, m_data);
      --(*m_refCount);
      m_data = newData;
      m_refCount = new int(1);
    }
  }
private:
  char* m_data;
  int* m_refCount;
};

int main() {
  MyString str1("hello");
  MyString str2 = str1;
  std::cout << "str1: " << str1 << std::endl;
  std::cout << "str2: " << str2 << std::endl;
  str2.ModifyString();
  std::cout << "str1: " << str1 << std::endl;
  std::cout << "str2: " << str2 << std::endl;
  return 0;
}
```

在这个例子中，MyString类中的m_data指向一个包含字符串的内存块，m_refCount存储引用计数。在拷贝对象时，只有指向m_data的指针被复制，m_refCount也被增加。当对象被修改时，如果引用计数大于1，则表示有多个对象共享同一块内存，此时MyString类会将原始数据复制到一个新的内存地址，并将指针指向新的内存地址。这个过程是透明的，使用者无需关心内部实现。

在上述示例代码中，ModifyString()函数模拟了对字符串进行修改的操作，如果对象被多个引用共享，则会将数据复制到一个新的内存地址，并将引用指向新的内存地址。这样，其他对象的数据不会被修改，也就避免了浅拷贝带来的问题。

COW技术的缺点是它会增加额外的开销，因为每次修改对象时，都需要检查引用计数并复制数据。因此，COW适用于**多读少写**的场景，在写操作比较频繁的场景中可能会带来性能问题。
