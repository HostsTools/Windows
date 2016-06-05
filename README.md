# Download Executable File 

[![Build status](https://ci.appveyor.com/api/projects/status/8aya86796ipmuwr2/branch/master?svg=true)](https://ci.appveyor.com/project/Too-Naive/windows/branch/master)
Last update: May 31th , 2016

#### 下载地址:(v2.1.5p)
 - [点我来下载](https://git.io/vwsqL)
 - [如果上面那个不行的话点我](https://yunpan.cn/cPseSVaAVwMBL)(code:5c40)

OS type | Minimun Supported Version
--------|-------------------
WorkStation | Microsoft Windows XP Family
Server | Microsoft Windows Server 2003 Family

>If you choose `Download`, It means you agreed the MIT license(redefined).

# 9天后才会继续推出新的更新
目前`build passing` , 如有其他疑问请在issue中提出问题    
最新的版本更新是 现在程序不会忽略`#`开头的注释语句

# Hosts Tool

这个工具可以帮助你全自动的更换 备份原来的hosts文件 
所有麻烦的事情只需要打开一个程序就能搞定 
如果你愿意，程序还可以作为服务安装随系统启动 
每次开机后每隔30分钟会自动检测hosts文件的更新噢！

**在此特别感谢[@qwerty258](https://github.com/qwerty258)为本程序提供了Visual Studio的工程文件**

##New Feature

现在，可以在hosts的头部加上自定义的hosts，程序自动更新时不会覆盖掉自定义的hosts.例如

	127.0.0.1 baidu.com
	127.0.0.1 www.baidu.com
	# Copyright (c) 2014-2016, racaljk.
	# https://github.com/racaljk/hosts

程序不会将第一行过滤掉，而会从`# Copyright (c) 2014-2016, racaljk.`开始改写hosts文件。

## 警告

1. **请不要删除`# Copyright (c) 2014-2016, racaljk.`(作为程序的识别位点) 否则 有可能发生不可预料的后果**
2. **如果先前没有使用本项目hosts文件 而使用了其他项目的hosts文件的 请重置hosts文件(具体方法看下方使用说明)后 再使用本程序**
3. ~~**(在自定义的hosts中)程序会抛掉所有的注释`#`开头的语句**~~(功能已经弃用 将会在2.1.5的正式版中启用新的选项)

## How to use?

修改hosts涉及到系统文件的修改，安装服务也有可能触发安全软件的提示，如有安全软件提示请放行通过.

main program file: `tool.exe` 

 - 无参数运行`tool.exe` 用来更新hosts文件 如有更新 程序会备份原有的hosts文件
 - 带参数 `-fi` 运行 `tool.exe` 安装一个名为`racaljk-hoststool`的服务
 - 带参数 `-fu` 运行 `tool.exe` 卸载已经安装的`racaljk-hoststool`服务
 - 带参数 `-r` 运行 `tool.exe` 来重置hosts文件
 - 带参数 `-?` 运行 `tool.exe` 来获得更多的使用方法

## 注意事项

1. 如果安装服务 程序会往`%SystemRoot%`下复制一个`tool.exe`文件用来作为服务启动的主程序
2. 安装服务后 日志文件会保存在`C:\Hosts_Tool_log.log`下 您可以通过查看日志观察服务的工作状态
3. 卸载服务请使用原来的`tool.exe`文件 请不要在命令行中直接执行`hoststools -fu`(如执行 需要手动删除`%SystemRoot%`目录下的`hoststools.exe`)
7. 本程序一切有更改hosts文件的行为前都会先备份hosts文件。
4. 请间隔一段时间后清理`%SystemRoot%\system32\drivers\etc\`文件夹 (因为可能堆满了备份的文件)
5. 如有任何疑问或bug反馈，请开新的issue或者发邮件到 sometimes.naive[at]hotmail.com (如是服务问题请附上日志文件)

## for Developer

### Code license:

General public license version 3

### Service Debug Mode :

If you want to enter debug mode, follow the steps blow.

1. Open the console.
2. Change the working directory where is the `tool.exe` position.
3. Input `tool --debug-pipe` and press enter.
4. Now you can see the current working status of service.
5. Press **Ctrl+C** to Exit debug mode.
6. Exit debug mode will cause service uninstall.

**WARNING: IN DEBUG MODE, DO NOT CLOSE THE CONSOLE DIRECT**

*Notice:In debug mode, service run cycle is very sort*

Fixed: if exit program unexpectally, service will run with system start.(and then sent unexpect data)

### How to compile?

Please see the [documents.md](https://github.com/HostsTools/Windows/blob/master/documents.md)
