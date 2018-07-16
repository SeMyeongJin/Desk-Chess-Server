#pragma once


class EventSelect
{
public:
	EventSelect(VOID);
	virtual ~EventSelect(VOID);

private:
	HANDLE			mSelectEventHandle;		// 소켓 이벤트를 관리
	HANDLE			mStartupEventHandle;	// 소켓 이벤트를 관리하는 스레드 시작을 체크하는 이벤트
	HANDLE			mDestroyEventHandle;	// 스레드를 종료할 때 사용하는 이벤트
	HANDLE			mSelectThreadHandle;	// 소켓 이벤트를 관리하는 스레드 핸들 값

	SOCKET			mSocket;				// 사용할 소켓 핸들

protected:
	virtual VOID	OnIoRead(VOID) = 0;
	//virtual VOID	OnIoWrote(VOID)				= 0;
	virtual VOID	OnIoConnected(VOID) = 0;
	virtual VOID	OnIoDisconnected(VOID) = 0;

public:
	BOOL			Begin(SOCKET socket);
	BOOL			End(VOID);

	VOID			SelectThreadCallback(VOID);
};