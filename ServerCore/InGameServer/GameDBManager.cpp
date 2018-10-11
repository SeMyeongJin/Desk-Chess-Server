#include "stdafx.h"
#include "GameDBManager.h"

GameDBManager GgameDBManager;

GameDBManager::GameDBManager()
{
	DB_HOST = "localhost";
	DB_NAME = "testdb";
	DB_SOCK = NULL;
	DB_USER = "root";
	DB_PASS = "0306";
}


GameDBManager::~GameDBManager()
{
	mysql_free_result(res);

	mysql_close(conn);
}

BOOL GameDBManager::Begin(VOID)
{
	conn = mysql_init(NULL);

	// DB connection
	if (!mysql_real_connect(conn, DB_HOST, DB_USER, DB_PASS, DB_NAME, DB_PORT, DB_SOCK, DB_OPT))
	{
		cout << "DB connect query error : " << mysql_error(conn) << endl;
		return FALSE;
	}

	// show tables
	if (mysql_query(conn, "SHOW TABLES"))
	{
		cout << "show tables query error : " << mysql_error(conn) << endl;
		return FALSE;
	}

	res = mysql_use_result(conn);
	printf(("DB Show Tables in %s\n"), DB_NAME);
	while ((row = mysql_fetch_row(res)) != NULL)
	{
		printf(("%s\n"), row[0]);
	}

	// select * from 'table'
	if (mysql_query(conn, "SELECT * FROM USER"))
	{
		cout << "select * from 'table' query error : " << mysql_error(conn) << endl;
		return FALSE;
	}

	res = mysql_use_result(conn);
	while ((row = mysql_fetch_row(res)) != NULL)
	{
		printf("( %s | %s | %s | %s | %s | %s )\n\n", row[0], row[1], row[2], row[3], row[4], row[5]);
	}

	return TRUE;
}

BOOL GameDBManager::WinGame(WCHAR *name)
{
	wstring _wname(name);

	string _sname(_wname.begin(), _wname.end());
	
	string beforeWinRate;
	int tempRate;
	string afterWinRate;

	string query = "Select Win from User where username = '";
	query += _sname;
	query += "'";

	if (mysql_query(conn, query.c_str()) != NULL)
	{
		cout << "Win query error : 1" << mysql_error(conn) << endl;
		return FALSE;
	}

	res = mysql_use_result(conn);
	while ((row = mysql_fetch_row(res)) != NULL)
	{
		beforeWinRate = row[0];
		tempRate = atoi(beforeWinRate.c_str());
		tempRate += 1;
		afterWinRate = to_string(tempRate);
	}

	string query2 = "Update user set Win = '";
	query2 += afterWinRate;
	query2 += "' where username ='";
	query2 += _sname;
	query2 += "'";

	if (mysql_query(conn, query2.c_str()) != NULL)
	{
		cout << "Win query error : 2" << mysql_error(conn) << endl;
		return FALSE;
	}

	return TRUE;
}

BOOL GameDBManager::LoseGame(WCHAR *name)
{
	wstring _wname(name);

	string _sname(_wname.begin(), _wname.end());

	string beforeLoseRate;
	int tempRate;
	string afterLoseRate;

	string query = "Select Lose from User where username = '";
	query += _sname;
	query += "'";

	if (mysql_query(conn, query.c_str()) != NULL)
	{
		cout << "Lose query error : 1" << mysql_error(conn) << endl;
		return FALSE;
	}

	res = mysql_use_result(conn);
	while ((row = mysql_fetch_row(res)) != NULL)
	{
		beforeLoseRate = row[0];
		tempRate = atoi(beforeLoseRate.c_str());
		tempRate += 1;
		afterLoseRate = to_string(tempRate);
	}

	string query2 = "Update user set Lose = '";
	query2 += afterLoseRate;
	query2 += "' where username ='";
	query2 += _sname;
	query2 += "'";

	if (mysql_query(conn, query2.c_str()) != NULL)
	{
		cout << "Lose query error : 2" << mysql_error(conn) << endl;
		return FALSE;
	}

	return TRUE;
}