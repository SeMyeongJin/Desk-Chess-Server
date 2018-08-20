#pragma once
class GameRoom : public MultiThreadSync<GameRoom>
{
public:
	GameRoom();
	virtual ~GameRoom();

private:
	DWORD mRoomNumber;						// �� ��ȣ
	DWORD mMapNumber;						// ���� �ʵ� ����

	BOOL mIsReadyComplete;					// ���� �غ� �Ϸ� Ȯ��
	BOOL mIsGameStarted;					// ���� ���� Ȯ�� ( ����~�� TRUE )

	UserInfo* mUsers[2];					// �ο� 
	UserInfo *mRoomManager;					// ���� ����

	DWORD mRemainGameTime;					// ���� ���� �ð�

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
	BOOL EndGame(VOID); // �������

	BOOL IsAllLoadComplete(VOID);
	BOOL IsAllIntroComplete(VOID);

	inline BOOL DecreaseGameTime(VOID) { ThreadSync sync; mRemainGameTime = max(INT(mRemainGameTime--), 0); return mRemainGameTime ? TRUE : FALSE; }
};