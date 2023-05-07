# 使用git上传代码到GitHub

此文档记录我使用git上传代码到GitHub的流程，以前均使用gitee进行代码托管，但由于gitee文件大小限制问题，所以转向使用GitHub

我已经安装好了git，并且已经科学上网。

------

## 操作流程

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

`git config --global user.name "你的名字"`

`git config --global user.email "你的邮箱"`

然后生成公钥，请注意在生成公钥前务必进行正确的邮箱和姓名配置。

在git bash中执行命令`ssh-keygen -t rsa -C "youremail@example.com"`即可创建SSH公钥，位于用户主目录，比如你的电脑名称叫admin，那么位于/c/Users/admin/.ssh下，名为id_rsa.pub，使用cat命令查看，复制该公钥到github网站中个人设置里的SSH keys中，这使得每次上传代码可以免去密码。

之后我再使用git clone命令将远程仓库拉取到本地，我就可以在本地修改该远程仓库，再进行提交。

唯一蛋疼的是，我创建的文件夹里又多了一个文件夹，我只好把我的文件夹改一下名，以免命名冗余。

## 冲突记录

*此条目用于记录本人实时遇到的冲突情况即解决办法。*

昨天晚上，我在macOS上对swiftUI文档进行了新增，但是由于太晚了，我忘了提交。今天早上我没有立即对其进行提交，而是在Windows上对Hadoop文档进行了新增，并完成了提交。今天下午我使用macOS时，没有立即把最新仓库拉取下来，而是对昨晚的修改进行提交，在执行`git push`的时候出现了错误：

```sh
git push
To github.com:SucRunBug/win_mac_linux.git
 ! [rejected]        main -> main (fetch first)
error: failed to push some refs to 'github.com:SucRunBug/win_mac_linux.git'
hint: Updates were rejected because the remote contains work that you do
hint: not have locally. This is usually caused by another repository pushing
hint: to the same ref. You may want to first integrate the remote changes
hint: (e.g., 'git pull ...') before pushing again.
hint: See the 'Note about fast-forwards' in 'git push --help' for details.
```

这时候我才想起，本地仓库还不是最新的，于是执行`git pull`，然后再次出现错误：

```sh
git pull
remote: Enumerating objects: 5, done.
remote: Counting objects: 100% (5/5), done.
remote: Compressing objects: 100% (2/2), done.
remote: Total 4 (delta 1), reused 4 (delta 1), pack-reused 0
Unpacking objects: 100% (4/4), 1.44 KiB | 294.00 KiB/s, done.
From github.com:SucRunBug/win_mac_linux
   d3b8aa9..32d8269  main       -> origin/main
hint: You have divergent branches and need to specify how to reconcile them.
hint: You can do so by running one of the following commands sometime before
hint: your next pull:
hint: 
hint:   git config pull.rebase false  # merge
hint:   git config pull.rebase true   # rebase
hint:   git config pull.ff only       # fast-forward only
hint: 
hint: You can replace "git config" with "git config --global" to set a default
hint: preference for all repositories. You can also pass --rebase, --no-rebase,
hint: or --ff-only on the command line to override the configured default per
hint: invocation.
fatal: Need to specify how to reconcile divergent branches.
```

通过询问ChatGPT了解解决方案，需要先将远程代码合并到本地，再进行上传。

合并代码执行`git pull --rebase origin main`

这个命令将使用rebase方式合并远程仓库的更改到本地仓库，这可以使你的提交历史更加线性化。

如果你希望使用merge方式合并更改，可以使用以下命令：

```
git pull --no-rebase origin main
```

执行完`git pull`命令后，你需要处理合并冲突，编辑文件以解决冲突，然后将更改提交到本地仓库。最后，你可以使用以下命令将更改推送到远程仓库：

```
git push origin main
```
