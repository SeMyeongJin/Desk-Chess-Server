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

	// KeepAlive Thread Callback
	VOID KeepThreadCallback(VOID);

private:
	// ������ Listen�� ����� ��ü
	CNetworkSession * mListen;

	CSessionManager mSessionManager;

	HANDLE mKeepThread;
	HANDLE mKeepThreadDestroyEvent;

protected:
	// CIocp ��� ���� �Լ����� �缱��
	VOID OnIoRead(VOID * object, DWORD dwDataLength);
	VOID OnIoWrote(VOID * object, DWORD dwDataLength);
	VOID OnIoConnected(VOID * object);
	VOID OnIoDisconnected(VOID * object);

private:
	// ProtocolProcess���� ó���ؾ� �� �Լ���
	VOID PROC_PT_LOGIN(CConnectedSession * pConnectedSession, DWORD dwProtocol, BYTE * pPacket, DWORD dwPacketLength);
	VOID PROC_PT_CHAT(CConnectedSession * pConnectedSession, DWORD dwProtocol, BYTE * pPacket, DWORD dwPacketLength);
};

/* ServerIocpŬ������ IocpŬ������ ��ӹ��� Ŭ������, ��� ��� ���� �۾� ���*/

/*
TODO:
Listen�� ����� ������ �����ϰ� �������� ����� IOCP�� ���۽�Ű�� �۾� (Begin -> Done)
Accept�� �ϴ� �۾� (DONE)
*/