@set TARGETNAME=tool
@set LDCOMMAND=-lwininet
windres %TARGETNAME%.rc -o tmp.o
g++ -o %TARGETNAME%.exe %TARGETNAME%.cpp tmp.o -s %LDCOMMAND% -O2
del /q tmp.o
@cd object-runwithcode
@set TARGETNAME=choose
@set LDCOMMAND=
windres %TARGETNAME%.rc -o tmp.o
g++ -o %TARGETNAME%.exe %TARGETNAME%.cpp tmp.o -s %LDCOMMAND% -O2
del /q tmp.o
@cd ..
pause