记录《第一本Docker书》的阅读和操作记录，先学习基本知识，再完成《Docker技术入门与实战》的项目部分。

## 学前准备

### 教程选择

《第一本Docker书》的内容非常的精美，对我这种初学者来说，先有了兴趣，才不会轻易地放弃学习。

这本书也是先基础再项目的，只是说可能会比较旧了。

此书是基于Docker 1.9版本的，也详细说明了多平台的安装教程。

据我之前的了解，Docker是非常吃内存的，突然有点后悔没给我的台式机上32GB内存。

### 基本知识

在简介中我了解到Docker的伟大之处，能将软件自动部署到容器中。这么一看好像也没什么，但容器本来就和虚拟机不分上下，有了Docker之后，胜负已分，因为容器最大的缺点就是细节复杂与不好安装。

Docker组件：客户端和服务器，Docker镜像，Registry，Docker容器。

Docker架构：客户端访问服务端，客户端会经过服务端上的Docker守护进程管理容器。

### 环境配置

由于我的Ubuntu系统是WSL2的，所以我看了两本书上的安装方案，都不行，包括官方的图形化界面安装，因为我这个WSL2压根就没有图形化界面。

但是通过google我发现微软还有对此的一个[官方文档](https://learn.microsoft.com/zh-cn/windows/wsl/tutorials/wsl-containers)，大概内容是给Windows装一个Docker Desktop，然后能打通WSL2，真巧妙。

如果想单独在Ubuntu上安装好像也可以，这是一篇[Docker官方的文档](https://docs.docker.com/engine/install/ubuntu/)，但最终是找不到要安装的包。所以我还是使用了在Windows上的版本，但需要注意不能同时运行WSL2和Windows的容器。

大致浏览了这两本书的内容后，我好像并不是太喜欢以纯命令行的方式进行操作，并且它们的内容稍微有些落后了，我还是选择官方的教程，虽然是全英文但毕竟我有翻译，值得一提的是我又可以在Mac上进行操作了，对于M系列的芯片来说，虽然只有16GB的内存，但它们在调度方面非常厉害，以至于在某些场合能达到Windows下内存32GB的效果。



## 学习收获记录

### 对容器和镜像的理解

我目前还是对image和container无法做到细致的理解，不过image翻译为镜像，类似于一个小型的操作系统，container是容器，类似于一个沙盒，被image所运行，里面装着你的软件，也可以是操作系统，但和其它沙盒隔离开来。

### 基本指令

在第一部分，我学习到了，在软件的某个路径下创建一个Dockerfile文件，写上一堆类似脚本或者CMakeLists.txt之类的东西，然后执行`docker build -t app_name .` 就可以生成容器了。然后运行该程序`docker run -dp 3000:3000 app_name` ，其中-d是指分离线程，-p是指定端口映射，比如示例中的容器的3000映射到本机的3000，这样就可以访问启动的程序了。

示例程序是一个网页版TODO-List，感觉之后也可以把我的洗碗网站放进容器中发布。

不过我现在最好奇的是，我能不能把Mac的16GB内存用满，因为Docker Desktop显示已经使用了3.7GB的内存了，活动监视器中显示虚拟机已经占了快8GB内存。

### 容器的同步更新和数据持久化存储

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



## 重复学习记录

### 学前准备

目前我拥有了一台32GB内存的台式机，我打算在WSL2上进行各种docker部署。

所以我打算重头再看一遍官方入门教程文档，基于之前django的学习经验，这种重复的方式对加深记忆很有帮助。

由于是在Windows平台上阅读纯英文的教程，我发现了一个好用的翻译插件，我觉得除开准确性，易用性可以超过Mac上原生的翻译操作。“沙拉翻译”帮助你只需要选中想要翻译的文本，再点击出现的小图标，就可以进行翻译，不过我目前只能用彩云小译的API。

### 基础知识和命令

再次记录，容器是软件所在的一个沙盒，镜像是运行容器的环境或者称之为操作系统。

要为软件构建一个容器，首先需要进入到软件的app路径下（我的写过项目好像没有这个路径，总之示例项目中需要，并且以下展示的所有命令都需要在该路径下执行，可能是因为存在某些文件），然后创建一个Dockerfile文件，前面讲过，就例如CMake，比如下面这样写

```dockerfile
# syntax=docker/dockerfile:1
   
FROM node:18-alpine
WORKDIR /app
COPY . .
RUN yarn install --production
CMD ["node", "src/index.js"]
EXPOSE 3000
```

构建容器镜像：

```bash
docker build -t PROJECT_NAME .
```

> Docker build 命令使用 Dockerfile 生成新的容器映像。您可能已经注意到 Docker 下载了许多“层”。这是因为您指示构建器要从节点: 18-alpine 映像开始。但是，因为你的机器上没有这个，所以 Docker 需要下载这个映像。
>
> 在 Docker 下载了该映像之后，Dockerfile 的指令复制到您的应用程序中，并使用yarn（js套件管理器）来安装应用程序的依赖项。CMD 指令指定从此映像启动容器时运行的默认命令。
>
> 最后的 -t 标志标记您的图像。可以简单地将它看作是最终图像的一个人类可读的名称。由于命名了映像的开始，所以在运行容器时可以引用该映像。
>
> 那个`.`在 Docker build 命令的末尾告诉 Docker 应该在工作目录中查找 Dockerfile。

启动容器：

```bash
docker run -dp 3000:3000 PROJECT_NAME
```

> 您可以使用-d 标志以“ detached”模式(在后台)运行新容器。还可以使用-p 标志创建主机端口3000到容器端口3000之间的映射。如果没有端口映射，就无法访问应用程序。

查看运行的容器：

```bash
docker ps
```



通过查询到的容器id进行容器的停止和删除：

```bash
docker stop CONTAINER_ID
docker rm CONTAINER_ID
```



### 容器共享

这和GitHub非常类似，先到[Docker Hub Container Image Library | App Containerization](https://hub.docker.com/)注册账号，然后创建自己的仓库，最后上传自己的容器即可。 

在命令行登录自己的docker账号

```bash
docker login -uYOUR_NAEM
```

给自己的容器打上tag

```bash
docker tag PROJECT_NAME YOUR_NAEM/PROJECT_NAME
```

推送容器到远程仓库

```bash
docker push YOUR-USER-NAME/PROJECT_NAME
```

在其它机器上运行容器

```bash
docker run -dp 3000:3000 YOUR_NAEM/YOUR_PROJECT
```

我是推送的镜像上去，却能直接运行的容器，而不用再把容器下载下来，是因为镜像中就直接包含容器吗？看来我还是有点没搞清楚它们之间的关系。

> 当你推送一个镜像到Docker Hub时，该镜像中包含了完整的容器环境和软件。

以后想看远程Linux主机的CPU配置不用再上网查了，因为我记下来了：

```bash
cat /proc/cpuinfo
```



### 容器数据持久化存储

有了之前的经验，我不会再被教程欺骗两次了，首先需要使用volume mount，保证每次添加的用户数据都能被加载。

其次是每次更改代码后，不需要再重新构建容器，而是和运行的容器同步更新，这里需要bind mounts。



教程中对volume的原理貌似没讲太清楚，所以我只要记住创建的volume的名字即可。

1. 创建volume

```bash
docker volume create NAME-db
```



2. 启动容器时指定挂载卷

请注意：target指定的路径时你项目数据库所在位置。（这是我进到了容器的terminal获取的信息）

```bash
docker run -dp 3000:3000 --mount type=volume,src=NAME-db,target=/etc/todos PROJECT_NAME
```



### bind mount & nodemon 同步改动

1. 将源代码装入容器

2. 安装所有依赖项

3. 启动 noemon 来监视文件系统的更改



```bash
docker run -dp 3000:3000 \
    -w /app --mount type=bind,src="$(pwd)",target=/app \
    node:18-alpine \
    sh -c "yarn install && yarn run dev"
```

> -dp 3000:3000 和之前一样。在分离(背景)模式下运行并创建一个端口映射
>
> \-w /app 设置命令运行的“工作目录”或工作目录
>
> \--mount type = bind，src = “ $(pwd)”，target =/app 将工作目录从主机挂载到容器中的/app 目录
>
> node: 18-alpine-要使用的图像。注意，这是 Dockerfile 中我们应用程序的基础图像
>
> sh -c "yarn install && yarn run dev" 我们使用 sh (alpine 没有 bash)启动一个 shell，运行纱线安装来安装包，然后运行纱线运行开发程序来启动开发服务器。如果我们查看 package.json，我们将看到开发脚本启动nodemon。



卷绑定和挂载绑定不能同时进行？

这是可以做到的，只需要同时加上选项即可。但目前来说还用不着这样，挂载绑定更适合软件开发的过程中使用，卷绑定更适合容器发布时使用。



### 多容器

比如多了个MySQL数据库来存储todo app上的数据，这个数据库引擎需要运行在另外的容器上（那本身自带的SQLite在哪？之前的均为单容器，那我想应该就是集成到了一个容器里）。

要确保容器之间通信，需要建立容器之间的网络。

- 创建网络：

```bash
docker network create todo-app
```

这里的todo-app应该是网络的名称。

- 创建一个MySQL容器，并连接到网络：

```bash
docker run -d \
     --network todo-app --network-alias mysql \
     -v todo-mysql-data:/var/lib/mysql \
     -e MYSQL_ROOT_PASSWORD=secret \
     -e MYSQL_DATABASE=todos \
     mysql:8.0
```

> 您会注意到在上面的命令中有一个名为 todo-mysql-data 的卷，它挂载在/var/lib/mysql 上，MySQL 在这里存储它的数据。但是，您从未运行过 docker 卷创建命令。Docker 识别出您希望使用命名卷，并自动为您创建一个。

- 登录MySQL以确认是否创建成功

```bash
docker exec -it <mysql-container-id> mysql -u root -p
```



要连接到同一网络下的另外一个容器，可以通过容器的IP地址。下面会介绍nicolaka/netshot 容器，它附带了许多对于故障排除或调试网络问题非常有用的工具。



- 在与MySQL同一网络下启动nicolaka/netshot 容器

```bash
docker run -it --network todo-app nicolaka/netshoot
```

> 在容器内部，您将使用 dig 命令，这是一个有用的 DNS 工具。查找主机名 mysql 的 IP 地址。

```bash
dig mysql
```

- 运行你的项目并连接到网络和MySQL

```bash
docker run -dp 3000:3000 \
   -w /app -v "$(pwd):/app" \
   --network todo-app \
   -e MYSQL_HOST=mysql \
   -e MYSQL_USER=root \
   -e MYSQL_PASSWORD=secret \
   -e MYSQL_DB=todos \
   node:18-alpine \
   sh -c "yarn install && yarn run dev"
```

- 查看MySQL数据库

```bash
docker exec -it <mysql-container-id> mysql -p todos
```

```sql
select * from todo_items;
```

因为我添加的数据中含有中文，所以存在中文不显示的情况，要解决这种情况可以选择更改数据库的编码为utf8



### Docker Compose 

这是一个定义和分析多容器的工具，它可以在一个文件中定义你的应用栈，这个插件是随docker自动安装好的。

- 通过命令查看版本：

```bash
docker compose version
```

- 创建Compose文件

在项目的app目录下新建一个docker-compose.yml文件，然后就像一个makefile一样，将运行容器的一对命令集中在这里，以`services:`开头。

比如下面这个命令：

```bash
docker run -dp 3000:3000 \
  -w /app -v "$(pwd):/app" \
  --network todo-app \
  -e MYSQL_HOST=mysql \
  -e MYSQL_USER=root \
  -e MYSQL_PASSWORD=secret \
  -e MYSQL_DB=todos \
  node:18-alpine \
  sh -c "yarn install && yarn run dev"
```

放入.yml文件后是：

```yml
services:
  app:
    image: node:18-alpine
    command: sh -c "yarn install && yarn run dev"
    ports:
      - 3000:3000
    working_dir: /app
    volumes:
      - ./:/app
    environment:
      MYSQL_HOST: mysql
      MYSQL_USER: root
      MYSQL_PASSWORD: secret
      MYSQL_DB: todos
```

这个文件在vscode里竟然是一个粉色的鲸鱼，然后还需要注意缩进格式，应该类似于python那样的缩进格式。
