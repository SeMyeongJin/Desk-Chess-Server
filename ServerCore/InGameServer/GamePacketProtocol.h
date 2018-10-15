#pragma once
/*
	성공 패킷의 경우 SUCC, 실패 패킷의 경우 FAIL 문자열 포함
	
	클라이언트 -> 서버로 전송할 경우 프로토콜의 의미를 이름으로
	
	서버 -> 클라이언트로 전송할 경우 클라이언트가 사용한 프로토콜에
	
	SUCC, FAIL로 응답 처리

	1. 로비 프로토콜 { 방 생성, 방 입장, 빠른 시작 ...}

	2. 게임 시작관리 { Ready, Start, Intro, Load ...}

	3. 게임 규칙관리 { 게임 종류에 따라 다양하게 구성함}	
*/

typedef enum _TCP_PROTOCOL
{
	PT_VERSION = 0x1000000,
	PT_REQ_USER_INFO,
	PT_RES_USER_INFO,
	PT_OFFICIAL_GAME_START,
	PT_OFFICIAL_GAME_START_SUCC,
	PT_OFFICIAL_GAME_START_FAIL,
	PT_FRIENDSHIP_GAME_START,
	PT_FRIENDSHIP_GAME_START_SUCC,
	PT_FRIENDSHIP_GAME_START_FAIL,
	PT_ROOM_LEAVE,
	PT_ROOM_LEAVE_SUCC,
	PT_ROOM_LEAVE_FAIL,
	PT_GAME_START_ALL,
	PT_GAME_END_ALL,
	PT_CHAT,
	PT_DELIVERY_CHAT,
	PT_PIECE_MOVE,
	PT_PIECE_PROMOTION,
	PT_OFFICIAL_GAME_WIN,
	PT_OFFICIAL_GAME_LOSE,
	PT_FRIENDSHIP_GAME_WIN,
	PT_END
} TCP_PROTOCOL;


/*
	PT_REQ_USER_INFO
*/
typedef struct _S_PT_REQ_USER_INFO
{
	WCHAR userName[20];
} S_PT_REQ_USER_INFO;
// REQ_USER_INFO_READ
inline VOID READ_PT_REQ_USER_INFO(BYTE *buffer, S_PT_REQ_USER_INFO &parameter)
{
	StreamSP stream;
	stream->SetBuffer(buffer);

	stream->ReadWCHARs(parameter.userName, 20);
}
// REQ_USER_INFO_WRITE
inline DWORD WRITE_PT_REQ_USER_INFO(BYTE *buffer, S_PT_REQ_USER_INFO &parameter)
{
	StreamSP stream;
	stream->SetBuffer(buffer);

	stream->WriteWCHARs(parameter.userName, 20);

	return stream->GetLength();
}
inline DWORD WRITE_PT_REQ_USER_INFO(BYTE *buffer, WCHAR *name)
{
	StreamSP stream;
	stream->SetBuffer(buffer);

	WCHAR _name[20] = { 0, };
	_tcsncpy(_name, name, 20);
	stream->WriteWCHARs(_name, 20);

	return stream->GetLength();
}


/*
	PT_RES_USER_INFO
*/
typedef struct _S_PT_RES_USER_INFO
{
	DWORD Rating;
	DWORD OFF_WIN;
	DWORD OFF_LOSE;
	DWORD FRI_WIN;
} S_PT_RES_USER_INFO;
// RES_USER_INFO_READ
inline VOID READ_PT_RES_USER_INFO(BYTE *buffer, S_PT_RES_USER_INFO &parameter)
{
	StreamSP stream;
	stream->SetBuffer(buffer);

	stream->ReadDWORD(&parameter.Rating);
	stream->ReadDWORD(&parameter.OFF_WIN);
	stream->ReadDWORD(&parameter.OFF_LOSE);
	stream->ReadDWORD(&parameter.FRI_WIN);
}
// RES_USER_INFO_WRITE
inline DWORD WRITE_PT_RES_USER_INFO(BYTE *buffer, S_PT_RES_USER_INFO &parameter)
{
	StreamSP stream;
	stream->SetBuffer(buffer);

	stream->WriteDWORD(parameter.Rating);
	stream->WriteDWORD(parameter.OFF_WIN);
	stream->WriteDWORD(parameter.OFF_LOSE);
	stream->WriteDWORD(parameter.FRI_WIN);

	return stream->GetLength();
}
inline DWORD WRITE_PT_RES_USER_INFO(BYTE *buffer, DWORD rating, DWORD off_win, DWORD off_lose, DWORD fri_win)
{
	StreamSP stream;
	stream->SetBuffer(buffer);

	stream->WriteDWORD(rating);
	stream->WriteDWORD(off_win);
	stream->WriteDWORD(off_lose);
	stream->WriteDWORD(fri_win);

	return stream->GetLength();
}


