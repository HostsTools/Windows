/*
 * This source code was published under GPL v3
 *
 * Copyright (C) 2016 Too-Naive E-mail:sometimes.naive@hotmail.com
 *
 */

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "hander/download.hpp"
#include <tchar.h>
#include <stdlib.h>
#include "hander/ptrerr.hpp"
#include "hander/diff.hpp"
#include <signal.h>
#include "hander/pipedebug.hpp"
#include "hander/default.hpp"

#define WIN32_LEAN_AND_MEAN

//For microsoft visual studio debug(online)

#ifdef _MSC_VER
#pragma warning (disable:4996 4390)
#endif

#ifdef _TESTONLINE
#undef MessageBox
#define MessageBox(Reserved1,x,reserved2,reserved3) \
	_tprintf(_T("%s\n"),x),exit(0)
#define callsystempause ((void*)0)
#else
#define callsystempause system("pause")
#endif

//end.

#define DEFBUF(x,y) x[y]=_T("")
#define THROWERR(x) throw expection(x)

//Hosts file web address set
#define hostsfile _T("https://raw.githubusercontent.com/racaljk/hosts/master/hosts")
#define hostsfile1 _T("https://coding.net/u/scaffrey/p/hosts/git/raw/master/hosts")

#ifdef _LOCAL
	#undef hostsfile
	#undef hostsfile1
//	#ifdef _LOCAL
		#define hostsfile _T("http://localhost/hosts")
		#define hostsfile1 hostsfile
/*	#else
		#define hostsfile _T("https://raw.githubusercontent.com/YoungIsSimple/Object-Release/testbranch/minHOSTS")
		#define hostsfile1 hostsfile
	#endif*/
#endif
#define objectwebsite _T("https:\x2f\x2fgithub.com/HostsTools/Windows")
//end.

#define ConsoleTitle _T("racaljk-host tool    v2.1.10  Build time:Jul. 14th, '16")

#define CASE(x,y) case x : y; break;
#define pWait _T("\n    \
There seems something wrong in download file, we will retry after 5 seconds.\n")

//tmpfile set
#define DownLocated _T("hosts.tmp")
#define ChangeCTLR _T("hostsq.tmp")
#define ReservedFile _T("hosts_n.tmp")
//end.

#define BAD_EXIT \
		_tprintf(_T("Bad Parameters.\nUsing \"-?\" Parameter to show how to use.\n")),\
		abort();
#define _BAKFORMAT _T("%s\\drivers\\etc\\hosts.%04d%02d%02d.%02d%02d%02d.bak")

//debug & log file set
#define LogFileLocate _T("c:\\Hosts_Tool_log.log")
const TCHAR * pipeName=_T("\\\\.\\pipe\\hoststoolnamepipe");
#define szErrMeg _T("\nFatal Error:\n%s (GetLastError():%ld)\n\
Please contact the application's support team for more information.\n")
using namespace __Dpipe;
//end.

//service name
//for backward compatibility DO NOT CHANGE IT
TCHAR Sname[]=_T("racaljk-hoststool");
TCHAR const *SzName[]={
	Sname
};
const TCHAR * szServiceShowName=_T("racaljk-hosts Tool");
//end

const size_t localbufsize=1024;

//private file const set
const TCHAR * privateFileName=_T("hstool.ini");
const TCHAR * privateAppName=_T("toolIgnore");
const TCHAR * privateCommitName=_T("Commit");
const TCHAR * privateNewLineName=_T("NewLine");
//end

struct expection{
	const TCHAR *Message;
	expection(const TCHAR * _1){
		this->Message=_1;
	}
};

#define SHOW_HELP _T("\
------------------------------------------------------------\n\
Hosts Tool for Windows Console by: Too-Naive\n\
Copyright (C) 2016 @Too-Naive License:General Public License\n\
------------------------------------------------------------\n\n\
Usage: tool [-? | -r | -fi | -fu | --debug-pipe]\n\n\
Options:\n\
    -?    : Show this help message.\n\
    -r    : Reset system hosts file to default.\n\
    -fi   : Install Auto-Update hosts service(Service Name:%s).\n\
    -fu   : Uninstall service.\n\
    --debug-pipe : Debug Mode(reserved for deverloper)\n\
Example:\n\
    tool -fi\n\n\
    If you need more imformation about debug mode,\n\
    Please see the page in: https:\x2f\x2fgit.io/vwjyB\n\n")

#define welcomeShow _T("\
    **********************************************\n\
    *                                            *\n\
    *                                            *\n\
    *                                            *\n\
    *     Welcome to use racaljk/hosts tool!     *\n\
    *                                            *\n\
    *                                            *\n\
    *                    Powered by: @Too-Naive  *\n\
    **********************************************")

//Global variable
SERVICE_STATUS_HANDLE ssh;
SERVICE_STATUS ss;
HANDLE lphdThread[]={
	INVALID_HANDLE_VALUE
};
bool request_client,bReserved,bIgnoreNewline,bIgnoreCommit;
WIN32_FIND_DATA wfd={0,{0,0},{0,0},{0,0},0,0,0,0,{0},{0}};
//end.

