#include "stdafx.h"
#include "UserInfo.h"


UserInfo::UserInfo()
{
	mEnteredRoom			= NULL;
	mEnteredFriendshipRoom  = NULL;

	mIsConnected			= FALSE;
}


UserInfo::~UserInfo()
{
}


BOOL UserInfo::Begin(VOID)
{
	ThreadSync sync;

	mEnteredRoom			= NULL;
	mEnteredFriendshipRoom  = NULL;
	mIsConnected			= FALSE;

	return PacketWrapping::Begin();
}

BOOL UserInfo::End(VOID)
{
	ThreadSync sync;

	mEnteredRoom			= NULL;
	mEnteredRoom			= NULL;
	mIsConnected			= FALSE;

	return PacketWrapping::End();
}

BOOL UserInfo::Reload(SOCKET listenSocket)
{
	ThreadSync sync;

	End();

	if (!Begin())
		return FALSE;

	if (!NetworkWrapping::Accept(listenSocket))
		return FALSE;

	return TRUE;
}