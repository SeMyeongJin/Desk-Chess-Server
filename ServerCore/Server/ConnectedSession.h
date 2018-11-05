#pragma once

class ConnectedSession : public PacketWrapping
{
public:
	ConnectedSession(VOID);
	virtual ~ConnectedSession(VOID);

public:
	// 연결된 세션 종료 후 다시 Accept 상태로 만들어주는 함수
	BOOL Restart(SOCKET listenSock);

	VOID SetConnected(BOOL bConnected);
	BOOL GetConnected(VOID);

	inline BOOL SetUserID(LPTSTR userID) { ThreadSync sync; if (!userID) return FALSE; _tcsncpy(mUserID, userID, 20); return TRUE; }
	inline LPTSTR GetUserID(VOID) { ThreadSync sync; return mUserID; }

private:
	BOOL mConnected;

	TCHAR mUserID[20];
};

/* 접속 개체 클래스, 접속 개체에 모든 행동을 관리하는 클래스 */