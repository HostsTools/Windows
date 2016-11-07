/*
 * This source code was published under GPL v3
 *
 * Copyright (C) 2016 Too-Naive
 *
 */
//download.hpp(Re-edit)\

#pragma once
 
#include <windows.h>
#include <wininet.h>
#include <tchar.h>

//need wininet.lib

#ifdef _MSC_VER
#pragma comment(lib,"wininet.lib")
#endif

//define _pNULL_
#ifndef _pNULL_
	#if (defined(__GXX_EXPERIMENTAL_CXX0X__)||\
	    (defined(_MSC_VER)&&(_MSC_VER>=1800)))
		#define _pNULL_ nullptr
	#else
		#define _pNULL_ NULL
	#endif
#endif
//end


#define ___userAgent _T("Mozilla/4.0 (compatible; Windows NT 6.1)")

bool Func_DownloadEx(const TCHAR * url,const TCHAR * file,const DWORD FileAttributes){
	const size_t dwBuffer=2048;		//buffer size
	HINTERNET hWeb,hRequest;		//Internet request handle
	DWORD dwReadByte=0,dwReserved;		//read byte count
	char szBuffer[dwBuffer]="";		//read buff
	HANDLE hdFile=INVALID_HANDLE_VALUE;	//file handle
	//bug check begin
	if (!(hWeb=InternetOpen(___userAgent,INTERNET_OPEN_TYPE_PRECONFIG,_pNULL_,_pNULL_,0))) return false;
	if (!(hRequest=InternetOpenUrl(hWeb,url,_pNULL_,0,INTERNET_FLAG_DONT_CACHE,0))){
		InternetCloseHandle(hWeb);
		return false;
	}
	if ((hdFile=CreateFile(file,GENERIC_WRITE,0,_pNULL_,CREATE_ALWAYS,FileAttributes,_pNULL_))==INVALID_HANDLE_VALUE){
		InternetCloseHandle(hWeb);
		InternetCloseHandle(hRequest);
		return false;
	}
	//end.
	while (InternetReadFile(hRequest,(PVOID)szBuffer,dwBuffer,&dwReadByte) && dwReadByte)
		WriteFile(hdFile,szBuffer,dwReadByte,&dwReserved,_pNULL_);
	CloseHandle(hdFile);
	InternetCloseHandle(hRequest);
	InternetCloseHandle(hWeb);
	return true;
}

inline bool __fastcall Func_Download(const TCHAR * url,const TCHAR * file){//for backward compatibility
	return Func_DownloadEx(url,file,FILE_ATTRIBUTE_NORMAL);
}

inline bool __fastcall Func_easyDownload(const TCHAR * url){
	const TCHAR * _szLocate_=_pNULL_;
	for (_szLocate_=url;*++_szLocate_;);
	if (*--_szLocate_=='/') return false;
	for (;*(--_szLocate_)!='/';);
	_szLocate_++;
	return Func_Download(url,_szLocate_);
}
