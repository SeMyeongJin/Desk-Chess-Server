#include "stdafx.h"
#include "CriticalSection.h"
#include "MultiThreadSync.h"
#include "NetworkWrapping.h"
#include "PacketWrapping.h"

PacketWrapping::PacketWrapping(VOID)
{
	// ��Ŷ Ȯ���� ���ؼ� �����ϴ� mPacketBuffer
	ZeroMemory(mPacketBuffer, sizeof(mPacketBuffer));

	mRemainLength = 0; // mPacketBuffer�� ���� ����
	mCurrentPacketNumber = 0; // ������ ��뿡�� ���� �� ����� ��Ŷ ��ȣ

	mLastReadPacketNumber = 0; // ���������� ���� ��Ŷ ��ȣ
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

// ���� ��Ŷ�� �м��ϴ� �Լ�
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

		if (PacketNumber <= mLastReadPacketNumber) // ��Ŷ��ȣ�� �������̰ų� ���� ���(����) �ش� ��Ŷ�� �����Ѵ�.
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

// �����͸� ���� �� �̷��� �־ ������.
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

	// ��Ŷ�� �ε����� �ٿ� ���� ������ �� �־�� �Ѵ�.
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