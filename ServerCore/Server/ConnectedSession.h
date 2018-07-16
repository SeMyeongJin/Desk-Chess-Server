#pragma once

class ConnectedSession : public PacketSession
{
public:
	ConnectedSession(VOID);
	virtual ~ConnectedSession(VOID);

public:
	// 연결된 세션 종료 후 다시 Accept 상태로 만들어주는 함수
	BOOL Restart(SOCKET listenSock);

	VOID SetConnected(BOOL bConnected);
	BOOL GetConnected(VOID);

private:
	BOOL mConnected;
};

/* 접속 개체 클래스, 접속 개체에 모든 행동을 관리하는 클래스 */