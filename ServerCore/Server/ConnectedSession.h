#pragma once

class CConnectedSession : public CPacketSession
{
public:
	CConnectedSession(VOID);
	virtual ~CConnectedSession(VOID);

public:
	BOOL Restart(SOCKET listenSock);
};

/* ���� ��ü Ŭ����, ���� ��ü�� ��� �ൿ�� �����ϴ� Ŭ���� */