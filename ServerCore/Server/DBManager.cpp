#include "stdafx.h"
#include "DBManager.h"

DBManager * GDBManager = new DBManager();

DBManager::DBManager()
{
	DB_HOST = "localhost";
	DB_NAME = "testdb";
	DB_SOCK = NULL;
	DB_USER = "root";
	DB_PASS = "0306";
}


DBManager::~DBManager()
{
	mysql_free_result(res);

	mysql_close(conn);
}

BOOL DBManager::Begin(VOID)
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
		printf("( %s | %s | %s | %s )\n\n", row[0], row[1], row[2], row[3]);
	}

	return TRUE;
}

BOOL DBManager::RegistUserQuery(WCHAR *id, WCHAR *pw, WCHAR *name, DWORD * errorCode)
{
	// wchar to string
	wstring _wid(id);
	wstring _wpw(pw);
	wstring _wname(name);
	
	string _sid(_wid.begin(), _wid.end());
	string _spw(_wpw.begin(), _wpw.end());
	string _sname(_wname.begin(), _wname.end());

	string checkErrQuery = "SELECT UserID, UserName from USER";
	if (mysql_query(conn, checkErrQuery.c_str()) != NULL)
	{
		cout << "SELECT UserID from Table error : " << mysql_error(conn) << endl;
		return FALSE;
	}
	res = mysql_store_result(conn);
	while ((row = mysql_fetch_row(res)) != NULL)
	{
		if (_sid == row[0])
		{
			*errorCode = EC_SIGNUP_ID_ALREADY_REGIST;
			return FALSE;
		}
		if (_sname == row[1])
		{
			*errorCode = EC_SIGNUP_NAME_ALREADY_REGIST;
			return FALSE;
		}
	}

	if (_sid.size() > 20)
	{
		*errorCode = EC_SIGNUP_ID_ERROR;
		return FALSE;
	}

	if (_spw.size() > 20)
	{
		*errorCode = EC_SIGNUP_PW_ERROR;
		return FALSE;
	}

	if (_sname.size() > 20)
	{
		*errorCode = EC_SIGNUP_NAME_ERROR;
		return FALSE;
	}

	string query = "INSERT INTO USER (UserID, UserPassword, UserName) VALUE('";
	query += _sid;
	query += "', '";
	query += _spw;
	query += "', '";
	query += _sname;
	query += "')";

	if (mysql_query(conn, query.c_str()) != NULL)
	{
		cout << "regist user query error : " << mysql_error(conn) << endl;
		return FALSE;
	}

	_tprintf(_T("RegisterUserQuery Success!\n"));

	return TRUE;
}

BOOL DBManager::LoginCheckQuery(WCHAR *id, WCHAR *pw)
{
	// wchar to string
	wstring _wid(id);	
	wstring _wpw(pw);

	string _sid(_wid.begin(), _wid.end());
	string _spw(_wpw.begin(), _wpw.end());

	string query = "SELECT * FROM USER WHERE UserID = '";
	query += _sid;
	query += "' AND UserPassword = '";
	query += _spw + "'";

	if (mysql_query(conn, query.c_str()) != NULL)
	{
		cout << "Login check query error : " << mysql_error(conn) << endl;
		return FALSE;
	}
	
	res = mysql_store_result(conn);
	
	if (res->row_count == 0) return FALSE;
	
	while ((row = mysql_fetch_row(res)) != NULL)
	{
		if (row[0] == _sid)
		{
			if (row[1] == _spw)
				break;
			else
				return FALSE;
		}
		else
			return FALSE;
	
	}

	cout << "Login Success!" << endl;

	// 로그인 성공
	return TRUE;
}

BOOL DBManager::LoadUserData(WCHAR *id, WCHAR *pw, WCHAR *name, INT32 * lifePoint)
{
	wstring _wid(id);
	wstring _wpw(pw);

	string _sid(_wid.begin(), _wid.end());
	string _spw(_wpw.begin(), _wpw.end());
	string point;

	string query = "SELECT UserName, LifePoint FROM USER WHERE UserID = '";
	query += _sid;
	query += "' and UserPassword = '";
	query += _spw + "'";
	
	if (mysql_query(conn, query.c_str()) != NULL)
	{
		cout << "LoadUserData query error : " << mysql_error(conn) << endl;
		return FALSE;
	}

	res = mysql_use_result(conn);
	while ((row = mysql_fetch_row(res)) != NULL)
	{
		_tcsncpy(name, (const WCHAR *)row[0], 20);
		point = row[1];
		*lifePoint = atoi(point.c_str());
	}

	return TRUE;
}