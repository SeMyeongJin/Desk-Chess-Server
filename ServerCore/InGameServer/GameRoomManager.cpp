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

		if (!room->GetIsFull() && !room->GetIsEmpty() && !room->GetIsReadyComplete())
		{
			// ����� �ְ� ������ ����������~ ���۵� ���� �ʾҰ�..
			if (!room->JoinUser(userInfo, slotNumber))
				continue;
			else
				return room;
		}
	}

	// ����ִ� ���� ���ų�,,, ��� �� ã�� ����.
	// �켱 ����� ã�´�.
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

	// ��¿�� ����... �� á��
	return NULL;
}

BOOL GameRoomManager::CheckGameTime(InGameIocp *iocp)
{
	ThreadSync Sync;

	BYTE	WriteBuffer[MAX_BUFFER_LENGTH] = { 0, };

	for (DWORD i = 0; i<mRoomVector.size(); i++)
	{
		GameRoom *room = mRoomVector[i];

		// ���� ������ �������� ���
		if (room->GetIsGameStarted())
		{
			// ���� ����
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