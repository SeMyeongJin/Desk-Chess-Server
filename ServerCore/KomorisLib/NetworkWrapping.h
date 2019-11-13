#pragma once

class NetworkWrapping : public MultiThreadSync<NetworkWrapping>
{
public:
	NetworkWrapping(VOID);
	virtual ~NetworkWrapping(VOID);

public:
	BOOL			Begin(VOID);
	BOOL			End(VOID);

	BOOL			Listen(USHORT port, INT backLog);
	BOOL			Accept(SOCKET listenSocket);
	BOOL			Connect(LPSTR address, USHORT port);
	BOOL			TcpBind(VOID);

	BOOL			Write(BYTE* data, DWORD dataLength);

	BOOL			InitializeReadForIocp(VOID);
	BOOL			ReadForIocp(BYTE* data, DWORD &dataLength);

	SOCKET			GetSocket(VOID);

private:
	OVERLAPPED_EX			mAcceptOverlapped;
	OVERLAPPED_EX			mReadOverlapped;
	OVERLAPPED_EX			mWriteOverlapped;

	BYTE					mReadBuffer[MAX_BUFFER_LENGTH];

	SOCKET					mSocket;
};