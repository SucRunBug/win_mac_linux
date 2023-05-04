Linux虚拟机配置全流程记录



- win使用finalshell连接centos

在centos终端中输入命令ifconfig，找到ens33后的inet后的点分十进制ip地址

finalshell下载地址https://www.hostbuf.com/t/988.html

下载好后进入界面，点击顶部「蓝色边框文件夹」图标，再点击正下方「白色方块，右上角有绿色加号」图标，再选择ssh连接，然后输入查到的ip即相关信息即可连接。

<img src="/Users/yfy/Library/Application Support/typora-user-images/image-20230128150003800.png" alt="image-20230128150003800" style="zoom:50%;" />

macOS也是类似的操作。



- WSL配置安装Ubuntu

![image-20230128152959427](/Users/yfy/Library/Application Support/typora-user-images/image-20230128152959427.png)

进入「控制面板」，点击「程序」，在「程序与功能」下方找到「启用或关闭Windows功能」，在其中勾选「适用于Linux的Windows子系统」，系统会引导你重启，然后再次检查此项是否已经勾选。



接下来打开系统自带的「Microsoft store」，搜索「Ubuntu」，下载好后会弹出黑框，用命令行进行初始化。

如果出现错误代码0x800701bc

可参考https://learn.microsoft.com/zh-cn/windows/wsl/install-manual#step-4---download-the-linux-kernel-update-package

接下来下载「Windows terminal」软件，也是在微软商店中搜索。

![image-20230131104548591](/Users/yfy/Library/Application Support/typora-user-images/image-20230131104548591.png)

打开后，上边栏有个向下的小箭头，点击后选择ubuntu，为了保证每次打开都是Ubuntu，可以在箭头里选择「设置」

![image-20230131104905289](/Users/yfy/Library/Application Support/typora-user-images/image-20230131104905289.png)

「启动」栏中有「默认配置文件」，选择为「Ubuntu」。



- 创建快照

VMware上边栏处找到，拍摄当前虚拟机快照。p

![image-20230131181222014](/Users/yfy/Library/Application Support/typora-user-images/image-20230131181222014.png)



![image-20230131181136537](/Users/yfy/Library/Application Support/typora-user-images/image-20230131181136537.png)
