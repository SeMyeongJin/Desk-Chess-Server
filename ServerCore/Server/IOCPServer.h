#pragma once

class IOCPServer : public IOCP
{
public:
	IOCPServer(VOID);
	virtual ~IOCPServer(VOID);

public:
	// ��ü�� �ʱ�ȭ�ϰ� �����ϱ� ���� �Լ�
	BOOL Begin(VOID);

	// ��ü�� �����ϱ� ���� �Լ�
	VOID End(VOID);

	// KeepAlive Thread Callback
	VOID KeepThreadCallback(VOID);

private:
	// ������ Listen�� ����� ��ü
	NetworkWrapping * mListen;

	SessionManager mSessionManager;

	HANDLE mKeepThread;
	HANDLE mKeepThreadDestroyEvent;

protected:
	// CIocp ��� ���� �Լ����� �缱��
	VOID OnIoRead(VOID * object, DWORD dwDataLength);
	VOID OnIoWrote(VOID * object, DWORD dwDataLength);
	VOID OnIoConnected(VOID * object);
	VOID OnIoDisconnected(VOID * object);

private:
	// ProtocolProcess
	VOID PROC_PT_SIGNUP(ConnectedSession * pConnectedSession, DWORD dwProtocol, BYTE * pPacket, DWORD dwPacketLength);
	VOID PROC_PT_LOGIN(ConnectedSession * pConnectedSession, DWORD dwProtocol, BYTE * pPacket, DWORD dwPacketLength);
};

/* IOCPServerŬ������ IocpŬ������ ��ӹ��� Ŭ������, ��� ��� ���� �۾� ���*/

/*
TODO:
Listen�� ����� ������ �����ϰ� �������� ����� IOCP�� ���۽�Ű�� �۾� (Begin -> Done)
Accept�� �ϴ� �۾� (DONE)
*/