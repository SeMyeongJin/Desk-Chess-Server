#include "stdafx.h"
#include "GameRoom.h"


GameRoom::GameRoom()
{
	mRoomNumber					= 0;
	mMapNumber					= 0;
	mIsReadyComplete			= FALSE;
	mIsGameStarted				= FALSE;
	mRemainGameTime				= GAME_TIME;
}


GameRoom::~GameRoom()
{
}


BOOL GameRoom::Begin(DWORD roomNumber)
{
	ThreadSync sync;

	mRoomNumber					= roomNumber;
	mMapNumber					= 0;
	mIsReadyComplete			= FALSE;
	mIsGameStarted				= FALSE;
	mRemainGameTime				= GAME_TIME;

	return TRUE;
}

BOOL GameRoom::End(VOID)
{
	ThreadSync sync;

	mRoomNumber					= 0;
	mMapNumber					= 0;
	mIsReadyComplete			= FALSE;
	mIsGameStarted				= FALSE;
	mRemainGameTime				= GAME_TIME;
}

BOOL GameRoom::WriteAll(DWORD protocol, BYTE *packet, DWORD packetLength)
{
	ThreadSync sync;

	if (protocol <= 0 || !packet)
		return FALSE;

	mUsers[0]->WritePacket(protocol, packet, packetLength);
	mUsers[1]->WritePacket(protocol, packet, packetLength);

	return TRUE;
}

BOOL GameRoom::WriteAllInitGameData(VOID)
{
	ThreadSync sync;

	BYTE writeBuffer[MAX_BUFFER_LENGTH] = { 0, };

	return TRUE;
}