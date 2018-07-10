#pragma once
class DBManager
{
public:
	DBManager();
	~DBManager();

public:
	BOOL Begin(VOID);

	// ���� ��� ����
	BOOL RegistUserQuery(WCHAR *id, WCHAR *pw, WCHAR *name);
	// �α��� üũ ����
	BOOL LoginCheckQuery(WCHAR *id, WCHAR *pw);

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

	MYSQL			*conn;			// DB ����
	MYSQL_RES		*res;			// ���� ���
	MYSQL_ROW		row;			// ������� ��Ÿ�� ���ڿ�
};

extern DBManager *GDBManager;