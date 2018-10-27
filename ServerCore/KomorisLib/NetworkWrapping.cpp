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
	// Accept 관련 Overlapped구조체
	ZeroMemory(&mAcceptOverlapped, sizeof(mAcceptOverlapped));
	// Read 관련 Overlapped구조체
	ZeroMemory(&mReadOverlapped, sizeof(mReadOverlapped));
	// Write 관련 Overlapped구조체
	ZeroMemory(&mWriteOverlapped, sizeof(mWriteOverlapped));
	// Read에서 사용하는 버퍼
	ZeroMemory(mReadBuffer, sizeof(mReadBuffer));
	// UDP를 사용할 때 상대 주소 정보
	ZeroMemory(&mUdpRemoteInfo, sizeof(mUdpRemoteInfo));

	mSocket = NULL;

	mReliableUdpThreadHandle = NULL;	// 패킷을 받았다는 신호가 오기 전까지 동일 패킷을 계속전송해줄 스래드 핸들
	mReliableUdpThreadStartupEvent = NULL;	// 스레드 시작 이벤트
	mReliableUdpThreadDestroyEvent = NULL;	// 스레드 종료 이벤트
	mReliableUdpThreadWakeUpEvent = NULL;	// 스레드 재시작 이벤트
	mReliableUdpWriteCompleteEvent = NULL;	// 패킷을 더이상 보낼 필요가 없을때 날리는 이벤트
	mIsReliableUdpSending = FALSE;

	mAcceptOverlapped.ioType = IO_ACCEPT;// Overlapped 구조체의 종류를 정의
	mReadOverlapped.ioType = IO_READ;	// Overlapped 구조체의 종류를 정의
	mWriteOverlapped.ioType = IO_WRITE;	// Overlapped 구조체의 종류를 정의

	mAcceptOverlapped.object = this;	// 현재 개체의 포인터를 가지고 있습니다.
	mReadOverlapped.object = this;	// 현재 개체의 포인터를 가지고 있습니다.
	mWriteOverlapped.object = this;	// 현재 개체의 포인터를 가지고 있습니다.
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
			// Write 일 경우 Pop을 해주고
			if (mReliableWriteQueue.Pop(&Object, Data, DataLength, RemoteAddress, RemotePort))
			{
				// 데이터가 있을 경우
				// 실제 Write를 해 주고 WaitForSingleObject를 해 준다.
				// 받았을때 그 SetEvent를 해 주면 풀린다.
			RETRY:
				if (!WriteTo2(RemoteAddress, RemotePort, Data, DataLength))
					return;

				DWORD Result = WaitForSingleObject(mReliableUdpWriteCompleteEvent, 10);

				// 받았을 경우 다음 데이터 처리
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
	ThreadSync sync; // 다중 스레드 동기화

	if (mSocket) // 실행중인 소켓이 있으면 실패
		return FALSE;

	ZeroMemory(mReadBuffer, sizeof(mReadBuffer));
	ZeroMemory(&mUdpRemoteInfo, sizeof(mUdpRemoteInfo));

	mSocket = NULL;

	mReliableUdpThreadHandle = NULL;
	mReliableUdpThreadStartupEvent = NULL;
	mReliableUdpThreadDestroyEvent = NULL;
	mReliableUdpThreadWakeUpEvent = NULL;
	mReliableUdpWriteCompleteEvent = NULL;

	mIsReliableUdpSending = FALSE; // 현재 데이터를 전송중인지 확인하는 FLAG 변수

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

	if (mReliableUdpThreadHandle) {
		// 스레드 종료 이벤트 생성
		SetEvent(mReliableUdpThreadDestroyEvent);

		// 스레드 종료시까지 무한대기
		// INFINITE가 아닌 3~5초 대기후 TerminateThread를 이용해 강제 종료해도 됨
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

	// Reliable UDP로 보낼 데이터를 저장하는 WriteQueue를 비운다
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

	// 받은 내용 복사
	CopyMemory(data, mReadBuffer, readBytes);
	// 받은 데이터 길이도 넣어준다.
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

BOOL NetworkWrapping::UdpBind(USHORT port)
{
	ThreadSync sync;

	if (mSocket)
		return FALSE;

	// 사용할 UDP주소, 포트를 실행
	SOCKADDR_IN remoteAddressInfo;

	remoteAddressInfo.sin_family = AF_INET;
	remoteAddressInfo.sin_port = htons(port);
	remoteAddressInfo.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	// UDP 소캣을 생성
	mSocket = WSASocket(AF_INET, SOCK_DGRAM, IPPROTO_UDP, NULL, 0, WSA_FLAG_OVERLAPPED);

	if (INVALID_SOCKET == mSocket)
		return FALSE;

	// 설정된 주소에 bind
	if (SOCKET_ERROR == bind(mSocket, (struct sockaddr*)&remoteAddressInfo, sizeof(SOCKADDR_IN))) {
		End();

		return FALSE;
	}

	// ReliableUDPThread에서 사용할 핸들과 이벤트 생성
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

	// ReliagleUDPThread에서 사용할 큐를 초기화
	if (!mReliableWriteQueue.Begin()) {
		End();

		return FALSE;
	}

	// ReliableUdpThread생성
	DWORD ReliableUdpThreadID = 0;
	mReliableUdpThreadHandle = CreateThread(NULL, 0, ::ReliableUdpThreadCallback, this, 0, &ReliableUdpThreadID);

	WaitForSingleObject(mReliableUdpThreadStartupEvent, INFINITE);

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