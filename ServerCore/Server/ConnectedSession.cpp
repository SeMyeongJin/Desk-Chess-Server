#include "stdafx.h"
#include "ConnectedSession.h"


CConnectedSession::CConnectedSession()
{
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