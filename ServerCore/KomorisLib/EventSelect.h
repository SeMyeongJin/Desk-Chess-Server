#pragma once


class EventSelect
{
public:
	EventSelect(VOID);
	virtual ~EventSelect(VOID);

private:
	HANDLE			mSelectEventHandle;		// ���� �̺�Ʈ�� ����
	HANDLE			mStartupEventHandle;	// ���� �̺�Ʈ�� �����ϴ� ������ ������ üũ�ϴ� �̺�Ʈ
	HANDLE			mDestroyEventHandle;	// �����带 ������ �� ����ϴ� �̺�Ʈ
	HANDLE			mSelectThreadHandle;	// ���� �̺�Ʈ�� �����ϴ� ������ �ڵ� ��

	SOCKET			mSocket;				// ����� ���� �ڵ�

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