/*
	PT_OFFICIAL_GAME_START
*/
typedef struct _S_PT_OFFICIAL_GAME_START
{
	
} S_PT_OFFICIAL_GAME_START;
// OFFICIAL_GAME_START_READ
inline VOID READ_PT_OFFICIAL_GAME_START(BYTE *buffer, S_PT_OFFICIAL_GAME_START &parameter)
{
	StreamSP stream;
	stream->SetBuffer(buffer);
}
// OFFICIAL_GAME_START_WRITE
inline DWORD WRITE_PT_OFFICIAL_GAME_START(BYTE *buffer, S_PT_OFFICIAL_GAME_START &parameter)
{
	StreamSP stream;
	stream->SetBuffer(buffer);

	return stream->GetLength();
}
inline DWORD WRITE_PT_OFFICIAL_GAME_START(BYTE *buffer)
{
	StreamSP stream;
	stream->SetBuffer(buffer);

	return stream->GetLength();
}


/*
	PT_OFFICIAL_GAME_START_SUCC
*/
typedef struct _S_PT_OFFICIAL_GAME_START_SUCC
{

} S_PT_OFFICIAL_GAME_START_SUCC;
// OFFICIAL_GAME_START_SUCC_READ
inline VOID READ_PT_OFFICIAL_GAME_START_SUCC(BYTE *buffer, S_PT_OFFICIAL_GAME_START_SUCC &parameter)
{
	StreamSP stream;
	stream->SetBuffer(buffer);
}
// OFFICIAL_GAME_START_SUCC_WRITE
inline DWORD WRITE_PT_OFFICIAL_GAME_START_SUCC(BYTE *buffer, S_PT_OFFICIAL_GAME_START_SUCC &parameter)
{
	StreamSP stream;
	stream->SetBuffer(buffer);

	return stream->GetLength();
}
inline DWORD WRITE_PT_OFFICIAL_GAME_START_SUCC(BYTE *buffer)
{
	StreamSP stream;
	stream->SetBuffer(buffer);

	return stream->GetLength();
}


/*
	PT_OFFICIAL_GAME_START_FAIL
*/
typedef struct _S_PT_OFFICIAL_GAME_START_FAIL
{
	DWORD errorCode;
} S_PT_OFFICIAL_GAME_START_FAIL;
// OFFICIAL_GAME_START_FAIL_READ
inline VOID READ_PT_OFFICIAL_GAME_START_FAIL(BYTE *buffer, S_PT_OFFICIAL_GAME_START_FAIL &parameter)
{
	StreamSP stream;
	stream->SetBuffer(buffer);

	stream->ReadDWORD(&parameter.errorCode);
}
// OFFICIAL_GAME_START_FAIL_WRITE
inline DWORD WRITE_PT_OFFICIAL_GAME_START_FAIL(BYTE *buffer, S_PT_OFFICIAL_GAME_START_FAIL &parameter)
{
	StreamSP stream;
	stream->SetBuffer(buffer);

	stream->WriteDWORD(parameter.errorCode);

	return stream->GetLength();
}
inline DWORD WRITE_PT_OFFICIAL_GAME_START_FAIL(BYTE *buffer, DWORD errorCode)
{
	StreamSP stream;
	stream->SetBuffer(buffer);

	stream->WriteDWORD(errorCode);

	return stream->GetLength();
}


/*
	PT_FRIENDSHIP_GAME_START
*/
typedef struct _S_PT_FRIENDSHIP_GAME_START
{

} S_PT_FRIENDSHIP_GAME_START;
// FRIENDSHIP_GAME_START_READ
inline VOID READ_PT_FRIENDSHIP_GAME_START(BYTE *buffer, S_PT_FRIENDSHIP_GAME_START &parameter)
{
	StreamSP stream;
	stream->SetBuffer(buffer);
}
// FRIENDSHIP_GAME_START_WRITE
inline DWORD WRITE_PT_FRIENDSHIP_GAME_START(BYTE *buffer, S_PT_FRIENDSHIP_GAME_START &parameter)
{
	StreamSP stream;
	stream->SetBuffer(buffer);

	return stream->GetLength();
}
inline DWORD WRITE_PT_FRIENDSHIP_GAME_START(BYTE *buffer)
{
	StreamSP stream;
	stream->SetBuffer(buffer);

	return stream->GetLength();
}


