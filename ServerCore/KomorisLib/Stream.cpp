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


// 외부의 버퍼를 지정하여 포인터를 넣습니다.
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

// INT32형 데이터를 읽을 때 사용합니다.
BOOL Stream::ReadInt32(INT *data)
{
	CopyMemory(data, mBufferPointer + mLength, sizeof(INT));

	mLength += sizeof(INT);

	return TRUE;
}

// DWORD형 데이터를 읽을 때 사용합니다.
BOOL Stream::ReadDWORD(DWORD *data)
{
	CopyMemory(data, mBufferPointer + mLength, sizeof(DWORD));

	mLength += sizeof(DWORD);

	return TRUE;
}

// DWORD_PTR형 데이터를 읽을 때 사용합니다.
BOOL Stream::ReadDWORD_PTR(DWORD_PTR *data)
{
	CopyMemory(data, mBufferPointer + mLength, sizeof(DWORD_PTR));

	mLength += sizeof(DWORD_PTR);

	return TRUE;
}

// BYTE형 데이터를 읽을 때 사용합니다.
BOOL Stream::ReadByte(BYTE *data)
{
	CopyMemory(data, mBufferPointer + mLength, sizeof(BYTE));

	mLength += sizeof(BYTE);

	return TRUE;
}

// BYTE 배열형 데이터를 읽을 때 사용합니다.
BOOL Stream::ReadBytes(BYTE *data, DWORD length)
{
	CopyMemory(data, mBufferPointer + mLength, length);

	mLength += length;

	return TRUE;
}

// FLOAT형 데이터를 읽을 때 사용합니다.
BOOL Stream::ReadFloat(FLOAT *data)
{
	CopyMemory(data, mBufferPointer + mLength, sizeof(FLOAT));

	mLength += sizeof(FLOAT);

	return TRUE;
}

// INT64형 데이터를 읽을 때 사용합니다.
BOOL Stream::ReadInt64(INT64 *data)
{
	CopyMemory(data, mBufferPointer + mLength, sizeof(INT64));

	mLength += sizeof(INT64);

	return TRUE;
}

// USHORT형 데이터를 읽을 때 사용합니다.
BOOL Stream::ReadUSHORT(USHORT *data)
{
	CopyMemory(data, mBufferPointer + mLength, sizeof(USHORT));

	mLength += sizeof(USHORT);

	return TRUE;
}

// SHORT형 데이터를 읽을 때 사용합니다.
BOOL Stream::ReadSHORT(SHORT *data)
{
	CopyMemory(data, mBufferPointer + mLength, sizeof(SHORT));

	mLength += sizeof(SHORT);

	return TRUE;
}

// BOOL형 데이터를 읽을 때 사용합니다.
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

// INT32형 데이터를 쓸 때 사용합니다.
BOOL Stream::WriteInt32(INT data)
{
	CopyMemory(mBufferPointer + mLength, &data, sizeof(INT));

	mLength += sizeof(INT);

	return TRUE;
}

// DWORD형 데이터를 쓸 때 사용합니다.
BOOL Stream::WriteDWORD(DWORD data)
{
	CopyMemory(mBufferPointer + mLength, &data, sizeof(DWORD));

	mLength += sizeof(DWORD);

	return TRUE;
}

// DWORD_PTR형 데이터를 쓸 때 사용합니다.
BOOL Stream::WriteDWORD_PTR(DWORD_PTR data)
{
	CopyMemory(mBufferPointer + mLength, &data, sizeof(DWORD_PTR));

	mLength += sizeof(DWORD_PTR);

	return TRUE;
}

// BYTE형 데이터를 쓸 때 사용합니다.
BOOL Stream::WriteByte(BYTE data)
{
	CopyMemory(mBufferPointer + mLength, &data, sizeof(BYTE));

	mLength += sizeof(BYTE);

	return TRUE;
}

// BYTE 배열형 데이터를 쓸 때 사용합니다.
BOOL Stream::WriteBytes(BYTE *data, DWORD length)
{
	CopyMemory(mBufferPointer + mLength, data, length);

	mLength += length;

	return TRUE;
}

// FLOAT형 데이터를 쓸 때 사용합니다.
BOOL Stream::WriteFloat(FLOAT data)
{
	CopyMemory(mBufferPointer + mLength, &data, sizeof(FLOAT));

	mLength += sizeof(FLOAT);

	return TRUE;
}

// INT64형 데이터를 쓸 때 사용합니다.
BOOL Stream::WriteInt64(INT64 data)
{
	CopyMemory(mBufferPointer + mLength, &data, sizeof(INT64));

	mLength += sizeof(INT64);

	return TRUE;
}

// USHORT형 데이터를 쓸 때 사용합니다.
BOOL Stream::WriteUSHORT(USHORT data)
{
	CopyMemory(mBufferPointer + mLength, &data, sizeof(USHORT));

	mLength += sizeof(USHORT);

	return TRUE;
}

// SHORT형 데이터를 쓸 때 사용합니다.
BOOL Stream::WriteSHORT(SHORT data)
{
	CopyMemory(mBufferPointer + mLength, &data, sizeof(SHORT));

	mLength += sizeof(SHORT);

	return TRUE;
}

// BOOL형 데이터를 쓸 때 사용합니다.
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

// 전체 길이를 받아올 때 사용합니다.
DWORD Stream::GetLength(VOID)
{
	return mLength;
}