#include "stdafx.h"
#include "ConnectedSession.h"


CConnectedSession::CConnectedSession()
{
	mConnected = FALSE;
}


CConnectedSession::~CConnectedSession()
{
}

BOOL CConnectedSession::Restart(SOCKET listenSock)
{
	CThreadSync sync;

	End();

	return Begin() && Accept(listenSock);
}

VOID CConnectedSession::SetConnected(BOOL bConnected)
{
	CThreadSync sync;

	mConnected = bConnected;
}

BOOL CConnectedSession::GetConnected(VOID)
{
	CThreadSync sync;

	return mConnected;
}