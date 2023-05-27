记录《第一本Docker书》的阅读和操作记录，先学习基本知识，再完成《Docker技术入门与实战》的项目部分。

《第一本Docker书》的内容非常的精美，对我这种初学者来说，先有了兴趣，才不会轻易地放弃学习。

这本书也是先基础再项目的，只是说可能会比较旧了。

此书是基于Docker 1.9版本的，也详细说明了多平台的安装教程。

据我之前的了解，Docker是非常吃内存的，突然有点后悔没给我的台式机上32GB内存。



在简介中我了解到Docker的伟大之处，能将软件自动部署到容器中。这么一看好像也没什么，但容器本来就和虚拟机不分上下，有了Docker之后，胜负已分，因为容器最大的缺点就是细节复杂与不好安装。

Docker组件：客户端和服务器，Docker镜像，Registry，Docker容器。

Docker架构：客户端访问服务端，客户端会经过服务端上的Docker守护进程管理容器。



由于我的Ubuntu系统是WSL2的，所以我看了两本书上的安装方案，都不行，包括官方的图形化界面安装，因为我这个WSL2压根就没有图形化界面。

但是通过google我发现微软还有对此的一个[官方文档](https://learn.microsoft.com/zh-cn/windows/wsl/tutorials/wsl-containers)，大概内容是给Windows装一个Docker Desktop，然后能打通WSL2，真巧妙。

如果想单独在Ubuntu上安装好像也可以，这是一篇[Docker官方的文档](https://docs.docker.com/engine/install/ubuntu/)，但最终是找不到要安装的包。所以我还是使用了在Windows上的版本，但需要注意不能同时运行WSL2和Windows的容器。

大致浏览了这两本书的内容后，我好像并不是太喜欢以纯命令行的方式进行操作，并且它们的内容稍微有些落后了，我还是选择官方的教程，虽然是全英文但毕竟我有翻译，值得一提的是我又可以在Mac上进行操作了，对于M系列的芯片来说，虽然只有16GB的内存，但它们在调度方面非常厉害，以至于在某些场合能达到Windows下内存32GB的效果。



我目前还是对image和container无法做到细致的理解，不过image翻译为镜像，类似于一个小型的操作系统，container是容器，类似于一个沙盒，被image所运行，里面装着你的软件，也可以是操作系统，但和其它沙盒隔离开来。



在第一部分，我学习到了，在软件的某个路径下创建一个Dockerfile文件，写上一堆类似脚本或者CMakeLists.txt之类的东西，然后执行`docker build -t app_name .` 就可以生成容器了。然后运行该程序`docker run -dp 3000:3000 app_name` ，其中-d是指分离线程，-p是指定端口映射，比如示例中的容器的3000映射到本机的3000，这样就可以访问启动的程序了。

示例程序是一个网页版TODO-List，感觉之后也可以把我的洗碗网站放进容器中发布。

不过我现在最好奇的是，我能不能把Mac的16GB内存用满，因为Docker Desktop显示已经使用了3.7GB的内存了，活动监视器中显示虚拟机已经占了快8GB内存。



之后教程又引导我对程序进行了修改，然后进行更新软件，简单粗暴的办法是创建一个新容器，将旧容器停止运行然后删除，但需要考虑你的用户用得好好的，突然被强制退出，那多闹心啊。

但我尝试将旧容器停止再重启，好像也没有得到修改的内容。



使用像GitHub一样的操作，创建了远程仓库，然后要把本地的镜像推送上去，然后教程又给我挖坑，使用命令推送失败，说是没找到你这个名的镜像，然后通过登陆帐号，为仓库创建标签啥的，然后就可以推送了。

登陆的时候输入用户名应该是全小写，也就是你仓库显示的你的名，不然我本来注册Docker账户时的命名有大写字母的。

不过推送上去我看系统是Linux arm64的，不知道x86下载下来能用不。

操作如下

`docker login -u YOUR-USER-NAME`

使用docker标签命令给启动镜像一个新名称。

`docker tag getting-started YOUR-USER-NAME/getting-started`

`docker push YOUR-USER-NAME/getting-started`



果然，在其它机器测试时，解决了之前关于不同系统架构的困惑。我的Mac使用默认的命令构建的镜像会是arm64的Linux版本，是没有办法在play with docker平台（基于amd64的）或者我的Windows笔记本上运行，但是在构建镜像的时候可以使用选项，就可以构建amd64的Linux版本。

```bash
docker build --platform linux/amd64 -t YOUR-USER-NAME/getting-started .
```

然后再重新推送上去，再在新平台上执行：

```bash
docker run -dp 3000:3000 YOUR-USER-NAME/getting-started
```



但我们每次重启容器时，数据都会丢失。

我其实不太明白Docker 持久化存储的原理，但我会记录下是怎么操作的，因为这真的很有必要。

```bash
docker volume create todo-db
```

```bash
docker run -dp 3000:3000 --mount type=volume,src=todo-db,target=/etc/todos getting-started
```

这次构建不太一样，因为添加mount 选项，将上一步创建的数据库挂在到容器中。

天呐，我三个月前添加的项目都还在这里，这真是厉害。

如果需要测试是否有效，就添加几个代办事项，然后删除现有容器，再使用和上面同样的命令构建一个新容器，看是否还有那几个项目。



查看数据库信息

```bash
docker volume inspect todo-db
[
    {
        "CreatedAt": "2023-03-03T13:17:04Z",
        "Driver": "local",
        "Labels": {},
        "Mountpoint": "/var/lib/docker/volumes/todo-db/_data",
        "Name": "todo-db",
        "Options": {},
        "Scope": "local"
    }
]
```



volume这个词很神奇，有许多意思，我认为这里的意思应该是「卷」juǎn，还有音量和体积的意思。



当我们修改了某些东西后，都需要花一些时间来重建一个容器，这样比较麻烦，接下来会有更方便的方式进行重建。

具体思路是使用bind mounts，翻译是绑定挂载，mount这个词还有坐骑的意思，上部分使用的是容器挂载，这里的绑定挂载不太一样，是将本机的文件和容器的虚拟环境中的文件进行同步，这样在本机修改的部分就可以自然同步到容器中，然后还有一个叫nodemon的东西在容器中，会对修改进行同步。

下面介绍一下实际操作的命令：

首先确保你的容器正在运行，进入项目的app路径下，如`getting-started/app`，现在只考虑Linux和Mac平台，所以执行命令：

```bash
docker run -dp 3000:3000 \
    -w /app --mount type=bind,src="$(pwd)",target=/app \
    node:18-alpine \
    sh -c "yarn install && yarn run dev"
```

现在修改你的项目，会和发布的容器立即同步，而不需要重启和重构。