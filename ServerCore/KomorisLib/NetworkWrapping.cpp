#include "stdafx.h"
#include "CriticalSection.h"
#include "MultiThreadSync.h"
#include "NetworkWrapping.h"


NetworkWrapping::NetworkWrapping(VOID)
{
	// Accept 관련 Overlapped구조체
	ZeroMemory(&mAcceptOverlapped, sizeof(mAcceptOverlapped));
	// Read 관련 Overlapped구조체
	ZeroMemory(&mReadOverlapped, sizeof(mReadOverlapped));
	// Write 관련 Overlapped구조체
	ZeroMemory(&mWriteOverlapped, sizeof(mWriteOverlapped));
	// Read에서 사용하는 버퍼
	ZeroMemory(mReadBuffer, sizeof(mReadBuffer));
	
	mSocket = NULL;

	mAcceptOverlapped.ioType = IO_ACCEPT;// Overlapped 구조체의 종류를 정의
	mReadOverlapped.ioType = IO_READ;	// Overlapped 구조체의 종류를 정의
	mWriteOverlapped.ioType = IO_WRITE;	// Overlapped 구조체의 종류를 정의

	mAcceptOverlapped.object = this;	// 현재 개체의 포인터를 가지고 있습니다.
	mReadOverlapped.object = this;	// 현재 개체의 포인터를 가지고 있습니다.
	mWriteOverlapped.object = this;	// 현재 개체의 포인터를 가지고 있습니다.
}

NetworkWrapping::~NetworkWrapping(VOID)
{
}


BOOL NetworkWrapping::Begin(VOID)
{
	ThreadSync sync; // 다중 스레드 동기화

	if (mSocket) // 실행중인 소켓이 있으면 실패
		return FALSE;

	ZeroMemory(mReadBuffer, sizeof(mReadBuffer));
	
	mSocket = NULL;

	return TRUE;
}

BOOL NetworkWrapping::End(VOID)
{
	ThreadSync sync; // 다중 스레드 동기화

	if (!mSocket)
		return FALSE;

	shutdown(mSocket, SD_BOTH);	// 해당 소캣의 Read/Write를 멈춘다.
	closesocket(mSocket);		// 소캣을 닫는다.

	mSocket = NULL;

	return TRUE;
}

BOOL NetworkWrapping::TcpBind(VOID)
{
	ThreadSync Sync; // 동기화 개체

	if (mSocket)
		return FALSE;

	// TCP형태의 소캣을 생성
	mSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

	if (mSocket == INVALID_SOCKET)
		return FALSE;

	return TRUE;
}

BOOL NetworkWrapping::Listen(USHORT port, INT backLog)
{
	ThreadSync Sync;

	if (port <= 0 || backLog <= 0)
		return FALSE;

	if (!mSocket)
		return FALSE;

	SOCKADDR_IN listenSocketInfo;
	
	listenSocketInfo.sin_family = AF_INET;
	listenSocketInfo.sin_port = htons(port);
	listenSocketInfo.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	if (bind(mSocket, (struct sockaddr*)&listenSocketInfo, sizeof(SOCKADDR_IN)) == SOCKET_ERROR) 
	{
		End();

		return FALSE;
	}

	if (listen(mSocket, backLog) == SOCKET_ERROR) 
	{
		End();
	
		return FALSE;
	}

	LINGER Linger;
	Linger.l_onoff = 1;
	Linger.l_linger = 0;

	if (setsockopt(mSocket, SOL_SOCKET, SO_LINGER, (char*)&Linger, sizeof(LINGER)) == SOCKET_ERROR)
	{
		End();

		return FALSE;
	}

	return TRUE;
}

