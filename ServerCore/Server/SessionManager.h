#pragma once

class CSessionManager : public CMultiThreadSync<CSessionManager>
{
public:
	CSessionManager();
	~CSessionManager();

private:
	std::vector<CConnectedSession*> mConnectedSessions;

public:
	BOOL Begin(SOCKET listenSock);

	VOID End(VOID);

	// 접속된 모든 ConnectedSession에 패킷을 전송하는 함수
	VOID WriteAll(DWORD dwProtocol, BYTE * data, DWORD dwLength);
};


/* ConnectedSession 객체들을 관리하는 클래스 */

/* 
TODO:
생성, 삭제 이외의 모든 ConnectedSession 클래스에게 행해야 하는 행위가 있을 경우 사용되어지게 작업

생성된 객체를 관리하는 벡터만드는 작업
*/