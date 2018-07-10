#include "stdafx.h"
#include "ServerIocp.h"


VOID CServerIocp::PROC_PT_SIGNUP(CConnectedSession * pConnectedSession, DWORD dwProtocol, BYTE * pPacket, DWORD dwPacketLength)
{
	S_PT_SIGNUP Data;
	READ_PT_SIGNUP(pPacket, Data);

	GDBManager->RegistUserQuery(Data.userID, Data.userPW, Data.user_name);
	_tprintf(_T("%s completed registration in DB.\n"), Data.userID);
}

VOID CServerIocp::PROC_PT_LOGIN(CConnectedSession * pConnectedSession, DWORD dwProtocol, BYTE * pPacket, DWORD dwPacketLength)
{
	BYTE WriteBuffer[MAX_BUFFER_LENGTH] = { 0, };
	S_PT_LOGIN Data;
	READ_PT_LOGIN(pPacket, Data);

	if (GDBManager->LoginCheckQuery(Data.userID, Data.userPW))
		mSessionManager.WriteAll(PT_LOGIN, WriteBuffer, WRITE_PT_LOGIN(WriteBuffer, Data.userID, Data.userPW));
	_tprintf(_T("%s is connected to the server.\n"), Data.userID);
}

VOID CServerIocp::PROC_PT_CHAT(CConnectedSession * pConnectedSession, DWORD dwProtocol, BYTE * pPacket, DWORD dwPacketLength)
{
	BYTE WriteBuffer[MAX_BUFFER_LENGTH] = { 0, };
	S_PT_CHAT Data;
	READ_PT_CHAT(pPacket, Data);

	mSessionManager.WriteAll(PT_CHAT, WriteBuffer, WRITE_PT_CHAT(WriteBuffer, Data.user_name, Data.message));
	if (wcslen(Data.message) > 0)
		_tprintf(_T("%s :[chat message] %s\n"), Data.user_name, Data.message);
}