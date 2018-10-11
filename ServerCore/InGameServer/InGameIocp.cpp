#include "stdafx.h"
#include "InGameIocp.h"

extern GameDBManager GgameDBManager;

InGameIocp::InGameIocp()
{
}


InGameIocp::~InGameIocp()
{
}


DWORD WINAPI GameThreadCallback(LPVOID parameter)
{
	InGameIocp *owner = (InGameIocp*)parameter;
	owner->GameThreadCallback();

	return 0;
}

BOOL InGameIocp::Begin(VOID)
{
	if (!IOCP::Begin())
	{
		End();

		return FALSE;
	}

	mListenSession = new NetworkWrapping();
	if (!mListenSession->Begin())
	{
		End();

		return FALSE;
	}

	if (!mListenSession->TcpBind())
	{
		End();

		return FALSE;
	}

	if (!mListenSession->Listen(SERVER_PORT, MAX_USER))
	{
		End();

		return FALSE;
	}

	if (!IOCP::RegisterSocketToIocp(mListenSession->GetSocket(), (ULONG_PTR)&mListenSession))
	{
		End();

		return FALSE;
	}

	if (!mUserInfoManager.Begin(MAX_USER, mListenSession->GetSocket()))
	{
		End();

		return FALSE;
	}

	if (!mRoomManager.Begin(MAX_USER))
	{
		End();

		return FALSE;
	}

	if (!mUserInfoManager.AcceptALL())
	{
		End();

		return FALSE;
	}

	mGameThreadDestroyEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (!mGameThreadDestroyEvent)
	{
		End();

		return FALSE;
	}

	mGameThreadHandle = CreateThread(NULL, 0, ::GameThreadCallback, this, 0, NULL);
	if (!mGameThreadHandle)
	{
		End();

		return FALSE;
	}

	//GgameDBManager.Begin();

	return TRUE;
}

BOOL InGameIocp::End(VOID)
{
	if (mGameThreadDestroyEvent && mGameThreadHandle)
	{
		SetEvent(mGameThreadDestroyEvent);

		WaitForSingleObject(mGameThreadHandle, INFINITE);

		CloseHandle(mGameThreadDestroyEvent);
		CloseHandle(mGameThreadHandle);
	}

	IOCP::End();

	mRoomManager.End();

	mUserInfoManager.End();

	mListenSession->End();

	return TRUE;
}

VOID InGameIocp::GameThreadCallback(VOID)
{
	while (TRUE)
	{
		DWORD result = WaitForSingleObject(mGameThreadDestroyEvent, 1000);

		if (result == WAIT_OBJECT_0)
			return;

		mRoomManager.CheckGameTime(this);
	}
}

VOID InGameIocp::OnIoConnected(VOID * object)
{
	UserInfo *userInfo = (UserInfo*)object;

	printf("Connected User...\n");
	if (!IOCP::RegisterSocketToIocp(userInfo->GetSocket(), (ULONG_PTR)userInfo))
	{
		End();

		return;
	}

	userInfo->SetIsConnected(TRUE);

	if (!userInfo->InitializeReadForIocp())
	{
		if (!userInfo->Reload(mListenSession->GetSocket()))
		{
			End();

			return;
		}
	}

	mUserInfoManager.IncreaseUserInfoNum();
}

VOID InGameIocp::OnIoDisconnected(VOID *object)
{
	UserInfo *userInfo = (UserInfo*)object;

	printf("Disconnected Session...\n");
	if (!userInfo->Reload(mListenSession->GetSocket()))
	{
		End();

		return;
	}

	mUserInfoManager.DecreaseUserInfoNum();
}

VOID InGameIocp::OnIoRead(VOID *object, DWORD dataLength)
{
	UserInfo *userInfo = (UserInfo*)object;

	DWORD protocol = 0;
	BYTE packet[MAX_BUFFER_LENGTH] = { 0, };
	DWORD packetLength = 0;

	BYTE writeBuffer[MAX_BUFFER_LENGTH] = { 0, };

	if (userInfo->ReadPacketForIocp(dataLength))
	{
		while (userInfo->GetPacket(protocol, packet, packetLength))
		{
			if (protocol == PT_OFFICIAL_GAME_START) PROC_PT_OFFICIAL_GAME_START(userInfo, packet);
			if (protocol == PT_FRIENDSHIP_GAME_START) PROC_PT_FRIENDSHIP_GAME_START(userInfo, packet);
			if (protocol == PT_CHAT) PROC_PT_CHAT(userInfo, packet);
			if (protocol == PT_PIECE_MOVE) PROC_PT_PIECE_MOVE(userInfo, packet);
		}
	}

	if (!userInfo->InitializeReadForIocp())
		userInfo->End();
}

VOID InGameIocp::OnIoWrote(VOID *object, DWORD dataLength)
{
	UserInfo *userInfo = (UserInfo*)object;

	userInfo->WriteComplete();
}