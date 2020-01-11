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
			mRoomMap.insert(pair<int, GameRoom*>((int)i, room));

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
	for (DWORD i = 0; i<mRoomMap.size(); i++)
	{
		GameRoom *room = mRoomMap[i];

		room->End();

		delete room;
	}

	mRoomMap.clear();

	return TRUE;
}

GameRoom* GameRoomManager::QuickJoin(UserInfo *userInfo, USHORT &slotNumber)
{
	if (!userInfo)
		return NULL;

	for (DWORD i = 0; i<mRoomMap.size(); i++)
	{
		GameRoom *room = mRoomMap[i];

		if (!room->GetIsFull() && !room->GetIsEmpty() && !room->GetIsGameStarting())
		{
			if (!room->JoinUser(userInfo, slotNumber))
				continue;
			else
				return room;
		}
	}

	for (DWORD i = 0; i<mRoomMap.size(); i++)
	{
		GameRoom *room = mRoomMap[i];

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