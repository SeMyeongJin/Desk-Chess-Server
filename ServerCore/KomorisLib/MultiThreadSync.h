#pragma once

template <class T>
class MultiThreadSync
{
	friend class ThreadSync;
public:
	class ThreadSync
	{
	public:
		ThreadSync(VOID)
		{
			T::mSync.Enter();
		}

		~ThreadSync(VOID)
		{
			T::mSync.Leave();
		}
	};

private:
	static CriticalSection mSync;
};

template <class T>
CriticalSection MultiThreadSync<T>::mSync;