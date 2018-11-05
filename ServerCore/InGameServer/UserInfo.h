#pragma once

class GameRoom;
class FriendshipGameRoom;

class UserInfo : public PacketWrapping
{
public:
	UserInfo();
	virtual ~UserInfo();

private:
	GameRoom*			mEnteredRoom;
	FriendshipGameRoom* mEnteredFriendshipRoom;

	BOOL				mIsConnected;

public:
	BOOL				Begin(VOID);
	BOOL				End(VOID);

	BOOL				Reload(SOCKET listenSocket);

public:
	inline BOOL GetIsConnected(VOID)							{ ThreadSync sync; return mIsConnected; }
	inline BOOL	SetIsConnected(BOOL isConnected)				{ ThreadSync sync; mIsConnected = isConnected; return TRUE; }

	inline GameRoom*		GetEnteredRoom(VOID)				{ ThreadSync sync; return mEnteredRoom; }
	inline FriendshipGameRoom* GetEnteredFriendshipRoom(VOID)	{ ThreadSync sync; return mEnteredFriendshipRoom; }
	inline BOOL				SetEnteredRoom(GameRoom *room)		{ ThreadSync sync; mEnteredRoom = room; return TRUE; }
	inline BOOL				SetEnteredFriendshipRoom(FriendshipGameRoom *room) { ThreadSync sync; mEnteredFriendshipRoom = room; return TRUE; }
};