//function declaration
int __fastcall __Check_Parameters(int,TCHAR const**);
void WINAPI Service_Main(DWORD, LPTSTR *);
void WINAPI Service_Control(DWORD);
void Func_Service_Install(bool);
void Func_Service_UnInstall(bool);
DWORD __stdcall NormalEntry(LPVOID);
void ___debug_point_reset(int);
inline void __show_str(TCHAR const *,TCHAR const *);
void Func_ResetFile();
void ___Func_pipeCallBack(TCHAR const*);
void Func_CallCopyHostsFile();
void Func_CheckProFile();
TCHAR * dotdotcheck(TCHAR *);
void Func_countBackupFile(SYSTEMTIME *);
bool Func_checkBackupFileTime(const SYSTEMTIME & , TCHAR const *);

//DWORD __stdcall Func_Update(LPVOID);


SERVICE_TABLE_ENTRY STE[2]={{Sname,Service_Main},{_ptrresev_NULL_,_ptrresev_NULL_}};

//define buffer
TCHAR DEFBUF(buf1,localbufsize),DEFBUF(buf2,localbufsize),
	DEFBUF(buf3,localbufsize),DEFBUF(szline,localbufsize);
char iobuffer[localbufsize];
//end.

//define parameters
#define	EXEC_START_NORMAL		(1<<0x00)
//#define 	EXEC_START_RUNAS		(1<<0x01)
#define	EXEC_START_SERVICE		(1<<0x02)
#define	EXEC_START_INSTALL_SERVICE	(1<<0x03)
#define	EXEC_START_UNINSTALL_SERVICE	(1<<0x04)
#define	EXEC_START_HELP			(1<<0x05)
//#define	SHOW_LICENSE			(1<<0x06)
#define	RESET_FILE			(1<<0x07)
#define	EXEC_BAD_PARAMETERS		(1073741824)


#define DEBUG_ENTRY 			(1<<0x00)
#define	EXEC_DEBUG_RESET		(DEBUG_ENTRY|(1<<0x01))
#define	DEBUG_SERVICE_STOP		(DEBUG_ENTRY|(1<<0x02))
#define	DEBUG_SERVICE_START		(DEBUG_ENTRY|(1<<0x03))
#define	DEBUG_SERVICE_REINSTALL		(DEBUG_ENTRY|(1<<0x04))
#define	OPEN_LISTEN			(DEBUG_ENTRY|(1<<0x05))
//end.

//define _In_ parameters string
TCHAR const *szParameters[]={
	_T("svc"),				//for backward compatibility
	_T("fi"),				//1
	_T("fu"),				//2
	_T("\x02\x03"),				//3
	_T("-debug-reset"),			//4
	_T("\x02\x03"),				//5
	_T("?"),				//6
	_T("-debug-stop"),			//7
	_T("-debug-start"),			//8
	_T("-debug-reiu"),			//9
	_T("-debug-pipe"),			//10
	_T("--pipe"),				//11
	_T("r")					//12
};

//Check parameters function
int __fastcall __Check_Parameters(int argc,TCHAR const **argv){
	if (argc==1) return EXEC_START_NORMAL;
	if (argc>3||!(( argv[1][0]==_T('/') ||
		argv[1][0]==_T('-')) && argv[1][1]!=_T('\0'))) BAD_EXIT;
	size_t i=0;
	for (;_tcscmp(&(argv[1][1]),szParameters[i]) &&
		i<sizeof(szParameters)/sizeof(szParameters[0]);i++);
	if (!(i==0 || i==1 ||i==9)
		&& argc>2) BAD_EXIT;
	if (argc==3 && !_tcscmp(argv[2],szParameters[11])) request_client=1;
		else if (argc==3 && _tcscmp(argv[2],szParameters[11])) BAD_EXIT;
	switch (i){
		case  0: bReserved=true;
			 return EXEC_START_SERVICE;
		case  1: return EXEC_START_INSTALL_SERVICE;
		case  2: return EXEC_START_UNINSTALL_SERVICE;
		case  4: return EXEC_DEBUG_RESET;	//restart service
		case  6: return EXEC_START_HELP;
		case  7: return DEBUG_SERVICE_STOP;	//stop service
		case  8: return DEBUG_SERVICE_START;	//start service
		case  9: return DEBUG_SERVICE_REINSTALL;//reinstall service
		case 10: return OPEN_LISTEN;
		case 12: return RESET_FILE;
		default: BAD_EXIT;
	}
	BAD_EXIT;
}
//end.

