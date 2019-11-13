#include "stdafx.h"
#include "CriticalSection.h"
#include "MultiThreadSync.h"
#include "NetworkWrapping.h"


NetworkWrapping::NetworkWrapping(VOID)
{
	// Accept ���� Overlapped����ü
	ZeroMemory(&mAcceptOverlapped, sizeof(mAcceptOverlapped));
	// Read ���� Overlapped����ü
	ZeroMemory(&mReadOverlapped, sizeof(mReadOverlapped));
	// Write ���� Overlapped����ü
	ZeroMemory(&mWriteOverlapped, sizeof(mWriteOverlapped));
	// Read���� ����ϴ� ����
	ZeroMemory(mReadBuffer, sizeof(mReadBuffer));
	
	mSocket = NULL;

	mAcceptOverlapped.ioType = IO_ACCEPT;// Overlapped ����ü�� ������ ����
	mReadOverlapped.ioType = IO_READ;	// Overlapped ����ü�� ������ ����
	mWriteOverlapped.ioType = IO_WRITE;	// Overlapped ����ü�� ������ ����

	mAcceptOverlapped.object = this;	// ���� ��ü�� �����͸� ������ �ֽ��ϴ�.
	mReadOverlapped.object = this;	// ���� ��ü�� �����͸� ������ �ֽ��ϴ�.
	mWriteOverlapped.object = this;	// ���� ��ü�� �����͸� ������ �ֽ��ϴ�.
}

NetworkWrapping::~NetworkWrapping(VOID)
{
}


BOOL NetworkWrapping::Begin(VOID)
{
	ThreadSync sync; // ���� ������ ����ȭ

	if (mSocket) // �������� ������ ������ ����
		return FALSE;

	ZeroMemory(mReadBuffer, sizeof(mReadBuffer));
	
	mSocket = NULL;

	return TRUE;
}

BOOL NetworkWrapping::End(VOID)
{
	ThreadSync sync; // ���� ������ ����ȭ

	if (!mSocket)
		return FALSE;

	shutdown(mSocket, SD_BOTH);	// �ش� ��Ĺ�� Read/Write�� �����.
	closesocket(mSocket);		// ��Ĺ�� �ݴ´�.

	mSocket = NULL;

	return TRUE;
}

BOOL NetworkWrapping::TcpBind(VOID)
{
	ThreadSync Sync; // ����ȭ ��ü

	if (mSocket)
		return FALSE;

	// TCP������ ��Ĺ�� ����
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

	// ACCEPT�� ������ ��Ĺ ���� �̸� ����
	mSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

	if (INVALID_SOCKET == mSocket) {
		End();

		return FALSE;
	}

	// AcceptEx��� (Accept�� ����ϸ� �ش� ������ ó���ϴ� ���� �ٸ� ó���� �Ҽ� ����
	// ���� ������ ������� ���� ���и� �Ҽ� �����Ƿ� �̸� ������ ��Ĺ�� ����Ͽ� Acceptó���� �Ѵ�)
	if (!AcceptEx(listenSocket, // ������ ����
		mSocket,				// ���� ������ ����
		mReadBuffer,			// recive buffer 
		0,						// ���� ����� 0���� �־� ���� ����
		sizeof(sockaddr_in) + 16, // ���� �ּ� (16����Ʈ Ŀ����)
		sizeof(sockaddr_in) + 16, // ����Ʈ �ּ� (16����Ʈ Ŀ����)
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

	// SOCKET_ERROR������ WSA_IO_PENDING or WSAEWOULDBLOCK�̸� ��������
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

	// WSARecv�� ���� �޾ƿ� �����Ͱ� �ִ� mReadBuffer�� ���� ������ ����
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

	WSABUF wsaBuf;			// write���� WSABUF�� ���
	DWORD writeBytes = 0;
	DWORD writeFlag = 0;

	wsaBuf.buf = (CHAR*)data;	// ������ ������ �����͸� �Է�
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
		//���������� �����¶�� ������ ����
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