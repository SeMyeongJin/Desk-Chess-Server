#pragma once

class GameRoom;
class FriendshipGameRoom;

class UserInfo : public PacketWrapping
{
public:
	UserInfo();
	virtual ~UserInfo();

private:
	TCHAR				mUserID[20];
	TCHAR				mNickName[20];
	DWORD				mLifePoint;

	USER_STATUS			mStatus;

	GameRoom*			mEnteredRoom;
	FriendshipGameRoom* mEnteredFriendshipRoom;

	BOOL				mIsConnected;

private:
	/*
		variable in game
	*/
	DWORD				mGameGold;
	DWORD				mSpaceShipHP;
	DWORD				mTowerHP;
	// minion class
	BOOL				mIsReady;
	BOOL				mIsLoadComplete;
	BOOL				mIsIntroComplete;

public:
	BOOL				Begin(VOID);
	BOOL				End(VOID);

	BOOL				Reload(SOCKET listenSocket);

public:
	inline BOOL InitGame(VOID) 
	{ 
		ThreadSync sync;
		mGameGold = 500;
		mSpaceShipHP = SPACESHIP_HP;
		mTowerHP = TOWER_HP;
		return TRUE;
	}
	inline DWORD GetGameGold(VOID)								{ ThreadSync sync; return mGameGold; }
	inline DWORD GetSpaceShipHP(VOID)							{ ThreadSync sync; return mSpaceShipHP; }
	inline DWORD GetTowerHP(VOID)								{ ThreadSync sync; return mTowerHP; }
	inline BOOL  SetGameGold(DWORD gold)						{ ThreadSync sync; mGameGold = gold; return TRUE; }
	inline BOOL  SetSpaceShipHP(DWORD hp)						{ ThreadSync sync; mSpaceShipHP = hp; return TRUE; }
	inline BOOL  SetTowerHP(DWORD hp)							{ ThreadSync sync; mTowerHP = hp; return TRUE; }
	
	inline BOOL GetIsConnected(VOID)							{ ThreadSync sync; return mIsConnected; }
	inline BOOL GetIsReady(VOID)								{ ThreadSync sync; return mIsReady; }
	inline BOOL GetIsLoadComplete(VOID)							{ ThreadSync sync; return mIsLoadComplete; }
	inline BOOL GetIsIntroComplete(VOID)						{ ThreadSync sync; return mIsIntroComplete; }
	inline BOOL	SetIsConnected(BOOL isConnected)				{ ThreadSync sync; mIsConnected = isConnected; return TRUE; }
	inline BOOL	SetIsReady(BOOL isReady)						{ ThreadSync sync; mIsReady = isReady; return TRUE; }
	inline BOOL	SetIsLoadComplete(BOOL isLoadComplete)			{ ThreadSync sync; mIsLoadComplete = isLoadComplete; return TRUE; }
	inline BOOL	SetIsIntroComplete(BOOL isIntroComplete)		{ ThreadSync sync; mIsIntroComplete = isIntroComplete; return TRUE; }

	inline LPTSTR			GetUserID(VOID)						{ ThreadSync sync; return mUserID; }
	inline LPTSTR			GetNickName(VOID)					{ ThreadSync sync; return mNickName; }
	inline DWORD			GetLifePoint(VOID)					{ ThreadSync sync; return mLifePoint; }
	inline USER_STATUS		GetStatus(VOID)						{ ThreadSync sync; return mStatus; }
	inline GameRoom*		GetEnteredRoom(VOID)				{ ThreadSync sync; return mEnteredRoom; }
	inline FriendshipGameRoom* GetEnteredFriendshipRoom(VOID)	{ ThreadSync sync; return mEnteredFriendshipRoom; }
	inline BOOL				SetUserID(LPTSTR userID)			{ ThreadSync sync; _tcsncpy(mUserID, userID, 20); return TRUE; }
	inline BOOL				SetUserName(LPTSTR userName)		{ ThreadSync sync; _tcsncpy(mNickName, userName, 20); return TRUE; }
	inline BOOL				SetLifePoint(DWORD lp)				{ ThreadSync sync; mLifePoint = lp; return TRUE; }
	inline BOOL				SetStatus(USER_STATUS status)		{ ThreadSync sync; mStatus = status; return TRUE; }
	inline BOOL				SetEnteredRoom(GameRoom *room)		{ ThreadSync sync; mEnteredRoom = room; return TRUE; }
	inline BOOL				SetEnteredFriendshipRoom(FriendshipGameRoom *room) { ThreadSync sync; mEnteredFriendshipRoom = room; return TRUE; }
};