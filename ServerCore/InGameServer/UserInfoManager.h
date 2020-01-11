#pragma once
class UserInfoManager : public MultiThreadSync<UserInfoManager>
{
public:
	UserInfoManager();
	virtual ~UserInfoManager();

private:
	std::map<int, UserInfo*> mUserInfoMap;

	DWORD mMaxUserNum;						// �ִ� �ο�
	DWORD mCurrentUserNum;					// ���� �ο�

	SOCKET mlistenSocket;					// ������ ����

public:
	BOOL Begin(DWORD maxUserNum, SOCKET listenSocket);
	BOOL End(VOID);

	// ��� UserInfo Ŭ���� ��ü�� Accept ��� ���·� ����
	BOOL AcceptALL(VOID);
	BOOL WriteAll(DWORD protocol, BYTE *data, DWORD dataLength);

	inline VOID IncreaseUserInfoNum(VOID) { InterlockedIncrement((LONG*)&mCurrentUserNum); }
	inline VOID DecreaseUserInfoNum(VOID) { InterlockedDecrement((LONG*)&mCurrentUserNum); }
};