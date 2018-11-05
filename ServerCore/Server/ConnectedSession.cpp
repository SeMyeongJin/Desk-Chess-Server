#include "stdafx.h"
#include "ConnectedSession.h"


ConnectedSession::ConnectedSession()
{
	mConnected = FALSE;
	ZeroMemory(mUserID, sizeof(mUserID));
}


ConnectedSession::~ConnectedSession()
{
}

BOOL ConnectedSession::Restart(SOCKET listenSock)
{
	ThreadSync sync;

	End();

	return Begin() && Accept(listenSock);
}

VOID ConnectedSession::SetConnected(BOOL bConnected)
{
	ThreadSync sync;

	mConnected = bConnected;
}

BOOL ConnectedSession::GetConnected(VOID)
{
	ThreadSync sync;

	return mConnected;
}