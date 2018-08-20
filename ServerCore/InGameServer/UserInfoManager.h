#pragma once
class UserInfoManager : public MultiThreadSync<UserInfoManager>
{
public:
	UserInfoManager();
	virtual ~UserInfoManager();

private:
	std::vector<UserInfo*> mUserInfoVector;

	DWORD mMaxUserNum;						// 최대 인원
	DWORD mCurrentUserNum;					// 현재 인원

	SOCKET mlistenSocket;					// 문지기 소켓

public:
	BOOL Begin(DWORD maxUserNum, SOCKET listenSocket);
	BOOL End(VOID);

	BOOL AcceptALL(VOID);
	BOOL WriteAll(DWORD protocol, BYTE *data, DWORD dataLength);
};