#include "stdafx.h"
#include "ServerIocp.h"


CServerIocp::CServerIocp(VOID)
{
}

CServerIocp::~CServerIocp(VOID)
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

	if (!mListen->TcpBind())
	{
		CServerIocp::End();

		return FALSE;
	}

	if (!mListen->Listen(SERVER_PORT, MAX_USER))
	{
		CServerIocp::End();
		
		return FALSE;
	}

	if (!CIocp::RegisterSocketToIocp(mListen->GetSocket(), reinterpret_cast<ULONG_PTR>(mListen)))
	{
		CServerIocp::End();

		return FALSE;
	}
	
	printf("SessionManager Start\n");
	if (!mSessionManager.Begin(mListen->GetSocket()))
	{
		CServerIocp::End();

		return FALSE;
	}

	return TRUE;
}

VOID CServerIocp::End(VOID)
{
	CIocp::End();

	mSessionManager.End();

	if (mListen)
	{
		mListen->End();

		delete mListen;
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
}

VOID CServerIocp::OnIoDisconnected(VOID * object)
{
	CConnectedSession * connectedSession = reinterpret_cast<CConnectedSession*>(object);

	connectedSession->Restart(mListen->GetSocket());
}

VOID CServerIocp::OnIoRead(VOID * object, DWORD dwDataLength)
{
}

VOID CServerIocp::OnIoWrote(VOID * object, DWORD dwDataLength)
{
}