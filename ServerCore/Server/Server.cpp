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
	if (!CMiniDump::Begin())
		return 0;

	// ���� ����
	CServerIocp serverIocp;
	if (serverIocp.Begin())
	{
		_tprintf(_T("=============== Start Iocp Server ===============\n"));
		getchar();
		serverIocp.End();
	}

	// ����� ��ε� �� MiniDump ����.
	WSACleanup();
	CMiniDump::End();
	CoUninitialize();

    return 0;
}

