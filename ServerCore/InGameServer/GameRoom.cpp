#include "stdafx.h"
#include "GameRoom.h"


GameRoom::GameRoom()
{
	mRoomNumber					= 0;
	mCurrentUserNum				= 0;
	mRoomManager				= NULL;
	mIsGameStarting				= FALSE;

	ZeroMemory(mUsers, sizeof(mUsers));
}


GameRoom::~GameRoom()
{
}


BOOL GameRoom::Begin(DWORD roomNumber)
{
	mRoomNumber					= roomNumber;
	mCurrentUserNum				= 0;
	mRoomManager				= NULL;
	mIsGameStarting				= FALSE;

	ZeroMemory(mUsers, sizeof(mUsers));

	return TRUE;
}

BOOL GameRoom::End(VOID)
{
	mRoomNumber					= 0;
	mCurrentUserNum				= 0;
	mRoomManager				= NULL;
	mIsGameStarting				= FALSE;

	ZeroMemory(mUsers, sizeof(mUsers));

	return TRUE;
}

BOOL GameRoom::JoinUser(UserInfo *userInfo, USHORT &slotNumber)
{
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
			userInfo->SetEnteredRoom(this);

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

BOOL GameRoom::LeaveUser(BOOL isDisconnected, InGameIocp *iocp, UserInfo *userInfo)
{
	BYTE writeBuffer[MAX_BUFFER_LENGTH] = { 0, };

	if (!userInfo)
		return FALSE;

	if (mUsers[0] == userInfo)
	{
		mUsers[0] = NULL;
		userInfo->SetEnteredRoom(NULL);
		mCurrentUserNum -= 1;
	}

	if (mUsers[1] == userInfo)
	{
		mUsers[1] = NULL;
		userInfo->SetEnteredRoom(NULL);
		mCurrentUserNum -= 1;
	}

	if (mUsers[0] == NULL && mUsers[1] == NULL && GetIsGameStarting())
	{
		SetIsGameStarting(FALSE);
	}

	for (USHORT i = 0; i < 2; i++)
	{
		if (!isDisconnected)
		{
			userInfo->WritePacket(PT_ROOM_LEAVE_SUCC, writeBuffer, WRITE_PT_ROOM_LEAVE_SUCC(writeBuffer));
		}
		return TRUE;
	}

	return FALSE;
}

BOOL GameRoom::WriteAll(DWORD protocol, BYTE *packet, DWORD packetLength)
{
	if (protocol <= 0 || !packet)
		return FALSE;

	mUsers[0]->WritePacket(protocol, packet, packetLength);
	mUsers[1]->WritePacket(protocol, packet, packetLength);

	return TRUE;
}

BOOL GameRoom::WriteOpponent(UserInfo *userInfo, DWORD protocol, BYTE *packet, DWORD packetLength)
{
	if (!userInfo)
		return FALSE;

	if (protocol <= 0 || !packet)
		return FALSE;

	if (userInfo->GetEnteredRoom()->mUsers[0] == userInfo)
		mUsers[1]->WritePacket(protocol, packet, packetLength);

	if (userInfo->GetEnteredRoom()->mUsers[1] == userInfo)
		mUsers[0]->WritePacket(protocol, packet, packetLength);

	return TRUE;
}