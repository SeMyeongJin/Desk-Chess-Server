#pragma once

class ConnectedSession : public PacketSession
{
public:
	ConnectedSession(VOID);
	virtual ~ConnectedSession(VOID);

public:
	// ����� ���� ���� �� �ٽ� Accept ���·� ������ִ� �Լ�
	BOOL Restart(SOCKET listenSock);

	VOID SetConnected(BOOL bConnected);
	BOOL GetConnected(VOID);

private:
	BOOL mConnected;
};

/* ���� ��ü Ŭ����, ���� ��ü�� ��� �ൿ�� �����ϴ� Ŭ���� */