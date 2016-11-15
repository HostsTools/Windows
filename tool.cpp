/*
 * This source code was published under GPL v3
 *
 * Copyright (C) 2016 Too-Naive
 *
 */

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "header/download.hpp"
#include <tchar.h>
#include <stdlib.h>
#include "header/ptrerr.hpp"
#include "header/diff.hpp"
#include <signal.h>
#include "header/pipedebug.hpp"
#include "header/default.hpp"

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
//#define hostsfile1 _T("https://coding.net/u/scaffrey/p/hosts/git/raw/master/hosts")

#ifdef _LOCAL
	#undef hostsfile
	#undef hostsfile1
//	#ifdef _LOCAL
		#define hostsfile _T("http://localhost/hosts")
		#define hostsfile1 hostsfile
/*	#else
		#define hostsfile1 hostsfile
	#endif*/
#endif
#define objectwebsite _T("https:\x2f\x2fgithub.com/HostsTools/Windows")
//end.

#define ConsoleTitle _T("racaljk-host tool    v2.1.19  Build time:Nov. 15th, '16")

#define CASE(x,y) case x : y; break;
#define DEBUGCASE(x) CASE(x,___debug_point_reset(x))
#define pWait _T("\n    \
There seems something wrong in download file, we will retry after 5 seconds.\n")

#define createT(x) CreateThread(_pNULL_,0,(x),_pNULL_,0,_pNULL_)

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

//register key name
const TCHAR * _keyname=_T("hoststool-windows");
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

#define SHOW_HELP _T("%s\n\
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
    
#define copyrightshow _T("\
------------------------------------------------------------\n\
Hosts Tool for Windows Console by: Too-Naive\n\
Copyright (C) 2016 @Too-Naive License:General Public License\n\
------------------------------------------------------------\n")

//Global variable
SERVICE_STATUS_HANDLE ssh;
SERVICE_STATUS ss;
HANDLE lphdThread[]={
	INVALID_HANDLE_VALUE,INVALID_HANDLE_VALUE
};
bool request_client,bIsServiceMode,bIgnoreNewline,bIgnoreCommit,bIsNulFile=true;
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
inline void __show_str(TCHAR const *,TCHAR const *,TCHAR const *);
void Func_ResetFile();
void ___Func_pipeCallBack(TCHAR const*);
void Func_CallCopyHostsFile();
void Func_CheckProFile();
TCHAR * dotdotcheck(TCHAR *);
void Func_countBackupFile(SYSTEMTIME *);
bool Func_checkBackupFileTime(const SYSTEMTIME & , TCHAR const *);
DWORD WINAPI MonitorServiceThread(LPVOID);
inline bool Func_checkBusyTime();
void Func_hiddenStart(const TCHAR *);
void Func_hiddenEntry();
void Func_installHiddenStart(const TCHAR *);
//DWORD __stdcall Func_Update(LPVOID);


SERVICE_TABLE_ENTRY STE[2]={{Sname,Service_Main},{_pNULL_,_pNULL_}};

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
#define EXEC_START_INSTALL2			(1<<0x06|EXEC_START_INSTALL_SERVICE)
#define EXEC_HIDDEN_START	(1<<0x01)
#define EXEC_HIDDEN_STARTED 	(EXEC_HIDDEN_START|1<<0x03)
#define EXEC_START_UNINSTALL_SERVICE2 (1<<0x06|EXEC_START_UNINSTALL_SERVICE)

#define DEBUG_ENTRY 			(1<<0x00)
#define	EXEC_DEBUG_RESET		(DEBUG_ENTRY|(1<<0x01))
#define	DEBUG_SERVICE_STOP		(DEBUG_ENTRY|(1<<0x02))
#define	DEBUG_SERVICE_START		(DEBUG_ENTRY|(1<<0x03))
#define	DEBUG_SERVICE_REINSTALL		(DEBUG_ENTRY|(1<<0x04))
#define	OPEN_LISTEN			(DEBUG_ENTRY|(1<<0x05))

#define STOP_WITH_RESTART (1<<0x02)
//end.

