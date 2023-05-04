# 跟着廖雪峰学Java

## 基本知识

Java是先将代码编译为字节码，然后字节码可以在不同平台的Java虚拟机（JVM）上运行。

一些缩写单词的全称和解释

- Java SE：Standard Edition
- Java EE：Enterprise Edition

- JDK：Java Development Kit
- JRE：Java Runtime Environment

JRE就是运行Java字节码的虚拟机，但如果只有Java源码，要编译成Java字节码，就需要JDK，因为JDK除了包含JRE，还提供了编译器、调试器等开发工具。



## 下载jdk

进入oracle官网，根据系统下载最新版本的jdk，根据提示安装。

macOS上好像并不用特意配置环境变量，下载好.dmg文件后，完成安装引导，即可在终端中输入`java -version`查看最新下载的jdk版本信息。

Windows上需要进入系统设置，配置系统的Path环境变量，为下载的jdk目录下的bin目录

在Windows中如何创建环境变量？在设置搜索框中，直接搜索环境变量，找到后可以新建环境变量，比如`JAVA_HOME`，要查看环境变量，可以在命令行中输入`echo %JAVA_HOME%`，我一开始是没有查看到的，重启电脑后就可以查看了。

下载好jdk后，当然需要将其目录下的bin目录导入环境变量Path中，这样才能使用`java -version`查看版本信息。



## 第一个Java程序

使用vs code创建一个hello.java文件

```java
public class hello {
    public static void main(String args[]) {
        System.out.println("hello world!");
    }
}
```

编译这个hello.java文件：

`javac hello.java`

这一步是将Java源码编译成Java字节码，可以看到当前目录下多了一个hello.class的文件。

再运行这个Java字节码：

`java hello`



但还有更简单的方式，可以直接将Java源码运行：

`java hello.java`

而且不会产生hello.class的文件。



原文中还提到了：

*这是Java 11新增的一个功能，它可以直接运行一个单文件源码！*

*需要注意的是，在实际项目中，单个不依赖第三方库的Java源码是非常罕见的，所以，绝大多数情况下，我们无法直接运行一个Java源码文件，原因是它需要依赖其他的库。*

所以此方法虽然方便，但也是有局限性的。



## 使用IDE编程

之前没有在Mac上用过eclipse，只用过jet brains的IntelliJ Idea，所以打算尝试一下eclipse

Mac版的安装完成后，我发现如果按默认的路径安装，会在用户路径下安装而不是程序应用的路径下，所以不能在Mac的启动台里看到，需要更改下路径才行。还有一点是关于偏好设置的位置已经不在原文中说的地方，是在导航栏中的eclipse->settings中

在修改Java compiler的时候，我的Compiler compliance level没有20这个选项，最多只支持到了19，我的也是最新的eclipse，也安装原文中介绍的方法，打开Help - Eclipse Marketplace，搜索Java 20 Support安装后重启，但依旧没能出现20

Windows平台在安装eclipse后，暂时没有上述问题，jdk为20版本。

创建新的Java project：File->New

然后在弹出的选项框内，JRE选择为Java SE 20，暂时不要勾选“Create module-info.java file”

然后新工程的src的目录下右键创建一个class，class名需要和主类名相同。

比如我们创建好了一个Hello.java的类，右键选择run as，再选择Java Application

在运行上述的hello world代码时，两个平台均出现了如下错误：

```sh
java.lang.UnsupportedClassVersionError: Hello (class file version 63.65535) was compiled with preview features that are unsupported. This version of the Java Runtime only recognizes preview features for class file version 64.65535
```

我估计原因是勾选了preview features即预览功能，而且奇怪的是编译版本是jdk-19，而不是20

取消勾选后，成功运行代码。

### 在IDE中安装插件

