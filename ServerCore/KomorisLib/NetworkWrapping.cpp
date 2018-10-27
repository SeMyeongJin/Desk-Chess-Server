#include "stdafx.h"
#include "CriticalSection.h"
#include "MultiThreadSync.h"
#include "CircularQueue.h"
#include "NetworkWrapping.h"

DWORD WINAPI ReliableUdpThreadCallback(LPVOID parameter)
{
	NetworkWrapping *Owner = (NetworkWrapping*)parameter;
	Owner->ReliableUdpThreadCallback();

	return 0;
}

NetworkWrapping::NetworkWrapping(void)
{
	// Accept ���� Overlapped����ü
	ZeroMemory(&mAcceptOverlapped, sizeof(mAcceptOverlapped));
	// Read ���� Overlapped����ü
	ZeroMemory(&mReadOverlapped, sizeof(mReadOverlapped));
	// Write ���� Overlapped����ü
	ZeroMemory(&mWriteOverlapped, sizeof(mWriteOverlapped));
	// Read���� ����ϴ� ����
	ZeroMemory(mReadBuffer, sizeof(mReadBuffer));
	// UDP�� ����� �� ��� �ּ� ����
	ZeroMemory(&mUdpRemoteInfo, sizeof(mUdpRemoteInfo));

	mSocket = NULL;

	mReliableUdpThreadHandle = NULL;	// ��Ŷ�� �޾Ҵٴ� ��ȣ�� ���� ������ ���� ��Ŷ�� ����������� ������ �ڵ�
	mReliableUdpThreadStartupEvent = NULL;	// ������ ���� �̺�Ʈ
	mReliableUdpThreadDestroyEvent = NULL;	// ������ ���� �̺�Ʈ
	mReliableUdpThreadWakeUpEvent = NULL;	// ������ ����� �̺�Ʈ
	mReliableUdpWriteCompleteEvent = NULL;	// ��Ŷ�� ���̻� ���� �ʿ䰡 ������ ������ �̺�Ʈ
	mIsReliableUdpSending = FALSE;

	mAcceptOverlapped.ioType = IO_ACCEPT;// Overlapped ����ü�� ������ ����
	mReadOverlapped.ioType = IO_READ;	// Overlapped ����ü�� ������ ����
	mWriteOverlapped.ioType = IO_WRITE;	// Overlapped ����ü�� ������ ����

	mAcceptOverlapped.object = this;	// ���� ��ü�� �����͸� ������ �ֽ��ϴ�.
	mReadOverlapped.object = this;	// ���� ��ü�� �����͸� ������ �ֽ��ϴ�.
	mWriteOverlapped.object = this;	// ���� ��ü�� �����͸� ������ �ֽ��ϴ�.
}

NetworkWrapping::~NetworkWrapping(void)
{
}

VOID NetworkWrapping::ReliableUdpThreadCallback(VOID)
{
	DWORD				EventID = 0;
	HANDLE				ThreadEvents[2] = { mReliableUdpThreadDestroyEvent, mReliableUdpThreadWakeUpEvent };

	CHAR				RemoteAddress[32] = { 0, };
	USHORT				RemotePort = 0;
	BYTE				Data[MAX_BUFFER_LENGTH] = { 0, };
	DWORD				DataLength = 0;
	VOID				*Object = NULL;

	while (TRUE)
	{
		SetEvent(mReliableUdpThreadStartupEvent);

		EventID = WaitForMultipleObjects(2, ThreadEvents, FALSE, INFINITE);
		switch (EventID)
		{
		case WAIT_OBJECT_0:
			return;

		case WAIT_OBJECT_0 + 1:
		NEXT_DATA:
			// Write �� ��� Pop�� ���ְ�
			if (mReliableWriteQueue.Pop(&Object, Data, DataLength, RemoteAddress, RemotePort))
			{
				// �����Ͱ� ���� ���
				// ���� Write�� �� �ְ� WaitForSingleObject�� �� �ش�.
				// �޾����� �� SetEvent�� �� �ָ� Ǯ����.
			RETRY:
				if (!WriteTo2(RemoteAddress, RemotePort, Data, DataLength))
					return;

				DWORD Result = WaitForSingleObject(mReliableUdpWriteCompleteEvent, 10);

				// �޾��� ��� ���� ������ ó��
				if (Result == WAIT_OBJECT_0)
					goto NEXT_DATA;
				else
					goto RETRY;
			}
			else
				mIsReliableUdpSending = FALSE;

			break;
		}
	}
}

