#include "stdafx.h"
#include "CriticalSection.h"
#include "MultiThreadSync.h"
#include "CircularQueue.h"
#include "NetworkWrapping.h"
#include "PacketWrapping.h"

PacketWrapping::PacketWrapping(VOID)
{
	// 패킷 확인을 위해서 저장하는 mPacketBuffer
	ZeroMemory(mPacketBuffer, sizeof(mPacketBuffer));

	mRemainLength = 0; // mPacketBuffer에 남은 길이
	mCurrentPacketNumber = 0; // 현재의 상대에서 보낼 때 사용할 패킷 번호

	mLastReadPacketNumber = 0; // 마지막으로 받은 패킷 번호
}

PacketWrapping::~PacketWrapping(VOID)
{
}

BOOL PacketWrapping::Begin(VOID)
{
	ThreadSync sync;

	memset(mPacketBuffer, 0, sizeof(mPacketBuffer));

	mRemainLength = 0;
	mCurrentPacketNumber = 0;
	mLastReadPacketNumber = 0;

	if (!WriteQueue.Begin())
		return FALSE;

	return NetworkWrapping::Begin();
}

BOOL PacketWrapping::End(VOID)
{
	ThreadSync sync;

	mLastReadPacketInfoVectorForUdp.clear();

	if (!WriteQueue.End())
		return FALSE;

	return NetworkWrapping::End();
}

// 받은 패킷을 분석하는 함수
BOOL PacketWrapping::GetPacket(DWORD &protocol, BYTE *packet, DWORD &packetLength)
{
	ThreadSync sync;

	if (!packet)
		return FALSE;

	if (mRemainLength < sizeof(DWORD))
		return FALSE;

	INT PacketLength = 0;
	CopyMemory(&PacketLength, mPacketBuffer, sizeof(INT));

	if (PacketLength > MAX_BUFFER_LENGTH || PacketLength <= 0) // Invalid Packet
	{
		mRemainLength = 0;
		return FALSE;
	}

	if (PacketLength <= mRemainLength)
	{
		DWORD PacketNumber = 0;
		DWORD Protocol = 0;

		CopyMemory(&PacketNumber, mPacketBuffer + sizeof(DWORD)/*LENGTH*/, sizeof(DWORD));
		CopyMemory(&Protocol, mPacketBuffer +
			sizeof(DWORD)/*LENGTH*/ +
			sizeof(DWORD)/*PACKETNUMBER*/, sizeof(DWORD));

		protocol = Protocol;
		packetLength = PacketLength -
			sizeof(DWORD)/*LENGTH*/ -
			sizeof(DWORD)/*PACKETNUMBER*/ -
			sizeof(DWORD)/*PROTOCOL*/;

		CopyMemory(packet, mPacketBuffer +
			sizeof(DWORD)/*LENGTH*/ +
			sizeof(DWORD)/*PACKETNUMBER*/ +
			sizeof(DWORD)/*PROTOCOL*/, packetLength);

		if (mRemainLength - PacketLength > 0)
			MoveMemory(mPacketBuffer, mPacketBuffer + PacketLength, mRemainLength - PacketLength);

		mRemainLength -= PacketLength;

		if (mRemainLength <= 0)
		{
			mRemainLength = 0;
			ZeroMemory(mPacketBuffer, sizeof(mPacketBuffer));
		}

		if (PacketNumber <= mLastReadPacketNumber) // 패킷번호가 이전것이거나 같을 경우(복사) 해당 패킷은 무시한다.
			return FALSE;
		else
		{
			mLastReadPacketNumber = PacketNumber;

			return TRUE;
		}
	}

	return FALSE;
}

