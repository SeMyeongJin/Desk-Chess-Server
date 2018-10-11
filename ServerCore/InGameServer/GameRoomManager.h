#pragma once
class GameRoomManager : public MultiThreadSync<GameRoomManager>
{
public:
	GameRoomManager();
	virtual ~GameRoomManager();

private:
	DWORD mMaxRoomNum;
	DWORD mCurrentRoomNum;

	std::vector<GameRoom*> mRoomVector;

public:
	BOOL Begin(DWORD maxRoomNum);
	BOOL End(VOID);

	GameRoom* QuickJoin(UserInfo* userInfo, USHORT &slotNumber);
	BOOL CheckGameTime(InGameIocp *iocp);
};

