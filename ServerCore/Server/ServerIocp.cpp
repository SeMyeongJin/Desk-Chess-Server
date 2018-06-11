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

	return TRUE;
}

VOID CServerIocp::End(VOID)
{
	CIocp::End();

	if (mListen)
	{
		mListen->End();

		delete mListen;
	}
}

// CIocp의 가상 함수
VOID CServerIocp::OnIoConnected(VOID * object)
{
}

VOID CServerIocp::OnIoDisconnected(VOID * object)
{
}

VOID CServerIocp::OnIoRead(VOID * object, DWORD dwDataLength)
{
}

VOID CServerIocp::OnIoWrote(VOID * object, DWORD dwDataLength)
{
}