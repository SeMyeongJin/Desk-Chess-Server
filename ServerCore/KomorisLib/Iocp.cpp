#include "stdafx.h"
#include "Iocp.h"


DWORD WINAPI WorkerThreadCallback(LPVOID parameter)
{
	IOCP *owner = (IOCP*)parameter;
	owner->WorkerThreadCallback();

	return 0;
}

IOCP::IOCP(VOID)
{
	mIocpHandle = NULL; 
	mWorkerThreadCount = 0;

	mStartupEventHandle = NULL;
}

IOCP::~IOCP(VOID)
{
}

BOOL IOCP::Begin(VOID)
{
	mIocpHandle = NULL;

	SYSTEM_INFO systemInfo;
	GetSystemInfo(&systemInfo);

	// CPU의 개수의 2배만큼 WorkerThread를 만듭니다.
	mWorkerThreadCount = systemInfo.dwNumberOfProcessors * 2;

	// IOCP 핸들 생성 부분. 연결은 Register 함수에서
	mIocpHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (!mIocpHandle)
		return FALSE;

	mStartupEventHandle = CreateEvent(0, FALSE, FALSE, 0);
	if (mStartupEventHandle == NULL)
	{
		End();

		return FALSE;
	}

	// CPU개수의 2배만큼의 WorkerThread를 생성해서 mWorkerThreadVector로 관리합니다.
	for (DWORD i = 0; i < mWorkerThreadCount; i++)
	{
		HANDLE workerThread = CreateThread(NULL, 0, ::WorkerThreadCallback, this, 0, NULL);
		mWorkerThreadVector.push_back(workerThread);

		WaitForSingleObject(mStartupEventHandle, INFINITE);
	}
	
	return TRUE;
}

BOOL IOCP::End(VOID)
{
	// IOCP를 이용해서 각 WorkerThread마다 종료를 요청합니다
	for (DWORD i = 0; i < mWorkerThreadVector.size(); i++)
		PostQueuedCompletionStatus(mIocpHandle, 0, 0, NULL);

	// 각 WorkerThread가 종료되기를 기다려 핸들을 닫아줍니다
	for (DWORD i = 0; i < mWorkerThreadVector.size(); i++)
	{
		WaitForSingleObject(mWorkerThreadVector[i], INFINITE);

		CloseHandle(mWorkerThreadVector[i]);
	}

	if (mIocpHandle)
		CloseHandle(mIocpHandle);

	mWorkerThreadVector.clear();

	if (mStartupEventHandle)
		CloseHandle(mStartupEventHandle);

	return TRUE;
}

BOOL IOCP::RegisterSocketToIocp(SOCKET socket, ULONG_PTR completionKey)
{
	if (!socket || !completionKey)
		return FALSE;

	mIocpHandle = CreateIoCompletionPort((HANDLE)socket, mIocpHandle, completionKey, 0);

	if (!mIocpHandle)
		return FALSE;

	return TRUE;
}

VOID IOCP::WorkerThreadCallback(VOID)
{
	BOOL				successed = FALSE;
	DWORD				numberOfByteTransfered = 0;
	VOID				*completionKey = NULL;
	OVERLAPPED			*overlapped = NULL;
	OVERLAPPED_EX		*overlappedEx = NULL;
	VOID				*object = NULL;

	while (TRUE)
	{
		// WorkerThread를 시작할 수 있다고 알려주는 이벤트
		SetEvent(mStartupEventHandle);

		successed = GetQueuedCompletionStatus(
			mIocpHandle, // IOCP 핸들
			&numberOfByteTransfered, // IO에 이용된 데이터 크기
			(PULONG_PTR)&completionKey, // 소켓이나 파일의 핸들 값
			&overlapped, // Read, Write 등에 사용된 Overlapped 값
			INFINITE);

		if (!completionKey)
			return;

		// Overlapped 포인터를 받아 OverlappedEx의 포인터를 확인. 
		overlappedEx = (OVERLAPPED_EX*)overlapped;
		// NetworkSession에서 Object에 This를 넣어주어 개체를 확인할 수 있다.
		object = overlappedEx->object;


		if (!successed || (successed && !numberOfByteTransfered))
		{
			if (overlappedEx->ioType == IO_ACCEPT)
				OnIoConnected(object);
			else
				OnIoDisconnected(object);

			continue;
		}

		switch (overlappedEx->ioType)
		{
		case IO_READ:
			OnIoRead(object, numberOfByteTransfered);
			break;

		case IO_WRITE:
			OnIoWrote(object, numberOfByteTransfered);
			break;
		}
	}
}