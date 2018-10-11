#pragma once
class GameDBManager
{
public:
	GameDBManager();
	virtual ~GameDBManager();

public:
	BOOL Begin(VOID);

	// when user won the game
	BOOL WinGame(WCHAR *name);
	// when user lost the game
	BOOL LoseGame(WCHAR *name);

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