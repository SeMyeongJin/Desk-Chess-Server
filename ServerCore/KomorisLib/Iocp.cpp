#include "stdafx.h"
#include "Iocp.h"


DWORD WINAPI WorkerThreadCallback(LPVOID parameter)
{
	CIocp *owner = (CIocp*)parameter;
	owner->WorkerThreadCallback();

	return 0;
}

CIocp::CIocp(VOID)
{
	mIocpHandle = NULL; // IOCP �����ϴ� �ڵ� ��
	mWorkerThreadCount = 0; // ����� WorkerThread ����

	mStartupEventHandle = NULL; // ������ �����ϴ� �̺�Ʈ
}

CIocp::~CIocp(VOID)
{
}

BOOL CIocp::Begin(VOID)
{
	mIocpHandle = NULL;

	// �ý����� ������ �����ɴϴ�.
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

BOOL CIocp::End(VOID)
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

BOOL CIocp::RegisterSocketToIocp(SOCKET socket, ULONG_PTR completionKey)
{
	if (!socket || !completionKey)
		return FALSE;

	mIocpHandle = CreateIoCompletionPort((HANDLE)socket, mIocpHandle, completionKey, 0);

	if (!mIocpHandle)
		return FALSE;

	return TRUE;
}

VOID CIocp::WorkerThreadCallback(VOID)
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
			INFINITE); // ������ ���

					   // Ű�� NULL�� ��� ����
		if (!completionKey)
			return;

		// Overlapped �����͸� �޾� OverlappedEx�� �����͸� Ȯ��. 
		overlappedEx = (OVERLAPPED_EX*)overlapped;
		// NetworkSession���� Object�� This�� �־��־� ��ü�� Ȯ���� �� �ִ�.
		object = overlappedEx->object;

		// GQCS�� ����� �����̰ų� ���������� ���� ũ�Ⱑ 0�� ���� 
		if (!successed || (successed && !numberOfByteTransfered))
		{
			// Accept�� ����
			if (overlappedEx->ioType == IO_ACCEPT)
				OnIoConnected(object);
			// ������ ����� ���
			else
				OnIoDisconnected(object);

			continue;
		}

		// Ÿ�Կ� �°� ������ OverlappedEx�� �־��ִ� ��. �����͸� �޾��� ���� ���� ��
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