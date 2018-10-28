#include "stdafx.h"
#include "FriendshipGameRoom.h"


FriendshipGameRoom::FriendshipGameRoom()
{
	mRoomNumber = 0;
	mCurrentUserNum = 0;
	mRoomManager = NULL;
	mIsReadyComplete = FALSE;
	mIsGameStarted = FALSE;

	ZeroMemory(mUsers, sizeof(mUsers));
}


FriendshipGameRoom::~FriendshipGameRoom()
{
}


BOOL FriendshipGameRoom::Begin(DWORD roomNumber)
{
	ThreadSync sync;

	mRoomNumber = roomNumber;
	mCurrentUserNum = 0;
	mRoomManager = NULL;
	mIsReadyComplete = FALSE;
	mIsGameStarted = FALSE;

	ZeroMemory(mUsers, sizeof(mUsers));

	return TRUE;
}

BOOL FriendshipGameRoom::End(VOID)
{
	ThreadSync sync;

	mRoomNumber = 0;
	mCurrentUserNum = 0;
	mRoomManager = NULL;
	mIsReadyComplete = FALSE;
	mIsGameStarted = FALSE;

	ZeroMemory(mUsers, sizeof(mUsers));

	return TRUE;
}

BOOL FriendshipGameRoom::JoinUser(UserInfo *userInfo, USHORT &slotNumber)
{
	ThreadSync Sync;

	if (!userInfo)
		return FALSE;

	USHORT	whiteTeamCount = 0;
	USHORT	blackTeamCount = 0;

	for (USHORT i = 0; i<2; i++)
	{
		if (mUsers[i] && i < 1)
			whiteTeamCount++;
		else if (mUsers[i] && i >= 1)
			blackTeamCount++;
	}

	if (whiteTeamCount + blackTeamCount == 2)
		return FALSE;

	USHORT StartPos = whiteTeamCount <= blackTeamCount ? 0 : 1;

	for (USHORT i = StartPos; i<2; i++)
	{
		if (mUsers[i] == NULL)
		{
			mUsers[i] = userInfo;
			userInfo->SetEnteredFriendshipRoom(this);

			mCurrentUserNum = min(SHORT(mCurrentUserNum++), 2);

			slotNumber = i;

			if (mCurrentUserNum == 1)
			{
				mRoomManager = userInfo;
			}

			return TRUE;
		}
	}

	return FALSE;
}

BOOL FriendshipGameRoom::LeaveUser(BOOL isDisconnected, InGameIocp *iocp, UserInfo *userInfo)
{
	ThreadSync Sync;

	BYTE writeBuffer[MAX_BUFFER_LENGTH] = { 0, };

	if (!userInfo)
		return FALSE;

	if (mUsers[0] == userInfo)
	{
		mUsers[0] = NULL;
		userInfo->SetEnteredFriendshipRoom(NULL);
		mCurrentUserNum -= 1;
	}

	if (mUsers[1] == userInfo)
	{
		mUsers[1] = NULL;
		userInfo->SetEnteredFriendshipRoom(NULL);
		mCurrentUserNum -= 1;
	}

	for (USHORT i = 0; i < 2; i++)
	{
		if (!isDisconnected)
		{
			userInfo->WritePacket(PT_ROOM_LEAVE_SUCC, writeBuffer, WRITE_PT_ROOM_LEAVE_SUCC(writeBuffer));
		}

		if (mIsGameStarted)
		{
			EndGame(iocp);

			WriteAll(PT_GAME_END_ALL, writeBuffer, WRITE_PT_GAME_END_ALL(writeBuffer));
		}

		return TRUE;
	}

	return FALSE;
}

BOOL FriendshipGameRoom::WriteAll(DWORD protocol, BYTE *packet, DWORD packetLength)
{
	ThreadSync sync;

	if (protocol <= 0 || !packet)
		return FALSE;

	mUsers[0]->WritePacket(protocol, packet, packetLength);
	mUsers[1]->WritePacket(protocol, packet, packetLength);

	return TRUE;
}

BOOL FriendshipGameRoom::WriteOpponent(UserInfo *userInfo, DWORD protocol, BYTE *packet, DWORD packetLength)
{
	ThreadSync sync;

	if (!userInfo)
		return FALSE;

	if (protocol <= 0 || !packet)
		return FALSE;

	if (userInfo->GetEnteredFriendshipRoom()->mUsers[0] == userInfo)
		mUsers[1]->WritePacket(protocol, packet, packetLength);

	if (userInfo->GetEnteredFriendshipRoom()->mUsers[1] == userInfo)
		mUsers[0]->WritePacket(protocol, packet, packetLength);

	return TRUE;
}


DWORD FriendshipGameRoom::ReadyComplete(VOID)
{
	ThreadSync sync;

	mIsReadyComplete = TRUE;

	return 0;
}

BOOL FriendshipGameRoom::StartGame(VOID)
{
	ThreadSync Sync;

	mIsGameStarted = TRUE;

	return TRUE;
}

BOOL FriendshipGameRoom::EndGame(InGameIocp *iocp)
{
	ThreadSync sync;

	mIsReadyComplete = FALSE;
	mIsGameStarted = FALSE;

	return TRUE;
}