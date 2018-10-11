// Server.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


int _tmain(int argc, _TCHAR* argv[])
{
	// 동시성 런타임과 함께 COM 라이브러리 사용
	CoInitialize(NULL);

	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	
	// 서버 충돌 되었을 때 처리할 수 있게 하는 MiniDump
	if (!MiniDump::Begin())
		return 0;

	// 서버 시작
	IOCPServer iocpServer;
	if (iocpServer.Begin())
	{
		_tprintf(_T("=============== Start Login Server ===============\n"));
		getchar();
		iocpServer.End();
	}

	// 종료시 언로드 및 MiniDump 끝냄.
	WSACleanup();
	MiniDump::End();
	CoUninitialize();

    return 0;
}

