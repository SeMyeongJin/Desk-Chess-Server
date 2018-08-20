#pragma once
class GameRoom : public MultiThreadSync<GameRoom>
{
public:
	GameRoom();
	virtual ~GameRoom();

private:
	DWORD mRoomNumber;						// 방 번호
	DWORD mMapNumber;						// 게임 필드 종류

	BOOL mIsReadyComplete;					// 게임 준비 완료 확인
	BOOL mIsGameStarted;					// 게임 시작 확인 ( 시작~끝 TRUE )

	UserInfo* mUsers[2];					// 인원 
	UserInfo *mRoomManager;					// 방장 유저

	DWORD mRemainGameTime;					// 남은 게임 시간

public:
	// Get, Set Function
	inline DWORD GetRoomNumber(VOID)						{ ThreadSync sync; return mRoomNumber; }
	inline DWORD GetMapNumber(VOID)							{ ThreadSync sync; return mMapNumber; }
	inline VOID SetMapNumber(DWORD mapNumber)				{ ThreadSync sync; mMapNumber = mapNumber; }

public:
	BOOL Begin(DWORD roomNumber);
	BOOL End(VOID);

	BOOL WriteAll(DWORD protocol, BYTE *packet, DWORD packetLength);
	BOOL WriteAllInitGameData(VOID);

	DWORD AllocRoom(VOID);
	BOOL StartGame(VOID);
	BOOL EndGame(VOID); // 수정요망

	BOOL IsAllLoadComplete(VOID);
	BOOL IsAllIntroComplete(VOID);

	inline BOOL DecreaseGameTime(VOID) { ThreadSync sync; mRemainGameTime = max(INT(mRemainGameTime--), 0); return mRemainGameTime ? TRUE : FALSE; }
};