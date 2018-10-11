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
		TODO: ��Ŷ ���μ��� ó��
	*/
	VOID PROC_PT_OFFICIAL_GAME_START(UserInfo * userInfo, BYTE * pPacket);
	VOID PROC_PT_FRIENDSHIP_GAME_START(UserInfo * userInfo, BYTE * pPacket);
	VOID PROC_PT_ROOM_LEAVE(UserInfo * userInfo, BYTE * pPacket);
	VOID PROC_PT_CHAT(UserInfo * userInfo, BYTE * pPacket);
	VOID PROC_PT_PIECE_MOVE(UserInfo * userInfo, BYTE * pPacket);
};