#pragma once

class CriticalSection
{
public:
	CriticalSection(VOID)
	{
		InitializeCriticalSection(&mSync);
	}

	~CriticalSection(VOID)
	{
		DeleteCriticalSection(&mSync);
	}

	inline VOID Enter(VOID)
	{
		EnterCriticalSection(&mSync);
	}

	inline VOID Leave(VOID)
	{
		LeaveCriticalSection(&mSync);
	}

private:
	CRITICAL_SECTION	mSync;
};