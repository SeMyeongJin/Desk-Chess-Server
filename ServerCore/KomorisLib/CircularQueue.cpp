#include "stdafx.h"
#include "CriticalSection.h"
#include "MultiThreadSync.h"
#include "CircularQueue.h"

CircularQueue::CircularQueue(VOID)
{
	memset(mQueue, 0, sizeof(mQueue));

	mQueueHead = 0;
	mQueueTail = 0;
}

CircularQueue::~CircularQueue(VOID)
{
}

BOOL CircularQueue::Begin(VOID)
{
	memset(mQueue, 0, sizeof(mQueue));

	mQueueHead = 0;
	mQueueTail = 0;

	return TRUE;
}

BOOL CircularQueue::End(VOID)
{
	return TRUE;
}

BYTE* CircularQueue::Push(VOID *object, BYTE *data, DWORD dataLength)
{
	ThreadSync Sync;

	if (!object || !data)
		return NULL;

	DWORD TempTail = (mQueueTail + 1) % MAX_QUEUE_LENGTH;

	if (TempTail == mQueueHead)
		return NULL;

	mQueue[TempTail].Object = object;
	mQueue[TempTail].DataLength = dataLength;

	memcpy(mQueue[TempTail].Data, data, dataLength);

	mQueueTail = TempTail;

	return mQueue[TempTail].Data;
}

BYTE* CircularQueue::Push(VOID *object, DWORD protocol, BYTE *data, DWORD dataLength)
{
	ThreadSync Sync;

	if (!object || !data)
		return NULL;

	DWORD TempTail = (mQueueTail + 1) % MAX_QUEUE_LENGTH;

	if (TempTail == mQueueHead)
		return NULL;

	mQueue[TempTail].Object = object;
	mQueue[TempTail].DataLength = dataLength;
	mQueue[TempTail].Protocol = protocol;

	memset(mQueue[TempTail].Data, 0, sizeof(mQueue[TempTail].Data));
	memcpy(mQueue[TempTail].Data, data, dataLength);

	mQueueTail = TempTail;

	return mQueue[TempTail].Data;
}

BYTE *CircularQueue::Push(VOID *object, BYTE *data, DWORD dataLength, LPCSTR remoteAddress, USHORT remotePort)
{
	ThreadSync Sync;

	if (!object || !data)
		return NULL;

	DWORD TempTail = (mQueueTail + 1) % MAX_QUEUE_LENGTH;

	if (TempTail == mQueueHead)
		return NULL;

	mQueue[TempTail].Object = object;
	mQueue[TempTail].DataLength = dataLength;
	mQueue[TempTail].RemotePort = remotePort;

	memset(mQueue[TempTail].RemoteAddress, 0, sizeof(mQueue[TempTail].RemoteAddress));
	strncpy(mQueue[TempTail].RemoteAddress, remoteAddress, sizeof(mQueue[TempTail].RemoteAddress));

	memset(mQueue[TempTail].Data, 0, sizeof(mQueue[TempTail].Data));
	memcpy(mQueue[TempTail].Data, data, dataLength);

	mQueueTail = TempTail;

	return mQueue[TempTail].Data;
}

BYTE *CircularQueue::Push(VOID *object, DWORD protocol, BYTE *data, DWORD dataLength, LPCSTR remoteAddress, USHORT remotePort)
{
	ThreadSync Sync;

	if (!object || !data)
		return NULL;

	DWORD TempTail = (mQueueTail + 1) % MAX_QUEUE_LENGTH;

	if (TempTail == mQueueHead)
		return NULL;

	mQueue[TempTail].Object = object;
	mQueue[TempTail].DataLength = dataLength;
	mQueue[TempTail].RemotePort = remotePort;
	mQueue[TempTail].Protocol = protocol;

	memset(mQueue[TempTail].RemoteAddress, 0, sizeof(mQueue[TempTail].RemoteAddress));
	strncpy(mQueue[TempTail].RemoteAddress, remoteAddress, sizeof(mQueue[TempTail].RemoteAddress));

	memset(mQueue[TempTail].Data, 0, sizeof(mQueue[TempTail].Data));
	memcpy(mQueue[TempTail].Data, data, dataLength);

	mQueueTail = TempTail;

	return mQueue[TempTail].Data;
}

