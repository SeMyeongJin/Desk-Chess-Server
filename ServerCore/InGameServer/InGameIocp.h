#pragma once
class InGameIocp : public IOCP
{
public:
	InGameIocp();
	virtual ~InGameIocp();

private:
	NetworkWrapping *mListenSession;
	UserInfoManager mUserInfoManager;

	GameRoomManager mRoomManager;
	FriGameRoomManager mFriRoomManager;

	HANDLE mGameThreadHandle;
	HANDLE mGameThreadDestroyEvent;

public:
	BOOL Begin(VOID);
	BOOL End(VOID);

	VOID GameThreadCallback(VOID);

protected:
	VOID OnIoRead(VOID *object, DWORD dataLength);
	VOID OnIoWrote(VOID *object, DWORD dataLength);
	VOID OnIoConnected(VOID *object);
	VOID OnIoDisconnected(VOID *object);

private:
	/*
		TODO: 패킷 프로세스 처리
	*/
	VOID PROC_PT_REQ_USER_INFO(UserInfo * userInfo, BYTE * pPacket);
	VOID PROC_PT_OFFICIAL_GAME_START(UserInfo * userInfo, BYTE * pPacket);
	VOID PROC_PT_FRIENDSHIP_GAME_START(UserInfo * userInfo, BYTE * pPacket);
	VOID PROC_PT_ROOM_LEAVE(UserInfo * userInfo, BYTE * pPacket);
	VOID PROC_PT_CHAT(UserInfo * userInfo, BYTE * pPacket);
	VOID PROC_PT_PIECE_MOVE(UserInfo * userInfo, BYTE * pPacket);
	VOID PROC_PT_PIECE_PROMOTION(UserInfo * userInfo, BYTE * pPacket);
	VOID PROC_PT_OFFICIAL_GAME_WIN(UserInfo * userInfo, BYTE * pPacket);
	VOID PROC_PT_OFFICIAL_GAME_LOSE(UserInfo * userInfo, BYTE * pPacket);
	VOID PROC_PT_FRIENDSHIP_GAME_WIN(UserInfo * userInfo, BYTE * pPacket);
};