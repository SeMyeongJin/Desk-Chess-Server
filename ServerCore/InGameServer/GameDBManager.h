#pragma once
class GameDBManager
{
public:
	GameDBManager();
	virtual ~GameDBManager();

public:
	BOOL Begin(VOID);

	// when users request their infomation
	BOOL ReqUserInfo(WCHAR *name, DWORD * rating, DWORD * off_win, DWORD * off_lose, DWORD * fri_win);
	// when user won the game
	BOOL WinOfficialGame(WCHAR *name);
	// when user lost the game
	BOOL LoseOfficialGame(WCHAR *name);
	// when user won friendship game
	BOOL WinFriendshipGame(WCHAR *name);

private:
	const char * DB_HOST;
	const char * DB_NAME;
	const char * DB_SOCK;
	const char * DB_USER;
	const char * DB_PASS;

	enum {
		DB_PORT = 3306,
		DB_OPT = 0
	};

	MYSQL			*conn;			// DB 연결
	MYSQL_RES		*res;			// 쿼리 결과
	MYSQL_ROW		row;			// 결과값을 나타낼 문자열
};