# vector的基本使用和模拟实现

## 一、std::vector基本介绍

### 1.1 常用接口说明

`std::vector`是STL中的一个动态数组容器，它可以**自动调整大小**，支持**在数组末尾快速添加和删除元素**，还支持**随机访问元素**。

以下是`std::vector`常用的接口及其说明：

1. `push_back()`: 在容器末尾添加元素

```cpp
std::vector<int> vec{1, 2, 3};
vec.push_back(4);
```

2. `pop_back()`: 删除容器末尾的元素

```cpp
std::vector<int> vec{1, 2, 3};
vec.pop_back();
```

3. `size()`: 返回容器中元素的个数

```cpp
std::vector<int> vec{1, 2, 3};
std::cout << vec.size() << std::endl; //输出3
```

4. `empty()`: 判断容器是否为空

5. `reserve()`: 分配容器的内部存储空间，但不改变元素个数

```cpp
std::vector<int> vec{1, 2, 3};
vec.reserve(10);	// 分配10个int大小的空间
```

其中的reserve接口，你说是分配容器空间，这是在堆上还是栈上开辟空间？那如果重新分配的空间比现有空间小，会发生什么？以及重新分配的空间大于现有空间，是在现有的基础上直接扩容，还是舍弃现有空间，将现有数据拷贝到新的空间上？

新分配的内存空间位于堆上。如果重新分配的空间比现有空间小，`std::vector` 会舍弃多余的元素。如果新分配的空间比现有空间大，`std::vector` 会重新分配内存，并将原有数据复制到新的内存空间中。

需要注意的是，重新分配内存并将原有数据复制到新的内存空间中，可能会导致性能问题。因此，如果您能够预估存储的数据量，建议在创建 `std::vector` 时就预分配足够的内存空间，以避免频繁地重新分配内存。

6. `resize()`: 改变容器的元素个数

```cpp
std::vector<int> vec{1, 2, 3};
vec.resize(5);
```

7. `clear()`: 删除容器中的所有元素

8. `at()`: 返回指定位置的元素

```cpp
std::vector<int> vec{1, 2, 3};
std::cout << vec.at(1) << std::endl; //输出2
```

9. `front()`: 返回容器中第一个元素

10. `back()`: 返回容器中最后一个元素
11. `begin()`: 返回指向容器中第一个元素的迭代器，`end()`: 返回指向容器中**最后一个元素之后位置**的迭代器

```cpp
std::vector<int> vec{1, 2, 3};
for (auto it = vec.begin(); it != vec.end(); ++it) {
    std::cout << *it << " ";
}
```

12. `reverse()`：反转vector

`std::vector::reverse` 不是重新分配容器空间的接口，它是用于反转容器中元素的顺序。也就是将容器中第一个元素和最后一个元素交换，第二个元素和倒数第二个元素交换，以此类推。

### 1.2 代码示例

#### 1.2.1 遍历vector的几种方式

以下示例中分别提到了:下标+[], 迭代器, 范围for, 反向迭代器

```cpp
void test_vector1()
{
	vector<int> v;
	v.push_back(1);
	v.push_back(2);
	v.push_back(3);
	v.push_back(4);
	v.push_back(5);

	for (size_t i = 0; i < v.size(); i++)		//1、下标+[]
	{
		cout << v[i] << " ";
	}

	vector<int>::iterator it = v.begin();		//2、迭代器
	while (it != v.end())
	{
		cout << *it << " ";
		it++;
	}

	for (auto e : v)			//3、范围for
	{
		cout << e << " ";
	}

	vector<int>::reverse_iterator rit = v.rbegin();	// 4、反向迭代器
	while (rit != v.rend())
	{
		cout << *rit << " ";
		rit++;
	}
	// 5 4 3 2 1
}
```

不难发现，用范围for访问的元素可以直接输出，而通过for循环，从v.begin()开始的迭代器it，却需要解引用。这是因为，在范围for循环中，迭代器是被自动解引用的。

在范围for循环中，循环变量的类型是根据容器元素的类型自动推导出来的，而不是容器的迭代器类型。对于`std::vector<int>`容器，其元素类型是`int`，因此在范围for循环中，循环变量的类型是`int`，而不是`std::vector<int>::iterator`。

#### 1.2.2 使用迭代器区间构造对象

