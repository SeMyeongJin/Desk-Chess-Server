#pragma once

class ClientSession : public EventSelect
{
public:
	ClientSession(VOID);
	virtual ~ClientSession(VOID);

private:
	PacketSession	mSession; // 통신을 담당하는 Session 개체
	CircularQueue	mReadPacketQueue; // 받은 패킷을 저장하고 있는 큐

	BOOL			isUdp; // 현재 UDP로 사용 중인지 확인하는 변수

public:
	BOOL			BeginTcp(LPSTR remoteAddress, USHORT remotePort);
	BOOL			BeginUdp(USHORT remotePort);

	BOOL			End(VOID);

	BOOL			GetLocalIP(WCHAR* pIP);
	USHORT			GetLocalPort(VOID);

	BOOL			ReadPacket(DWORD &protocol, BYTE *packet, DWORD &packetLength);
	BOOL			ReadFromPacket(DWORD &protocol, LPSTR remoteAddress, USHORT &remotePort, BYTE *packet, DWORD &packetLength);

	BOOL			WritePacket(DWORD protocol, const BYTE *packet, DWORD packetLength);
	BOOL			WriteToPacket(LPCSTR remoteAddress, USHORT remotePort, DWORD protocol, const BYTE *packet, DWORD packetLength);

protected:
	VOID			OnIoRead(VOID);
};