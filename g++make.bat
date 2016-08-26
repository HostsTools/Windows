windres tool.rc -o tmp.o
g++ -o tool.exe tool.cpp tmp.o -s -lwininet -O2 -DHOSTS_HARD_RESET
del /q tmp.o
pause