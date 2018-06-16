#pragma once

class CConnectedSession : public CPacketSession
{
public:
	CConnectedSession(VOID);
	virtual ~CConnectedSession(VOID);

public:
	BOOL Restart(SOCKET listenSock);

	VOID SetConnected(BOOL bConnected);
	BOOL GetConnected(VOID);

private:
	BOOL mConnected;
};

/* 접속 개체 클래스, 접속 개체에 모든 행동을 관리하는 클래스 */