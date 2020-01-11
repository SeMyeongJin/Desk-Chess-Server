#include "stdafx.h"
#include "FriGameRoomManager.h"


FriGameRoomManager::FriGameRoomManager()
{
}


FriGameRoomManager::~FriGameRoomManager()
{
}


BOOL FriGameRoomManager::Begin(DWORD maxRoomNum)
{
	if (maxRoomNum <= 0)
		return FALSE;

	mMaxRoomNum = maxRoomNum;

	for (DWORD i = 0; i<mMaxRoomNum; i++)
	{
		FriendshipGameRoom *room = new FriendshipGameRoom();

		if (room->Begin(i))
			mRoomMap.insert(pair<int, FriendshipGameRoom*>((int)i, room));

		else
		{
			End();

			return FALSE;
		}
	}

	return TRUE;
}

BOOL FriGameRoomManager::End(VOID)
{
	for (DWORD i = 0; i<mRoomMap.size(); i++)
	{
		FriendshipGameRoom *room = mRoomMap[i];

		room->End();

		delete room;
	}

	mRoomMap.clear();

	return TRUE;
}

FriendshipGameRoom* FriGameRoomManager::QuickJoin(UserInfo *userInfo, USHORT &slotNumber)
{
	if (!userInfo)
		return NULL;

	for (DWORD i = 0; i<mRoomMap.size(); i++)
	{
		FriendshipGameRoom *room = mRoomMap[i];

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
		FriendshipGameRoom *room = mRoomMap[i];

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