#pragma once

class CConnectedSession : public CPacketSession
{
public:
	CConnectedSession(VOID);
	virtual ~CConnectedSession(VOID);

public:
	// ����� ���� ���� �� �ٽ� Accept ���·� ������ִ� �Լ�
	BOOL Restart(SOCKET listenSock);

	VOID SetConnected(BOOL bConnected);
	BOOL GetConnected(VOID);

private:
	BOOL mConnected;
};

/* ���� ��ü Ŭ����, ���� ��ü�� ��� �ൿ�� �����ϴ� Ŭ���� */