BOOL PacketWrapping::GetPacket(LPSTR remoteAddress, USHORT remotePort, DWORD &protocol, BYTE *packet, DWORD &packetLength)
{
	ThreadSync sync;

	if (!packet)
		return FALSE;

	if (mRemainLength < sizeof(DWORD))
		return FALSE;

	INT PacketLength = 0;
	CopyMemory(&PacketLength, mPacketBuffer, sizeof(INT));

	if (PacketLength > MAX_BUFFER_LENGTH || PacketLength <= 0) // Invalid Packet
	{
		mRemainLength = 0;

		return FALSE;
	}

	if (PacketLength <= mRemainLength)
	{
		DWORD PacketNumber = 0;
		DWORD Protocol = 0;

		CopyMemory(&PacketNumber, mPacketBuffer + sizeof(DWORD)/*LENGTH*/, sizeof(DWORD));
		CopyMemory(&Protocol, mPacketBuffer +
			sizeof(DWORD)/*LENGTH*/ +
			sizeof(DWORD)/*PACKETNUMBER*/, sizeof(DWORD));

		protocol = Protocol;
		packetLength = PacketLength -
			sizeof(DWORD)/*LENGTH*/ -
			sizeof(DWORD)/*PACKETNUMBER*/ -
			sizeof(DWORD)/*PROTOCOL*/;

		CopyMemory(packet, mPacketBuffer +
			sizeof(DWORD)/*LENGTH*/ +
			sizeof(DWORD)/*PACKETNUMBER*/ +
			sizeof(DWORD)/*PROTOCOL*/, packetLength);

		if (mRemainLength - PacketLength > 0)
			MoveMemory(mPacketBuffer, mPacketBuffer + PacketLength, mRemainLength - PacketLength);

		mRemainLength -= PacketLength;

		if (mRemainLength <= 0)
		{
			mRemainLength = 0;
			ZeroMemory(mPacketBuffer, sizeof(mPacketBuffer));
		}

		for (DWORD i = 0; i<mLastReadPacketInfoVectorForUdp.size(); i++)
		{
			READ_PACKET_INFO ReadPacketInfo = mLastReadPacketInfoVectorForUdp[i];

			if (!strcmp(ReadPacketInfo.remoteAddress, remoteAddress) && ReadPacketInfo.remotePort == remotePort)
			{
				if (ReadPacketInfo.packetNumber < PacketNumber)
				{
					mLastReadPacketInfoVectorForUdp[i].packetNumber = PacketNumber;

					return TRUE;
				}
				else
					return FALSE;
			}
		}

		READ_PACKET_INFO ReadPacketInfo;
		ReadPacketInfo.packetNumber = PacketNumber;
		ReadPacketInfo.remotePort = remotePort;
		strcpy(ReadPacketInfo.remoteAddress, remoteAddress);

		mLastReadPacketInfoVectorForUdp.push_back(ReadPacketInfo);

		return TRUE;
	}

	return FALSE;
}

BOOL PacketWrapping::ReadPacketForIocp(DWORD readLength)
{
	ThreadSync sync;

	if (!NetworkWrapping::ReadForIocp(mPacketBuffer + mRemainLength, readLength))
		return FALSE;

	mRemainLength += readLength;

	return TRUE;
}

BOOL PacketWrapping::ReadPacketForEventSelect(VOID)
{
	ThreadSync sync;

	DWORD ReadLength = 0;

	if (!NetworkWrapping::ReadForEventSelect(mPacketBuffer + mRemainLength, ReadLength))
		return FALSE;

	mRemainLength += ReadLength;

	return TRUE;
}

BOOL PacketWrapping::ReadFromPacketForIocp(LPSTR remoteAddress, USHORT &remotePort, DWORD readLength)
{
	ThreadSync sync;

	if (!NetworkWrapping::ReadFromForIocp(remoteAddress, remotePort, mPacketBuffer + mRemainLength, readLength))
		return FALSE;

	mRemainLength += readLength;

	return TRUE;
}

BOOL PacketWrapping::ReadFromPacketForEventSelect(LPSTR remoteAddress, USHORT &remotePort)
{
	ThreadSync sync;

	DWORD ReadLength = 0;

	if (!NetworkWrapping::ReadFromForEventSelect(remoteAddress, remotePort, mPacketBuffer + mRemainLength, ReadLength))
		return FALSE;

	mRemainLength += ReadLength;

	return TRUE;
}