/*
	PT_FRIENDSHIP_GAME_START_SUCC
*/
typedef struct _S_PT_FRIENDSHIP_GAME_START_SUCC
{

} S_PT_FRIENDSHIP_GAME_START_SUCC;
// FRIENDSHIP_GAME_START_READ
inline VOID READ_PT_FRIENDSHIP_GAME_START_SUCC(BYTE *buffer, S_PT_FRIENDSHIP_GAME_START_SUCC &parameter)
{
	StreamSP stream;
	stream->SetBuffer(buffer);
}
// FRIENDSHIP_GAME_START_WRITE
inline DWORD WRITE_PT_FRIENDSHIP_GAME_START_SUCC(BYTE *buffer, S_PT_FRIENDSHIP_GAME_START_SUCC &parameter)
{
	StreamSP stream;
	stream->SetBuffer(buffer);

	return stream->GetLength();
}
inline DWORD WRITE_PT_FRIENDSHIP_GAME_START_SUCC(BYTE *buffer)
{
	StreamSP stream;
	stream->SetBuffer(buffer);

	return stream->GetLength();
}


/*
	PT_FRIENDSHIP_GAME_START_FAIL
*/
typedef struct _S_PT_FRIENDSHIP_GAME_START_FAIL
{
	DWORD errorCode;
} S_PT_FRIENDSHIP_GAME_START_FAIL;
// FRIENDSHIP_GAME_START_FAIL_READ
inline VOID READ_PT_FRIENDSHIP_GAME_START_FAIL(BYTE *buffer, S_PT_FRIENDSHIP_GAME_START_FAIL &parameter)
{
	StreamSP stream;
	stream->SetBuffer(buffer);

	stream->ReadDWORD(&parameter.errorCode);
}
// FRIENDSHIP_GAME_START_FAIL_WRITE
inline DWORD WRITE_PT_FRIENDSHIP_GAME_START_FAIL(BYTE *buffer, S_PT_FRIENDSHIP_GAME_START_FAIL &parameter)
{
	StreamSP stream;
	stream->SetBuffer(buffer);

	stream->WriteDWORD(parameter.errorCode);

	return stream->GetLength();
}
inline DWORD WRITE_PT_FRIENDSHIP_GAME_START_FAIL(BYTE *buffer, DWORD errorCode)
{
	StreamSP stream;
	stream->SetBuffer(buffer);

	stream->WriteDWORD(errorCode);

	return stream->GetLength();
}


/*
	PT_ROOM_LEAVE
*/
typedef struct _S_PT_ROOM_LEAVE
{

} S_PT_ROOM_LEAVE;
// ROOM_LEAVE_READ
inline VOID READ_PT_ROOM_LEAVE(BYTE *buffer, S_PT_ROOM_LEAVE &parameter)
{
	StreamSP stream;
	stream->SetBuffer(buffer);
}
// ROOM_LEAVE_WRITE
inline DWORD WRITE_PT_ROOM_LEAVE(BYTE *buffer, S_PT_ROOM_LEAVE &parameter)
{
	StreamSP stream;
	stream->SetBuffer(buffer);

	return stream->GetLength();
}
inline DWORD WRITE_PT_ROOM_LEAVE(BYTE *buffer)
{
	StreamSP stream;
	stream->SetBuffer(buffer);

	return stream->GetLength();
}


/*
	PT_ROOM_LEAVE_SUCC
*/
typedef struct _S_PT_ROOM_LEAVE_SUCC
{

} S_PT_ROOM_LEAVE_SUCC;
// ROOM_LEAVE_SUCC_READ
inline VOID READ_PT_ROOM_LEAVE_SUCC(BYTE *buffer, S_PT_ROOM_LEAVE_SUCC &parameter)
{
	StreamSP stream;
	stream->SetBuffer(buffer);
}
// ROOM_LEAVE_SUCC_WRITE
inline DWORD WRITE_PT_ROOM_LEAVE_SUCC(BYTE *buffer, S_PT_ROOM_LEAVE_SUCC &parameter)
{
	StreamSP stream;
	stream->SetBuffer(buffer);

	return stream->GetLength();
}
inline DWORD WRITE_PT_ROOM_LEAVE_SUCC(BYTE *buffer)
{
	StreamSP stream;
	stream->SetBuffer(buffer);

	return stream->GetLength();
}