//define _In_ parameters string
TCHAR const *szParameters[]={
	_T("svc"),				//for backward compatibility
	_T("fi"),				//1
	_T("fu"),				//2
	_T("hardreset"),				//3
	_T("-debug-reset"),			//4
	_T("fi2"),				//5
	_T("?"),				//6
	_T("-debug-stop"),			//7
	_T("-debug-start"),			//8
	_T("-debug-reiu"),			//9
	_T("-debug-pipe"),			//10
	_T("--pipe"),				//11
	_T("r"),					//12
	_T("h"),
	_T("h2")
};

//Check parameters function
int __fastcall __Check_Parameters(int argc,TCHAR const **argv){
	if (argc==1) return EXEC_START_NORMAL;
	if (argc>3||!(( argv[1][0]==_T('/') ||
		argv[1][0]==_T('-')) && argv[1][1]!=_T('\0'))) BAD_EXIT;
	size_t i=0;
	for (;_tcscmp(&(argv[1][1]),szParameters[i]) &&
		i<sizeof(szParameters)/sizeof(szParameters[0]);i++);
	if (!(i==0 || i==1 || i==9 || i==12)
		&& argc>2) BAD_EXIT;
	if (argc==3 && 
		(!_tcscmp(argv[2],szParameters[11]) || !_tcscmp(argv[2],szParameters[3]))) 
			request_client=1;
		else if (argc==3 && !request_client) BAD_EXIT;
	switch (i){
		case  0: bIsServiceMode=true;
			 return EXEC_START_SERVICE;
		case  1: return EXEC_START_INSTALL_SERVICE;
		case  2: return EXEC_START_UNINSTALL_SERVICE;
		case  4: return EXEC_DEBUG_RESET;	//restart service
		case  5: return EXEC_START_INSTALL2;
		case  6: return EXEC_START_HELP;
		case  7: return DEBUG_SERVICE_STOP;	//stop service
		case  8: return DEBUG_SERVICE_START;	//start service
		case  9: return DEBUG_SERVICE_REINSTALL;//reinstall service
		case 10: return OPEN_LISTEN;
		case 12: return RESET_FILE;
		case 13: return EXEC_HIDDEN_START;
		case 14: return EXEC_HIDDEN_STARTED;
		default: BAD_EXIT;
	}
	BAD_EXIT;
}
//end.

//main entry
int _tmain(int argc,TCHAR const ** argv){
	SetConsoleTitle(ConsoleTitle);
	try {
		switch (__Check_Parameters(argc,argv)){
			CASE(EXEC_START_NORMAL,NormalEntry(_pNULL_));
			CASE(EXEC_START_INSTALL_SERVICE,Func_Service_Install(true));
			CASE(EXEC_START_UNINSTALL_SERVICE,Func_Service_UnInstall(true));
			CASE(EXEC_START_SERVICE,StartServiceCtrlDispatcher(STE));
			CASE(EXEC_START_HELP,__show_str(SHOW_HELP,copyrightshow,Sname));
//			CASE(SHOW_LICENSE,__show_str(szgpl_Raw,_pNULL_));
			DEBUGCASE(EXEC_DEBUG_RESET);
			DEBUGCASE(DEBUG_SERVICE_STOP);
			DEBUGCASE(DEBUG_SERVICE_START);
			DEBUGCASE(DEBUG_SERVICE_REINSTALL);
			CASE(OPEN_LISTEN,___debug_point_reset(OPEN_LISTEN));
			CASE(RESET_FILE,Func_ResetFile());
			CASE(EXEC_HIDDEN_START,Func_hiddenStart(argv[0]));
			CASE(EXEC_HIDDEN_STARTED,Func_hiddenEntry());
			CASE(EXEC_START_INSTALL2,Func_installHiddenStart(argv[0]));
			default:break;
		}
	}
	catch (...){
		
	}
	return 0;
}

void Func_hiddenStart(const TCHAR * _exec){
	ShellExecute(NULL,_T("open"),_exec,_T("-h2"),NULL,SW_HIDE);
	exit(0);
}

void Func_hiddenEntry(){
	bIsServiceMode=true;
	Func_SetErrorFile(LogFileLocate,_T("a+"));
	Sleep(30000);//waiting for network
	NormalEntry(_pNULL_);
	//TODO: if program throw expections, user may cannot find error.
	return ;
}

