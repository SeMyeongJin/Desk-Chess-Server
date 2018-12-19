#pragma once

class UserInfo;
class InGameIocp;

class GameRoom : public MultiThreadSync<GameRoom>
{
public:
	GameRoom();
	virtual ~GameRoom();

private:
	DWORD mRoomNumber;						// 방 번호
	BOOL mIsGameStarting;

	UserInfo* mUsers[2];					// 인원 
	UserInfo *mRoomManager;					// 방장 유저
	USHORT mCurrentUserNum;

public:
	// Get, Set Function
	inline DWORD GetRoomNumber(VOID)						{ ThreadSync sync; return mRoomNumber; }
	inline USHORT GetCurrentUserNum(VOID)					{ ThreadSync sync; return mCurrentUserNum; }
	inline UserInfo* GetManagerInfo(VOID)					{ ThreadSync sync; return mRoomManager; }
	inline BOOL GetIsEmpty(VOID)							{ ThreadSync sync; return mCurrentUserNum ? FALSE : TRUE; }
	inline BOOL GetIsFull(VOID)								{ ThreadSync sync; return mCurrentUserNum == 2 ? TRUE : FALSE; }
	inline BOOL GetIsGameStarting(VOID)						{ ThreadSync sync; return mIsGameStarting; }
	inline VOID SetIsGameStarting(BOOL isGameStarting)		{ ThreadSync sync; mIsGameStarting = isGameStarting; }

public:
	BOOL Begin(DWORD roomNumber);
	BOOL End(VOID);

	BOOL JoinUser(UserInfo *userInfo, USHORT &slotNumber);
	BOOL LeaveUser(BOOL isDisconnected, InGameIocp *iocp, UserInfo *userInfo);

	BOOL WriteAll(DWORD protocol, BYTE *packet, DWORD packetLength);
	BOOL WriteOpponent(UserInfo *userInfo, DWORD protocol, BYTE *packet, DWORD packetLength);
};