//main entry
int _tmain(int argc,TCHAR const ** argv){
	SetConsoleTitle(ConsoleTitle);
	switch (__Check_Parameters(argc,argv)){
		CASE(EXEC_START_NORMAL,NormalEntry(_ptrresev_NULL_));
		CASE(EXEC_START_INSTALL_SERVICE,Func_Service_Install(true));
		CASE(EXEC_START_UNINSTALL_SERVICE,Func_Service_UnInstall(true));
		CASE(EXEC_START_SERVICE,StartServiceCtrlDispatcher(STE));
		CASE(EXEC_START_HELP,__show_str(SHOW_HELP,Sname));
		CASE(EXEC_DEBUG_RESET,___debug_point_reset(EXEC_DEBUG_RESET));
//		CASE(SHOW_LICENSE,__show_str(szgpl_Raw,_ptrresev_NULL_));
		CASE(DEBUG_SERVICE_STOP,___debug_point_reset(DEBUG_SERVICE_STOP));
		CASE(DEBUG_SERVICE_START,___debug_point_reset(DEBUG_SERVICE_START));
		CASE(DEBUG_SERVICE_REINSTALL,___debug_point_reset(DEBUG_SERVICE_REINSTALL));
		CASE(OPEN_LISTEN,___debug_point_reset(OPEN_LISTEN));
		CASE(RESET_FILE,Func_ResetFile());
		default:break;
	}
	return 0;
}

void Func_CheckProFile(){
	SetLastError(ERROR_SUCCESS);
	GetPrivateProfileString(privateAppName,
							privateCommitName,
							_T("false"),
							szline,
							localbufsize,
							privateFileName);
//	printf("%ld",GetLastError());
	if (GetLastError()==ERROR_FILE_NOT_FOUND){
		WritePrivateProfileString(privateAppName,
								  privateCommitName,
								  _T("false"),
								  privateFileName);
		WritePrivateProfileString(privateAppName,
								  privateNewLineName,
								  _T("false"),
								  privateFileName);
	}
	if (!_tcscmp(szline,_T("true"))) bIgnoreCommit=true;
	GetPrivateProfileString(privateAppName,
							privateNewLineName,
							_T("false"),
							szline,
							localbufsize,
							privateFileName);
	if (!_tcscmp(szline,_T("true"))) bIgnoreNewline=true;
	return ;
}


