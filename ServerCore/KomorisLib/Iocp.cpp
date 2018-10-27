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

	// CPU�� ������ 2�踸ŭ WorkerThread�� ����ϴ�.
	mWorkerThreadCount = systemInfo.dwNumberOfProcessors * 2;

	// IOCP �ڵ� ���� �κ�. ������ Register �Լ�����
	mIocpHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (!mIocpHandle)
		return FALSE;

	mStartupEventHandle = CreateEvent(0, FALSE, FALSE, 0);
	if (mStartupEventHandle == NULL)
	{
		End();

		return FALSE;
	}

	// CPU������ 2�踸ŭ�� WorkerThread�� �����ؼ� mWorkerThreadVector�� �����մϴ�.
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
	// IOCP�� �̿��ؼ� �� WorkerThread���� ���Ḧ ��û�մϴ�
	for (DWORD i = 0; i < mWorkerThreadVector.size(); i++)
		PostQueuedCompletionStatus(mIocpHandle, 0, 0, NULL);

	// �� WorkerThread�� ����Ǳ⸦ ��ٷ� �ڵ��� �ݾ��ݴϴ�
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
		// WorkerThread�� ������ �� �ִٰ� �˷��ִ� �̺�Ʈ
		SetEvent(mStartupEventHandle);

		successed = GetQueuedCompletionStatus(
			mIocpHandle, // IOCP �ڵ�
			&numberOfByteTransfered, // IO�� �̿�� ������ ũ��
			(PULONG_PTR)&completionKey, // �����̳� ������ �ڵ� ��
			&overlapped, // Read, Write � ���� Overlapped ��
			INFINITE);

		if (!completionKey)
			return;

		// Overlapped �����͸� �޾� OverlappedEx�� �����͸� Ȯ��. 
		overlappedEx = (OVERLAPPED_EX*)overlapped;
		// NetworkSession���� Object�� This�� �־��־� ��ü�� Ȯ���� �� �ִ�.
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