BOOL NetworkWrapping::Accept(SOCKET listenSocket)
{
	ThreadSync sync;

	if (!listenSocket)
		return FALSE;

	if (mSocket)
		return FALSE;

	// ACCEPT시 연결할 소캣 생성 미리 생성
	mSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

	if (INVALID_SOCKET == mSocket) {
		End();

		return FALSE;
	}

	// AcceptEx사용 (Accept를 사용하면 해당 접속을 처리하는 동안 다른 처리를 할수 없다
	// 따라서 접속이 몰릴경우 접속 실패를 할수 있으므로 미리 생성된 소캣을 사용하여 Accept처리를 한다)
	if (!AcceptEx(listenSocket, // 문지기 소켓
		mSocket,				// 새로 생성된 소켓
		mReadBuffer,			// recive buffer 
		0,						// 버퍼 사이즈를 0으로 주어 받지 않음
		sizeof(sockaddr_in) + 16, // 로컬 주소 (16바이트 커야함)
		sizeof(sockaddr_in) + 16, // 리모트 주소 (16바이트 커야함)
		NULL,
		&mAcceptOverlapped.overlapped)) {
		if (WSAGetLastError() != ERROR_IO_PENDING && WSAGetLastError() != WSAEWOULDBLOCK) {
			End();

			return FALSE;
		}
	}

	return TRUE;
}

BOOL NetworkWrapping::InitializeReadForIocp(VOID)
{
	ThreadSync sync;

	if (!mSocket)
		return FALSE;

	WSABUF wsaBuf;
	DWORD readBytes = 0;
	DWORD readFlag = 0;

	wsaBuf.buf = (CHAR*)mReadBuffer;
	wsaBuf.len = MAX_BUFFER_LENGTH;

	INT returnValue = WSARecv(mSocket,
		&wsaBuf,
		1,
		&readBytes,
		&readFlag,
		&mReadOverlapped.overlapped,
		NULL);

	// SOCKET_ERROR이지만 WSA_IO_PENDING or WSAEWOULDBLOCK이면 정상진행
	if (SOCKET_ERROR == returnValue && WSA_IO_PENDING != WSAGetLastError() && WSAEWOULDBLOCK != WSAGetLastError()) {
		End();

		return FALSE;
	}

	return TRUE;
}

BOOL NetworkWrapping::ReadForIocp(BYTE* data, DWORD &dataLength)
{
	ThreadSync sync;

	if (!mSocket)
		return FALSE;

	if (!data || dataLength <= 0)
		return FALSE;

	// WSARecv를 통해 받아온 데이터가 있는 mReadBuffer로 부터 데이터 복사
	CopyMemory(data, mReadBuffer, dataLength);
	
	return TRUE;
}

BOOL NetworkWrapping::Write(BYTE* data, DWORD dataLength)
{
	ThreadSync sync;

	if (!mSocket)
		return FALSE;

	if (!data || dataLength <= 0)
		return FALSE;

	WSABUF wsaBuf;			// write에도 WSABUF를 사용
	DWORD writeBytes = 0;
	DWORD writeFlag = 0;

	wsaBuf.buf = (CHAR*)data;	// 보내는 데이터 포인터를 입력
	wsaBuf.len = dataLength;

	INT returnValue = WSASend(mSocket,
		&wsaBuf,
		1,
		&writeBytes,
		writeFlag,
		&mWriteOverlapped.overlapped,
		NULL);

	if (SOCKET_ERROR == returnValue && WSA_IO_PENDING == WSAGetLastError() && WSAEWOULDBLOCK == WSAGetLastError()) {
		End();

		return FALSE;
	}

	return TRUE;
}

BOOL NetworkWrapping::Connect(LPSTR address, USHORT port)
{
	ThreadSync sync;

	if (!address || port <= 0)
		return FALSE;

	if (!mSocket)
		return FALSE;

	SOCKADDR_IN remoteAddressInfo;

	remoteAddressInfo.sin_family = AF_INET;
	remoteAddressInfo.sin_port = htons(port);
	remoteAddressInfo.sin_addr.S_un.S_addr = inet_addr(address);

	if (WSAConnect(mSocket, (LPSOCKADDR)&remoteAddressInfo, sizeof(SOCKADDR_IN), NULL, NULL, NULL, NULL) == SOCKET_ERROR) {
		//실패이지만 대기상태라고 나오면 성공
		if (WSAGetLastError() != WSAEWOULDBLOCK) {
			End();
			return FALSE;
		}
	}

	return TRUE;
}

SOCKET NetworkWrapping::GetSocket(VOID)
{
	ThreadSync Sync;

	return mSocket;
}