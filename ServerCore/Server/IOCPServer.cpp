#include "stdafx.h"
#include "IOCPServer.h"


IOCPServer::IOCPServer(VOID)
{
}

IOCPServer::~IOCPServer(VOID)
{
}

DWORD WINAPI KeepThreadCallback(LPVOID pParam)
{
	IOCPServer * owner = (IOCPServer*)pParam;

	owner->KeepThreadCallback();

	return 0;
}

VOID IOCPServer::KeepThreadCallback(VOID)
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
VOID IOCPServer::OnIoConnected(VOID * object)
{
	ConnectedSession *pConnectedSession = reinterpret_cast<ConnectedSession*>(object);
	
	if (!IOCP::RegisterSocketToIocp(pConnectedSession->GetSocket(), reinterpret_cast<ULONG_PTR>(pConnectedSession)))
		return;

	if (!pConnectedSession->InitializeReadForIocp())
	{
		pConnectedSession->Restart(mListen->GetSocket());

		return;
	}

	pConnectedSession->SetConnected(TRUE);
}

VOID IOCPServer::OnIoDisconnected(VOID * object)
{
	ConnectedSession *pConnectedSession = reinterpret_cast<ConnectedSession*>(object);
	
	pConnectedSession->Restart(mListen->GetSocket());

	pConnectedSession->SetConnected(FALSE);
}

VOID IOCPServer::OnIoRead(VOID * object, DWORD dwDataLength)
{
	ConnectedSession *pConnectedSession = reinterpret_cast<ConnectedSession*>(object);
	
	DWORD dwProtocol = 0, dwPacketLength = 0;
	BYTE packet[MAX_BUFFER_LENGTH] = { 0, };
	
	if (pConnectedSession->ReadPacketForIocp(dwDataLength))
	{
		while (pConnectedSession->GetPacket(dwProtocol, packet, dwPacketLength))
		{
			switch (dwProtocol)
			{
			case PT_SIGNUP:
				PROC_PT_SIGNUP(pConnectedSession, dwProtocol, packet, dwPacketLength);
				break;
			case PT_LOGIN:
				PROC_PT_LOGIN(pConnectedSession, dwProtocol, packet, dwPacketLength);
				break;
			}
		}
	}
	if (!pConnectedSession->InitializeReadForIocp())
		pConnectedSession->Restart(mListen->GetSocket());
}

VOID IOCPServer::OnIoWrote(VOID * object, DWORD dwDataLength)
{
}

BOOL IOCPServer::Begin(VOID)
{
	if (!IOCP::Begin()) return FALSE;
	_tprintf(_T("IO Completion Port handle creation completed\n"));
	
	mListen = new NetworkWrapping();
	if (!mListen->Begin())
	{
		IOCPServer::End();

		return FALSE;
	}
	_tprintf(_T("Initialize NetworkSession\n"));

	if (!mListen->TcpBind())
	{
		IOCPServer::End();

		return FALSE;
	}
	_tprintf(_T("TCP Socket creation completed\n"));

	if (!mListen->Listen(SERVER_PORT, MAX_USER))
	{
		IOCPServer::End();
		
		return FALSE;
	}
	_tprintf(_T("Bind, Listen function completed\n"));

	if (!IOCP::RegisterSocketToIocp(mListen->GetSocket(), reinterpret_cast<ULONG_PTR>(mListen)))
	{
		IOCPServer::End();

		return FALSE;
	}
	_tprintf(_T("Socket registration completed\n"));
	
	if (!mSessionManager.Begin(mListen->GetSocket()))
	{
		IOCPServer::End();

		return FALSE;
	}
	_tprintf(_T("Initialize SessionManager\n\n"));

	mKeepThreadDestroyEvent = CreateEvent(0, FALSE, FALSE, 0);
	if (!mKeepThreadDestroyEvent)
	{
		IOCPServer::End();
		return FALSE;
	}

	mKeepThread = CreateThread(NULL, 0, ::KeepThreadCallback, this, 0, NULL);
	if (!mKeepThread)
	{
		IOCPServer::End();
		return FALSE;
	}
	
	GDBManager->Begin();

	return TRUE;
}

VOID IOCPServer::End(VOID)
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

	IOCP::End();

	mSessionManager.End();

	if (mListen)
	{
		mListen->End();

		delete mListen;
	}
}