#pragma once

typedef enum _PROTOCOL
{
	PT_VERSION = 0x1000000,
	PT_SIGNUP,
	PT_SIGNUP_SUCC,
	PT_SIGNUP_FAIL,
	PT_LOGIN,
	PT_LOGIN_SUCC,
	PT_LOGIN_FAIL,
	PT_END
} PROTOCOL;

typedef enum _ERROR_CODE
{
	EC_SIGNUP_ID_ALREADY_REGIST = 1,
	EC_SIGNUP_NAME_ALREADY_REGIST,
	EC_SIGNUP_ID_ERROR,
	EC_SIGNUP_PW_ERROR,
	EC_SIGNUP_NAME_ERROR,
	EC_LOGIN_ID_AND_PASS_ERROR,
} ERROR_CODE;


/* 
	///////////////////////////////////////////////////
						프로토콜 정의 
	///////////////////////////////////////////////////
*/


/*
	PT_SIGNUP
*/
typedef struct _S_PT_SIGNUP
{
	WCHAR userID[20];
	WCHAR userPW[20];
	WCHAR user_name[20];
} S_PT_SIGNUP;
// SIGNUP_READ
inline VOID READ_PT_SIGNUP(BYTE *buffer, S_PT_SIGNUP &parameter)
{
	StreamSP stream;
	stream->SetBuffer(buffer);

	stream->ReadWCHARs(parameter.userID, 20);
	stream->ReadWCHARs(parameter.userPW, 20);
	stream->ReadWCHARs(parameter.user_name, 20);
}
// SIGNUP_WRITE
inline DWORD WRITE_PT_SIGNUP(BYTE *buffer, S_PT_SIGNUP &parameter)
{
	StreamSP stream;
	stream->SetBuffer(buffer);

	stream->WriteWCHARs(parameter.userID, 20);
	stream->WriteWCHARs(parameter.userPW, 20);
	stream->WriteWCHARs(parameter.user_name, 20);

	return stream->GetLength();
}
inline DWORD WRITE_PT_SIGNUP(BYTE *buffer, WCHAR *id, WCHAR *password, WCHAR *name)
{
	StreamSP stream;
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
	PT_SIGNUP_SUCC
*/
typedef struct _S_PT_SIGNUP_SUCC
{

} S_PT_SIGNUP_SUCC;
// SIGNUP_SUCC_READ
inline VOID READ_PT_SIGNUP_SUCC(BYTE *buffer, S_PT_SIGNUP_SUCC &parameter)
{
	StreamSP stream;
	stream->SetBuffer(buffer);
}
// SIGNUP_SUCC_WRITE
inline DWORD WRITE_PT_SIGNUP_SUCC(BYTE *buffer, S_PT_SIGNUP_SUCC &parameter)
{
	StreamSP stream;
	stream->SetBuffer(buffer);

	return stream->GetLength();
}
inline DWORD WRITE_PT_SIGNUP_SUCC(BYTE *buffer)
{
	StreamSP stream;
	stream->SetBuffer(buffer);

	return stream->GetLength();
}


/*
	PT_SIGNUP_FAIL
*/
typedef struct _S_PT_SIGNUP_FAIL
{
	DWORD errorCode;
} S_PT_SIGNUP_FAIL;
// SIGNUP_FAIL_READ
inline VOID READ_PT_SIGNUP_FAIL(BYTE *buffer, S_PT_SIGNUP_FAIL &parameter)
{
	StreamSP stream;
	stream->SetBuffer(buffer);

	stream->ReadDWORD(&parameter.errorCode);
}
// SIGNUP_FAIL_WRITE
inline DWORD WRITE_PT_SIGNUP_FAIL(BYTE *buffer, S_PT_SIGNUP_FAIL &parameter)
{
	StreamSP stream;
	stream->SetBuffer(buffer);

	stream->WriteDWORD(parameter.errorCode);

	return stream->GetLength();
}
inline DWORD WRITE_PT_SIGNUP_FAIL(BYTE *buffer, DWORD errorCode)
{
	StreamSP stream;
	stream->SetBuffer(buffer);

	stream->WriteDWORD(errorCode);

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
// LOGIN_READ
inline VOID READ_PT_LOGIN(BYTE *buffer, S_PT_LOGIN &parameter)
{
	StreamSP stream;
	stream->SetBuffer(buffer);

	stream->ReadWCHARs(parameter.userID, 20);
	stream->ReadWCHARs(parameter.userPW, 20);
}
// LOGIN_WRITE
inline DWORD WRITE_PT_LOGIN(BYTE *buffer, S_PT_LOGIN &parameter)
{
	StreamSP stream;
	stream->SetBuffer(buffer);

	stream->WriteWCHARs(parameter.userID, 20);
	stream->WriteWCHARs(parameter.userPW, 20);

	return stream->GetLength();
}
inline DWORD WRITE_PT_LOGIN(BYTE *buffer, WCHAR *id, WCHAR *password)
{
	StreamSP stream;
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
	PT_LOGIN_SUCC
*/
typedef struct _S_PT_LOGIN_SUCC
{
	WCHAR user_name[20];
	DWORD rating;
} S_PT_LOGIN_SUCC;
// LOGIN_SUCC_READ
inline VOID READ_PT_LOGIN_SUCC(BYTE *buffer, S_PT_LOGIN_SUCC &parameter)
{
	StreamSP stream;
	stream->SetBuffer(buffer);

	stream->ReadWCHARs(parameter.user_name, 20);
	stream->ReadDWORD(&parameter.rating);
}
// LOGIN_SUCC_WRITE
inline DWORD WRITE_PT_LOGIN_SUCC(BYTE *buffer, S_PT_LOGIN_SUCC &parameter)
{
	StreamSP stream;
	stream->SetBuffer(buffer);

	stream->WriteWCHARs(parameter.user_name, 20);
	stream->WriteDWORD(parameter.rating);

	return stream->GetLength();
}
inline DWORD WRITE_PT_LOGIN_SUCC(BYTE *buffer, WCHAR *name, DWORD ratingPoint)
{
	StreamSP stream;
	stream->SetBuffer(buffer);

	WCHAR _name[20] = { 0, };
	_tcsncpy(_name, name, 20);
	stream->WriteWCHARs(_name, 20);
	stream->WriteDWORD(ratingPoint);

	return stream->GetLength();
}


/*
	PT_LOGIN_FAIL
*/
typedef struct _S_PT_LOGIN_FAIL
{
	DWORD errorCode;
} S_PT_LOGIN_FAIL;
// LOGIN_FAIL_READ
inline VOID READ_PT_LOGIN_FAIL(BYTE *buffer, S_PT_LOGIN_FAIL &parameter)
{
	StreamSP stream;
	stream->SetBuffer(buffer);

	stream->ReadDWORD(&parameter.errorCode);
}
// LOGIN_FAIL_WRITE
inline DWORD WRITE_PT_LOGIN_FAIL(BYTE *buffer, S_PT_LOGIN_FAIL &parameter)
{
	StreamSP stream;
	stream->SetBuffer(buffer);

	stream->WriteDWORD(parameter.errorCode);

	return stream->GetLength();
}
inline DWORD WRITE_PT_LOGIN_FAIL(BYTE *buffer, DWORD errorCode)
{
	StreamSP stream;
	stream->SetBuffer(buffer);

	stream->WriteDWORD(errorCode);

	return stream->GetLength();
}