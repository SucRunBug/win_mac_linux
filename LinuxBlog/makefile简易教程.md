# makefile简易教程

## 一、学习目标

达到多文件快速编译的需求，相关符号的意思，以及其它注意事项。

## 二、快速入门

### 2.1 基本概念

Makefile 是一个在Unix和Linux操作系统上使用的构建工具，用于自动化编译和构建源代码。

### 2.2 用处

通过Makefile，你可以定义一系列规则来**指示如何构建源代码**，以及定义每个规则所需要的依赖关系。Makefile也可以在构建过程中执行自定义命令，例如清理旧的构建输出、运行测试等。

### 2.3 基本语法

```makefile
目标文件:依赖文件
	指令
target: dependencies
	command
```

*其中，`target` 表示构建目标（例如可执行文件、库文件、对象文件等），`dependencies` 表示构建目标所依赖的文件（可以是其他目标或源代码文件），`command` 表示构建目标的命令。注意，`command` 前面必须有一个tab（制表符）。*

例如：

```makefile
test:test.c
	gcc -o test test.c
```



### 2.4 变量

Makefile还支持变量，用于在不同规则之间共享常量或者路径等信息。变量的定义格式如下：

```makefile
VARIABLE_NAME = variable_value
```

示例：

```makefile
CC = gcc
CFLAGS = -Wall -Werror

hello: hello.c
    $(CC) $(CFLAGS) -o hello hello.c
```

*在这个示例中，`CC` 变量定义了编译器的名称，`CFLAGS` 变量定义了编译器的参数。在构建`hello`目标时，Makefile使用这些变量来执行编译命令。*



### 2.5 构建多个目标

当有多个.c文件需要进行编译时，可以这样构建makefile

```makefile
all: hello world

hello: hello.c
    $(CC) $(CFLAGS) -o hello hello.c

world: world.c
    $(CC) $(CFLAGS) -o world world.c
```



*在这个示例中，`all` 是一个伪目标，它依赖于`hello`和`world`。当我们运行`make all`时，Make会自动构建`hello`和`world`目标。*

*注意，`all`是一个伪目标，它并不会生成任何文件。在Makefile中，有一些特殊的目标（例如`all`、`clean`等）通常被定义为伪目标。*

### 2.6 清理目标

Makefile还可以定义清理目标，用于删除构建过程中生成的临时文件。例如：

```makefile
clean:
    rm -f hello world *.o
```

*在这个示例中，`clean` 是一个伪目标，它的命令会删除当前目录下的`hello`、`world`和所有`.o`文件。*

### 2.7 .PHONY

在Makefile中，`.PHONY`是一个特殊的目标，用于声明一些伪目标（phony target），也就是那些不对应任何实际的文件或目录，只是用来执行一些特定的操作的目标。

声明伪目标的方式很简单，只需要在Makefile中添加以下语句：

```makefile
.PHONY: target1 target2 ...
```

其中，`target1`、`target2`等为伪目标的名称，可以声明多个伪目标，它们之间用空格分隔。

使用`.PHONY`声明伪目标的好处在于，它可以避免出现一些潜在的问题，例如：

1. 当伪目标与实际文件或目录同名时，Makefile可能会将其误认为是一个文件或目录，从而导致错误的行为。使用`.PHONY`可以**显式地声明这些目标是伪目标**，避免这种情况发生。
2. 如果在命令行中执行`make target`时，`target`已经存在，并且其时间戳比依赖文件的时间戳更晚，Makefile默认情况下不会重新构建`target`。但是如果`target`是一个伪目标，那么即使时间戳较晚，Makefile也会执行伪目标下的命令，因为伪目标并不对应任何实际的文件或目录。

例如，我们可以在Makefile中声明一个名为`clean`的伪目标，用于清理项目中生成的临时文件和目录，如下所示：

```makefile
.PHONY: clean

clean:
    rm -rf build/* bin/*

```

在命令行中执行`make clean`时，Makefile会执行`rm -rf build/* bin/*`命令，清理`build`目录和`bin`目录中的所有文件。

需要注意的是，`.PHONY`目标本身并不会执行任何操作，它只是用来声明伪目标。因此，在Makefile中，`.PHONY`目标通常放在文件的最前面或最后面，以方便查看和维护。

### 2.8 $符号

在Makefile中，$@和$^等符号是自动化变量，它们代表了不同的含义：

- $@ 表示当前规则中的目标文件，即被生成的文件名。
- $^ 表示当前规则中所有的依赖文件，以空格分隔。

例如：

```makefile
makefileCopy code
app: main.o utils.o
    gcc $^ -o $@

main.o: main.c
    gcc -c $< -o $@

utils.o: utils.c
    gcc -c $< -o $@
```

在上面的示例中，我们定义了一个目标文件app，它依赖于两个对象文件main.o和utils.o。在生成目标文件时，我们使用$^将所有依赖文件都链接在一起，使用$@作为生成的目标文件名。同时，我们还定义了两个规则分别用于生成main.o和utils.o文件，在这里我们使用$<代表当前规则中的第一个依赖文件，即源代码文件名。

### 2.9 makefile与其它语言冲突

当使用makefile辅助编译时，如果编译选项中包含了$()这样的字符，它会与makefile的语法产生冲突，导致make失败报错。这是因为$()是makefile中的语法，用于在命令中引用变量。

为了解决这个问题，有几种可能的方法：

1. 转义字符：可以在$()前添加转义字符（如\）来告诉make解释器它是一个普通字符而不是makefile中的变量。例如，可以将编译选项中的$()替换为$()。
2. 单引号：可以将编译选项用单引号括起来，这样make解释器就不会解释其中的字符。例如，可以将编译选项写为'$(CFLAGS)'，其中CFLAGS是一个makefile变量。
3. 双美元符号：在一些情况下，可以使用$$来代替$，这样make解释器就不会将它视为makefile变量，而是将其传递给shell。例如，可以将编译选项写为'-L$$HOME/lib'，其中$$HOME将被shell解释为$HOME。

需要注意的是，以上方法中的每一种都有其适用的情况和限制，具体使用哪种方法要根据具体情况而定。另外，如果编译选项中的$()是用于在shell脚本中引用变量，那么可以使用shell的变量替换功能，将$()替换为$${}，例如将编译选项写为'-L$${HOME}/lib'。



例如我在使用C语言连接MySQL数据库时，使用到的编译选项中含有$()的字样，然而makefile中，这类字样指代某个变量，所以产生冲突，导致无法make成功。

可以将编译选项改为$$()，双美元符号可以提示makefile需要显式调用的。

上述例子如下：

```makefile
.PHONY:all
all:test_cgi mysql_conn

test_cgi:test_cgi.cc
	g++ -o $@ $^

mysql_conn:mysql_conn.cc
	g++ -o $@ $^ -std=c++11 $$(mysql_config --cflags --libs)

.PHONY:clean
clean:
	rm -f test_cgi mysql_conn
```

