/*
 * This source code was published under GPL v3
 *
 * Copyright (C) 2016 Too-Naive
 *
 */

#pragma once
#include <tchar.h>
#include <stdio.h>
#include <memory.h>

bool Func_CheckDiff(const TCHAR *lFilePath, const TCHAR * rFilePath) throw(){
	const size_t BUFFER_SIZE=2048;
	FILE * lFile=_tfopen(lFilePath,_T("rb")),*rFile=_tfopen(rFilePath,_T("rb"));
	if(!(lFile && rFile))
		return true;
	char *lBuffer = new char[BUFFER_SIZE];
	char *rBuffer = new char[BUFFER_SIZE];
	if (!lBuffer||!rBuffer)
		_tprintf(_T("Can't allocate memory to buffer in Func_diff\n")),abort();
	do {
		memset(lBuffer,0,BUFFER_SIZE);
		memset(rBuffer,0,BUFFER_SIZE);
		fread(lBuffer,sizeof(char),BUFFER_SIZE,lFile);
		fread(rBuffer,sizeof(char),BUFFER_SIZE,rFile);
		if (memcmp(lBuffer, rBuffer, BUFFER_SIZE)||
			((!feof(lFile)&&feof(rFile))||(feof(lFile)&&!(feof(rFile))))){
			delete [] lBuffer;
			delete [] rBuffer;
			fclose(lFile);
			fclose(rFile);
			return true;
		}
	} while ((!feof(lFile))&&(!feof(rFile)));
	delete [] lBuffer;
	delete [] rBuffer;
	fclose(lFile);
	fclose(rFile);
	return false;
}
