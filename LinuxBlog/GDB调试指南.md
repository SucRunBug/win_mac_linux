# Linux下C++调试指南

在 Linux 下进行 C++ 调试时，可以使用以下几种方法：

## 1. GDB 调试

GDB 是一款强大的调试工具，可以用于调试 C++ 程序。以下是 GDB 的基本使用方式：

### 1.1 编译时添加调试信息

使用 `-g` 选项编译程序时，可以将调试信息加入可执行文件中。例如：

```sh
g++ -g -o prog prog.cpp
```

### 1.2 启动 GDB

使用 `gdb` 命令启动 GDB：

```sh
gdb prog
```

### 1.3 设置断点

可以使用 `break` 命令设置断点。例如：

```sh
break main
```

`break main` 命令是将断点设置在 `main` 函数的第一行上。

要在指定的某一行上打上断点，可以使用以下命令：

```sh
break filename:linenum
```

其中，`filename` 是要打断点的文件名，`linenum` 是要打断点的行号。例如，要在 `example.cpp` 文件的第 10 行打上断点，可以使用以下命令：

```sh
break example.cpp:10
```

另外，如果想要在函数内部打上断点，可以使用函数名加上行号的方式来指定。例如，要在 `example.cpp` 文件中的 `my_function()` 函数的第 5 行打上断点，可以使用以下命令：

```sh
break example.cpp:my_function:5
```

### 1.4 运行程序

使用 `run` 命令运行程序。例如：

```sh
run arg1 arg2
```

其中，`arg1` 和 `arg2` 是被调试程序的两个命令行参数。

### 1.5 打印变量

使用 `print` 命令打印变量的值。例如：

```sh
print i
```

### 1.6 单步执行

使用 `next` 命令单步执行。

`next` 命令是单步执行当前代码行，不会进入函数体内部执行。如果当前代码行是一个函数调用语句，则 `next` 命令会将整个函数调用看作一个单独的操作，并直接执行函数调用并返回到函数调用后的下一行代码。

如果您需要进入函数体内部单步执行，可以使用 `step` 命令。这个命令会单步执行函数内部的代码，直到执行完函数体内的所有语句并返回到函数调用后的下一行代码。

### 1.7 继续执行

使用 `continue` 命令继续执行。

`continue` 命令用于从当前断点处继续执行程序，直到遇到下一个断点或者程序结束。

如果您想在没有下一个断点的情况下让程序停止，可以使用 `ctrl + c` 组合键强制中断程序的执行。这样程序将被停止并回到 GDB 命令行界面，您可以使用其他命令来检查程序状态、修改变量值等。

### 1.8 结束程序

使用 `quit` 命令结束程序。

### 1.9 利用GDB分析Core Dump文件

GDB 可以分析 Core Dump 文件，这些文件是在程序崩溃时生成的包含程序崩溃信息的文件。

要分析 Core Dump 文件，可以使用以下命令启动 GDB：

```sh
gdb <executable> <core_dump_file>
```

其中，`<executable>` 是生成 Core Dump 文件的可执行文件的名称，`<core_dump_file>` 是 Core Dump 文件的名称。例如，如果可执行文件名为 `my_program`，Core Dump 文件名为 `core`，则启动 GDB 的命令为：

```sh
gdb my_program core
```

启动 GDB 后，您可以像调试正在运行的程序一样使用 GDB 进行调试。您可以使用 `bt` 命令查看程序崩溃时的函数调用栈，使用 `print` 命令查看变量的值，使用 `run` 命令重现程序崩溃时的情况，并使用其他命令来分析和调试程序崩溃的原因。

注意，如果程序崩溃时未生成 Core Dump 文件，则无法使用 GDB 进行分析。因此，在调试程序时，如果您希望能够使用 GDB 分析程序崩溃信息，可以通过设置系统的 Core Dump 大小和文件生成方式来生成 Core Dump 文件。具体方法可以参考您所使用的操作系统的文档。

## 2. 多线程调试

在多线程程序中进行调试时，需要考虑线程之间的同步和竞争条件。以下是在 GDB 中进行多线程调试的一些常用命令：

### 2.1 查看线程

使用 `info threads` 命令可以查看当前所有线程的信息。

### 2.2 切换线程

使用 `thread` 命令可以切换当前执行的线程。例如：

```sh
thread 2
```

`thread` 命令后的数字是线程 ID。在多线程程序中，每个线程都有一个唯一的线程 ID，用于区分不同的线程。

使用 `info threads` 命令查看当前程序中的线程ID

### 2.3 打印线程调用栈

使用 `bt` 命令可以打印当前线程的调用栈。

### 2.4 打印线程的局部变量

使用 `info locals` 命令可以打印当前线程的局部变量。

### 2.5 打印线程的全局变量

使用 `info variables` 命令可以打印当前线程的全局变量。

## 3. 多进程调试

下面我列举一个代码示例，可以帮助您快速上手多进程调试。

```c
#include <unistd.h>
#include <stdio.h>

int main() {
    pid_t pid = fork();
    if (pid == 0) {
        printf("Child process\n");
    } else {
        printf("Parent process\n");
    }
    return 0;
}

```

该程序会创建一个子进程并打印出相应的信息。现在，假设我们想要在 GDB 中进行多进程调试，可以按照以下步骤进行：

### 3.1 启动 GDB 并加载程序

```sh
gdb program_name
```

### 3.2 设置跟踪进程模式

```sh
set follow-fork-mode child
```

该命令将 GDB 的跟踪进程模式设置为跟踪子进程。

### 3.3 设置断点

```sh
break main
```

### 3.4 运行程序

```sh
run
```

### 3.5 在程序运行到断点处时，切换到子进程

如果调试完子进程，可以将 GDB 的跟踪进程模式切换回父进程，并暂停在 fork() 函数的返回处。

```sh
set follow-fork-mode parent
```

### 3.6 列出进程

```sh
info inferiors
```

该命令将列出当前 GDB 控制的所有进程。

### 3.7 切换到子进程

```sh
inferior <n>
```

 其中，`<n>` 是要切换到的进程编号。在本例中，由于子进程是第二个被创建的进程，因此子进程的编号应为 2。

`inferior`命令后面可以跟进程号或者进程标识符（PID）

### 3.8 开始调试子进程：

现在，您可以使用 GDB 调试子进程，如在子进程中设置断点、查看变量值等。

在完成子进程调试后，可以使用 `inferior 1` 命令切换回父进程，继续进行父进程的调试。

## 4. 其它调试方式

除了 GDB 调试之外，还有一些其它的调试方式，例如：

### 4.1 使用 Valgrind 进行内存泄漏检查

Valgrind 是一款用于检查内存泄漏、内存越界等问题的工具。使用 Valgrind 可以通过以下命令进行检查：

```sh
valgrind --leak-check=full ./prog
```

### 4.2 使用 strace 跟踪系统调用

strace 可以用来跟踪程序执行时的系统调用。例如：

```bash
strace ./prog
```

### 4.3 使用 ltrace 跟踪库函数调用

ltrace 可以用来跟踪程序执行时的库函数调用。例如：

```sh
ltrace ./prog
```

### 4.4 使用 gdbserver 远程调试

gdbserver 是 GDB 的一个组件，可以在远程主机上进行调试。首先在远程主机上启动 gdbserver：

```sh
gdbserver host:port prog arg1 arg2
```

然后在本地主机上启动 GDB 并连接到远程主机：

```sh
gdb prog
(gdb) target remote host:port
```