/*
	PT_ROOM_LEAVE_FAIL
*/
typedef struct _S_PT_ROOM_LEAVE_FAIL
{
	DWORD errorCode;
} S_PT_ROOM_LEAVE_FAIL;
// ROOM_LEAVE_FAIL_READ
inline VOID READ_PT_ROOM_LEAVE_FAIL(BYTE *buffer, S_PT_ROOM_LEAVE_FAIL &parameter)
{
	StreamSP stream;
	stream->SetBuffer(buffer);

	stream->ReadDWORD(&parameter.errorCode);
}
// ROOM_LEAVE_FAIL_WRITE
inline DWORD WRITE_PT_ROOM_LEAVE_FAIL(BYTE *buffer, S_PT_ROOM_LEAVE_FAIL &parameter)
{
	StreamSP stream;
	stream->SetBuffer(buffer);

	stream->WriteDWORD(parameter.errorCode);

	return stream->GetLength();
}
inline DWORD WRITE_PT_ROOM_LEAVE_FAIL(BYTE *buffer, DWORD errorCode)
{
	StreamSP stream;
	stream->SetBuffer(buffer);

	stream->WriteDWORD(errorCode);

	return stream->GetLength();
}


/*
	PT_GAME_START_ALL
*/
typedef struct _S_PT_GAME_START_ALL
{
	BOOL isWhite;
	BOOL isOfficialGame;
} S_PT_GAME_START_ALL;
// GAME_START_ALL_READ
inline VOID READ_PT_GAME_START_ALL(BYTE *buffer, S_PT_GAME_START_ALL &parameter)
{
	StreamSP stream;
	stream->SetBuffer(buffer);

	stream->ReadBOOL(&parameter.isWhite);
	stream->ReadBOOL(&parameter.isOfficialGame);
}
// GAME_START_ALL_WRITE
inline DWORD WRITE_PT_GAME_START_ALL(BYTE *buffer, S_PT_GAME_START_ALL &parameter)
{
	StreamSP stream;
	stream->SetBuffer(buffer);

	stream->WriteBOOL(parameter.isWhite);
	stream->WriteBOOL(parameter.isOfficialGame);

	return stream->GetLength();
}
inline DWORD WRITE_PT_GAME_START_ALL(BYTE *buffer, BOOL isWhite, BOOL isOfficialGame)
{
	StreamSP stream;
	stream->SetBuffer(buffer);

	stream->WriteBOOL(isWhite);
	stream->WriteBOOL(isOfficialGame);

	return stream->GetLength();
}


