#include "stdafx.h"
#include "CriticalSection.h"
#include "MultiThreadSync.h"
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

	return NetworkWrapping::Begin();
}

BOOL PacketWrapping::End(VOID)
{
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

BOOL PacketWrapping::ReadPacketForIocp(DWORD readLength)
{
	ThreadSync sync;

	if (!NetworkWrapping::ReadForIocp(mPacketBuffer + mRemainLength, readLength))
		return FALSE;

	mRemainLength += readLength;

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

	return NetworkWrapping::Write(TempBuffer, PacketLength);
}