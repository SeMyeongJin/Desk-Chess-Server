#include "stdafx.h"
#include "ServerIocp.h"


CServerIocp::CServerIocp(VOID)
{
}

CServerIocp::~CServerIocp(VOID)
{
}

DWORD WINAPI KeepThreadCallback(LPVOID pParam)
{
	CServerIocp * owner = (CServerIocp*)pParam;

	owner->KeepThreadCallback();

	return 0;
}

VOID CServerIocp::KeepThreadCallback(VOID)
{
	DWORD dwKeepAlive = 0xFFFF;

	while (TRUE)
	{
		DWORD dwResult = WaitForSingleObject(mKeepThreadDestroyEvent, 30000);

		if (dwResult == WAIT_OBJECT_0) return;

		mSessionManager.WriteAll(0x3000000, (BYTE*)&dwKeepAlive, sizeof(DWORD));
	}
}

// CIocp의 가상 함수
VOID CServerIocp::OnIoConnected(VOID * object)
{
	CConnectedSession * connectedSession = reinterpret_cast<CConnectedSession*>(object);

	printf("Connected Session...\n");
	if (!CIocp::RegisterSocketToIocp(connectedSession->GetSocket(), reinterpret_cast<ULONG_PTR>(connectedSession)))
		return;

	if (!connectedSession->InitializeReadForIocp())
	{
		connectedSession->Restart(mListen->GetSocket());

		return;
	}

	connectedSession->SetConnected(TRUE);
}

VOID CServerIocp::OnIoDisconnected(VOID * object)
{
	CConnectedSession * connectedSession = reinterpret_cast<CConnectedSession*>(object);

	printf("Disconnected Session...\n");
	connectedSession->Restart(mListen->GetSocket());

	connectedSession->SetConnected(FALSE);
}

VOID CServerIocp::OnIoRead(VOID * object, DWORD dwDataLength)
{
}

VOID CServerIocp::OnIoWrote(VOID * object, DWORD dwDataLength)
{
}

BOOL CServerIocp::Begin(VOID)
{
	if (!CIocp::Begin()) return FALSE;

	mListen = new CNetworkSession();

	if (!mListen->Begin())
	{
		CServerIocp::End();

		return FALSE;
	}
	printf("Network Initialize\n");

	if (!mListen->TcpBind())
	{
		CServerIocp::End();

		return FALSE;
	}
	printf("Bind Complete\n");

	if (!mListen->Listen(SERVER_PORT, MAX_USER))
	{
		CServerIocp::End();
		
		return FALSE;
	}
	printf("Listen Complete\n");

	if (!CIocp::RegisterSocketToIocp(mListen->GetSocket(), reinterpret_cast<ULONG_PTR>(mListen)))
	{
		CServerIocp::End();

		return FALSE;
	}
	printf("Socket connects Iocp\n");
	
	if (!mSessionManager.Begin(mListen->GetSocket()))
	{
		CServerIocp::End();

		return FALSE;
	}
	printf("SessionManager initialize\n");

	mKeepThreadDestroyEvent = CreateEvent(0, FALSE, FALSE, 0);
	if (!mKeepThreadDestroyEvent)
	{
		CServerIocp::End();
		return FALSE;
	}

	mKeepThread = CreateThread(NULL, 0, ::KeepThreadCallback, this, 0, NULL);
	if (!mKeepThread)
	{
		CServerIocp::End();
		return FALSE;
	}

	return TRUE;
}

VOID CServerIocp::End(VOID)
{
	if (mKeepThread)
	{
		SetEvent(mKeepThreadDestroyEvent);

		WaitForSingleObject(mKeepThread, INFINITE);

		CloseHandle(mKeepThread);
		mKeepThread = NULL;
	}

	if (mKeepThreadDestroyEvent)
	{
		CloseHandle(mKeepThreadDestroyEvent);
		mKeepThreadDestroyEvent = NULL;
	}

	CIocp::End();

	mSessionManager.End();

	if (mListen)
	{
		mListen->End();

		delete mListen;
	}
}