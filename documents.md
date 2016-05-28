# Code license:

GPL v3

# How to Compile?

**Save `tool.exe.manifest` to directory first**

## for Mingw g++

Compile commandline:

```
windres tool.rc -o toolr.o
g++ -o tool.exe tool.cpp toolr.o -lwininet -O2 -s
```

File `tool.exe.manifest`:

```xml
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
    <assembly xmlns="urn:schemas-microsoft-com:asm.v1" manifestVersion="1.0">
    <trustInfo xmlns="urn:schemas-microsoft-com:asm.v3">
        <security>
            <requestedPrivileges>
                <requestedExecutionLevel level="requireAdministrator" uiAccess="false"/>
            </requestedPrivileges>
        </security>
    </trustInfo>
</assembly>
```

## Files description

File Name | Description
----------|-------------
tool.cpp | Main program file
default.hpp | Default hosts file
diff.hpp | Support file check different
download.hpp | Download support
make.cmd | Compile script
pipedebug.hpp | Debug tool for service mode
ptrerr.hpp | Log file support
tool.exe.manifest | Executable manifest file
tool.rc | program resource script file
wininet.lib | g++ compile library

## Functions description

Function name | Description
--------------|-------------
`__Check_Parameters` | Function check parameters
`Service_Main` | Service main function
`Service_Control` | Service control function
`Func_Service_Install` | Install service function
`Func_Service_UnInstall` | Uninstall service function
` NormalEntry` | Replace/download hosts function
`___debug_point_reset` | Debug function(reserved)
`__show_str` | Show normal information
`Func_ResetFile` | Reset hosts file function

## Program runtime process(Run without parameters)

1. Check parameters
2. Get system path
3. Download hosts file
4. Check diff between downloaded file and system hosts file
5. If different copy a backup file and replace it
6. Delete temp file


## Hosts Backup file(Windows 7+)

```
# Copyright (c) 1993-2009 Microsoft Corp.
#
# This is a sample HOSTS file used by Microsoft TCP/IP for Windows.
#
# This file contains the mappings of IP addresses to host names. Each
# entry should be kept on an individual line. The IP address should
# be placed in the first column followed by the corresponding host name.
# The IP address and the host name should be separated by at least one
# space.
#
# Additionally, comments (such as these) may be inserted on individual
# lines or following the machine name denoted by a '#' symbol.
#
# For example:
#
#      102.54.94.97     rhino.acme.com          # source server
#       38.25.63.10     x.acme.com              # x client host

# localhost name resolution is handled within DNS itself.
#	127.0.0.1       localhost
#	::1             localhost
```