```cpp
vector<int> v2(++v.begin(), --v.end());		//利用迭代器区间构造对象————区间左闭右开 
// 2 3 4
string s("hello world");
vector<char> v3(s.begin(), s.end());		//其它容器的迭代器只要类型匹配同样适用

vector<int> v4;
v4.assign(s.begin(), s.end());				//assign接口类似————中文意思为分配
```

以上示例中，提到了利用迭代器区间构建对象这种方式是左闭右开的，例如：`(v.begin() + 1, v.begin() + 4)`，这代表下标为[2, 5)的元素。

只要迭代器的类型与vector所存储的元素类型相同，就可以使用迭代器区间构建新的vector对象。例如上面提到的char类型的vector和char类型的string是可以匹配的。



#### 1.2.3 vector的初始化

```cpp
void test_vector2()
{
	vector<int> v;
	v.reserve(10);//开空间改变容量，但不初始化
	//错误访问——————下标引用操作符会检查插入位置是否合法，即小于_size
	//for (size_t i = 0; i < 10; i++)
	//{
	//	v[i] = i;
	//}
	//正确访问
	for (size_t i = 0; i < 10; i++)
	{
		v.push_back(i);
	}

	v.resize(20);//开空间+初始化
}
```

上述示例中，提到了我在OJ题中经常弄糊涂的内容，使用reserve进行初始化是最好的，因为不会像resize那样，擅自向vector中填入值，但使用后记得不要使用下标引用操作符去访问，而是使用push_back



#### 1.2.4 insert\查找\排序

```cpp
void test_vector3()
{
	int a[] = { 1,2,3,4,5 };
	vector<int> v(a, a + 5);
	//头插
	v.insert(v.begin(), 0);			//第一个参数传入的是迭代器
	//在2前面插入
	vector<int>::iterator pos = find(v.begin(), v.end(), 2);		//find函数位于算法库中algorithm
	if (pos != v.end())		//查找失败会返回end位置的迭代器
	{
		v.insert(pos, 20);
	}
  // 0 1 20 2 3 4 5  
	//sort排序
	sort(v.begin(), v.end());	// 0 1 2 3 4 5 20 
	sort(v.begin(), v.end(), greater<int>());		//greater<int>是一个仿函数类，需要调用库函数是functional
  // 20 5 4 3 2 1 0
}
```

上述例子注释写得很清楚，印象最深的是二叉树后序遍历可以巧用头插获取返回结果。下面主要讲解sort的第三个参数：

`sort`函数的第三个参数是可选的比较函数，用于指定排序时的元素比较规则。当不指定比较函数时，默认使用小于号进行比较，即升序。

在`sort(v.begin(), v.end(), greater<int>())`中，`greater<int>()`是一个函数对象，用于指定降序排序的比较规则。`greater<int>()`是一个模板类，它重载了函数调用运算符`operator()`，实现了比较规则。对于两个元素x和y，如果`greater<int>()(x, y)`返回`true`，则`x`会被排在`y`之前。

因此，`sort(v.begin(), v.end(), greater<int>())`实现了对容器`v`进行降序排序的操作，`greater<int>()`是用于指定比较规则的函数对象，它实现了元素的比较运算。

下面是一个简单的实现：

```cpp
template<typename T>
struct greater
{
    bool operator()(const T& x, const T& y) const
    {
        return x > y;
    }
};
```

这个定义了一个模板类`greater`，它有一个函数调用运算符`operator()`。`operator()`接受两个参数`x`和`y`，表示要比较的两个元素，它的返回值是一个`bool`类型，表示`x`是否应该排在`y`之前。在这个实现中，`operator()`的比较规则是`x`大于`y`，即从大到小排序。

#### 1.2.5 erase删除

```cpp
void test_vector4()
{
	int a[] = { 1,2,3,4,5 };
	vector<int> v(a, a + 5);
	//头删
	v.erase(v.begin());		//参数传入下标位置的迭代器，或迭代器区间
	//删除2
	vector<int>::iterator pos = find(v.begin(), v.end(), 2);
	if (pos != v.end())
	{
		v.erase(pos);
	}
}
```

上述例子中主要写了erase的使用方式，参数是一个vector的迭代器。

## 二、vector模拟实现

这个 `vector` 类中包含了：构造函数、拷贝构造、使用迭代器区间初始化的构造函数、析构。

实现了普通迭代器和只读迭代器，及其对应的`begin()`和`end()`函数。

这个类中的成员变量为私有类型的普通迭代器类型的。记录了vector开始位置、最后一个数据的下一个位置、最大容量的下一个位置。

