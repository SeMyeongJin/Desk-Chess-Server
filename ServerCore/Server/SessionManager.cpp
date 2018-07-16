#include "stdafx.h"
#include "SessionManager.h"


SessionManager::SessionManager()
{
}


SessionManager::~SessionManager()
{
}

BOOL SessionManager::Begin(SOCKET listenSock)
{
	ThreadSync sync;

	if (!listenSock) return FALSE;

	for (DWORD i = 0; i < MAX_USER; i++)
	{
		ConnectedSession *pConnectedSession = new ConnectedSession();
		mConnectedSessions.push_back(pConnectedSession);
	}

	for (DWORD i = 0; i < MAX_USER; i++)
	{
		if (!mConnectedSessions[i]->Begin())
		{
			SessionManager::End();
			return FALSE;
		}

		if (!mConnectedSessions[i]->Accept(listenSock))
		{
			SessionManager::End();
			return FALSE;
		}
	}

	return TRUE;
}

VOID SessionManager::End(VOID)
{
	ThreadSync sync;

	for (DWORD i = 0; i < mConnectedSessions.size(); i++)
	{
		mConnectedSessions[i]->End();

		delete mConnectedSessions[i];
	}

	mConnectedSessions.clear();
}

VOID SessionManager::WriteAll(DWORD dwProtocol, BYTE * data, DWORD dwLength)
{
	ThreadSync sync;

	for (DWORD i = 0; i < mConnectedSessions.size(); i++)
	{
		if (mConnectedSessions[i]->GetConnected())
			mConnectedSessions[i]->WritePacket(dwProtocol, data, dwLength);
	}
}