/*
	PT_GAME_END_ALL
*/
typedef struct _S_PT_GAME_END_ALL
{

} S_PT_GAME_END_ALL;
// GAME_END_ALL_READ
inline VOID READ_PT_GAME_END_ALL(BYTE *buffer, S_PT_GAME_END_ALL &parameter)
{
	StreamSP stream;
	stream->SetBuffer(buffer);
}
// GAME_END_ALL_WRITE
inline DWORD WRITE_PT_GAME_END_ALL(BYTE *buffer, S_PT_GAME_END_ALL &parameter)
{
	StreamSP stream;
	stream->SetBuffer(buffer);

	return stream->GetLength();
}
inline DWORD WRITE_PT_GAME_END_ALL(BYTE *buffer)
{
	StreamSP stream;
	stream->SetBuffer(buffer);

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
// CHAT_READ
inline VOID READ_PT_CHAT(BYTE *buffer, S_PT_CHAT &parameter)
{
	StreamSP stream;
	stream->SetBuffer(buffer);

	stream->ReadWCHARs(parameter.user_name, 20);
	stream->ReadWCHARs(parameter.message, 40);
}
// CHAT_WRITE
inline DWORD WRITE_PT_CHAT(BYTE *buffer, S_PT_CHAT &parameter)
{
	StreamSP stream;
	stream->SetBuffer(buffer);

	stream->WriteWCHARs(parameter.user_name, 20);
	stream->WriteWCHARs(parameter.message, 40);

	return stream->GetLength();
}
inline DWORD WRITE_PT_CHAT(BYTE *buffer, WCHAR *name, WCHAR *message)
{
	StreamSP stream;
	stream->SetBuffer(buffer);

	WCHAR _name[20] = { 0, };
	_tcsncpy(_name, name, 20);
	stream->WriteWCHARs(_name, 20);
	WCHAR _message[40] = { 0, };
	_tcsncpy(_message, message, 40);
	stream->WriteWCHARs(_message, 40);

	return stream->GetLength();
}


/*
	PT_DELIVERY_CHAT
*/
typedef struct _S_PT_DELIVERY_CHAT
{
	WCHAR user_name[20];
	WCHAR message[40];
} S_PT_DELIVERY_CHAT;
// DELIVERY_CHAT_READ
inline VOID READ_PT_DELIVERY_CHAT(BYTE *buffer, S_PT_DELIVERY_CHAT &parameter)
{
	StreamSP stream;
	stream->SetBuffer(buffer);

	stream->ReadWCHARs(parameter.user_name, 20);
	stream->ReadWCHARs(parameter.message, 40);
}
// DELIVERY_CHAT_WRITE
inline DWORD WRITE_PT_DELIVERY_CHAT(BYTE *buffer, S_PT_DELIVERY_CHAT &parameter)
{
	StreamSP stream;
	stream->SetBuffer(buffer);

	stream->WriteWCHARs(parameter.user_name, 20);
	stream->WriteWCHARs(parameter.message, 40);

	return stream->GetLength();
}
inline DWORD WRITE_PT_DELIVERY_CHAT(BYTE *buffer, WCHAR *name, WCHAR *message)
{
	StreamSP stream;
	stream->SetBuffer(buffer);

	WCHAR _name[20] = { 0, };
	_tcsncpy(_name, name, 20);
	stream->WriteWCHARs(_name, 20);
	WCHAR _message[40] = { 0, };
	_tcsncpy(_message, message, 40);
	stream->WriteWCHARs(_message, 40);

	return stream->GetLength();
}


/*
	PT_PIECE_MOVE
*/
typedef struct _S_PT_PIECE_MOVE
{
	DWORD beforeMoveXpos;
	DWORD beforeMoveYpos;
	DWORD afterMoveXpos;
	DWORD afterMoveYpos;
} S_PT_PIECE_MOVE;
// PIECE_MOVE_READ
inline VOID READ_PT_PIECE_MOVE(BYTE *buffer, S_PT_PIECE_MOVE &parameter)
{
	StreamSP stream;
	stream->SetBuffer(buffer);

	stream->ReadDWORD(&parameter.beforeMoveXpos);
	stream->ReadDWORD(&parameter.beforeMoveYpos);
	stream->ReadDWORD(&parameter.afterMoveXpos);
	stream->ReadDWORD(&parameter.afterMoveYpos);
}
// PIECE_MOVE_WRITE
inline DWORD WRITE_PIECE_MOVE(BYTE *buffer, S_PT_PIECE_MOVE &parameter)
{
	StreamSP stream;
	stream->SetBuffer(buffer);

	stream->WriteDWORD(parameter.beforeMoveXpos);
	stream->WriteDWORD(parameter.beforeMoveYpos);
	stream->WriteDWORD(parameter.afterMoveXpos);
	stream->WriteDWORD(parameter.afterMoveYpos);

	return stream->GetLength();
}
inline DWORD WRITE_PT_PIECE_MOVE(BYTE *buffer, DWORD bXpos, DWORD bYpos, DWORD aXpos, DWORD aYpos)
{
	StreamSP stream;
	stream->SetBuffer(buffer);

	stream->WriteDWORD(bXpos);
	stream->WriteDWORD(bYpos);
	stream->WriteDWORD(aXpos);
	stream->WriteDWORD(aYpos);

	return stream->GetLength();
}


/*
	PT_PIECE_PROMOTION
*/
typedef struct _S_PT_PIECE_PROMOTION
{
	DWORD pieceType;
	DWORD xpos;
	DWORD ypos;
} S_PT_PIECE_PROMOTION;
// PIECE_PROMOTION_READ
inline VOID READ_PT_PIECE_PROMOTION(BYTE *buffer, S_PT_PIECE_PROMOTION &parameter)
{
	StreamSP stream;
	stream->SetBuffer(buffer);

	stream->ReadDWORD(&parameter.pieceType);
	stream->ReadDWORD(&parameter.xpos);
	stream->ReadDWORD(&parameter.ypos);
}
// PIECE_PROMOTION_WRITE
inline DWORD WRITE_PIECE_PROMOTION(BYTE *buffer, S_PT_PIECE_PROMOTION &parameter)
{
	StreamSP stream;
	stream->SetBuffer(buffer);

	stream->WriteDWORD(parameter.pieceType);
	stream->WriteDWORD(parameter.xpos);
	stream->WriteDWORD(parameter.ypos);

	return stream->GetLength();
}
inline DWORD WRITE_PT_PIECE_PROMOTION(BYTE *buffer, DWORD type, DWORD xpos, DWORD ypos)
{
	StreamSP stream;
	stream->SetBuffer(buffer);

	stream->WriteDWORD(type);
	stream->WriteDWORD(xpos);
	stream->WriteDWORD(ypos);

	return stream->GetLength();
}


/*
	PT_OFFICIAL_GAME_WIN
*/
typedef struct _S_PT_OFFICIAL_GAME_WIN
{
	WCHAR userName[20];
} S_PT_OFFICIAL_GAME_WIN;
// OFFICIAL_GAME_WIN_READ
inline VOID READ_PT_OFFICIAL_GAME_WIN(BYTE *buffer, S_PT_OFFICIAL_GAME_WIN &parameter)
{
	StreamSP stream;
	stream->SetBuffer(buffer);

	stream->ReadWCHARs(parameter.userName, 20);
}
// OFFICIAL_GAME_WIN_WRITE
inline DWORD WRITE_PT_OFFICIAL_GAME_WIN(BYTE *buffer, S_PT_OFFICIAL_GAME_WIN &parameter)
{
	StreamSP stream;
	stream->SetBuffer(buffer);

	stream->WriteWCHARs(parameter.userName, 20);

	return stream->GetLength();
}
inline DWORD WRITE_PT_OFFICIAL_GAME_WIN(BYTE *buffer, WCHAR *name)
{
	StreamSP stream;
	stream->SetBuffer(buffer);

	WCHAR _name[20] = { 0, };
	_tcsncpy(_name, name, 20);
	stream->WriteWCHARs(_name, 20);

	return stream->GetLength();
}


/*
	PT_OFFICIAL_GAME_LOSE
*/
typedef struct _S_PT_OFFICIAL_GAME_LOSE
{
	WCHAR userName[20];
} S_PT_OFFICIAL_GAME_LOSE;
// OFFICIAL_GAME_LOSE_READ
inline VOID READ_PT_OFFICIAL_GAME_LOSE(BYTE *buffer, S_PT_OFFICIAL_GAME_LOSE &parameter)
{
	StreamSP stream;
	stream->SetBuffer(buffer);

	stream->ReadWCHARs(parameter.userName, 20);
}
// OFFICIAL_GAME_LOSE_WRITE
inline DWORD WRITE_PT_OFFICIAL_GAME_LOSE(BYTE *buffer, S_PT_OFFICIAL_GAME_LOSE &parameter)
{
	StreamSP stream;
	stream->SetBuffer(buffer);

	stream->WriteWCHARs(parameter.userName, 20);

	return stream->GetLength();
}
inline DWORD WRITE_PT_OFFICIAL_GAME_LOSE(BYTE *buffer, WCHAR *name)
{
	StreamSP stream;
	stream->SetBuffer(buffer);

	WCHAR _name[20] = { 0, };
	_tcsncpy(_name, name, 20);
	stream->WriteWCHARs(_name, 20);

	return stream->GetLength();
}


/*
	PT_FRIENDSHIP_GAME_WIN
*/
typedef struct _S_PT_FRIENDSHIP_GAME_WIN
{
	WCHAR userName[20];
} S_PT_FRIENDSHIP_GAME_WIN;
// FRIENDSHIP_GAME_WIN_READ
inline VOID READ_PT_FRIENDSHIP_GAME_WIN(BYTE *buffer, S_PT_FRIENDSHIP_GAME_WIN &parameter)
{
	StreamSP stream;
	stream->SetBuffer(buffer);

	stream->ReadWCHARs(parameter.userName, 20);
}
// FRIENDSHIP_GAME_WIN_WRITE
inline DWORD WRITE_PT_FRIENDSHIP_GAME_WIN(BYTE *buffer, S_PT_FRIENDSHIP_GAME_WIN &parameter)
{
	StreamSP stream;
	stream->SetBuffer(buffer);

	stream->WriteWCHARs(parameter.userName, 20);

	return stream->GetLength();
}
inline DWORD WRITE_PT_FRIENDSHIP_GAME_WIN(BYTE *buffer, WCHAR *name)
{
	StreamSP stream;
	stream->SetBuffer(buffer);

	WCHAR _name[20] = { 0, };
	_tcsncpy(_name, name, 20);
	stream->WriteWCHARs(_name, 20);

	return stream->GetLength();
}