`size()`和`capacity()`能返回容器的元素个数和已开辟空间大小。

`reserve()`用于为容器重新分配空间，如果分配的空间小于现有空间容量，则不处理。否则，会重新分配空间，拷贝现有数据到新空间，并修改成员变量。

`insert()`用于向容器中插入元素，插入时需要将所在位置及其以后的元素全部向后移动，移动时是从后往前。

`erase()`用于删除某个元素，参数为要删除元素的迭代器。删除后还需要从前往后开始，逐一将元素向前移动。

`resize()`用于调整空间大小，并将未初始化的位置赋予指定的值。当调整的空间小于现有元素个数，会舍弃掉多出的元素。当调整的大小介于元素个数和有效空间之间时，会初始化这些多出来的部分。当调整的大小大于有效空间时，会重新开辟空间，并将现有数据拷贝过去，然后初始化多余部分。

其他接口比较简单，就不再单独描述。

```cpp
	template<class T>
	class vector
	{
	public:
		typedef T* iterator;
		typedef const T* const_iterator;
		vector()
			:_start(nullptr)
			,_finish(nullptr)
			,_endofstorage(nullptr)
		{}
		vector(const vector<T>& v)
			:_start(nullptr)
			, _finish(nullptr)
			, _endofstorage(nullptr)
		{
			reserve(v.capacity());
			for (const auto e : v) {
				push_back(e);
			}
		}
		template <class InputIterator>
		vector(InputIterator first, InputIterator last)
			:_start(nullptr)
			, _finish(nullptr)
			, _endofstorage(nullptr)
		{
			while (first != last) {
				push_back(*first);
				first++;
			}
		}
		~vector() {
			delete[] _start;
			_start = _finish = _endofstorage = nullptr;
		}
		iterator begin() {
			return _start;
		}
		iterator end() {
			return _finish;
		}
		const_iterator begin()const {
			return _start;
		}
		const_iterator end()const {
			return _finish;
		}
		size_t capacity() const{
			return _endofstorage - _start;
		}
		size_t size() const{
			return _finish - _start;
		}
		void reserve(size_t num) {
			if (num > capacity()) {
				size_t sz = size();
				T* tmp = new T[num];
				memcpy(tmp, _start, sz * sizeof(T));
				_start = tmp;
				_finish = _start + sz;
				_endofstorage = _start + num;
			}
		}
		iterator insert(iterator pos, const T& num){
			assert(pos >= begin() && pos <= end());
			if (_finish == _endofstorage) {
				size_t len = pos - _start;
				size_t newcapacity = capacity() == 0 ? 4 : capacity() * 2;
				reserve(newcapacity);
				pos = _start + len;
			}
			iterator end = _finish - 1;
			while (end >= pos) {
				*(end + 1) = *end;
				end--;
			}
			*pos = num;
			_finish++;
			return pos;
		}
		iterator erase(iterator pos) {
			assert(pos >= begin() && pos < end());
			//删除指定下标的数据，并把其后的数据依次向前挪动
			iterator it = pos + 1;
			while (it != end()){
				*(it - 1) = *it;
				it++;
			}
			--_finish;
			return pos;
		}
		void push_back(const T& num){
			insert(end(), num);
		}
		T& operator[](size_t i) {
			assert(i < size());
			return *(_start + i);
		}
		void swap(vector<T>& v) {
			std::swap(v._start, _start);
			std::swap(v._finish, _finish);
			std::swap(v._endofstorage, _endofstorage);
		}
		vector<T>& operator=(vector<T> v) {
			swap(v);
			return *this;
		}
		void resize(size_t n, const T& val = T()) {
			//开的空间小于size（把超出范围的舍弃）介于size和capacity（初始化_finish以后的空间）
			//大于capacity（要重新开空间，并且初始化_finish以后的空间）
			if (n <= size()){
				_finish = _start + n;
			}
			else{
				if (n > capacity()){
					reserve(n);
				}
				while (_finish < _start + n) {
					*_finish = val;
					_finish++;
				}
			}
		}
	private:
		iterator _start;
		iterator _finish;
		iterator _endofstorage;
	};
```

## 三、reverse浅拷贝Bug

不难发现，在上述模拟实现的`vector::reverse()`中，如果模版类型T为`std::string`，那string中含有一些指针成员变量，通过`memcpy`将其浅拷贝到新空间后，又进行了一次delete，在生命周期结束时，也会进行delete，就会造成崩溃。

### 3.1 解决方案

