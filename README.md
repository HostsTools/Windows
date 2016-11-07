使用本项目前 请仔细阅读本README文件

# Download Executable File 

[![Build status](https://ci.appveyor.com/api/projects/status/8aya86796ipmuwr2/branch/master?svg=true)](https://ci.appveyor.com/project/Too-Naive/windows/branch/master)
Last update: Oct. 28th, 2016

#### 下载地址:(v2.1.18)

 - zip Package (Include `tool.exe` and `choose.exe`)
  - [点我来下载](https://coding.net/u/scaffrey/p/hosts/git/raw/master/tools/windows.zip)

如只需下载tool.exe请到[release](https://github.com/HostsTools/Windows/releases)页面手动下载

OS type | Minimun Supported Version
--------|-------------------
WorkStation | Microsoft Windows XP Family
Server | Microsoft Windows Server 2003 Family

# Hosts Tool

这个工具可以帮助你全自动的更换 备份原来的hosts文件 
所有麻烦的事情只需要打开一个程序就能搞定 
如果你愿意，程序还可以作为服务安装随系统启动 
每次开机后每隔30分钟会自动检测hosts文件的更新噢！

**在此特别感谢[@qwerty258](https://github.com/qwerty258)为本程序提供了Visual Studio的工程文件**

**在此特别感谢spacetime的协助 是他帮助我发现了ci运行测试中程序的致命bug**

**在此特别感谢[@0xMeansHexadecimal](https://github.com/0xMeansHexadecimal)给我的支持 是她给了我灵感 协助我修复了[#22](https://github.com/HostsTools/Windows/issues/22)**

## 声明

~~作者弃坑了~~

请通过issue来反馈任何bug以及程序问题，bug会及时得到修复。

##New Feature

如果服务无法更新hosts 请尝试安装静默启动

打开choose.exe后输入11按下回车 重启即可

## 警告

1. **请不要删除`# Copyright (c) 2014-2016, racaljk.`(作为程序的识别位点) 否则 有可能发生不可预料的后果**
2. **如果先前没有使用本项目hosts文件 而使用了其他项目的hosts文件的 请重置hosts文件(具体方法看下方使用说明)后 再使用本程序**

## How to use?

修改hosts涉及到系统文件的修改，安装服务也有可能触发安全软件的提示，如有安全软件提示请放行通过.

main program file: `tool.exe` 

 - 无参数运行`tool.exe` 用来更新hosts文件 如有更新 程序会备份原有的hosts文件
 - 运行`choose.exe` 来选择更多的运行选项

## 注意事项

1. 如果安装服务 程序会往`%SystemRoot%`下复制一个`hoststools.exe`文件用来作为服务启动的主程序
2. 安装服务后 日志文件会保存在`C:\Hosts_Tool_log.log`下 您可以通过查看日志观察服务的工作状态
7. 本程序一切有更改hosts文件的行为前都会先备份hosts文件。
4. 请间隔一段时间后清理`%SystemRoot%\system32\drivers\etc\`文件夹 (程序加入了询问删除早期备份文件的功能)
5. 如有任何疑问或bug反馈，请开新的issue (如是服务问题请附上日志文件)


## Code license:

General Public License Version 3
