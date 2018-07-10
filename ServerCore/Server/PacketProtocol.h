#pragma once

typedef enum _PROTOCOL
{
	PT_VERSION = 0x1000000,
	PT_SIGNUP,
	PT_LOGIN,
	PT_CHAT,
	PT_END
} PROTOCOL;


/*
	PT_SIGNUP
*/
typedef struct _S_PT_SIGNUP
{
	WCHAR userID[20];
	WCHAR userPW[20];
	WCHAR user_name[20];
} S_PT_SIGNUP;

inline VOID READ_PT_SIGNUP(BYTE *buffer, S_PT_SIGNUP &parameter)
{
	CStreamSP stream;
	stream->SetBuffer(buffer);

	stream->ReadWCHARs(parameter.userID, 20);
	stream->ReadWCHARs(parameter.userPW, 20);
	stream->ReadWCHARs(parameter.user_name, 20);
}

inline DWORD WRITE_PT_SIGNUP(BYTE *buffer, S_PT_SIGNUP &parameter)
{
	CStreamSP stream;
	stream->SetBuffer(buffer);

	stream->WriteWCHARs(parameter.userID, 20);
	stream->WriteWCHARs(parameter.userPW, 20);
	stream->WriteWCHARs(parameter.user_name, 20);

	return stream->GetLength();
}

inline DWORD WRITE_PT_SIGNUP(BYTE *buffer, WCHAR *id, WCHAR *password, WCHAR *name)
{
	CStreamSP stream;
	stream->SetBuffer(buffer);

	WCHAR _id[20] = { 0, };
	_tcsncpy(_id, id, 20);
	stream->WriteWCHARs(_id, 20);
	WCHAR _password[20] = { 0, };
	_tcsncpy(_password, password, 20);
	stream->WriteWCHARs(_password, 20);
	WCHAR _name[20] = { 0, };
	_tcsncpy(_name, name, 20);
	stream->WriteWCHARs(_name, 20);

	return stream->GetLength();
}

/*
	PT_LOGIN
*/
typedef struct _S_PT_LOGIN
{
	WCHAR userID[20];
	WCHAR userPW[20];
} S_PT_LOGIN;

inline VOID READ_PT_LOGIN(BYTE *buffer, S_PT_LOGIN &parameter)
{
	CStreamSP stream;
	stream->SetBuffer(buffer);

	stream->ReadWCHARs(parameter.userID, 20);
	stream->ReadWCHARs(parameter.userPW, 20);
}

inline DWORD WRITE_PT_LOGIN(BYTE *buffer, S_PT_LOGIN &parameter)
{
	CStreamSP stream;
	stream->SetBuffer(buffer);

	stream->WriteWCHARs(parameter.userID, 20);
	stream->WriteWCHARs(parameter.userPW, 20);

	return stream->GetLength();
}

inline DWORD WRITE_PT_LOGIN(BYTE *buffer, WCHAR *id, WCHAR *password)
{
	CStreamSP stream;
	stream->SetBuffer(buffer);

	WCHAR _id[20] = { 0, };
	_tcsncpy(_id, id, 20);
	stream->WriteWCHARs(_id, 20);
	WCHAR _password[20] = { 0, };
	_tcsncpy(_password, password, 20);
	stream->WriteWCHARs(_password, 20);

	return stream->GetLength();
}

 /* 
	PT_CHAT 
 */
typedef struct _S_PT_CHAT
{
	WCHAR user_name[20];
	WCHAR message[40];
} S_PT_CHAT;

inline VOID READ_PT_CHAT(BYTE *buffer, S_PT_CHAT &parameter)
{
	CStreamSP stream;
	stream->SetBuffer(buffer);

	stream->ReadWCHARs(parameter.user_name, 20);
	stream->ReadWCHARs(parameter.message, 40);
}

inline DWORD WRITE_PT_CHAT(BYTE *buffer, S_PT_CHAT &parameter)
{
	CStreamSP stream;
	stream->SetBuffer(buffer);

	stream->WriteWCHARs(parameter.user_name, 20);
	stream->WriteWCHARs(parameter.message, 40);

	return stream->GetLength();
}

inline DWORD WRITE_PT_CHAT(BYTE *buffer, WCHAR *name, WCHAR *message)
{
	CStreamSP stream;
	stream->SetBuffer(buffer);

	WCHAR _name[20] = { 0, };
	_tcsncpy(_name, name, 20);
	stream->WriteWCHARs(_name, 20);
	WCHAR _message[40] = { 0, };
	_tcsncpy(_message, message, 40);
	stream->WriteWCHARs(_message, 40);

	return stream->GetLength();
}