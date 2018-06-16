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

private:
	CNetworkSession * mListen;

	CSessionManager mSessionManager;

protected:
	// CIocp 상속 가상 함수들을 재선언
	VOID OnIoRead(VOID * object, DWORD dwDataLength);
	VOID OnIoWrote(VOID * object, DWORD dwDataLength);
	VOID OnIoConnected(VOID * object);
	VOID OnIoDisconnected(VOID * object);
};

/* ServerIocp클래스는 Iocp클래스를 상속받은 클래스로, 모든 통신 관련 작업 담당*/

/*
TODO:
Listen을 담당할 소켓을 생성하고 서버에서 사용할 IOCP를 시작시키는 작업 (Begin -> Done)
Accept를 하는 작업
*/