void Func_ResetFile(){
	SYSTEMTIME st={0,0,0,0,0,0,0,0};
	_tprintf(_T("\
------------------------------------------------------------\n\
Hosts Tool for Windows Console by: Too-Naive\n\
Copyright (C) 2016 @Too-Naive License:General Public License\n\
------------------------------------------------------------\n"));
	if (!GetSystemDirectory(buf3,localbufsize))
		_tprintf(_T("    GetSystemDirectory() Error!(GetLastError():%ld)\n\
\tCannot get system path!"),GetLastError()),abort();
	GetLocalTime(&st);
	_stprintf(buf1,_T("%s\\drivers\\etc\\hosts"),buf3);
	_stprintf(buf2,_BAKFORMAT,buf3,st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
	if (CopyFile(buf1,buf2,FALSE))
		_tprintf(_T("Backup File success\nFilename:%s\n\n"),buf2);
	FILE *fp=_tfopen(buf1,_T("w"));
	if (!fp) _tprintf(_T("Cannot open file!\n")),abort();
	_ftprintf(fp,_T("%s"),szDefatult_hostsfile);
	fclose(fp);
	_tprintf(_T("    Reset file successfully.\n"));
	callsystempause;
	return ;
}

void __abrt(int){
	SetConsoleTitle(TEXT("Recieved signal SIGINT"));
	_tprintf(_T("Received signal SIGINT\nPlease waiting program exit!\n"));
	request_client=0;
	_tprintf(_T("Uninstall service.\n"));
	Func_Service_UnInstall(false);
	TerminateThread(lphdThread[0],0);
	CloseHandle(hdPipe);
	_tprintf(_T("Program will now exit.\n"));
	exit(0);
}

inline void __show_str(TCHAR const* st,TCHAR const * _ingore){
	if (!_ingore) _tprintf(_T("%s"),st);
	else _tprintf(st,_ingore);
	callsystempause;
	return ;
}

void ___debug_point_reset(int _par){
	SC_HANDLE shMang=_ptrresev_NULL_,shSvc=_ptrresev_NULL_;
	if (_par==DEBUG_SERVICE_REINSTALL){
		Func_Service_UnInstall(false);
		Func_Service_Install(false);
		return ;
	}
	try {
		if (_par!=OPEN_LISTEN) {
			if (!(shMang=OpenSCManager(_ptrresev_NULL_,_ptrresev_NULL_,SC_MANAGER_ALL_ACCESS)))
				THROWERR(_T("OpenSCManager() Error in debug_reset."));
			if (!(shSvc=OpenService(shMang,Sname,SERVICE_STOP|SERVICE_START)))
				THROWERR(_T("OpenService() Error in debug_reset."));
		}
		switch(_par){
			case DEBUG_SERVICE_STOP:
			case EXEC_DEBUG_RESET:
				if (!ControlService(shSvc,SERVICE_CONTROL_STOP,&ss))
					THROWERR(_T("ControlService() Error in debug_reset."));
				Sleep(1000);
				if (_par==DEBUG_SERVICE_STOP) break;
			case OPEN_LISTEN:
				if (_par!=EXEC_DEBUG_RESET){
					signal(SIGINT,__abrt);
					_tprintf(_T("    Warning:\n    \
You are in debug mode, please press CTRL+C to exit the debug mode.\n\t\t\
DO NOT CLOSE THE CONSOLE DIRECT!!!\n"));
					request_client=1;
					_tprintf(_T("Reinstall service\n"));
					___debug_point_reset(DEBUG_SERVICE_REINSTALL);
					___debug_point_reset(DEBUG_SERVICE_START);
					if (!(lphdThread[0]=CreateThread(_ptrresev_NULL_,0,OpenPipeService,_ptrresev_NULL_,0,_ptrresev_NULL_)))
						THROWERR(_T("CreateThread() Error!"));
					WaitForSingleObject(lphdThread[0],INFINITE);
					CloseHandle(hdPipe);
					return ;
				}
			case DEBUG_SERVICE_START:
				if (!StartService(shSvc,1,SzName))
					THROWERR(_T("StartService() Error in debug_reset."));
			default : break;
		}
	}
	catch (expection _r){
		_tprintf(_T("\nFatal Error:\n%s (GetLastError():%ld)\n\
Please contact the application's support team for more information.\n"),
		_r.Message,GetLastError());
		CloseServiceHandle(shSvc);
		CloseServiceHandle(shMang);
		abort();
	}
	CloseServiceHandle(shSvc);
	CloseServiceHandle(shMang);
	return ;
}

//short path if str has ".."
TCHAR * dotdotcheck(TCHAR * str){
	TCHAR * _;	TCHAR *_tmp=new TCHAR[100];
	memset(_tmp,0,sizeof(_tmp));
	if ((_=_tcsstr(str,_T("..")))){
		_stscanf(_+2,_T("%100s"),_tmp);
		while ((*(--_))!='\\');
		while ((*(--_))!='\\');
		_stprintf(_,_T("%s"),_tmp);
	}
	delete [] _tmp;
	return str;
}

void Func_Service_UnInstall(bool _quite){
	SC_HANDLE shMang=_ptrresev_NULL_,shSvc=_ptrresev_NULL_;
	Sleep(1000);
	try{
		if (!GetSystemDirectory(buf3,localbufsize))
			THROWERR(_T("GetSystemDirectory() Error in UnInstall Service."));
		_stprintf(buf1,_T("%s\\..\\hoststools.exe"),buf3);
		if (!GetModuleFileName(_ptrresev_NULL_,buf2,localbufsize))
			THROWERR(_T("GetModuleFileName() Error in UnInstall Service."));
		if (!_tcscmp(dotdotcheck(buf1),buf2)){
			if (!GetEnvironmentVariable(_T("TEMP"),buf3,localbufsize))
				THROWERR(_T("GetEnvironmentVariable() Error in UnInstall Service."));
			_stprintf(buf1,_T("%s\\Au__.exe"),buf3);
			if (!CopyFile(buf2,buf1,FALSE))
				THROWERR(_T("CopyFile() Error(To temp directory) in UnInstall Service."));
			if (!ShellExecute(NULL,_T("open"),buf1,_T("-fu"),NULL,SW_SHOWNORMAL))
				THROWERR(_T("ShellExecute() Error in UnInstall Service."));
			exit(0);
		}
		if (!(shMang=OpenSCManager(_ptrresev_NULL_,_ptrresev_NULL_,SC_MANAGER_ALL_ACCESS)))
			THROWERR(_T("OpenSCManager() Error in Uninstall service."));
		if (!(shSvc=OpenService(shMang,Sname,SERVICE_ALL_ACCESS)))
			if (_quite) THROWERR(_T("OpenService() Error in Uninstall service.\nIs service exist?"));
		if (!ControlService(shSvc,SERVICE_CONTROL_STOP,&ss))
			if (_quite) _tprintf(_T("ControlService() Error in Uninstall service.\n%s"),
				_T("Service may not in running.\n"));
		Sleep(2000);//Wait for service stop
		if (!DeleteService(shSvc))
			if (_quite) THROWERR(_T("DeleteService() Error in UnInstall service."));
		if (!DeleteFile(buf1))
			if (_quite) {
				_tprintf(_T("Executable File located:%s\n"),buf1);
				THROWERR(_T("DeleteFile() Error in Uninstall service.\n\
You may should delete it manually."));
		}
	}
	catch (expection _r){
		_tprintf(_T("\nFatal Error:\n%s (GetLastError():%ld)\n\
Please contact the application's support team for more information.\n"),
		_r.Message,GetLastError());
		_tprintf(_T("\n[Debug Message]\n%s\n%s\n"),buf1,buf2);
		CloseServiceHandle(shSvc);
		CloseServiceHandle(shMang);
		if (_quite) abort();
	}
	CloseServiceHandle(shSvc);
	CloseServiceHandle(shMang);
	_tprintf(_T("Service Uninstall Successfully\n"));
	return ;
}

void Func_Service_Install(bool _q){
	SC_HANDLE shMang=_ptrresev_NULL_,shSvc=_ptrresev_NULL_;
	if (_q){
		_tprintf(_T("    LICENSE:General Public License\n    \
Copyright (C) 2016 @Too-Naive\n\n"));
		_tprintf(_T("    Bug report:sometimes.naive[at]hotmail.com \n\t       \
Or open new issue\n------------------------------------------------------\n\n"));
	}
	try{
		if (!GetSystemDirectory(buf3,localbufsize))
			THROWERR(_T("GetSystemDirectory() Error in Install Service."));
		_stprintf(buf1,_T("%s\\..\\hoststools.exe"),buf3);
		_stprintf(buf2,_T("\"%s\\..\\hoststools.exe\" -svc"),buf3);
		if (request_client)
			_stprintf(szline,_T("%s %s"),buf2,szParameters[11]),
			_tcscpy(buf2,szline),memset(szline,0,sizeof(szline)/sizeof(TCHAR));
		if (!GetModuleFileName(_ptrresev_NULL_,szline,sizeof(szline)/sizeof(TCHAR)))
			THROWERR(_T("GetModuleFileName() Error in Install Service."));
		if (_q) _tprintf(_T("    Step1:Copy file.\n"));
		if (!CopyFile(szline,buf1,FALSE))
			THROWERR(_T("CopyFile() Error in Install Service.(Is service has been installed?)"));
		if (_q) _tprintf(_T("    Step2:Connect to SCM.\n"));
		if (!(shMang=OpenSCManager(_ptrresev_NULL_,_ptrresev_NULL_,SC_MANAGER_ALL_ACCESS)))
			THROWERR(_T("OpenSCManager() failed."));
		if (_q) _tprintf(_T("    Step3:Write service.\n"));
		if (!(shSvc=CreateService(shMang,
			Sname,
			szServiceShowName,
			SERVICE_ALL_ACCESS,
			SERVICE_WIN32_OWN_PROCESS,
			request_client?SERVICE_DEMAND_START:SERVICE_AUTO_START,
			SERVICE_ERROR_NORMAL,
			buf2,//Program located
			_ptrresev_NULL_,
			_ptrresev_NULL_,
			_ptrresev_NULL_,
			_ptrresev_NULL_,
			_ptrresev_NULL_
			))){
			if (GetLastError()==ERROR_SERVICE_EXISTS){
				if (!(shSvc=OpenService(shMang,Sname,SERVICE_ALL_ACCESS)))
					THROWERR(_T("OpenService() Error in install service."));
				if (!DeleteService(shSvc))
					THROWERR(_T("DeleteService() Error in Install Service."));
				CloseServiceHandle(shSvc);
				if (!(shSvc=CreateService(shMang,
					Sname,
					szServiceShowName,
					SERVICE_ALL_ACCESS,
					SERVICE_WIN32_OWN_PROCESS,
					request_client?SERVICE_DEMAND_START:SERVICE_AUTO_START,
					SERVICE_ERROR_NORMAL,
					buf2,
					_ptrresev_NULL_,
					_ptrresev_NULL_,
					_ptrresev_NULL_,
					_ptrresev_NULL_,
					_ptrresev_NULL_
					)))
					THROWERR(_T("CreateService() failed.(2)")),CloseServiceHandle(shMang);
			}
			else
				THROWERR(_T("CreateService() failed."));
		}
		else 
			_tprintf(_T("Install service successfully.\n"));
		if (!request_client){
			if (!(shSvc=OpenService(shMang,Sname,SERVICE_START)))
				THROWERR(_T("OpenService() Failed"));
			else
				if (!StartService(shSvc,1,SzName))
					THROWERR(_T("StartService() Failed."));
					else
				MessageBox(_ptrresev_NULL_,_T("Service started successfully"),_T("Congratulations!"),
				MB_SETFOREGROUND|MB_ICONINFORMATION);
		}
	}
	catch (expection _r){
		_tprintf(_T("\nFatal Error:\n%s (GetLastError():%ld)\n\
Please contact the application's support team for more information.\n"),
		_r.Message,GetLastError());
		_tprintf(_T("\n[Debug Message]\n%s\n%s\n%s\n"),buf1,buf2,buf3);
		abort();
	}
	CloseServiceHandle(shMang);
	CloseServiceHandle(shSvc);
	if (_q) system("pause");
	return ;
}

inline void __fastcall ___autocheckmess(const TCHAR * szPstr){
	if (!request_client)
		Func_FastPMNTS(szPstr);
	else
		___pipesentmessage(szPstr);
}

inline void __fastcall ___checkEx(const TCHAR * szPstr,bool space_need){
	if (!request_client)
		if (!space_need) Func_FastPMNSS(szPstr);
		else Func_FastPMNTS(szPstr);
	else
		___pipesentmessage(szPstr);
}

inline void __fastcall _perrtext(const TCHAR * _str,bool _Reserved){
	if (!bReserved)	_tprintf(_str);
	else if (_Reserved) Func_FastPMNTS(_str);
		 else Func_FastPMNSS(_str);
	return ;
}

void ___Func_pipeCallBack(const TCHAR * str){
	_tprintf(_T("%s"),str);
	return ;
}

void Func_CallCopyHostsFile(SYSTEMTIME & st){
	FILE * fp,*_;
	if (!CopyFile(buf1,buf2,FALSE))
		THROWERR(_T("CopyFile() Error on copy a backup file"));
	if (!bReserved) _tprintf(_T("\tDone.\n    Step3:Replace Default Hosts File..."));
	if (!CopyFile(ReservedFile,buf1,FALSE))
		THROWERR(_T("CopyFile() Error on copy hosts file to system path"));
	try {
		if (!(_=_tfopen(buf1,_T("ab+"))))
			throw buf1;
		if (!(fp=_tfopen(ChangeCTLR,_T("rb"))))
			throw ChangeCTLR;
		size_t readbyte=0;
		while ((readbyte=fread(iobuffer,sizeof(char),localbufsize,fp)))
			fwrite(iobuffer,sizeof(char),readbyte,_);
		fclose(fp);fclose(_);
	}
	catch(TCHAR const * _FileName){
		_stprintf(szline,_T("_tfopen() Error in open \"%s\".\n"),_FileName);
		MessageBox(NULL,szline,_T("Error!"),MB_ICONSTOP|MB_SETFOREGROUND);
		abort();
	}
	if (!bReserved) _tprintf(_T("Replace File Successfully\n"));
	else ___autocheckmess(_T("Replace File Successfully\n"));
	if (!bReserved) Func_countBackupFile(&st),MessageBox(_ptrresev_NULL_,_T("Hosts File Set Success!"),
		_T("Congratulations!"),MB_ICONINFORMATION|MB_SETFOREGROUND);
	return ;
}


//TODO: temp string length only 1000(TCHAR)
//is safe for this program? I don't know.

DWORD __stdcall NormalEntry(LPVOID){
	SYSTEMTIME st={0,0,0,0,0,0,0,0};
	FILE * fp=_ptrresev_NULL_,*_=_ptrresev_NULL_;
	HANDLE hdThread=INVALID_HANDLE_VALUE;
	Func_CheckProFile();
	if (!GetSystemDirectory(buf3,localbufsize))
		Func_PMNTTS(_T("GetSystemDirectory() Error!(GetLastError():%ld)\n\
\tCannot get system path!"),GetLastError()),abort();
	_stprintf(buf1,_T("%s\\.."),buf3);
//	printf(buf1);
	SetCurrentDirectory(buf1);
	Func_CheckProFile();
	if (!GetEnvironmentVariable(_T("TEMP"),szline,localbufsize))
		Func_PMNTTS(_T("GetEnvironmentVariable() Error!(GetLastError():%ld)\n\
\tCannot get %%TEMP%% path!"),GetLastError()),abort();
	SetCurrentDirectory(szline);
	if (!bReserved){
		_tprintf(_T("    LICENSE:General Public License\n%s\n    Copyright (C) 2016 @Too-Naive\n"),welcomeShow);
		_tprintf(_T("    Project website:%s\n"),objectwebsite);
		_tprintf(_T("    Bug report:sometimes.naive[at]hotmail.com \n\t       Or open new issue\n\n\n"));
		_tprintf(_T("    Start replace hosts file:\n"));//    Step1:Get System Driver..."));
	}
	else{
		if (request_client) ___pipeopen(),___pipesentmessage(_T("\nMessage from service:\n\n"));
		Func_FastPMNTS(_T("Open log file.\n"));
		___checkEx(_T("LICENSE:General Public License\n"),1);
		___checkEx(_T("Copyright (C) 2016 Too-Naive\n"),0);
		___checkEx(_T("Bug report:sometimes.naive[at]hotmail.com\n"),0);
		___checkEx(_T("           Or open new issue.(https://github.com/HostsTools/Windows)\n"),0);
	}
	do {
		Sleep(bReserved?(request_client?0:60000):0);//Waiting for network
		GetLocalTime(&st);
		if (bReserved) ___autocheckmess(_T("Start replace hosts file.\n"));
		_stprintf(buf1,_T("%s\\drivers\\etc\\hosts"),buf3);
		_stprintf(buf2,_BAKFORMAT,buf3,st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
		SetFileAttributes(buf1,FILE_ATTRIBUTE_NORMAL);//for avoid CopyFile or _tfopen failed.
		try {
			if (!bReserved) _tprintf(_T("    Step1:Download hosts file..."));
			//download
			if (bReserved) if (request_client) ___pipesentmessage(_T("Download files\n"));
			for (int errcunt=0;(!Func_Download(hostsfile1,DownLocated)&&
				!Func_Download(hostsfile,DownLocated));errcunt++)
					if (errcunt>2) THROWERR(_T("DownLoad hosts file Error!"));
					else if (!bReserved) {
						_tprintf(pWait);
						Sleep(bReserved?(request_client?1000:10000):5000);
						if (!bReserved) _tprintf(_T("\tDownload hosts file..."));
					}
			//end.
			if (!bReserved) _tprintf(_T("\tDone.\n    Step2:Change Line Endings..."));
			try {
				//Change Line Ending
				//Open file and check if is open
				if (!(fp=_tfopen(DownLocated,_T("r"))))
					throw DownLocated;
				if (!(_=_tfopen(ChangeCTLR,_T("w"))))
					throw ChangeCTLR;
				//end, Read and Write to change the line ending to CRLF
				while (!feof(fp)){
					memset(szline,0,sizeof(szline));
					_fgetts(szline,1000,fp);
					_fputts(szline,_);
				}
				fclose(fp);fclose(_);
				fp=_ptrresev_NULL_,_=_ptrresev_NULL_;
				//end

				//delete tmpfile
				if (!DeleteFile(DownLocated)){
					if (bReserved)
						Func_FastPMNTS(_T("Delete tmpfile error.(%ld)\n"),GetLastError());
					else
						_tprintf(_T("Delete tmpfile error.(%ld)\n"),GetLastError());
				}

				//Read user-defined hosts start
				//Open and check file is open?
				if (!(fp=_tfopen(buf1,_T("r"))))
					throw buf1;
				if (!(_=_tfopen(ReservedFile,_T("w"))))
					throw ReservedFile;
				while (!feof(fp)){//checking is end of file?
					//to prevent print complex line
					memset(szline,0,sizeof(szline));
					_fgetts(szline,1000,fp);
					if (*szline==_T('#')) {//fast check is commit
						//File original hosts start
						if (_tcsstr(szline,_T("racaljk")))
						break; else
						// check is need ignore the commits
						if (bIgnoreCommit) continue;
					}
					//check is need ignore the new line
					if (bIgnoreNewline && *szline==_T('\n')) continue;
					// print user-defined hosts to tmp file.
					_fputts(szline,_);
				}
				// close and save the file
				fclose(_);
				_=_ptrresev_NULL_;
				//end

				//Critical function: Check is original file same with hosts file from network
				if (!feof(fp)){//check is original file is read in endof file?
					// no? start copy file function
					if (!(_=_tfopen(DownLocated,_T("w"))))
						throw DownLocated;
					_fputts(szline,_);
					while (!feof(fp)) {
						memset(szline,0,sizeof(szline));
						_fgetts(szline,1000,fp);
						_fputts(szline,_);
					}
					fclose(_);
				}
				fclose(fp);
				fp=_ptrresev_NULL_,_=_ptrresev_NULL_;
				//end.
			}

			//catch area
			catch (TCHAR const * _FileName){
				_stprintf(szline,_T("Open \"%s\" Error(_tfopen() Error)!\n"),_FileName);
				MessageBox(NULL,szline,_T("Error!"),MB_ICONSTOP|MB_SETFOREGROUND);
				abort();
			}
			//end.

			if (!Func_CheckDiff(ChangeCTLR,DownLocated)){
				if (!bReserved) _tprintf(_T("\tDone.\n\n    \
Finish:Hosts file Not update.\n\n"));
				else ___autocheckmess(_T("Finish:Hosts file Not update.\n\n"));
				if (!bReserved) {
					Func_countBackupFile(&st);
					callsystempause;
					return GetLastError();
				}
			}
			else Func_CallCopyHostsFile(st);
			if (!hdThread) TerminateThread(hdThread,0);
		}
		catch(expection runtimeerr){
			if (!bReserved){
				if (!request_client){
					Func_FastPMNTS(_T("Fatal Error:\n"));
					Func_FastPMNSS(_T("%s (GetLastError():%ld)\n"),runtimeerr.Message,GetLastError());
					Func_FastPMNSS(_T("Please contact the application's support team for more information.\n"));
				}
				else {
					_stprintf(szline,szErrMeg,runtimeerr.Message,GetLastError());
					___pipesentmessage(szline);
				}
			}
			else{
				_tprintf(szErrMeg,runtimeerr.Message,GetLastError());
				_tprintf(_T("\n[Debug Message]\n%s\n%s\n%s\n"),buf1,buf2,buf3);
				abort();
			}
		}
		Sleep(bReserved?(request_client?5000:(29*60000)):0);
	} while (bReserved);
	return GetLastError();
}

inline long Func_time2long(const SYSTEMTIME & st){
	return st.wYear*365+st.wMonth*30+st.wDay;
}

inline long Func_time2long(const WORD & year, const WORD & month, const WORD & day){
	return year*365+month*30+day;
}

bool Func_checkBackupFileTime(const SYSTEMTIME & st,TCHAR const * name){
	WORD year,month,day;
	if (_stscanf(name,_T("hosts.%4hd%2hd%2hd.%*2d%*2d%*2d"),&year,&month,&day)!=3)
		return false;
	long systime=Func_time2long(st);
	if (labs(systime-Func_time2long(year,month,day))>=60)
		return true;
	return false;
}

void Func_countBackupFile(SYSTEMTIME * st){
	HANDLE hdHandle=INVALID_HANDLE_VALUE;
	DWORD __count__=0;
	TCHAR * sizbuf=new TCHAR[localbufsize];
	_stprintf(sizbuf,_T("%s\\drivers\\etc\\hosts.20*"),buf3);
	if ((hdHandle=FindFirstFile(sizbuf,&wfd))==INVALID_HANDLE_VALUE){
		_tprintf(TEXT("FindFirstFile() Error!(%ld)\n"),GetLastError());
		delete [] sizbuf;
		return ;
	}
	do {
		if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) continue;
		if (_tcsstr(wfd.cFileName,_T("hosts.")))
			__count__++;
	} while (FindNextFile(hdHandle,&wfd));
	FindClose(hdHandle);
#ifndef _TESTONLINE
	if (__count__>20)
		if (MessageBox(NULL,_T("You number of backup file is more than 20.\n\n\
Do you want to delete out of date(Over 60 days) backup file?"),
			_T("Delete request"),
			MB_ICONQUESTION|MB_YESNO|MB_SETFOREGROUND|MB_DEFBUTTON2)
			==IDYES){
				if ((hdHandle=FindFirstFile(sizbuf,&wfd))==INVALID_HANDLE_VALUE)
					_tprintf(TEXT("FindFirstFile() Error!(%ld)\n"),GetLastError());
				_stprintf(buf1,_T("%s\\drivers\\etc"),buf3);
				SetCurrentDirectory(buf1);
				do {
					if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) continue;
//					_tprintf(_T("%s"),wfd.cFileName);
					if (Func_checkBackupFileTime(*st,wfd.cFileName))
						if (DeleteFile(wfd.cFileName))
							_tprintf(_T("    \
Delete file \"%s\" successfully\n"),wfd.cFileName);
				} while (FindNextFile(hdHandle,&wfd));
				FindClose(hdHandle);
			}
#endif
	delete [] sizbuf;
	sizbuf=_ptrresev_NULL_;
	return ;
}

