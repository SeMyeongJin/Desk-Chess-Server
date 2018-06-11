#include "stdafx.h"
#include "CriticalSection.h"
#include "MultiThreadSync.h"
#include "MemoryPool.h"
#include "Stream.h"


CStream::CStream(VOID)
{
	mBufferPointer = NULL;
	mLength = 0;
}


CStream::~CStream(VOID)
{
}


// �ܺ��� ���۸� �����Ͽ� �����͸� �ֽ��ϴ�.
BOOL CStream::SetBuffer(BYTE *buffer)
{
	if (!buffer)
		return FALSE;

	mBufferPointer = buffer;
	mLength = 0;

	return TRUE;
}

BOOL CStream::SetBuffer()
{
	if (!mBufferPointer)
		return FALSE;

	mLength = 0;

	return TRUE;
}

// INT32�� �����͸� ���� �� ����մϴ�.
BOOL CStream::ReadInt32(INT *data)
{
	CopyMemory(data, mBufferPointer + mLength, sizeof(INT));

	mLength += sizeof(INT);

	return TRUE;
}

// DWORD�� �����͸� ���� �� ����մϴ�.
BOOL CStream::ReadDWORD(DWORD *data)
{
	CopyMemory(data, mBufferPointer + mLength, sizeof(DWORD));

	mLength += sizeof(DWORD);

	return TRUE;
}

// DWORD_PTR�� �����͸� ���� �� ����մϴ�.
BOOL CStream::ReadDWORD_PTR(DWORD_PTR *data)
{
	CopyMemory(data, mBufferPointer + mLength, sizeof(DWORD_PTR));

	mLength += sizeof(DWORD_PTR);

	return TRUE;
}

// BYTE�� �����͸� ���� �� ����մϴ�.
BOOL CStream::ReadByte(BYTE *data)
{
	CopyMemory(data, mBufferPointer + mLength, sizeof(BYTE));

	mLength += sizeof(BYTE);

	return TRUE;
}

// BYTE �迭�� �����͸� ���� �� ����մϴ�.
BOOL CStream::ReadBytes(BYTE *data, DWORD length)
{
	CopyMemory(data, mBufferPointer + mLength, length);

	mLength += length;

	return TRUE;
}

// FLOAT�� �����͸� ���� �� ����մϴ�.
BOOL CStream::ReadFloat(FLOAT *data)
{
	CopyMemory(data, mBufferPointer + mLength, sizeof(FLOAT));

	mLength += sizeof(FLOAT);

	return TRUE;
}

// INT64�� �����͸� ���� �� ����մϴ�.
BOOL CStream::ReadInt64(INT64 *data)
{
	CopyMemory(data, mBufferPointer + mLength, sizeof(INT64));

	mLength += sizeof(INT64);

	return TRUE;
}

// USHORT�� �����͸� ���� �� ����մϴ�.
BOOL CStream::ReadUSHORT(USHORT *data)
{
	CopyMemory(data, mBufferPointer + mLength, sizeof(USHORT));

	mLength += sizeof(USHORT);

	return TRUE;
}

// SHORT�� �����͸� ���� �� ����մϴ�.
BOOL CStream::ReadSHORT(SHORT *data)
{
	CopyMemory(data, mBufferPointer + mLength, sizeof(SHORT));

	mLength += sizeof(SHORT);

	return TRUE;
}

// BOOL�� �����͸� ���� �� ����մϴ�.
BOOL CStream::ReadBOOL(BOOL *data)
{
	CopyMemory(data, mBufferPointer + mLength, sizeof(BOOL));

	mLength += sizeof(BOOL);

	return TRUE;
}

BOOL CStream::ReadWCHAR(WCHAR *data)
{
	CopyMemory(data, mBufferPointer + mLength, sizeof(WCHAR));

	mLength += sizeof(WCHAR);

	return TRUE;
}

BOOL CStream::ReadWCHARs(LPWSTR data, DWORD length)
{
	CopyMemory(data, mBufferPointer + mLength, length * sizeof(WCHAR));

	mLength += length * sizeof(WCHAR);

	return TRUE;
}

// INT32�� �����͸� �� �� ����մϴ�.
BOOL CStream::WriteInt32(INT *data)
{
	CopyMemory(mBufferPointer + mLength, &data, sizeof(INT));

	mLength += sizeof(INT);

	return TRUE;
}

// DWORD�� �����͸� �� �� ����մϴ�.
BOOL CStream::WriteDWORD(DWORD *data)
{
	CopyMemory(mBufferPointer + mLength, &data, sizeof(DWORD));

	mLength += sizeof(DWORD);

	return TRUE;
}

// DWORD_PTR�� �����͸� �� �� ����մϴ�.
BOOL CStream::WriteDWORD_PTR(DWORD_PTR *data)
{
	CopyMemory(mBufferPointer + mLength, &data, sizeof(DWORD_PTR));

	mLength += sizeof(DWORD_PTR);

	return TRUE;
}

// BYTE�� �����͸� �� �� ����մϴ�.
BOOL CStream::WriteByte(BYTE *data)
{
	CopyMemory(mBufferPointer + mLength, &data, sizeof(BYTE));

	mLength += sizeof(BYTE);

	return TRUE;
}

// BYTE �迭�� �����͸� �� �� ����մϴ�.
BOOL CStream::WriteBytes(BYTE *data, DWORD length)
{
	CopyMemory(mBufferPointer + mLength, &data, length);

	mLength += length;

	return TRUE;
}

// FLOAT�� �����͸� �� �� ����մϴ�.
BOOL CStream::WriteFloat(FLOAT *data)
{
	CopyMemory(mBufferPointer + mLength, &data, sizeof(FLOAT));

	mLength += sizeof(FLOAT);

	return TRUE;
}

// INT64�� �����͸� �� �� ����մϴ�.
BOOL CStream::WriteInt64(INT64 *data)
{
	CopyMemory(mBufferPointer + mLength, &data, sizeof(INT64));

	mLength += sizeof(INT64);

	return TRUE;
}

// USHORT�� �����͸� �� �� ����մϴ�.
BOOL CStream::WriteUSHORT(USHORT *data)
{
	CopyMemory(mBufferPointer + mLength, &data, sizeof(USHORT));

	mLength += sizeof(USHORT);

	return TRUE;
}

// SHORT�� �����͸� �� �� ����մϴ�.
BOOL CStream::WriteSHORT(SHORT *data)
{
	CopyMemory(mBufferPointer + mLength, &data, sizeof(SHORT));

	mLength += sizeof(SHORT);

	return TRUE;
}

// BOOL�� �����͸� �� �� ����մϴ�.
BOOL CStream::WriteBOOL(BOOL *data)
{
	CopyMemory(mBufferPointer + mLength, &data, sizeof(BOOL));

	mLength += sizeof(BOOL);

	return TRUE;
}

BOOL CStream::WriteWCHAR(WCHAR data)
{
	CopyMemory(mBufferPointer + mLength, &data, sizeof(WCHAR));

	mLength += sizeof(WCHAR);

	return TRUE;
}

BOOL CStream::WriteWCHARs(LPCWSTR data, DWORD length)
{
	CopyMemory(mBufferPointer + mLength, data, length * sizeof(WCHAR));

	mLength += length * sizeof(WCHAR);

	return TRUE;
}

// ��ü ���̸� �޾ƿ� �� ����մϴ�.
DWORD CStream::GetLength(VOID)
{
	return mLength;
}