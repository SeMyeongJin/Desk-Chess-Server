#pragma once

class ClientSession : public EventSelect
{
public:
	ClientSession(VOID);
	virtual ~ClientSession(VOID);

private:
	PacketWrapping	mSession;
	CircularQueue	mReadPacketQueue; 

	BOOL			isUdp; 

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