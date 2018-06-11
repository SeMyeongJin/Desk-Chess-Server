#include "stdafx.h"
#include "SessionManager.h"


CSessionManager::CSessionManager()
{
}


CSessionManager::~CSessionManager()
{
}

BOOL CSessionManager::Begin(SOCKET listenSock)
{
	CThreadSync sync;

	if (!listenSock) return FALSE;

	for (DWORD i = 0; i < MAX_USER; i++)
	{
		CConnectedSession * CSession = new CConnectedSession();
		mConnectedSessions.push_back(CSession);
	}

	for (DWORD i = 0; i < MAX_USER; i++)
	{
		if (!mConnectedSessions[i]->Begin())
		{
			CSessionManager::End();
			return FALSE;
		}

		if (!mConnectedSessions[i]->Accept(listenSock))
		{
			CSessionManager::End();
			return FALSE;
		}
	}

	return TRUE;
}

VOID CSessionManager::End(VOID)
{
	CThreadSync sync;

	for (DWORD i = 0; i < mConnectedSessions.size(); i++)
	{
		mConnectedSessions[i]->End();

		delete mConnectedSessions[i];
	}

	mConnectedSessions.clear();
}