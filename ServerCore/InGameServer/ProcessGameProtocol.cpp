#include "stdafx.h"
#include "InGameIocp.h"

extern GameDBManager GgameDBManager;

VOID InGameIocp::PROC_PT_REQ_USER_INFO(UserInfo * userInfo, BYTE * pPacket)
{
	BYTE writeBuffer[MAX_BUFFER_LENGTH] = { 0, };
	S_PT_REQ_USER_INFO userInfoInputData;
	READ_PT_REQ_USER_INFO(pPacket, userInfoInputData);

	S_PT_RES_USER_INFO userInfoOutputData;
	
	GgameDBManager.ReqUserInfo(userInfoInputData.userName, &userInfoOutputData.Rating, &userInfoOutputData.OFF_WIN, &userInfoOutputData.OFF_LOSE, &userInfoOutputData.FRI_WIN);

	userInfo->WritePacket(PT_RES_USER_INFO, writeBuffer, WRITE_PT_RES_USER_INFO(writeBuffer, userInfoOutputData.Rating, userInfoOutputData.OFF_WIN, userInfoOutputData.OFF_LOSE, userInfoOutputData.FRI_WIN));
}

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

			userInfo->WritePacket(PT_GAME_START_ALL, writeBuffer, WRITE_PT_GAME_START_ALL(writeBuffer, whiteTeam, true));
			userInfo->GetEnteredRoom()->WriteOpponent(userInfo, PT_GAME_START_ALL, writeBuffer, WRITE_PT_GAME_START_ALL(writeBuffer, blackTeam, true));
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
	FriendshipGameRoom *room = mFriRoomManager.QuickJoin(userInfo, slotNumber);
	if (room)
	{
		if (room->GetIsFull())
		{
			BOOL whiteTeam = true;
			BOOL blackTeam = false;

			userInfo->WritePacket(PT_GAME_START_ALL, writeBuffer, WRITE_PT_GAME_START_ALL(writeBuffer, whiteTeam, false));
			userInfo->GetEnteredFriendshipRoom()->WriteOpponent(userInfo, PT_GAME_START_ALL, writeBuffer, WRITE_PT_GAME_START_ALL(writeBuffer, blackTeam, false));
		}
		else
			userInfo->WritePacket(PT_FRIENDSHIP_GAME_START_SUCC, writeBuffer, WRITE_PT_FRIENDSHIP_GAME_START_SUCC(writeBuffer));
	}
	else
	{
		userInfo->WritePacket(PT_FRIENDSHIP_GAME_START_FAIL, writeBuffer, WRITE_PT_FRIENDSHIP_GAME_START_FAIL(writeBuffer, EC_CANT_ALLOCATE_ROOM));
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
		userInfo->SetStatus(US_LOBBY_ENTERED);
	}

	FriendshipGameRoom *friRoom = userInfo->GetEnteredFriendshipRoom();
	if (friRoom)
	{
		friRoom->LeaveUser(FALSE, this, userInfo);
		userInfo->SetStatus(US_LOBBY_ENTERED);
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

	if (userInfo->GetEnteredRoom())
	{
		userInfo->GetEnteredRoom()->WriteOpponent(userInfo, PT_PIECE_MOVE, writeBuffer, WRITE_PT_PIECE_MOVE(writeBuffer, moveData.beforeMoveXpos, moveData.beforeMoveYpos, moveData.afterMoveXpos, moveData.afterMoveYpos));
	}
	if (userInfo->GetEnteredFriendshipRoom())
	{
		userInfo->GetEnteredFriendshipRoom()->WriteOpponent(userInfo, PT_PIECE_MOVE, writeBuffer, WRITE_PT_PIECE_MOVE(writeBuffer, moveData.beforeMoveXpos, moveData.beforeMoveYpos, moveData.afterMoveXpos, moveData.afterMoveYpos));
	}
}
VOID InGameIocp::PROC_PT_PIECE_PROMOTION(UserInfo * userInfo, BYTE * pPacket)
{
	BYTE writeBuffer[MAX_BUFFER_LENGTH] = { 0, };
	S_PT_PIECE_PROMOTION promotionData;
	READ_PT_PIECE_PROMOTION(pPacket, promotionData);

	if (userInfo->GetEnteredRoom())
	{
		userInfo->GetEnteredRoom()->WriteOpponent(userInfo, PT_PIECE_PROMOTION, writeBuffer, WRITE_PT_PIECE_PROMOTION(writeBuffer, promotionData.pieceType, promotionData.xpos, promotionData.ypos));
	}
	if (userInfo->GetEnteredFriendshipRoom())
	{
		userInfo->GetEnteredFriendshipRoom()->WriteOpponent(userInfo, PT_PIECE_PROMOTION, writeBuffer, WRITE_PT_PIECE_PROMOTION(writeBuffer, promotionData.pieceType, promotionData.xpos, promotionData.ypos));
	}
}

VOID InGameIocp::PROC_PT_OFFICIAL_GAME_WIN(UserInfo * userInfo, BYTE * pPacket)
{
	BYTE writeBuffer[MAX_BUFFER_LENGTH] = { 0, };
	S_PT_OFFICIAL_GAME_WIN winData;
	READ_PT_OFFICIAL_GAME_WIN(pPacket, winData);

	GgameDBManager.WinOfficialGame(winData.userName);
}

VOID InGameIocp::PROC_PT_OFFICIAL_GAME_LOSE(UserInfo * userInfo, BYTE * pPacket)
{
	BYTE writeBuffer[MAX_BUFFER_LENGTH] = { 0, };
	S_PT_OFFICIAL_GAME_LOSE loseData;
	READ_PT_OFFICIAL_GAME_LOSE(pPacket, loseData);

	GgameDBManager.LoseOfficialGame(loseData.userName);
}

VOID InGameIocp::PROC_PT_FRIENDSHIP_GAME_WIN(UserInfo * userInfo, BYTE * pPacket)
{
	BYTE writeBuffer[MAX_BUFFER_LENGTH] = { 0, };
	S_PT_FRIENDSHIP_GAME_WIN winData;
	READ_PT_FRIENDSHIP_GAME_WIN(pPacket, winData);

	GgameDBManager.WinFriendshipGame(winData.userName);
}