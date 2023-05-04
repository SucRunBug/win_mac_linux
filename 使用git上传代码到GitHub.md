# 使用git上传代码到GitHub

此文档记录我使用git上传代码到GitHub的流程，以前均使用gitee进行代码托管，但由于gitee文件大小限制问题，所以转向使用GitHub

我已经安装好了git，并且已经科学上网。

------

我使用给git的目的是让我的三台电脑，包含2个Windows系统，1个Mac系统和2个Linux系统，进行代码同步。

首先，我在桌面创建了一个文件夹，名为`win_mac_linux`

在Windows系统下使用git bash进入上述新文件夹，执行命令 `git init`

此时，本地已经初始化好了名为`win_mac_linux`的git仓库，接下来需要到github中创建一个远程仓库来与本地仓库进行同步。

进入GitHub后，我的账号使用的不是QQ邮箱，而是163邮箱，然后修改了我的头像（原来是默认的，很丑），换成了我使用stable diffusion生成的AI头像，不过非常遗憾的只支持不超过1mb大小的图片。

之后我删除了原本不用的仓库，需要进入仓库，在设置选项的最下面进行删除，删除时还需要你输入相关的路径。

然后就创建了一个新的仓库，也是叫`win_mac_linux`

接下来做的应该是将远程仓库和本地仓库关联起来，并进行提交测试即可。

在git bash中执行命令`git config --global --list`

可以查看配置文件，需要修改名字为GitHub同样的昵称，邮箱也是GitHub的邮箱。

然后生成公钥，请注意在生成公钥前务必进行正确的邮箱和姓名配置。

在git bash中执行命令`ssh-keygen -t rsa -C "youremail@example.com"`即可创建SSH公钥，位于用户主目录，比如你的电脑名称叫admin，那么位于/c/Users/admin/.ssh下，名为id_rsa.pub，使用cat命令查看，复制该公钥到github网站中个人设置里的SSH keys中，这使得每次上传代码可以免去密码。

之后我再使用git clone命令将远程仓库拉取到本地，我就可以在本地修改该远程仓库，再进行提交。

唯一蛋疼的是，我创建的文件夹里又多了一个文件夹，我只好把我的文件夹改一下名，以免命名冗余。