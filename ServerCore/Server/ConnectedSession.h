#pragma once

class ConnectedSession : public PacketWrapping
{
public:
	ConnectedSession(VOID);
	virtual ~ConnectedSession(VOID);

public:
	// ����� ���� ���� �� �ٽ� Accept ���·� ������ִ� �Լ�
	BOOL Restart(SOCKET listenSock);

	VOID SetConnected(BOOL bConnected);
	BOOL GetConnected(VOID);

	inline BOOL SetUserID(LPTSTR userID) { ThreadSync sync; if (!userID) return FALSE; _tcsncpy(mUserID, userID, 20); return TRUE; }
	inline LPTSTR GetUserID(VOID) { ThreadSync sync; return mUserID; }

private:
	BOOL mConnected;

	TCHAR mUserID[20];
};

/* ���� ��ü Ŭ����, ���� ��ü�� ��� �ൿ�� �����ϴ� Ŭ���� */