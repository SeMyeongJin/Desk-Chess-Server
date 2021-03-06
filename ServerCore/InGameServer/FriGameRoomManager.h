#pragma once
class FriGameRoomManager : public MultiThreadSync<FriGameRoomManager>
{
public:
	FriGameRoomManager();
	virtual ~FriGameRoomManager();

private:
	DWORD mMaxRoomNum;
	DWORD mCurrentRoomNum;

	std::map<int, FriendshipGameRoom*> mRoomMap;

public:
	BOOL Begin(DWORD maxRoomNum);
	BOOL End(VOID);

	FriendshipGameRoom* QuickJoin(UserInfo* userInfo, USHORT &slotNumber);
};

