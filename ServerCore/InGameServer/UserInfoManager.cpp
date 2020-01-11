#include "stdafx.h"
#include "UserInfoManager.h"


UserInfoManager::UserInfoManager()
{
	mMaxUserNum				= 0;
	mCurrentUserNum			= 0;
}


UserInfoManager::~UserInfoManager()
{
}


BOOL UserInfoManager::Begin(DWORD maxUserNum, SOCKET listenSocket)
{
	if (maxUserNum <= 0 || !listenSocket)
		return FALSE;

	mMaxUserNum = maxUserNum;		// 최대 인원 설정
	mCurrentUserNum = 0;			// 현재 인원 초기화

	mlistenSocket = listenSocket;

	for (DWORD i = 0; i < mMaxUserNum; i++)
	{
		UserInfo *userInfo = new UserInfo();
		
		if (userInfo->Begin())
			mUserInfoMap.insert(pair<int, UserInfo*>((int)i, userInfo));

		else
		{
			End();

			return FALSE;
		}
	}

	return TRUE;
}

BOOL UserInfoManager::End(VOID)
{
	for (DWORD i = 0; i < mUserInfoMap.size(); i++)
	{
		UserInfo *userInfo = mUserInfoMap[i];

		userInfo->End();

		delete userInfo;
	}

	mUserInfoMap.clear();


	return TRUE;
}

BOOL UserInfoManager::AcceptALL(VOID)
{
	for (DWORD i = 0; i < mUserInfoMap.size(); i++)
	{
		UserInfo *userInfo = mUserInfoMap[i];

		if (!userInfo->Accept(mlistenSocket))
		{
			End();

			return FALSE;
		}
	}

	return TRUE;
}

BOOL UserInfoManager::WriteAll(DWORD protocol, BYTE *data, DWORD dataLength)
{
	ThreadSync sync;

	if (!data)
		return FALSE;

	for (DWORD i = 0; i < mUserInfoMap.size(); i++)
	{
		UserInfo *userInfo = mUserInfoMap[i];

		if (userInfo->GetIsConnected())
		{
			if (!userInfo->WritePacket(protocol, data, dataLength))
				userInfo->End();
		}
	}

	return TRUE;
}