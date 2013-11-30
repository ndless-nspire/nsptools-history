#include <windows.h>
#include <stdio.h>
#include "AppStreamCalc.h"
#include "emu.h"

AppStreamCalc::AppStreamCalc()
{
	m_exit = false;
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
    emu_run();
    return true;
}


void AppStreamCalc::shutdownCalc()
{
	m_exit = true;
	exiting = true;
}