// 데이터를 보낼 때 이렇게 넣어서 보낸다.
BOOL PacketWrapping::WritePacket(DWORD protocol, const BYTE *packet, DWORD packetLength)
{
	ThreadSync sync;

	if (!packet)
		return FALSE;

	// LENGTH(4) | PACKET_NUMBER(4) | PROTOCOL(4) | DATA (4084)
	DWORD PacketLength = sizeof(DWORD)/*LENGTH*/ +
		sizeof(DWORD)/*PACKETNUMBER*/ +
		sizeof(DWORD)/*PROTOCOL*/ +
		packetLength;

	if (PacketLength >= MAX_BUFFER_LENGTH)
		return FALSE;

	mCurrentPacketNumber++;

	BYTE TempBuffer[MAX_BUFFER_LENGTH] = { 0, };

	// 패킷에 인덱스를 붙여 순서 보정을 해 주어야 한다.
	CopyMemory(TempBuffer, &PacketLength, sizeof(DWORD));

	CopyMemory(TempBuffer +
		sizeof(DWORD)/*LENGTH*/,
		&mCurrentPacketNumber, sizeof(DWORD));

	CopyMemory(TempBuffer +
		sizeof(DWORD)/*LENGTH*/ +
		sizeof(DWORD)/*PACKETNUMBER*/,
		&protocol, sizeof(DWORD));

	CopyMemory(TempBuffer +
		sizeof(DWORD)/*LENGTH*/ +
		sizeof(DWORD)/*PACKETNUMBER*/ +
		sizeof(DWORD)/*PROTOCOL*/,
		packet, packetLength);

	// WriteQueue를 이용해서 패킷이 전송 완료가 되었을까지 메모리를 살려둔다.
	BYTE *WriteData = WriteQueue.Push(this, TempBuffer, PacketLength);

	return NetworkWrapping::Write(WriteData, PacketLength);
}

BOOL PacketWrapping::WriteToPacket(LPCSTR remoteAddress, USHORT remotePort, DWORD protocol, const BYTE *packet, DWORD packetLength)
{
	ThreadSync sync;

	if (!packet)
		return FALSE;

	// LENGTH(4) | PACKET_NUMBER(4) | PROTOCOL(4) | DATA (4084)
	DWORD PacketLength = sizeof(DWORD)/*LENGTH*/ +
		sizeof(DWORD)/*PACKETNUMBER*/ +
		sizeof(DWORD)/*PROTOCOL*/ +
		packetLength;

	if (PacketLength >= MAX_BUFFER_LENGTH)
		return FALSE;

	mCurrentPacketNumber++;

	BYTE TempBuffer[MAX_BUFFER_LENGTH] = { 0, };

	//  패킷에 인덱스를 붙여 순서 보정을 해 주어야 한다.
	CopyMemory(TempBuffer, &PacketLength, sizeof(DWORD));

	CopyMemory(TempBuffer +
		sizeof(DWORD)/*LENGTH*/,
		&mCurrentPacketNumber, sizeof(DWORD));

	CopyMemory(TempBuffer +
		sizeof(DWORD)/*LENGTH*/ +
		sizeof(DWORD)/*PACKETNUMBER*/,
		&protocol, sizeof(DWORD));

	CopyMemory(TempBuffer +
		sizeof(DWORD)/*LENGTH*/ +
		sizeof(DWORD)/*PACKETNUMBER*/ +
		sizeof(DWORD)/*PROTOCOL*/,
		packet, packetLength);

	// WriteQueue를 이용해서 패킷이 전송 완료가 되었을까지 메모리를 살려둔다.
	BYTE *WriteData = WriteQueue.Push(this, TempBuffer, PacketLength);

	return NetworkWrapping::WriteTo(remoteAddress, remotePort, WriteData, PacketLength);
}

BOOL PacketWrapping::WriteComplete(VOID)
{
	ThreadSync sync;

	// WriteQueue에서 Pop을 해 주면 된다.
	return WriteQueue.Pop();
}

BOOL PacketWrapping::ResetUdp(VOID)
{
	ThreadSync	sync;

	mLastReadPacketInfoVectorForUdp.clear();

	return TRUE;
}