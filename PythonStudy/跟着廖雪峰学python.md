# 跟着廖雪峰学python

python的优缺点

python虽然简单明确，但是作为一门**解释型语言**，运行速度相较于C，慢了不少。在执行python代码时，会一行行地解释为CPU理解的机器码，而C语言是在运行前直接编译成CPU能直接运行的机器码。



但慢的时间用户不一定能感知到，因为网络的延迟会大的多。



第二个缺点是代码不能加密，不过只会影响需要把软件出售的人。



那Python适合开发哪些类型的应用呢？

首选是网络应用，包括网站、后台服务等等；

其次是许多日常需要的小工具，包括系统管理员需要的脚本任务等等；

另外就是把其他语言开发的程序再包装起来，方便使用。



Mac安装python

我的Mac以前装过python环境，而且Mac会自带python2的版本，要装python3，可以先安装homebrew，然后切换安装源到国内镜像，以免被墙，然后执行`brew install python3`

装好之后，执行`python3 --version`查看python版本，执行`python3`可以开始使用python进行编程了。

与C的编译器不同但类似，python的是叫解释器，我们安装的官方版本是叫Cpython的解释器，因为是用C语言写的，所以叫这个，也就是我们执行`python3`所启动的解释器。



在Linux或macOS上是可以直接运行.py文件的，前提是在文件首行加上`#!/usr/bin/env python3`，然后将.py文件权限修改一下，执行`chmod a+x hello.py `



输入和输出

print函数可以支持多个参数，如下所示：

```python
print('The peer', 'have been', 'closed connection')
```

像这样可以输出：`The peer have been closed connection`

print函数中的每个逗号会自动带上一个逗号。



print还可以直接输出数字或计算结果：

```py
print(100)	# 100
print(100 + 200)	# 300
```



有了上面print的用法，我们可以组织一次漂亮的打印

```py
print('114510 + 4 =', 114510 + 4)		# 114510 + 4 = 114514
```



如果想要从键盘输入某个值赋予某个变量，可以使用`input()`函数

```py
voice = input()	# 输入 哼哼啊啊啊啊啊啊啊啊啊啊啊啊啊啊
print(voice)		# 输出 哼哼啊啊啊啊啊啊啊啊啊啊啊啊啊啊
```

但如果直接这样使用，在需要输入时，用户并不知道什么时候开始输入，那么可以像如下例子一样添加一点提示用语：

```py
voice = input('请输入你的口头禅：')			# 请输入你的口头禅：How there, Mister!
print('我每天都会随口说出：', voice)		# 我每天都会随口说出： How there, Mister!
```

