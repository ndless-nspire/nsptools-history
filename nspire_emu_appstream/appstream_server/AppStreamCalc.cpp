#include <windows.h>
#include <stdio.h>
#include <shlwapi.h>
#include "AppStreamCalc.h"

AppStreamCalc::AppStreamCalc()
{
}

AppStreamCalc::~AppStreamCalc()
{
}

//
// Static threadProc function to start the thread with a Calc instance
//
DWORD WINAPI AppStreamCalc::threadProc(void* param)
{
    AppStreamCalc* pCalc = (AppStreamCalc*)param;
    return pCalc->runCalcThread() ? 0 : -1;
}

bool AppStreamCalc::runCalcThread()
{
	TCHAR exePath[MAX_PATH];
	GetModuleFileName(NULL, exePath, MAX_PATH);
	PathRemoveFileSpec(exePath);
	strncat(exePath, "\\nspire_emu.exe", MAX_PATH);
	STARTUPINFO startupInfo;
	memset(&startupInfo, 0, sizeof(startupInfo));
	startupInfo.wShowWindow = SW_SHOW;
	memset(&processInfo, 0, sizeof(processInfo));
	if (!CreateProcess(exePath, GetCommandLine(), NULL, NULL, 0, CREATE_NEW_CONSOLE, NULL, NULL, &startupInfo, &processInfo))
		return false;
	WaitForSingleObject(processInfo.hProcess, INFINITE);
	printf("waited\n");
    return true;
}


void AppStreamCalc::shutdownCalc()
{
	TerminateProcess(processInfo.hProcess, 0);
}