void Func_installHiddenStart(const TCHAR * __path){
	HKEY _hkey_=_pNULL_;
	try {
		if (!GetSystemDirectory(buf1,localbufsize))
			_tprintf(_T("GetSystemDirectory() Failed.(%ld)\n"),GetLastError());
		_stprintf(buf2,_T("%s\\..\\hostsstart.exe"),buf1);
		dotdotcheck(buf2);
		if (!CopyFile(__path,buf2,FALSE))
			THROWERR(_T("CopyFile() Failed in Func_installHiddenStart().\n"));
		_stprintf(buf1,_T("\"%s\" -h"),buf2);
		if (RegOpenKey(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"),&_hkey_))
				THROWERR(_T("RegOpenKey() Error!\n"));
		if (ERROR_FILE_NOT_FOUND==RegQueryValueEx(_hkey_,_keyname,NULL,NULL,NULL,NULL))
			if (RegSetValueEx(_hkey_,_keyname,0,REG_SZ,(BYTE *)buf1,_tcslen(buf1)))
				THROWERR(_T("RegSetValueEx() Error!\n"));
	}
	
	catch (expection r){
		if (!_hkey_) RegCloseKey(_hkey_);
//		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,NULL,0,0,szline,localbufsize,NULL);
		_tprintf(szErrMeg,r.Message,GetLastError());
		_tprintf(_T("\n[Debug Message]\n%s\n%s\n%s\n"),buf1,buf2,buf3);
		abort();
	}
	RegCloseKey(_hkey_);
	MessageBox(_pNULL_,_T("Register statrup successfully."),_T("Congratulations!"),MB_OK|MB_ICONINFORMATION);
	return ;
}
/*
void Func_uninstallHiddenStart(){
	HKEY _hkey_=_pNULL_;
	try {
		if (RegOpenKey(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"),&_hkey_))
				THROWERR(_T("RegOpenKey() Error!\n"));
		if (ERROR_FILE_NOT_FOUND==RegQueryValueEx(_hkey_,_keyname,NULL,NULL,NULL,NULL))
			if (!RegSetValueEx(_hkey_,_keyname,0,REG_SZ,(BYTE *)buf1,_tcslen(buf1)))
				THROWERR(_T("RegSetValueEx() Error!\n"));
		RegDeleteValue(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"));
	}
	catch (expection r){
		if (!_hkey_) RegCloseKey(_hkey_);
		_tprintf(szErrMeg,r.Message,GetLastError());
		_tprintf(_T("\n[Debug Message]\n%s\n%s\n%s\n"),buf1,buf2,buf3);
		abort();
	}
	RegCloseKey(_hkey_);
	MessageBox(_pNULL_,_T("Register statrup successfully."),_T("Congratulations!"),MB_OK|MB_ICONINFORMATION);
	return ;
}
*/
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

namespace __Dpipe{
	DWORD ___OnError(const TCHAR *__szAdditionalMessage){
		Func_FastPMNTS(_T("%s() Error! (%ld)\n"),__szAdditionalMessage,GetLastError());
		//We need it?
		ss.dwServiceSpecificExitCode=STOP_WITH_RESTART;
		ss.dwWin32ExitCode=ERROR_SERVICE_SPECIFIC_ERROR;
		ss.dwCurrentState=SERVICE_STOPPED;
		SetServiceStatus(ssh,&ss);
		ExitProcess(1);
		return GetLastError();
	}
}

void Func_ResetFile(){
	SYSTEMTIME st={0,0,0,0,0,0,0,0};
	bool chk=false;
	_tprintf(copyrightshow);
	if (!GetSystemDirectory(buf3,localbufsize))
		_tprintf(_T("    GetSystemDirectory() Error!(GetLastError():%ld)\n\
\tCannot get system path!"),GetLastError()),abort();
	GetLocalTime(&st);
	_stprintf(buf1,_T("%s\\drivers\\etc\\hosts"),buf3);
	_stprintf(buf2,_BAKFORMAT,buf3,st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
	if (CopyFile(buf1,buf2,FALSE))
		_tprintf(_T("Backup File success\nFilename:%s\n\n"),buf2);
	else
		_tprintf(_T("Unable to backup file.(%ld)\n"),GetLastError()),abort();
//reset file start
	FILE *fp=_tfopen(buf1,request_client?_T("w"):_T("r"));
	if (!fp) {//,abort();
		if (request_client){
			_tprintf(_T("Cannot open file!\nUsing plan B.\n"));
			fp=_tfopen(ReservedFile,_T("w"));
			fclose(fp);
			fp=_pNULL_;
			chk=true;
		}else{
			_tprintf(_T("Cannot open file!\n"));
			abort();
		}
	}
if (request_client){
		if (chk) {
			if (!CopyFile(ReservedFile,buf1,FALSE)){
				_stprintf(szline,
				_T("CopyFile() Failed!(%ld)\nPlease copy the \"%s\" file to\
\"%%systemroot%%\\drivers\\etc\\hosts\" Manually."),
				GetLastError(),ReservedFile);
				MessageBox(NULL,_T("Fatal Error"),szline,MB_SETFOREGROUND|MB_ICONSTOP);
				abort();
			}
		}
		else _ftprintf(fp,_T("%s"),szDefatult_hostsfile);
		fclose(fp);
		_tprintf(_T("    Reset file successfully.\n"));
		callsystempause;
		ExitProcess(0);
}
	if (!GetEnvironmentVariable(_T("TEMP"),szline,localbufsize))
		_tprintf(_T("Can't get TEMP path, using current directory.(%ld)\n"),
				GetLastError()),
		*szline=_T('.');
	SetCurrentDirectory(szline);
	FILE * _=_tfopen(ReservedFile,_T("w"));
	if (!_) _tprintf(_T("Can't open \"%s\" file.\n"),ReservedFile),abort();
//	memset(buf3,0,sizeof(buf3));
	while (!feof(fp)){
		memset(szline,0,sizeof(szline));
		_fgetts(szline,localbufsize,fp);
		if (_tcsstr(szline,_T("racaljk")))
			_fputts(szline,_);
		else break;
	}
	fclose(fp);
	fclose(_);
	if (!CopyFile(ReservedFile,buf1,FALSE))
		_tprintf(_T("CopyFile() Failed.(%ld)\nPlease copy the \"%s\" file to \
\"%%systemroot%%\\drivers\\etc\\hosts\" Manually."),GetLastError(),ReservedFile),
		abort();
//reset file end
	DeleteFile(ReservedFile);
	_tprintf(_T("    Reset file successfully.\n"));
	callsystempause;
	return ;
}

void __abrt(int){
	SetConsoleTitle(_T("Recieved signal SIGINT"));
	_tprintf(_T("Received signal SIGINT\nPlease waiting program exit!\n"));
	request_client=0;
	_tprintf(_T("Uninstall service.\n"));
	Func_Service_UnInstall(false);
	TerminateThread(lphdThread[0],0);
	if (!lphdThread[1]) TerminateThread(lphdThread[1],0);
	CloseHandle(lphdThread[0]);
	CloseHandle(lphdThread[1]);
	CloseHandle(hdPipe);
	_tprintf(_T("Program will now exit.\n"));
	exit(0);
}

void __abrt1(int){
	for (int i=3;i--;_tprintf(_T("Please check anti-virus software then open this program again.\n")));
}


inline void __show_str(TCHAR const* st,TCHAR const * str2,TCHAR const * str3){
	_tprintf(st,str2,str3);
	callsystempause;
	return ;
}

void ___debug_point_reset(int _par){
	SC_HANDLE shMang=_pNULL_,shSvc=_pNULL_;
	if (_par==DEBUG_SERVICE_REINSTALL){
		Func_Service_UnInstall(false);
		Func_Service_Install(false);
		return ;
	}
	try {
		if (_par!=OPEN_LISTEN) {
			if (!(shMang=OpenSCManager(_pNULL_,_pNULL_,SC_MANAGER_ALL_ACCESS)))
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
					if (!(lphdThread[0]=createT(OpenPipeService)))
						THROWERR(_T("CreateThread() Error!"));
					if (!(lphdThread[1]=createT(MonitorServiceThread)))
						_tprintf(_T("CreateThread() Error! in\
 create MonitorServiceThread.(%ld)\n"),GetLastError());
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

DWORD WINAPI MonitorServiceThread(LPVOID){
	SC_HANDLE shMang=_pNULL_,shSvc=_pNULL_;
	if (!(shMang=OpenSCManager(_pNULL_,_pNULL_,SC_MANAGER_ALL_ACCESS))){
		_tprintf(_T("OpenSCManager() Error in Monitor.(%ld)\n"),GetLastError());
		abort();
	}
	if (!(shSvc=OpenService(shMang,Sname,SERVICE_QUERY_STATUS))){
		if (GetLastError()==ERROR_SERVICE_NOT_FOUND){
			CloseServiceHandle(shMang);
			___debug_point_reset(DEBUG_SERVICE_REINSTALL);
		}
		else{
			_tprintf(_T("OpenService() Error in Monitor.(%ld)\n"),GetLastError());
			abort();
		}
	}
	SERVICE_STATUS mon_ss={0l,0l,0l,0l,0l,0l,0l};
	while (1){
		QueryServiceStatus(shSvc,&mon_ss);
		if (mon_ss.dwCurrentState==SERVICE_STOPPED || mon_ss.dwCurrentState==SERVICE_STOP_PENDING)
			if (mon_ss.dwWin32ExitCode==ERROR_SERVICE_SPECIFIC_ERROR &&
				mon_ss.dwServiceSpecificExitCode==STOP_WITH_RESTART){
//				___debug_point_reset(DEBUG_SERVICE_REINSTALL);
				_tprintf(_T("This production may need restart to finish this.\n\
Please press Ctrl+C to terminate program then restart again.\n\
Error code:(%ld)\n"),GetLastError());
				break;
			}
		Sleep(1000);
	}
	CloseServiceHandle(shMang);
	CloseServiceHandle(shSvc);
	return GetLastError();
}

//short path if str has ".."
//Should we need check "\\..\\" ?
TCHAR * dotdotcheck(TCHAR * str){
	TCHAR * _,*_tmp=new TCHAR[MAX_PATH];
	memset(_tmp,0,sizeof(_tmp));
	if ((_=_tcsstr(str,_T("..")))){
		_stscanf(_+2,_T("%100s"),_tmp);
		--_;
		while ((*(--_))!='\\');
		_stprintf(_,_T("%s"),_tmp);
	}
	delete [] _tmp;
	return str;
}

void Func_Service_UnInstall(bool _quite){
	SC_HANDLE shMang=_pNULL_,shSvc=_pNULL_;
	Sleep(1000);
	try{
		if (!GetSystemDirectory(buf3,localbufsize))
			THROWERR(_T("GetSystemDirectory() Error in UnInstall Service."));
		_stprintf(buf1,_T("%s\\..\\hoststools.exe"),buf3);
		if (!GetModuleFileName(_pNULL_,buf2,localbufsize))
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
		if (!(shMang=OpenSCManager(_pNULL_,_pNULL_,SC_MANAGER_ALL_ACCESS)))
			THROWERR(_T("OpenSCManager() Error in Uninstall service."));
		if (!(shSvc=OpenService(shMang,Sname,SERVICE_ALL_ACCESS)))
			if (_quite) 
				THROWERR(_T("OpenService() Error in Uninstall service.\nIs service exist?"));
		if (!ControlService(shSvc,SERVICE_CONTROL_STOP,&ss))
			if (_quite) 
				_tprintf(_T("ControlService() Error in Uninstall service.\n%s"),
					_T("Service may not in running.\n"));
		Sleep(2000);//Wait for service stop
		if (!DeleteService(shSvc))
			if (_quite) 
				THROWERR(_T("DeleteService() Error in UnInstall service."));
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
	SC_HANDLE shMang=_pNULL_,shSvc=_pNULL_;
	if (_q){
		_tprintf(_T("    LICENSE:General Public License\n    \
Copyright (C) 2016 @Too-Naive\n\n"));
		_tprintf(_T("    Bug report:[hidden] \n\t       \
Or open new issue\n------------------------------------------------------\n\n"));
	}
	try {
		if (!GetSystemDirectory(buf3,localbufsize))
			THROWERR(_T("GetSystemDirectory() Error in Install Service."));
		_stprintf(buf1,_T("%s\\..\\hoststools.exe"),buf3);
		_stprintf(buf2,_T("\"%s\\..\\hoststools.exe\" -svc"),buf3);
		if (request_client)
			_stprintf(szline,_T("%s %s"),buf2,szParameters[11]),
			_tcscpy(buf2,szline),memset(szline,0,sizeof(szline)/sizeof(TCHAR));
		if (!GetModuleFileName(_pNULL_,szline,sizeof(szline)/sizeof(TCHAR)))
			THROWERR(_T("GetModuleFileName() Error in Install Service."));
		if (_q) _tprintf(_T("    Step1:Copy file.\n"));
		if (!CopyFile(szline,buf1,FALSE))
			THROWERR(_T("CopyFile() Error in Install Service.(Is service has been installed?)"));
		if (_q) _tprintf(_T("    Step2:Connect to SCM.\n"));
		if (!(shMang=OpenSCManager(_pNULL_,_pNULL_,SC_MANAGER_ALL_ACCESS)))
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
			_pNULL_,
			_pNULL_,
			_pNULL_,
			_pNULL_,
			_pNULL_
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
					_pNULL_,
					_pNULL_,
					_pNULL_,
					_pNULL_,
					_pNULL_
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
				MessageBox(_pNULL_,_T("Service started successfully"),
				_T("Congratulations!"),
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

inline void __fastcall ___autocheckmsg(const TCHAR * szPstr){
	if (!request_client)
		Func_FastPMNTS(szPstr);
	else
		___pipesendmsg(szPstr);
}

inline void __fastcall ___checkEx(const TCHAR * szPstr,bool space_need){
	if (!request_client)
		if (!space_need) Func_FastPMNSS(szPstr);
		else Func_FastPMNTS(szPstr);
	else
		___pipesendmsg(szPstr);
}

inline void __fastcall _perr_T(const TCHAR * _str,bool _Reserved){
	if (!bIsServiceMode)	_tprintf(_str);
	else if (_Reserved) Func_FastPMNTS(_str);
		 else Func_FastPMNSS(_str);
	return ;
}

void ___Func_pipeCallBack(const TCHAR * str){
	_tprintf(_T("%s"),str);
	return ;
}

void Func_CallCopyHostsFile(SYSTEMTIME & st){
	FILE * filepoint1,*filepoint2;
	signal(SIGABRT,__abrt1);
	
	//empty file check
	if (!bIsNulFile){
		TCHAR ch;
		filepoint2=_tfopen(ReservedFile,_T("r"));
		for (ch=_fgettc(filepoint2);ch==_T(' ') || ch==_T('\n') || ch==_T('\r');ch=_fgettc(filepoint2));
		if (ch==EOF) bIsNulFile=true;
	}
	//end
	
	if (!CopyFile(buf1,buf2,FALSE))
		THROWERR(_T("CopyFile() Error on copy a backup file"));
	if (!bIsServiceMode) _tprintf(_T("\tDone.\n    Step3:Replace Default Hosts File..."));
	if (!CopyFile(ReservedFile,buf1,FALSE))
		THROWERR(_T("CopyFile() Error on copy hosts file to system path"));
	try {
		if (!(filepoint2=_tfopen(buf1,_T("ab+"))))
			throw buf1;
		_fputts(_T(""),filepoint2);
		if (!(filepoint1=_tfopen(ChangeCTLR,_T("rb"))))
			throw ChangeCTLR;
		if (!bIsNulFile) _ftprintf(filepoint2,_T("\n"));
		size_t readbyte=0;
		while ((readbyte=fread(iobuffer,sizeof(char),localbufsize,filepoint1)))
			fwrite(iobuffer,sizeof(char),readbyte,filepoint2);
		fclose(filepoint1);fclose(filepoint2);
	}
	catch(TCHAR const * _FileName){
		_stprintf(szline,_T("_tfopen() Error in open \"%s\".\n"),_FileName);
		MessageBox(NULL,szline,_T("Error!"),MB_ICONSTOP|MB_SETFOREGROUND);
		abort();
	}
	if (!bIsServiceMode) _tprintf(_T("Replace File Successfully\n"));
	else ___autocheckmsg(_T("Replace File Successfully\n"));
	if (!bIsServiceMode) Func_countBackupFile(&st),MessageBox(_pNULL_,
		_T("Hosts File Set Success!"),
		_T("Congratulations!"),MB_ICONINFORMATION|MB_SETFOREGROUND);
	return ;
}


//TODO: temp C-Sytle string length only 1000(TCHAR)
//Is safe for this program? I don't know.

//Q:Why don't you use std::string?
//A:STL will made my program so big(?)

DWORD __stdcall NormalEntry(LPVOID){
	SYSTEMTIME st={0,0,0,0,0,0,0,0};
	FILE * fp=_pNULL_,*_=_pNULL_;
	HANDLE hdThread=INVALID_HANDLE_VALUE;
	if (!GetSystemDirectory(buf3,localbufsize))
		Func_PMNTTS(_T("GetSystemDirectory() Error!(GetLastError():%ld)\n\
\tCannot get system path!"),GetLastError()),abort();
	_stprintf(buf1,_T("%s\\.."),buf3);
	SetCurrentDirectory(buf1);
	Func_CheckProFile();
	if (!GetEnvironmentVariable(_T("TEMP"),szline,localbufsize))
		Func_PMNTTS(_T("GetEnvironmentVariable() Error!(GetLastError():%ld)\n\
\tCannot get %%TEMP%% path!"),GetLastError()),abort();
	SetCurrentDirectory(szline);
	if (!bIsServiceMode){
		_tprintf(_T("    LICENSE:General Public License\n%s\n\
    Copyright (C) 2016 @Too-Naive\n"),welcomeShow);
		_tprintf(_T("    Project website:%s\n"),objectwebsite);
		_tprintf(_T("    Bug report:[hidden] \n\t\
       Or open new issue\n\n\n"));
		_tprintf(_T("    Start replace hosts file:\n"));
	} else {
		if (request_client) ___pipeopen(),___pipesendmsg(_T("\nMessage from service:\n\n"));
		Func_FastPMNTS(_T("Open log file.\n"));
		___checkEx(_T("LICENSE:General Public License\n"),1);
		___checkEx(_T("Copyright (C) 2016 Too-Naive\n"),0);
		___checkEx(_T("Bug report:[hidden]\n"),0);
		___checkEx(_T("           Or open new issue.(https://github.com/HostsTools/Windows)\n"),0);
	}
	do {
		Sleep(bIsServiceMode?(request_client?0:60000):0);//Waiting for network
		GetLocalTime(&st);
		if (bIsServiceMode) ___autocheckmsg(_T("Start replace hosts file.\n"));
		_stprintf(buf1,_T("%s\\drivers\\etc\\hosts"),buf3);
		_stprintf(buf2,_BAKFORMAT,buf3,st.wYear,st.wMonth,st.wDay,st.wHour,
									st.wMinute,st.wSecond);
		SetFileAttributes(buf1,FILE_ATTRIBUTE_NORMAL);//To avoid CopyFile or _tfopen failed.
		try {
			if (!bIsServiceMode) _tprintf(_T("    Step1:Download hosts file..."));
			//download
			if (bIsServiceMode) if (request_client) ___pipesendmsg(_T("Download files\n"));
			for (int errcunt=0;(//!Func_Download(hostsfile1,DownLocated)&&
				!Func_Download(hostsfile,DownLocated));errcunt++)
					if (errcunt>2) THROWERR(_T("DownLoad hosts file Error!"));
					else if (!bIsServiceMode) {
						_tprintf(pWait);
						Sleep(bIsServiceMode?(request_client?1000:10000):5000);
						if (!bIsServiceMode) _tprintf(_T("\tDownload hosts file..."));
					}
			//end.
			if (!bIsServiceMode) _tprintf(_T("\tDone.\n    Step2:Change Line Endings..."));
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
					_fgetts(szline,localbufsize,fp);
					_fputts(szline,_);
				}
				fclose(fp);fclose(_);
				fp=_pNULL_,_=_pNULL_;
				//end

				//delete tmpfile
				if (!DeleteFile(DownLocated)){
					if (bIsServiceMode)
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
					_fgetts(szline,localbufsize,fp);
					if (*szline==_T('#')) {//fast check is commit
						//File original hosts start
						if (_tcsstr(szline,_T("racaljk")))
						break; else
						// check is need ignore the commits
						if (bIgnoreCommit) continue;
					}
					//check is need ignore the new line
					if (bIgnoreNewline && *szline==_T('\n')) continue;
					//empty file check (if read least one line.)
					bIsNulFile=false;
					// print user-defined hosts to tmp file.
					_fputts(szline,_);
				}
				// close and save the file
				fclose(_);
				_=_pNULL_;
				//end

				//Critical function: Check is original file same with hosts file from network
				if (!feof(fp)){//check is original file is read in endof file?
					// no? start copy file function
					if (!(_=_tfopen(DownLocated,_T("w"))))
						throw DownLocated;
					_fputts(szline,_);
					while (!feof(fp)) {
						memset(szline,0,sizeof(szline));
						_fgetts(szline,localbufsize,fp);
						_fputts(szline,_);
					}
					fclose(_);
				}
				fclose(fp);
				fp=_pNULL_,_=_pNULL_;
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
				if (!bIsServiceMode) _tprintf(_T("\tDone.\n\n    \
Finish:Hosts file Not update.\n\n"));
				else ___autocheckmsg(_T("Finish:Hosts file Not update.\n\n"));
				if (!bIsServiceMode) {
					Func_countBackupFile(&st);
					callsystempause;
					return GetLastError();
				}
			}
			else Func_CallCopyHostsFile(st);
			if (!hdThread) TerminateThread(hdThread,0);
		}
		catch(expection runtimeerr){
			if (bIsServiceMode){
				if (!request_client){
					Func_FastPMNTS(_T("Fatal Error:\n"));
					Func_FastPMNSS(_T("%s (GetLastError():%ld)\n"),
									runtimeerr.Message,GetLastError());
					Func_FastPMNSS(_T("Please contact the application's\
 support team for more information.\n"));
				}
				else {
					_stprintf(szline,szErrMeg,runtimeerr.Message,GetLastError());
					___pipesendmsg(szline);
				}
			}
			else{
				_tprintf(szErrMeg,runtimeerr.Message,GetLastError());
				_tprintf(_T("\n[Debug Message]\n%s\n%s\n%s\n"),buf1,buf2,buf3);
				abort();
			}
		}
		Sleep(bIsServiceMode?(request_client?10000:(Func_checkBusyTime()?29*60000:60*60000)):0);
	} while (bIsServiceMode);
	return GetLastError();
}

bool Func_checkBackupFileTime(const SYSTEMTIME & st,TCHAR const * name){
	WORD year,month,day;
	if (_stscanf(name,_T("hosts.%4hd%2hd%2hd.%*2d%*2d%*2d"),&year,&month,&day)!=3)
		return false;
	long systime=st.wYear*365+st.wMonth*30+st.wDay;
	if (labs(systime-(year*365+month*30+day))>=60)
		return true;
	return false;
}

inline bool Func_checkBusyTime(){
	SYSTEMTIME st={0,0,0,0,0,0,0,0};
	GetSystemTime(&st);
	if (st.wHour<3 || st.wHour>7) return true;
	return false;
}

void Func_countBackupFile(SYSTEMTIME * st){
	HANDLE hdHandle=INVALID_HANDLE_VALUE;
	DWORD __count__=0;
	TCHAR * sizbuf=new TCHAR[localbufsize];
	_stprintf(sizbuf,_T("%s\\drivers\\etc\\hosts.20*"),buf3);
	if ((hdHandle=FindFirstFile(sizbuf,&wfd))==INVALID_HANDLE_VALUE){
		_tprintf(_T("FindFirstFile() Error!(%ld)\n"),GetLastError());
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
					_tprintf(_T("FindFirstFile() Error!(%ld)\n"),GetLastError());
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
	sizbuf=_pNULL_;
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
	if (!(lphdThread[0]=createT(NormalEntry))){
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
	switch (dwControl){
		case SERVICE_CONTROL_SHUTDOWN:
		case SERVICE_CONTROL_STOP:
			ss.dwCurrentState=SERVICE_STOP_PENDING;
			ss.dwCheckPoint=0;
			ss.dwWaitHint=1000;
			SetServiceStatus(ssh,&ss);
			//Why we use terminate?
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
