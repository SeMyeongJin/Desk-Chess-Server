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

	// ���ӵ� ��� ConnectedSession�� ��Ŷ�� �����ϴ� �Լ�
	VOID WriteAll(DWORD dwProtocol, BYTE * data, DWORD dwLength);
};


/* ConnectedSession ��ü���� �����ϴ� Ŭ���� */

/* 
TODO:
����, ���� �̿��� ��� ConnectedSession Ŭ�������� ���ؾ� �ϴ� ������ ���� ��� ���Ǿ����� �۾�

������ ��ü�� �����ϴ� ���͸���� �۾�
*/