void WINAPI Service_Main(DWORD,LPTSTR *){
	Func_SetErrorFile(LogFileLocate,_T("a+"));
	if (!(ssh=RegisterServiceCtrlHandler(Sname,Service_Control)))
		Func_FastPMNTS(_T("RegisterServiceCtrlHandler() Error with %ld \n\
Cannot start service!\n"),GetLastError()),abort();
	ss.dwServiceType=SERVICE_WIN32_OWN_PROCESS;
	ss.dwCurrentState=SERVICE_START_PENDING;
	ss.dwControlsAccepted=SERVICE_ACCEPT_STOP|SERVICE_ACCEPT_SHUTDOWN;
	ss.dwServiceSpecificExitCode=0;
	ss.dwWin32ExitCode=0;
	ss.dwCheckPoint=0;
	ss.dwWaitHint=1000;
	SetServiceStatus(ssh,&ss);
	ss.dwCurrentState=SERVICE_RUNNING;
	ss.dwCheckPoint=0;
	ss.dwWaitHint=0;
	SetServiceStatus(ssh,&ss);
	if (!(lphdThread[0]=CreateThread(_ptrresev_NULL_,0,NormalEntry,_ptrresev_NULL_,0,_ptrresev_NULL_))){
		Func_FastPMNTS(_T("CreateThread() Error with %ld \n\
Cannot start main thread to update hosts!\n"),GetLastError());
		ss.dwWin32ExitCode=ERROR_SERVICE_NO_THREAD;
		ss.dwCurrentState=SERVICE_STOPPED;
		ss.dwCheckPoint=1;
		ss.dwWaitHint=0;
		SetServiceStatus(ssh,&ss);
		abort();
	}
	WaitForSingleObject(lphdThread[0],INFINITE);
	ss.dwCurrentState=SERVICE_STOPPED;
	ss.dwCheckPoint=0;
	ss.dwWaitHint=0;
	SetServiceStatus(ssh,&ss);
	return ;
}

void WINAPI Service_Control(DWORD dwControl){
	switch (dwControl)
	{
		case SERVICE_CONTROL_SHUTDOWN:
		case SERVICE_CONTROL_STOP:
			ss.dwCurrentState=SERVICE_STOP_PENDING;
			ss.dwCheckPoint=0;
			ss.dwWaitHint=1000;
			SetServiceStatus(ssh,&ss);
			TerminateThread(lphdThread[0],0);
			if (request_client) CloseHandle(hdPipe);
			ss.dwCurrentState=SERVICE_STOPPED;
			ss.dwCheckPoint=0;
			ss.dwWaitHint=0;
			SetServiceStatus(ssh,&ss);
		default:break;
	}
	return ;
}
