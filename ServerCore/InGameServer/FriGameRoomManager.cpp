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

			mRoomVector.push_back(room);
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
	ThreadSync sync;

	for (DWORD i = 0; i<mRoomVector.size(); i++)
	{
		FriendshipGameRoom *room = mRoomVector[i];

		room->End();

		delete room;
	}

	mRoomVector.clear();

	return TRUE;
}

FriendshipGameRoom* FriGameRoomManager::QuickJoin(UserInfo *userInfo, USHORT &slotNumber)
{
	ThreadSync sync;

	if (!userInfo)
		return NULL;

	for (DWORD i = 0; i<mRoomVector.size(); i++)
	{
		FriendshipGameRoom *room = mRoomVector[i];

		if (!room->GetIsFull() && !room->GetIsEmpty() && !room->GetIsReadyComplete())
		{
			// 사람이 있고 꽉차지 않은방으로~ 시작도 하지 않았고..
			if (!room->JoinUser(userInfo, slotNumber))
				continue;
			else
				return room;
		}
	}

	// 사람있는 방이 없거나,,, 모두 꽉 찾을 경우다.
	// 우선 빈방을 찾는다.
	for (DWORD i = 0; i<mRoomVector.size(); i++)
	{
		FriendshipGameRoom *room = mRoomVector[i];

		if (room->GetIsEmpty())
		{
			if (!room->JoinUser(userInfo, slotNumber))
				continue;
			else
				return room;
		}
	}

	// 어쩔수 없다... 꽉 찼다
	return NULL;
}

BOOL FriGameRoomManager::CheckGameTime(InGameIocp *iocp)
{
	ThreadSync Sync;

	BYTE	WriteBuffer[MAX_BUFFER_LENGTH] = { 0, };

	for (DWORD i = 0; i<mRoomVector.size(); i++)
	{
		FriendshipGameRoom *room = mRoomVector[i];

		// 방의 게임을 시작했을 경우
		if (room->GetIsGameStarted())
		{
			// 게임 종료
			if (!room->DecreaseGameTime())
			{
				room->EndGame(iocp);

				//room->WriteAll(PT_GAME_END_ALL, WriteBuffer, WRITE_PT_GAME_END_ALL(WriteBuffer));

				Log::WriteLog(_T("# WriteAll packet : PT_GAME_END_M\n"));
			}
			else
			{
			}	// TODO:
		}
	}

	return TRUE;
}