#pragma once

class CServerIocp : public CIocp
{
public:
	CServerIocp(VOID);
	virtual ~CServerIocp(VOID);

public:
	// ��ü�� �ʱ�ȭ�ϰ� �����ϱ� ���� �Լ�
	BOOL Begin(VOID);

	// ��ü�� �����ϱ� ���� �Լ�
	VOID End(VOID);

private:
	CNetworkSession * mListen;

	CSessionManager mSessionManager;

protected:
	// CIocp ��� ���� �Լ����� �缱��
	VOID OnIoRead(VOID * object, DWORD dwDataLength);
	VOID OnIoWrote(VOID * object, DWORD dwDataLength);
	VOID OnIoConnected(VOID * object);
	VOID OnIoDisconnected(VOID * object);
};

/* ServerIocpŬ������ IocpŬ������ ��ӹ��� Ŭ������, ��� ��� ���� �۾� ���*/

/*
TODO:
Listen�� ����� ������ �����ϰ� �������� ����� IOCP�� ���۽�Ű�� �۾� (Begin -> Done)
Accept�� �ϴ� �۾�
*/