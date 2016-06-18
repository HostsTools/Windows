windres choose.rc -o tmp.o
g++ -o choose.exe choose.cpp tmp.o -s -O2
del /q tmp.o
pause