既然要销毁原有空间，那为何不通过`std::move`将左值转换为右值，然后拷贝到新空间去。

下面通过伪代码进行举例：

```cpp
template <typename T>
void MyVector<T>::reserve(size_t newCapacity)
{
    if (newCapacity <= m_capacity)
        return;

    T* newData = new T[newCapacity];
    for (size_t i = 0; i < m_size; i++)
    {
        newData[i] = std::move(m_data[i]); // 深复制
    }

    delete[] m_data;
    m_data = newData;
    m_capacity = newCapacity;
}
```

上述代码没有考虑使用迭代器成员变量，容量和有效数据数量均为`size_t`类型，数据类型为T*

在这个实现中，我们使用了 `std::move` 函数将 `m_data[i]` 的内容移动到 `newData[i]` 中，从而进行深复制，避免了多个 `std::string` 对象共享同一块内存空间的问题。同时，在析构函数中也只需要简单地使用 `delete[]` 删除 `m_data` 指向的内存即可。

### 3.2 move函数

`std::move` 是一个 C++11 中引入的函数，它能够将一个对象的值转移到另一个对象中，同时将原对象置于一种“移动状态”，从而避免不必要的对象复制和销毁。

`std::move` 本质上是将一个左值引用转换成右值引用。在 C++ 中，左值引用是指向左值的引用，右值引用是指向右值的引用。左值是可以取地址的、有持久性的、具名的、具有明确定义的生命周期的值，而右值则是无法取地址的、临时的、没有名称的、生命周期不确定的值。在 C++11 中，我们可以通过使用 `&&` 运算符来声明右值引用。

具体来说，当我们调用 `std::move` 函数时，它将接受一个左值引用，并返回一个右值引用，表示该对象的值可以被移动。通常情况下，我们会将返回的右值引用绑定到另一个对象上，从而将原对象的值移动到新对象中。例如：

```cpp
std::vector<int> v1{1, 2, 3};
std::vector<int> v2 = std::move(v1); // 将 v1 的值移动到 v2 中
```

需要注意的是，在使用 `std::move` 移动对象时，只会移动对象的值，也就是说会将对象的成员变量的值复制到新的内存位置，但不会复制对象的状态，比如对象的引用计数、对象的资源句柄等等。移动完成后，原对象的值会被置为“移后”的状态，这个状态下对象的行为是未定义的，我们不能再对其进行读取或修改。在移动一个对象之后，如果我们需要继续使用该对象，就必须重新对其进行赋值或初始化。



## 四、迭代器失效
### 4.1 基本概念

在 C++ STL 中，vector 的迭代器失效是指在对 vector 进行插入、删除元素等操作后，原先指向 vector 中某个元素的迭代器不再指向正确的元素，或者不能使用。例如，在insert(pos,  x)以后，pos迭代器失效了，不要再去使用pos了。

### 4.2 原因

原因：1、插入可能导致扩容，而异地扩容会导致pos变成“野指针”。
2、就算不扩容，pos指向的位置意义已经变化了，所以也认为失效。  

### 4.3 解决方式

一般来说，避免迭代器失效的方式有以下几种：

1. 使用 const_iterator：当不需要修改元素时，建议使用 const_iterator 迭代器，因为它们是不可修改的，避免了修改元素造成的迭代器失效问题。
2. 使用指针访问元素：使用指针访问元素时，可以直接使用指针算术运算来遍历元素，这样就不会因为在容器中插入或删除元素而使迭代器失效。
3. 避免在循环中删除元素：在循环中删除元素时，会导致迭代器失效，应该在循环外部记录需要删除的元素，然后再循环结束后一次性删除。
4. 使用 erase 或 insert 函数返回值：在删除或插入元素时，使用 erase 或 insert 函数的返回值来更新迭代器，避免迭代器失效。

例如：

```cpp
std::vector<int> v = {1, 2, 3};
std::vector<int>::iterator it = v.begin();

while (it != v.end()) {
    if (*it % 2 == 0) {
        it = v.insert(it, 0);
        ++it;
    }
    ++it;
}
```

5. 如果需要频繁对 vector 进行插入和删除操作，可以考虑使用 list 等其他容器，因为这些容器的插入和删除操作不会导致迭代器失效。

*需要注意的是，在使用 vector 时，迭代器失效可能会导致程序崩溃或者出现不可预测的结果，因此在对 vector 进行插入、删除等操作时，一定要注意迭代器的更新和使用。*