BOOL CircularQueue::Pop(VOID)
{
	ThreadSync Sync;

	if (mQueueHead == mQueueTail)
		return FALSE;

	DWORD TempHead = (mQueueHead + 1) % MAX_QUEUE_LENGTH;

	mQueueHead = TempHead;

	return TRUE;
}

BOOL CircularQueue::Pop(VOID **object, BYTE *data, DWORD &dataLength)
{
	ThreadSync Sync;

	if (!object || !data)
		return FALSE;

	if (mQueueHead == mQueueTail)
		return FALSE;

	DWORD TempHead = (mQueueHead + 1) % MAX_QUEUE_LENGTH;

	dataLength = mQueue[TempHead].DataLength;
	*object = mQueue[TempHead].Object;

	memcpy(data, mQueue[TempHead].Data, mQueue[TempHead].DataLength);

	mQueueHead = TempHead;

	return TRUE;
}

BOOL CircularQueue::Pop(VOID **object, DWORD &protocol, BYTE *data, DWORD &dataLength)
{
	ThreadSync Sync;

	if (!object || !data)
		return FALSE;

	if (mQueueHead == mQueueTail)
		return FALSE;

	DWORD TempHead = (mQueueHead + 1) % MAX_QUEUE_LENGTH;

	dataLength = mQueue[TempHead].DataLength;
	*object = mQueue[TempHead].Object;
	protocol = mQueue[TempHead].Protocol;

	memcpy(data, mQueue[TempHead].Data, mQueue[TempHead].DataLength);

	mQueueHead = TempHead;

	return TRUE;
}

BOOL CircularQueue::Pop(VOID **object, BYTE *data, DWORD &dataLength, LPSTR remoteAddress, USHORT &remotePort)
{
	ThreadSync Sync;

	if (!object || !data)
		return FALSE;

	if (mQueueHead == mQueueTail)
		return FALSE;

	DWORD TempHead = (mQueueHead + 1) % MAX_QUEUE_LENGTH;

	dataLength = mQueue[TempHead].DataLength;
	*object = mQueue[TempHead].Object;
	remotePort = mQueue[TempHead].RemotePort;

	strncpy(remoteAddress, mQueue[TempHead].RemoteAddress, sizeof(mQueue[TempHead].RemoteAddress));
	memcpy(data, mQueue[TempHead].Data, mQueue[TempHead].DataLength);

	mQueueHead = TempHead;

	return TRUE;
}

BOOL CircularQueue::Pop(VOID **object, DWORD &protocol, BYTE *data, DWORD &dataLength, LPSTR remoteAddress, USHORT &remotePort)
{
	ThreadSync Sync;

	if (!object || !data)
		return FALSE;

	if (mQueueHead == mQueueTail)
		return FALSE;

	DWORD TempHead = (mQueueHead + 1) % MAX_QUEUE_LENGTH;

	dataLength = mQueue[TempHead].DataLength;
	*object = mQueue[TempHead].Object;
	remotePort = mQueue[TempHead].RemotePort;
	protocol = mQueue[TempHead].Protocol;

	strncpy(remoteAddress, mQueue[TempHead].RemoteAddress, sizeof(mQueue[TempHead].RemoteAddress));
	memcpy(data, mQueue[TempHead].Data, mQueue[TempHead].DataLength);

	mQueueHead = TempHead;

	return TRUE;
}

BOOL CircularQueue::IsEmpty(VOID)
{
	ThreadSync Sync;

	if (mQueueHead == mQueueTail)
		return TRUE;

	return FALSE;
}
