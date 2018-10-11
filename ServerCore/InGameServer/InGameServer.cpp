// InGameServer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


int _tmain(int argc, _TCHAR* argv[])
{
	CoInitialize(NULL);

	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (!MiniDump::Begin())
		return 0;

	InGameIocp iocp;
	if (iocp.Begin())
	{
		_tprintf(_T("=============== Start InGame Server ===============\n"));
		getchar();
		iocp.End();
	}

	WSACleanup();
	MiniDump::End();
	CoUninitialize();

    return 0;
}

