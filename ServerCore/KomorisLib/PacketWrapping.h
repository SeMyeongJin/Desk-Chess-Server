#pragma once
class PacketWrapping : public NetworkWrapping
{
public:
	PacketWrapping(VOID);
	virtual ~PacketWrapping(VOID);

private:
	BYTE								mPacketBuffer[MAX_BUFFER_LENGTH * 3];
	INT									mRemainLength;
	DWORD								mCurrentPacketNumber;

	DWORD								mLastReadPacketNumber;

public:
	BOOL	Begin(VOID);
	BOOL	End(VOID);

	BOOL	ReadPacketForIocp(DWORD readLength);
	
	BOOL	WritePacket(DWORD protocol, const BYTE *packet, DWORD packetLength);
	BOOL	WriteComplete(VOID);

	BOOL	GetPacket(DWORD &protocol, BYTE *packet, DWORD &packetLength);
};