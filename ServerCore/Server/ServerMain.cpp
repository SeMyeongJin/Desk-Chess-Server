// Server.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


int _tmain(int argc, _TCHAR* argv[])
{
	// ���ü� ��Ÿ�Ӱ� �Բ� COM ���̺귯�� ���
	CoInitialize(NULL);

	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	
	// ���� �浹 �Ǿ��� �� ó���� �� �ְ� �ϴ� MiniDump
	if (!MiniDump::Begin())
		return 0;

	// ���� ����
	IOCPServer iocpServer;
	if (iocpServer.Begin())
	{
		_tprintf(_T("=============== Start Login Server ===============\n"));
		getchar();
		iocpServer.End();
	}

	// ����� ��ε� �� MiniDump ����.
	WSACleanup();
	MiniDump::End();
	CoUninitialize();

    return 0;
}

