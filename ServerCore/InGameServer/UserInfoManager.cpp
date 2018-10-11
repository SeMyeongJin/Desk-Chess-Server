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
	ThreadSync sync;

	if (maxUserNum <= 0 || !listenSocket)
		return FALSE;

	mMaxUserNum = maxUserNum;		// 최대 인원 설정
	mCurrentUserNum = 0;			// 현재 인원 초기화

	mlistenSocket = listenSocket;

	for (DWORD i = 0; i < mMaxUserNum; i++)
	{
		UserInfo *userInfo = new UserInfo();
		
		if (userInfo->Begin())
			mUserInfoVector.push_back(userInfo);
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
	ThreadSync sync;

	for (DWORD i = 0; i < mUserInfoVector.size(); i++)
	{
		UserInfo *userInfo = mUserInfoVector[i];

		userInfo->End();

		delete userInfo;
	}

	mUserInfoVector.clear();

	return TRUE;
}

BOOL UserInfoManager::AcceptALL(VOID)
{
	ThreadSync sync;

	for (DWORD i = 0; i < mUserInfoVector.size(); i++)
	{
		UserInfo *userInfo = mUserInfoVector[i];

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

	for (DWORD i = 0; i < mUserInfoVector.size(); i++)
	{
		UserInfo *userInfo = mUserInfoVector[i];

		if (userInfo->GetIsConnected())
		{
			if (!userInfo->WritePacket(protocol, data, dataLength))
				userInfo->End();
		}
	}

	return TRUE;
}

BOOL UserInfoManager::IsAlreadyLogined(LPTSTR userID)
{
	ThreadSync sync;

	if (!userID)
		return TRUE;

	TCHAR UserID[20] = { 0, };

	for (DWORD i = 0; i < mUserInfoVector.size(); i++)
	{
		UserInfo* userInfo = mUserInfoVector[i];

		if (!_tcscmp(userInfo->GetUserID(), userID))
			return TRUE;
	}

	return FALSE;
}