#include "stdafx.h"
#include "UserInfo.h"


UserInfo::UserInfo()
{
	ZeroMemory(mUserID, sizeof(mUserID));
	ZeroMemory(mNickName, sizeof(mNickName));
	mLifePoint				= 0;
	mEnteredRoom			= NULL;
	mEnteredFriendshipRoom  = NULL;

	mIsConnected			= FALSE;
	mIsReady				= FALSE;
	mIsLoadComplete			= FALSE;
	mIsIntroComplete		= FALSE;
}


UserInfo::~UserInfo()
{
}


BOOL UserInfo::Begin(VOID)
{
	ThreadSync sync;

	ZeroMemory(mUserID, sizeof(mUserID));
	ZeroMemory(mNickName, sizeof(mNickName));
	mLifePoint				= 0;
	mEnteredRoom			= NULL;
	mEnteredFriendshipRoom  = NULL;
	mIsConnected			= FALSE;
	mIsReady				= FALSE;
	mIsLoadComplete			= FALSE;
	mIsIntroComplete		= FALSE;

	return PacketWrapping::Begin();
}

BOOL UserInfo::End(VOID)
{
	ThreadSync sync;

	ZeroMemory(mUserID, sizeof(mUserID));
	ZeroMemory(mNickName, sizeof(mNickName));
	mLifePoint				= 0;
	mEnteredRoom			= NULL;
	mEnteredRoom			= NULL;
	mIsConnected			= FALSE;
	mIsReady				= FALSE;
	mIsLoadComplete			= FALSE;
	mIsIntroComplete		= FALSE;

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