#pragma once
class UserInfoManager : public MultiThreadSync<UserInfoManager>
{
public:
	UserInfoManager();
	virtual ~UserInfoManager();

private:
	std::vector<UserInfo*> mUserInfoVector;

	DWORD mMaxUserNum;						// �ִ� �ο�
	DWORD mCurrentUserNum;					// ���� �ο�

	SOCKET mlistenSocket;					// ������ ����

public:
	BOOL Begin(DWORD maxUserNum, SOCKET listenSocket);
	BOOL End(VOID);

	BOOL AcceptALL(VOID);
	BOOL WriteAll(DWORD protocol, BYTE *data, DWORD dataLength);
};