#include "stdafx.h"
#include "CriticalSection.h"
#include "MultiThreadSync.h"
#include "CircularQueue.h"
#include "NetworkSession.h"
#include "PacketSession.h"
#include "EventSelect.h"
#include "ClientSession.h"


ClientSession::ClientSession(VOID)
{
	isUdp = FALSE;
}

ClientSession::~ClientSession(VOID)
{
}

BOOL ClientSession::BeginTcp(LPSTR remoteAddress, USHORT remotePort)
{
	if (!remoteAddress || remotePort <= 0)
		return FALSE;

	if (!mSession.Begin())
	{
		End();

		return FALSE;
	}

	if (!mSession.TcpBind())
	{
		End();

		return FALSE;
	}

	if (!EventSelect::Begin(mSession.GetSocket()))
	{
		End();

		return FALSE;
	}

	if (!mSession.Connect(remoteAddress, remotePort))
	{
		End();

		return FALSE;
	}

	return TRUE;
}

BOOL ClientSession::BeginUdp(USHORT remotePort)
{
#if defined(_EXTERNAL_RELEASE_)
	if (remotePort <= 0)
		return FALSE;
#endif

	if (!mSession.Begin())
	{
		End();

		return FALSE;
	}

	if (!mSession.UdpBind(remotePort))
	{
		End();

		return FALSE;
	}

	if (!EventSelect::Begin(mSession.GetSocket()))
	{
		End();

		return FALSE;
	}

	isUdp = TRUE;

	return TRUE;
}

BOOL ClientSession::End(VOID)
{
	mSession.End();

	EventSelect::End();

	return TRUE;
}

BOOL ClientSession::GetLocalIP(WCHAR* pIP)
{
	return mSession.GetLocalIP(pIP);
}

USHORT ClientSession::GetLocalPort(VOID)
{
	return mSession.GetLocalPort();
}

BOOL ClientSession::ReadPacket(DWORD &protocol, BYTE *packet, DWORD &packetLength)
{
	VOID *object = NULL;

	return mReadPacketQueue.Pop(&object, protocol, packet, packetLength);
}

BOOL ClientSession::ReadFromPacket(DWORD &protocol, LPSTR remoteAddress, USHORT &remotePort, BYTE *packet, DWORD &packetLength)
{
	VOID *object = NULL;

	return mReadPacketQueue.Pop(&object, protocol, packet, packetLength, remoteAddress, remotePort);
}

BOOL ClientSession::WritePacket(DWORD protocol, const BYTE *packet, DWORD packetLength)
{
	if (!mSession.WritePacket(protocol, packet, packetLength))
		return FALSE;

	if (!mSession.WriteComplete())
		return FALSE;

	return TRUE;
}

BOOL ClientSession::WriteToPacket(LPCSTR remoteAddress, USHORT remotePort, DWORD protocol, const BYTE *packet, DWORD packetLength)
{
	if (!mSession.WriteToPacket(remoteAddress, remotePort, protocol, packet, packetLength))
		return FALSE;

	if (!mSession.WriteComplete())
		return FALSE;

	return TRUE;
}

VOID ClientSession::OnIoRead(VOID)
{
	BYTE	packet[MAX_BUFFER_LENGTH] = { 0, };
	DWORD	packetLength = 0;

	DWORD	protocol = 0;

	if (isUdp)
	{
		CHAR	remoteAddress[32] = { 0, };
		USHORT	remotePort = 0;

		if (mSession.ReadFromPacketForEventSelect(remoteAddress, remotePort))
		{
			while (mSession.GetPacket(remoteAddress, remotePort, protocol, packet, packetLength))
				mReadPacketQueue.Push(this, protocol, packet, packetLength, remoteAddress, remotePort);
		}
	}
	else
	{
		if (mSession.ReadPacketForEventSelect())
		{
			while (mSession.GetPacket(protocol, packet, packetLength))
				mReadPacketQueue.Push(this, protocol, packet, packetLength);
		}
	}
}