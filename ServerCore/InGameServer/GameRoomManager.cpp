#include "stdafx.h"
#include "GameRoomManager.h"


GameRoomManager::GameRoomManager()
{
}


GameRoomManager::~GameRoomManager()
{
}

BOOL GameRoomManager::Begin(DWORD maxRoomNum)
{
	if (maxRoomNum <= 0)
		return FALSE;

	mMaxRoomNum = maxRoomNum;

	for (DWORD i = 0; i<mMaxRoomNum; i++)
	{
		GameRoom *room = new GameRoom();

		if (room->Begin(i))

			mRoomVector.push_back(room);
		else
		{
			End();

			return FALSE;
		}
	}

	return TRUE;
}

BOOL GameRoomManager::End(VOID)
{
	ThreadSync sync;

	for (DWORD i = 0; i<mRoomVector.size(); i++)
	{
		GameRoom *room = mRoomVector[i];

		room->End();

		delete room;
	}

	mRoomVector.clear();

	return TRUE;
}

GameRoom* GameRoomManager::QuickJoin(UserInfo *userInfo, USHORT &slotNumber)
{
	ThreadSync sync;

	if (!userInfo)
		return NULL;

	for (DWORD i = 0; i<mRoomVector.size(); i++)
	{
		GameRoom *room = mRoomVector[i];

		if (!room->GetIsFull() && !room->GetIsEmpty() && !room->GetIsGameStarting())
		{
			if (!room->JoinUser(userInfo, slotNumber))
				continue;
			else
				return room;
		}
	}

	for (DWORD i = 0; i<mRoomVector.size(); i++)
	{
		GameRoom *room = mRoomVector[i];

		if (room->GetIsEmpty())
		{
			if (!room->JoinUser(userInfo, slotNumber))
				continue;
			else
				return room;
		}
	}

	return NULL;
}