BOOL NetworkWrapping::Begin(VOID)
{
	ThreadSync sync; // ���� ������ ����ȭ

	if (mSocket) // �������� ������ ������ ����
		return FALSE;

	ZeroMemory(mReadBuffer, sizeof(mReadBuffer));
	ZeroMemory(&mUdpRemoteInfo, sizeof(mUdpRemoteInfo));

	mSocket = NULL;

	mReliableUdpThreadHandle = NULL;
	mReliableUdpThreadStartupEvent = NULL;
	mReliableUdpThreadDestroyEvent = NULL;
	mReliableUdpThreadWakeUpEvent = NULL;
	mReliableUdpWriteCompleteEvent = NULL;

	mIsReliableUdpSending = FALSE; // ���� �����͸� ���������� Ȯ���ϴ� FLAG ����

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

	if (mReliableUdpThreadHandle) {
		// ������ ���� �̺�Ʈ ����
		SetEvent(mReliableUdpThreadDestroyEvent);

		// ������ ����ñ��� ���Ѵ��
		// INFINITE�� �ƴ� 3~5�� ����� TerminateThread�� �̿��� ���� �����ص� ��
		WaitForSingleObject(mReliableUdpThreadHandle, INFINITE);
		CloseHandle(mReliableUdpThreadHandle);
	}

	if (mReliableUdpThreadDestroyEvent)
		CloseHandle(mReliableUdpThreadDestroyEvent);

	if (mReliableUdpThreadStartupEvent)
		CloseHandle(mReliableUdpThreadStartupEvent);

	if (mReliableUdpThreadWakeUpEvent)
		CloseHandle(mReliableUdpThreadWakeUpEvent);

	if (mReliableUdpWriteCompleteEvent)
		CloseHandle(mReliableUdpWriteCompleteEvent);

	// Reliable UDP�� ���� �����͸� �����ϴ� WriteQueue�� ����
	mReliableWriteQueue.End();

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

BOOL NetworkWrapping::ReadForEventSelect(BYTE* data, DWORD &dataLength)
{
	ThreadSync sync;

	if (!mSocket)
		return FALSE;

	if (!data)
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

	if (SOCKET_ERROR == returnValue && WSA_IO_PENDING != WSAGetLastError() && WSAEWOULDBLOCK != WSAGetLastError()) {
		End();

		return FALSE;
	}

	// ���� ���� ����
	CopyMemory(data, mReadBuffer, readBytes);
	// ���� ������ ���̵� �־��ش�.
	dataLength = readBytes;

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

BOOL NetworkWrapping::UdpBind(USHORT port)
{
	ThreadSync sync;

	if (mSocket)
		return FALSE;

	// ����� UDP�ּ�, ��Ʈ�� ����
	SOCKADDR_IN remoteAddressInfo;

	remoteAddressInfo.sin_family = AF_INET;
	remoteAddressInfo.sin_port = htons(port);
	remoteAddressInfo.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	// UDP ��Ĺ�� ����
	mSocket = WSASocket(AF_INET, SOCK_DGRAM, IPPROTO_UDP, NULL, 0, WSA_FLAG_OVERLAPPED);

	if (INVALID_SOCKET == mSocket)
		return FALSE;

	// ������ �ּҿ� bind
	if (SOCKET_ERROR == bind(mSocket, (struct sockaddr*)&remoteAddressInfo, sizeof(SOCKADDR_IN))) {
		End();

		return FALSE;
	}

	// ReliableUDPThread���� ����� �ڵ�� �̺�Ʈ ����
	mReliableUdpThreadDestroyEvent = CreateEvent(0, FALSE, FALSE, 0);
	if (NULL == mReliableUdpThreadDestroyEvent) {
		End();

		return FALSE;
	}

	mReliableUdpThreadStartupEvent = CreateEvent(0, FALSE, FALSE, 0);
	if (NULL == mReliableUdpThreadStartupEvent) {
		End();

		return FALSE;
	}

	mReliableUdpThreadWakeUpEvent = CreateEvent(0, FALSE, FALSE, 0);
	if (NULL == mReliableUdpThreadWakeUpEvent) {
		End();

		return FALSE;
	}

	mReliableUdpWriteCompleteEvent = CreateEvent(0, FALSE, FALSE, 0);
	if (NULL == mReliableUdpWriteCompleteEvent) {
		End();

		return FALSE;
	}

	// ReliagleUDPThread���� ����� ť�� �ʱ�ȭ
	if (!mReliableWriteQueue.Begin()) {
		End();

		return FALSE;
	}

	// ReliableUdpThread����
	DWORD ReliableUdpThreadID = 0;
	mReliableUdpThreadHandle = CreateThread(NULL, 0, ::ReliableUdpThreadCallback, this, 0, &ReliableUdpThreadID);

	WaitForSingleObject(mReliableUdpThreadStartupEvent, INFINITE);

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

BOOL NetworkWrapping::GetLocalIP(WCHAR* pIP)
{
	ThreadSync Sync;

	if (!mSocket)
		return FALSE;

	CHAR	Name[256] = { 0, };

	gethostname(Name, sizeof(Name));

	PHOSTENT host = gethostbyname(Name);
	if (host)
	{
		if (MultiByteToWideChar(CP_ACP, 0, inet_ntoa(*(struct in_addr*)*host->h_addr_list), -1, pIP, 32) > 0)
			return TRUE;
	}

	return FALSE;
}

USHORT NetworkWrapping::GetLocalPort(VOID)
{
	ThreadSync Sync;

	if (!mSocket)
		return 0;

	SOCKADDR_IN Addr;
	ZeroMemory(&Addr, sizeof(Addr));

	INT AddrLength = sizeof(Addr);
	if (getsockname(mSocket, (sockaddr*)&Addr, &AddrLength) != SOCKET_ERROR)
		return ntohs(Addr.sin_port);

	return 0;
}

BOOL NetworkWrapping::InitializeReadFromForIocp(VOID)
{
	ThreadSync Sync;

	if (!mSocket)
		return FALSE;

	WSABUF		WsaBuf;
	DWORD		ReadBytes = 0;
	DWORD		ReadFlag = 0;
	INT			RemoteAddressInfoSize = sizeof(mUdpRemoteInfo);

	WsaBuf.buf = (CHAR*)mReadBuffer;
	WsaBuf.len = MAX_BUFFER_LENGTH;

	INT		ReturnValue = WSARecvFrom(mSocket,
		&WsaBuf,
		1,
		&ReadBytes,
		&ReadFlag,
		(SOCKADDR*)&mUdpRemoteInfo,
		&RemoteAddressInfoSize,
		&mReadOverlapped.overlapped,
		NULL);

	if (ReturnValue == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING && WSAGetLastError() != WSAEWOULDBLOCK)
	{
		End();

		return FALSE;
	}

	return TRUE;
}

BOOL NetworkWrapping::ReadFromForIocp(LPSTR remoteAddress, USHORT &remotePort, BYTE *data, DWORD &dataLength)
{
	ThreadSync Sync;

	if (!mSocket)
		return FALSE;

	if (!data || dataLength <= 0)
		return FALSE;

	memcpy(data, mReadBuffer, dataLength);

	//memcpy(remoteAddress, inet_ntoa(mUdpRemoteInfo.sin_addr), 32);
	strcpy(remoteAddress, inet_ntoa(mUdpRemoteInfo.sin_addr));
	remotePort = ntohs(mUdpRemoteInfo.sin_port);

	USHORT Ack = 0;
	memcpy(&Ack, mReadBuffer, sizeof(USHORT));

	if (Ack == 9999)
	{
		SetEvent(mReliableUdpWriteCompleteEvent);

		return FALSE;
	}
	else
	{
		Ack = 9999;
		WriteTo2(remoteAddress, remotePort, (BYTE*)&Ack, sizeof(USHORT));
	}

	return TRUE;
}

BOOL NetworkWrapping::ReadFromForEventSelect(LPSTR remoteAddress, USHORT &remotePort, BYTE *data, DWORD &dataLength)
{
	ThreadSync Sync;

	if (!mSocket)
		return FALSE;

	if (!data)
		return FALSE;

	if (!mSocket)
		return FALSE;

	WSABUF		WsaBuf;
	DWORD		ReadBytes = 0;
	DWORD		ReadFlag = 0;
	INT			RemoteAddressInfoSize = sizeof(mUdpRemoteInfo);

	WsaBuf.buf = (CHAR*)mReadBuffer;
	WsaBuf.len = MAX_BUFFER_LENGTH;

	INT		ReturnValue = WSARecvFrom(mSocket,
		&WsaBuf,
		1,
		&ReadBytes,
		&ReadFlag,
		(SOCKADDR*)&mUdpRemoteInfo,
		&RemoteAddressInfoSize,
		&mReadOverlapped.overlapped,
		NULL);

	if (ReturnValue == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING && WSAGetLastError() != WSAEWOULDBLOCK)
	{
		End();

		return FALSE;
	}

	memcpy(data, mReadBuffer, ReadBytes);
	dataLength = ReadBytes;

	//memcpy(remoteAddress, inet_ntoa(mUdpRemoteInfo.sin_addr), 32);
	strcpy(remoteAddress, inet_ntoa(mUdpRemoteInfo.sin_addr));
	remotePort = ntohs(mUdpRemoteInfo.sin_port);

	USHORT Ack = 0;
	memcpy(&Ack, mReadBuffer, sizeof(USHORT));

	if (Ack == 9999)
	{
		SetEvent(mReliableUdpWriteCompleteEvent);

		return FALSE;
	}
	else
	{
		Ack = 9999;
		WriteTo2(remoteAddress, remotePort, (BYTE*)&Ack, sizeof(USHORT));
	}

	return TRUE;
}

BOOL NetworkWrapping::WriteTo(LPCSTR remoteAddress, USHORT remotePort, BYTE *data, DWORD dataLength)
{
	ThreadSync Sync;

	if (!mSocket)
		return FALSE;

	if (!remoteAddress || remotePort <= 0 || !data || dataLength <= 0)
		return FALSE;

	if (!mReliableWriteQueue.Push(this, data, dataLength, remoteAddress, remotePort))
		return FALSE;

	if (!mIsReliableUdpSending)
	{
		mIsReliableUdpSending = TRUE;
		SetEvent(mReliableUdpThreadWakeUpEvent);
	}

	return TRUE;
}

BOOL NetworkWrapping::WriteTo2(LPSTR remoteAddress, USHORT remotePort, BYTE *data, DWORD dataLength)
{
	ThreadSync Sync;

	if (!mSocket)
		return FALSE;

	if (!remoteAddress || remotePort <= 0 || !data || dataLength <= 0)
		return FALSE;

	WSABUF		WsaBuf;
	DWORD		WriteBytes = 0;
	DWORD		WriteFlag = 0;

	SOCKADDR_IN	RemoteAddressInfo;
	INT			RemoteAddressInfoSize = sizeof(RemoteAddressInfo);

	WsaBuf.buf = (CHAR*)data;
	WsaBuf.len = dataLength;

	RemoteAddressInfo.sin_family = AF_INET;
	RemoteAddressInfo.sin_addr.S_un.S_addr = inet_addr(remoteAddress);
	RemoteAddressInfo.sin_port = htons(remotePort);

	INT		ReturnValue = WSASendTo(mSocket,
		&WsaBuf,
		1,
		&WriteBytes,
		WriteFlag,
		(SOCKADDR*)&RemoteAddressInfo,
		RemoteAddressInfoSize,
		&mWriteOverlapped.overlapped,
		NULL);

	if (ReturnValue == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING && WSAGetLastError() != WSAEWOULDBLOCK)
	{
		End();

		return FALSE;
	}

	return TRUE;
}

BOOL NetworkWrapping::GetRemoteAddressAfterAccept(LPTSTR remoteAddress, USHORT &remotePort)
{
	ThreadSync Sync;

	if (!remoteAddress)
		return FALSE;

	sockaddr_in		*Local = NULL;
	INT				LocalLength = 0;

	sockaddr_in		*Remote = NULL;
	INT				RemoteLength = 0;

	GetAcceptExSockaddrs(mReadBuffer,
		0,
		sizeof(sockaddr_in) + 16,
		sizeof(sockaddr_in) + 16,
		(sockaddr **)&Local,
		&LocalLength,
		(sockaddr **)&Remote,
		&RemoteLength);

	//_tcscpy(remoteAddress, (LPTSTR)(inet_ntoa(Remote->sin_addr)));
	CHAR	TempRemoteAddress[32] = { 0, };
	strcpy(TempRemoteAddress, inet_ntoa(Remote->sin_addr));

	MultiByteToWideChar(CP_ACP,
		0,
		TempRemoteAddress,
		-1,
		remoteAddress,
		32);

	remotePort = ntohs(Remote->sin_port);

	return TRUE;
}

SOCKET NetworkWrapping::GetSocket(VOID)
{
	ThreadSync Sync;

	return mSocket;
}