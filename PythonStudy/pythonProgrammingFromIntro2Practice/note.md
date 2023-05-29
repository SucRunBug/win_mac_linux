``

# Python编程：从入门到实践（第2版）



## 读前准备

本书的译者建议在42小时内读完第一部分，第一部分只需大致浏览，主要内容为基础的语法。

然后着手第二部分的实践，不会的地方在第一部分中寻找答案，所以第二部分才是精华，这样学习也不会一开始就丧失热情。

这也让我想起学习swiftUI的过程，过程其实痛苦但不失乐趣。



### 环境配置

在一开始，一定是环境安装，对我而言，我需要对三台设备，五个系统的python 3环境进行安装和检查。

安装环境是最有意思的一部分，但有时也会让我抓狂。

#### Windows

直接进入python官网，我此时下载的最新版本 3.11.3

之后下载了一个不错的编辑器，虽然我之前一直使用的是vs code，但我现在想尝试一下sublime text

#### macOS

我的Mac上装了很多的python版本，作为一个强迫症患者，只想保留一个最新的。

除此之外，我还学到了ctrl+d可以终止程序，类似于win上的ctrl+z

但非常可惜，我在官网安装了最新版的python后，在终端中查看python版本依然是旧版本。

google后发现一个奇招，在配置文件`.bash_profile`中加入

```bash
alias python="/usr/local/bin/python3"
alias python3="/usr/local/bin/python3"
```

这个路径是在官网安装最新版本的位置。

于是我又去研究了一下这个神秘的`alias`，发现还可以把macOS搞出和Linux一样命令，比如

```bash
alias ll='ls -al'
```

总之，我们将最新的python路径给了这两个命令，以便我们可以随时使用最新版本，而且不需要导入环境变量。

如果之后再有机会，我应该尝试用homebrew进行下载。

#### Linux

我用的是WSL2的ubuntu，以前也装过python环境，不过也是旧版本，暂且不折腾最新版了，因为3.8够用了。

还有就是没有办法装sublime text，就只能将就vs code使用了。



### 无穷的技艺

这是课后的一道练习题，让我草拟三个想要创建的程序。

首先我非常需要python来作为网站后端代码，来处理相关数据等，那么第一个期待的程序是一个由python负责后端的网站程序。

其次是一个天气软件，只需要使用python爬虫获取某个气象网站的数据即可，这为我开发苹果生态的天气app应该会很有帮助。

最后我希望不要忘记玩耍，即开发一个有趣的游戏，好像本书后面会介绍游戏开发。



## 项目

由于之前有了解python的基本语法，所以我直接上手项目了。

令我惊讶的是，这书中的三个项目正好囊括了我所有的需求，真是非常的巧，那么就按我的顺序来吧。

### Web应用程序——基于Django的在线学习笔记

在macOS上，进入虚拟环境，安装好django后，执行python manage.py migrate报错，找不到django，我怀疑是环境变量相关的问题，明天再看看，重启之后再试一下，或者改用Linux开发，甚至是Windows。

我将昨天通过alias重命名的配置文件进行注释后，果然就可以成功运行了。

我在Ubuntu上又试了一下，也是没问题。但书上说要在虚拟环境中使用python而不是python3，那我Ubuntu上的没法用python命令，那我可以安心在我的Mac上继续开发了。



这是关于Django数据查询的文档https://docs.djangoproject.com/en/4.2/topics/db/queries/



在我完成我的meal_planner的时候，我发现python命令失效了。

报错zsh: command not found: python

我查了下python版本，竟然从之前的3.8变成了3.11，没办法，我也不敢用重命名，只有硬着头皮用python3了。



课后练习已经第二次让我从头创建一个新项目了，所以在此记录从头创建Django项目的流程。

```bash
# 创建一个项目主目录，然后进去
# 创建一个虚拟环境
python -m venv your-env-name_env
# 书中说明务必使用python而不是python3

# 激活虚拟环境
source your-env-name_env/bin/
# 如果需要停止虚拟环境
deactive

# 在虚拟环境中安装Django
pip install django

# 在Django中创建项目
django-admin startproject project_name .
# 注意别忘了点号，否则需要删除新增的文件。

# 创建数据库
python manage.py migrate

# 启动项目
python manage.py runserver

# 创建应用程序
python manage.py startapp app_name
```

至此，基础的框架就搭建完毕了，现在只需要根据需求自定义编码了。

整理下来还是好的，因为我发现我把项目和应用名称都混为一谈了。



接下来是定义模型，在应用软件的目录下，找到model.py文件，在这里进行编辑。

模型主要是指用户想要输入的数据类型，比如学习的主题，学习的科目等等，那我们就可以使用类来接收这些数据。

再比如我要弄一个饮食计划表，那就需要日期、早中晚和对应的食谱。

```python
from django.db import models

# Create your models here.
class Date(models.Model):
    """日期，如一月三日"""
    text = models.TextField(max_length=20)
    date_added = models.DateTimeField(auto_now_add=True)

    def __str__(self):
        return self.text
    
class Time(models.Model):
    """早中晚"""
    date = models.ForeignKey(Date, on_delete=models.CASCADE)
    text = models.TextField(max_length=20)
    date_added = models.DateTimeField(auto_now_add=True)

    class Meta:
        verbose_name_plural = 'times'   # 没明白有啥用
    
    def __str__(self):
        return self.text
    
class Meal(models.Model):
    """吃的啥"""
    time = models.ForeignKey(Time, on_delete=models.CASCADE)
    text = models.TextField()
    date_added = models.DateTimeField(auto_now_add=True)

    def __str__(self):
        return self.text

```

创建好模型后就需要**激活模型**

进入项目目录中的setting.py，将此应用程序包含进该项目

为列表INSTALLED_APPS的首元素加上自己的应用程序的名字，再次强调，请务必将自己的程序放在该列表的最前面。



我们新增了模型，所以也要修改数据库

```bash
python manage.py makemigrations app_name
```

使数据库能存储之前定义的模型类型的数据

再**迁移数据库**

```bash
python manage.py migrate
```



接下来为我们的网站**添加一个超级用户**用于管理：

```bash
python manage.py createsuperuser
```

根据引导完成信息录入即可。

然后需要**向管理网站注册模型**

在应用程序目录下找到admin.py文件

向网站注册类名，比如注册之前的Date类，输入

```py
from .models import Date
admin.site.register(Date)
```



**通过Django shell 观察变量是否正确**

```bash
python manage.py shell
```





### 数据可视化



### 外星人入侵