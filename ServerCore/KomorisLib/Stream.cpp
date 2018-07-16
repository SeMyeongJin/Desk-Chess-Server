#include "stdafx.h"
#include "CriticalSection.h"
#include "MultiThreadSync.h"
#include "MemoryPool.h"
#include "Stream.h"


Stream::Stream(VOID)
{
	mBufferPointer = NULL;
	mLength = 0;
}


Stream::~Stream(VOID)
{
}


// �ܺ��� ���۸� �����Ͽ� �����͸� �ֽ��ϴ�.
BOOL Stream::SetBuffer(BYTE *buffer)
{
	if (!buffer)
		return FALSE;

	mBufferPointer = buffer;
	mLength = 0;

	return TRUE;
}

BOOL Stream::SetBuffer()
{
	if (!mBufferPointer)
		return FALSE;

	mLength = 0;

	return TRUE;
}

// INT32�� �����͸� ���� �� ����մϴ�.
BOOL Stream::ReadInt32(INT *data)
{
	CopyMemory(data, mBufferPointer + mLength, sizeof(INT));

	mLength += sizeof(INT);

	return TRUE;
}

// DWORD�� �����͸� ���� �� ����մϴ�.
BOOL Stream::ReadDWORD(DWORD *data)
{
	CopyMemory(data, mBufferPointer + mLength, sizeof(DWORD));

	mLength += sizeof(DWORD);

	return TRUE;
}

// DWORD_PTR�� �����͸� ���� �� ����մϴ�.
BOOL Stream::ReadDWORD_PTR(DWORD_PTR *data)
{
	CopyMemory(data, mBufferPointer + mLength, sizeof(DWORD_PTR));

	mLength += sizeof(DWORD_PTR);

	return TRUE;
}

// BYTE�� �����͸� ���� �� ����մϴ�.
BOOL Stream::ReadByte(BYTE *data)
{
	CopyMemory(data, mBufferPointer + mLength, sizeof(BYTE));

	mLength += sizeof(BYTE);

	return TRUE;
}

// BYTE �迭�� �����͸� ���� �� ����մϴ�.
BOOL Stream::ReadBytes(BYTE *data, DWORD length)
{
	CopyMemory(data, mBufferPointer + mLength, length);

	mLength += length;

	return TRUE;
}

// FLOAT�� �����͸� ���� �� ����մϴ�.
BOOL Stream::ReadFloat(FLOAT *data)
{
	CopyMemory(data, mBufferPointer + mLength, sizeof(FLOAT));

	mLength += sizeof(FLOAT);

	return TRUE;
}

// INT64�� �����͸� ���� �� ����մϴ�.
BOOL Stream::ReadInt64(INT64 *data)
{
	CopyMemory(data, mBufferPointer + mLength, sizeof(INT64));

	mLength += sizeof(INT64);

	return TRUE;
}

// USHORT�� �����͸� ���� �� ����մϴ�.
BOOL Stream::ReadUSHORT(USHORT *data)
{
	CopyMemory(data, mBufferPointer + mLength, sizeof(USHORT));

	mLength += sizeof(USHORT);

	return TRUE;
}

// SHORT�� �����͸� ���� �� ����մϴ�.
BOOL Stream::ReadSHORT(SHORT *data)
{
	CopyMemory(data, mBufferPointer + mLength, sizeof(SHORT));

	mLength += sizeof(SHORT);

	return TRUE;
}

// BOOL�� �����͸� ���� �� ����մϴ�.
BOOL Stream::ReadBOOL(BOOL *data)
{
	CopyMemory(data, mBufferPointer + mLength, sizeof(BOOL));

	mLength += sizeof(BOOL);

	return TRUE;
}

BOOL Stream::ReadWCHAR(WCHAR *data)
{
	CopyMemory(data, mBufferPointer + mLength, sizeof(WCHAR));

	mLength += sizeof(WCHAR);

	return TRUE;
}

BOOL Stream::ReadWCHARs(LPWSTR data, DWORD length)
{
	CopyMemory(data, mBufferPointer + mLength, length * sizeof(WCHAR));

	mLength += length * sizeof(WCHAR);

	return TRUE;
}

// INT32�� �����͸� �� �� ����մϴ�.
BOOL Stream::WriteInt32(INT data)
{
	CopyMemory(mBufferPointer + mLength, &data, sizeof(INT));

	mLength += sizeof(INT);

	return TRUE;
}

// DWORD�� �����͸� �� �� ����մϴ�.
BOOL Stream::WriteDWORD(DWORD data)
{
	CopyMemory(mBufferPointer + mLength, &data, sizeof(DWORD));

	mLength += sizeof(DWORD);

	return TRUE;
}

// DWORD_PTR�� �����͸� �� �� ����մϴ�.
BOOL Stream::WriteDWORD_PTR(DWORD_PTR data)
{
	CopyMemory(mBufferPointer + mLength, &data, sizeof(DWORD_PTR));

	mLength += sizeof(DWORD_PTR);

	return TRUE;
}

// BYTE�� �����͸� �� �� ����մϴ�.
BOOL Stream::WriteByte(BYTE data)
{
	CopyMemory(mBufferPointer + mLength, &data, sizeof(BYTE));

	mLength += sizeof(BYTE);

	return TRUE;
}

// BYTE �迭�� �����͸� �� �� ����մϴ�.
BOOL Stream::WriteBytes(BYTE *data, DWORD length)
{
	CopyMemory(mBufferPointer + mLength, data, length);

	mLength += length;

	return TRUE;
}

// FLOAT�� �����͸� �� �� ����մϴ�.
BOOL Stream::WriteFloat(FLOAT data)
{
	CopyMemory(mBufferPointer + mLength, &data, sizeof(FLOAT));

	mLength += sizeof(FLOAT);

	return TRUE;
}

// INT64�� �����͸� �� �� ����մϴ�.
BOOL Stream::WriteInt64(INT64 data)
{
	CopyMemory(mBufferPointer + mLength, &data, sizeof(INT64));

	mLength += sizeof(INT64);

	return TRUE;
}

// USHORT�� �����͸� �� �� ����մϴ�.
BOOL Stream::WriteUSHORT(USHORT data)
{
	CopyMemory(mBufferPointer + mLength, &data, sizeof(USHORT));

	mLength += sizeof(USHORT);

	return TRUE;
}

// SHORT�� �����͸� �� �� ����մϴ�.
BOOL Stream::WriteSHORT(SHORT data)
{
	CopyMemory(mBufferPointer + mLength, &data, sizeof(SHORT));

	mLength += sizeof(SHORT);

	return TRUE;
}

// BOOL�� �����͸� �� �� ����մϴ�.
BOOL Stream::WriteBOOL(BOOL data)
{
	CopyMemory(mBufferPointer + mLength, &data, sizeof(BOOL));

	mLength += sizeof(BOOL);

	return TRUE;
}

BOOL Stream::WriteWCHAR(WCHAR data)
{
	CopyMemory(mBufferPointer + mLength, &data, sizeof(WCHAR));

	mLength += sizeof(WCHAR);

	return TRUE;
}

BOOL Stream::WriteWCHARs(LPCWSTR data, DWORD length)
{
	CopyMemory(mBufferPointer + mLength, data, length * sizeof(WCHAR));

	mLength += length * sizeof(WCHAR);

	return TRUE;
}

// ��ü ���̸� �޾ƿ� �� ����մϴ�.
DWORD Stream::GetLength(VOID)
{
	return mLength;
}