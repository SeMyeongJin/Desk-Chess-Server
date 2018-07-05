#pragma once

class CServerIocp : public CIocp
{
public:
	CServerIocp(VOID);
	virtual ~CServerIocp(VOID);

public:
	// 개체를 초기화하고 시작하기 위한 함수
	BOOL Begin(VOID);

	// 개체를 종료하기 위한 함수
	VOID End(VOID);

	// KeepAlive Thread Callback
	VOID KeepThreadCallback(VOID);

private:
	// 서버의 Listen을 담당할 개체
	CNetworkSession * mListen;

	CSessionManager mSessionManager;

	HANDLE mKeepThread;
	HANDLE mKeepThreadDestroyEvent;

protected:
	// CIocp 상속 가상 함수들을 재선언
	VOID OnIoRead(VOID * object, DWORD dwDataLength);
	VOID OnIoWrote(VOID * object, DWORD dwDataLength);
	VOID OnIoConnected(VOID * object);
	VOID OnIoDisconnected(VOID * object);

private:
	// ProtocolProcess에서 처리해야 할 함수들
	VOID PROC_PT_LOGIN(CConnectedSession * pConnectedSession, DWORD dwProtocol, BYTE * pPacket, DWORD dwPacketLength);
	VOID PROC_PT_CHAT(CConnectedSession * pConnectedSession, DWORD dwProtocol, BYTE * pPacket, DWORD dwPacketLength);
};

/* ServerIocp클래스는 Iocp클래스를 상속받은 클래스로, 모든 통신 관련 작업 담당*/

/*
TODO:
Listen을 담당할 소켓을 생성하고 서버에서 사용할 IOCP를 시작시키는 작업 (Begin -> Done)
Accept를 하는 작업 (DONE)
*/