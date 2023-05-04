# Linux自主Web服务器连接MySQL数据库

**通过cgi的方式访问数据库，和http无关**

## 零、在基于WSL 2的Ubuntu中安装MySQL

参考文章[wsl2（Ubuntu-22.04）配置mysql8.0_wsl安装mysql8.0_叶梨子的博客-CSDN博客](https://blog.csdn.net/laichilizi/article/details/125779829)

原文非常细致，有多种登录方式，修改root 密码，远程访问数据库，甚至还有卸载指南。

```sh
sudo apt update
sudo apt install mysql-server
```

安装结束后需要启动MySQL服务。

WSL2子系统Ubuntu中不包含systemctl命令，取而代之的是service命令。

```sh
sudo service mysql start
```

最直接的登录方式：

```sh
sudo mysql -u root -p
```

默认是没有密码的。



## 一、下载c语言连接MySQL的相关工具

*此处仅具有阅读价值，不具备操作价值，如果执行了第三部分，依旧不能成功编译，再选择按此方式安装*

1. 进入MySQL官网，进入顶栏的DEVELOPER ZONE
2. 进入左边栏的MySQL Downloads，点击右下方的Download Archives
3. 点击MySQL Connector/C，选择最新版本和Linux-Generic，下载64位的版本
4. 将解压好的文件放入cgi目录下

*其中最需要的是/include/ 和/lib/ 路径下的文件*

将这两部分单独移动到上级目录使用。

**知识回顾：**编译时，会在环境变量和当前路径寻找头文件，如果需要使用其他路径下的头文件，添加编译选项-I 路径 -L 库目录 -l库名称

例如g++ -o mysql_conn mysql_conn.cc -I include -L lib -lmysqlclient

**判断库名称**，是去掉lib前缀和.及后缀。

因为mysqlclient库是动态链接的，通过ldd+可执行程序，可得编译出的程序并不包含该库，理论上是无法运行的，需要导入该库到环境变量中，即执行export LD_LIBRARY_PATH=./lib/

*但笔者在这里并未将该库路径导入环境变量，却可以成功执行*

为了防止之后出现问题，采用静态链接的方式进行编译，即g++ -o mysql_conn mysql_conn.cc -I include -L lib -lmysqlclient -lpthread -ldl -static

此时已经基本算搭建好环境了，下面需要为数据库创建用户，并赋予相应权限。

## 二、创建用户

```mysql
create user 'http_test'@'localhost' identified by '123456';
```

查看用户是否创建成功：

```mysql
use mysql;
select * from user\G;
```

再使用root用户创建数据库：

```mysql
mysql> create database http_db;

mysql> use http_db;
Database changed

mysql> create table if not exists user(
    -> id int unsigned auto_increment primary key,
    -> name varchar(16) not null,
    -> password varchar(32) not null
    -> )engine=InnoDB default charset=utf8;
Query OK, 0 rows affected, 1 warning (0.02 sec)
```

插入数据以便测试：

```mysql
mysql> insert into user (name, password) values('张三', '123456');
```

赋权：

```mysql
mysql> grant all on http_db.* to 'http_test'@'localhost';
```



## 三、运行代码连接数据库 

*笔者在运行下列代码时，出现了意外，不知道是什么错误，导致连接不上*

```cpp
#include <iostream>
#include <string>
#include <mysql.h>

int main()
{
    // std::cout << "version: " << mysql_get_client_info() << std::endl;    // mysql client vision
    MYSQL *conn = mysql_init(nullptr);  // 初始化mysql句柄
    if (nullptr == mysql_real_connect(conn, "127.0.0.1", "http_test", "123456", "http_db", 3306, nullptr, 0))   // 连接到MySQL
    {
        std::cerr << "connect error" << std::endl;
        return 1;
    }
    std::string sql = "insert into user (name, password) values(\'李四\', \'123456\')";
    std::cerr << "query: " << sql << std::endl;
    mysql_query(conn, sql.c_str());
    std::cerr << "connect success" << std::endl;
    mysql_close(conn);  // 关闭连接
    return 0;
}

```

*正好笔者今天拿到了一个chatGPT的账号，就去问了AI，没想到顺利解决了*

首先安装MySQL客户端：

```sh
sudo apt-get install libmysqlclient-dev
```

然后可以编译上面写好的代码：

```sh
g++ -o mysql_conn mysql_conn.cc -std=c++11 $(mysql_config --cflags --libs)
```

不需要冗余的编译选项，不用去官网下载开发组件，只需要加一串MySQL用的编译选项即可。

如果执行上述代码后，发现数据库中插入的数据为乱码，说明出现编码问题。

*笔者在这里并没出现乱码问题，如果您出现了此问题，可以在初始化MySQL句柄后添加一行代码：*

```c
mysq_set_character_set(mfp, "utf8");
```

基本测试已经完成，下面进行将表单信息提交给数据库。



如果表单提交的信息中含有中文，会涉及解码操作，先暂时只考虑英文的情况。

先打印出从表单获取的信息，再从中分解出有效信息，合成到sql语句中，然后执行。

执行成功的话可以跳转到一个提示页面或者打印提示信息。



