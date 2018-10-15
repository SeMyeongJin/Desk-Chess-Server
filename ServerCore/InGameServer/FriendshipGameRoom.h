#pragma once

class UserInfo;
class InGameIocp;

class FriendshipGameRoom : public MultiThreadSync<FriendshipGameRoom>
{
public:
	FriendshipGameRoom();
	virtual ~FriendshipGameRoom();

private:
	DWORD mRoomNumber;						// 방 번호
	DWORD mMapNumber;						// 게임 필드 종류

	BOOL mIsReadyComplete;					// 게임 준비 완료 확인
	BOOL mIsGameStarted;					// 게임 시작 확인 ( 시작~끝 TRUE )

	UserInfo* mUsers[2];					// 인원 
	UserInfo *mRoomManager;					// 방장 유저
	USHORT mCurrentUserNum;

	DWORD mRemainGameTime;					// 남은 게임 시간

public:
	// Get, Set Function
	inline DWORD GetRoomNumber(VOID) { ThreadSync sync; return mRoomNumber; }
	inline DWORD GetMapNumber(VOID) { ThreadSync sync; return mMapNumber; }
	inline USHORT GetCurrentUserNum(VOID) { ThreadSync sync; return mCurrentUserNum; }
	inline UserInfo* GetManagerInfo(VOID) { ThreadSync sync; return mRoomManager; }
	inline BOOL GetIsEmpty(VOID) { ThreadSync sync; return mCurrentUserNum ? FALSE : TRUE; }
	inline BOOL GetIsFull(VOID) { ThreadSync sync; return mCurrentUserNum == 2 ? TRUE : FALSE; }
	inline BOOL GetIsReadyComplete(VOID) { ThreadSync sync; return mIsReadyComplete; }
	inline BOOL GetIsGameStarted(VOID) { ThreadSync sync; return mIsGameStarted; }
	inline BOOL SetMapNumber(DWORD mapNumber) { ThreadSync sync; mMapNumber = mapNumber; return TRUE; }

public:
	BOOL Begin(DWORD roomNumber);
	BOOL End(VOID);

	BOOL JoinUser(UserInfo *userInfo, USHORT &slotNumber);
	BOOL LeaveUser(BOOL isDisconnected, InGameIocp *iocp, UserInfo *userInfo);

	BOOL WriteAll(DWORD protocol, BYTE *packet, DWORD packetLength);
	BOOL WriteOpponent(UserInfo *userInfo, DWORD protocol, BYTE *packet, DWORD packetLength);

	BOOL IsAllLoadComplete(VOID);
	BOOL IsAllIntroComplete(VOID);

	DWORD ReadyComplete(VOID);
	BOOL StartGame(VOID);
	BOOL EndGame(InGameIocp *iocp);

	inline BOOL DecreaseGameTime(VOID) { ThreadSync sync; mRemainGameTime = max(INT(mRemainGameTime--), 0); return mRemainGameTime ? TRUE : FALSE; }
};

