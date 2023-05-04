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
	yfy::test_string1();
	//yfy::test_string2();
	//yfy::test_string3();
	//yfy::test_string4();
	//yfy::test_string5();
	// yfy::test_string6();
	return 0;
}