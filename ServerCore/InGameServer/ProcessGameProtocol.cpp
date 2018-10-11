#include "stdafx.h"
#include "InGameIocp.h"

VOID InGameIocp::PROC_PT_OFFICIAL_GAME_START(UserInfo * userInfo, BYTE * pPacket)
{
	BYTE writeBuffer[MAX_BUFFER_LENGTH] = { 0, };

	userInfo->SetStatus(US_GAME_STARTING);

	printf("PROC_PT_OFFICIAL_GAME_START\n");

	S_PT_OFFICIAL_GAME_START OGData;
	READ_PT_OFFICIAL_GAME_START(pPacket, OGData);

	USHORT slotNumber = 0;
	GameRoom *room = mRoomManager.QuickJoin(userInfo, slotNumber);
	if (room)
	{
		if (room->GetIsFull())
		{
			BOOL whiteTeam = true;
			BOOL blackTeam = false;

			userInfo->WritePacket(PT_GAME_START_ALL, writeBuffer, WRITE_PT_GAME_START_ALL(writeBuffer, whiteTeam));
			userInfo->GetEnteredRoom()->WriteOpponent(userInfo, PT_GAME_START_ALL, writeBuffer, WRITE_PT_GAME_START_ALL(writeBuffer, blackTeam));
		}
		else
			userInfo->WritePacket(PT_OFFICIAL_GAME_START_SUCC, writeBuffer, WRITE_PT_OFFICIAL_GAME_START_SUCC(writeBuffer));
	}
	else
	{
		userInfo->WritePacket(PT_OFFICIAL_GAME_START_FAIL, writeBuffer, WRITE_PT_OFFICIAL_GAME_START_FAIL(writeBuffer, EC_CANT_ALLOCATE_ROOM));
	}
}

VOID InGameIocp::PROC_PT_FRIENDSHIP_GAME_START(UserInfo * userInfo, BYTE * pPacket)
{
	BYTE writeBuffer[MAX_BUFFER_LENGTH] = { 0, };

	userInfo->SetStatus(US_GAME_STARTING);

	printf("PROC_PT_FRIENDSHIP_GAME_START\n");

	S_PT_FRIENDSHIP_GAME_START FGData;
	READ_PT_FRIENDSHIP_GAME_START(pPacket, FGData);

	USHORT slotNumber = 0;
	GameRoom *room = mRoomManager.QuickJoin(userInfo, slotNumber);
	if (room)
	{
		if (room->GetIsFull())
		{
			BOOL whiteTeam = true;
			BOOL blackTeam = false;

			userInfo->WritePacket(PT_GAME_START_ALL, writeBuffer, WRITE_PT_GAME_START_ALL(writeBuffer, whiteTeam));
			userInfo->GetEnteredRoom()->WriteOpponent(userInfo, PT_GAME_START_ALL, writeBuffer, WRITE_PT_GAME_START_ALL(writeBuffer, blackTeam));
		}
		else
			userInfo->WritePacket(PT_FRIENDSHIP_GAME_START_SUCC, writeBuffer, WRITE_PT_OFFICIAL_GAME_START_SUCC(writeBuffer));
	}
	else
	{
		userInfo->WritePacket(PT_FRIENDSHIP_GAME_START_FAIL, writeBuffer, WRITE_PT_OFFICIAL_GAME_START_FAIL(writeBuffer, EC_CANT_ALLOCATE_ROOM));
	}
}

VOID InGameIocp::PROC_PT_ROOM_LEAVE(UserInfo * userInfo, BYTE * pPacket)
{
	BYTE writeBuffer[MAX_BUFFER_LENGTH] = { 0, };
	S_PT_ROOM_LEAVE leaveData;
	READ_PT_ROOM_LEAVE(pPacket, leaveData);

	GameRoom *room = userInfo->GetEnteredRoom();
	if (room)
	{
		room->LeaveUser(FALSE, this, userInfo);

		userInfo->WritePacket(PT_ROOM_LEAVE_SUCC, writeBuffer, WRITE_PT_ROOM_LEAVE_SUCC(writeBuffer));

		userInfo->SetStatus(US_LOBBY_ENTERED);
	}
	else
	{
		userInfo->WritePacket(PT_ROOM_LEAVE_FAIL, writeBuffer, WRITE_PT_ROOM_LEAVE_FAIL(writeBuffer, EC_CANT_COMPLETE_LEAVE_ROOM));
	}

}

VOID InGameIocp::PROC_PT_CHAT(UserInfo * userInfo, BYTE * pPacket)
{
	BYTE writeBuffer[MAX_BUFFER_LENGTH] = { 0, };
	S_PT_CHAT chatData;
	READ_PT_CHAT(pPacket, chatData);
	

	mUserInfoManager.WriteAll(PT_CHAT, writeBuffer, WRITE_PT_CHAT(writeBuffer, chatData.user_name, chatData.message));
	if (wcslen(chatData.message) > 0)
		_tprintf(_T("%s :[chat message] %s\n"), chatData.user_name, chatData.message);
}

VOID InGameIocp::PROC_PT_PIECE_MOVE(UserInfo * userInfo, BYTE * pPacket)
{
	BYTE writeBuffer[MAX_BUFFER_LENGTH] = { 0, };
	S_PT_PIECE_MOVE moveData;
	READ_PT_PIECE_MOVE(pPacket, moveData);

	printf("%d, %d, %d, %d", moveData.beforeMoveXpos, moveData.beforeMoveYpos, moveData.afterMoveXpos, moveData.afterMoveYpos);
	userInfo->GetEnteredRoom()->WriteOpponent(userInfo, PT_PIECE_MOVE, writeBuffer, WRITE_PT_PIECE_MOVE(writeBuffer, moveData.beforeMoveXpos, moveData.beforeMoveYpos, moveData.afterMoveXpos, moveData.afterMoveYpos));
}