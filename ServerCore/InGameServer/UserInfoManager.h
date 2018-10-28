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

	// 모든 UserInfo 클래스 개체를 Accept 대기 상태로 만듬
	BOOL AcceptALL(VOID);
	BOOL WriteAll(DWORD protocol, BYTE *data, DWORD dataLength);

	inline VOID IncreaseUserInfoNum(VOID) { InterlockedIncrement((LONG*)&mCurrentUserNum); }
	inline VOID DecreaseUserInfoNum(VOID) { InterlockedDecrement((LONG*)&mCurrentUserNum); }
};