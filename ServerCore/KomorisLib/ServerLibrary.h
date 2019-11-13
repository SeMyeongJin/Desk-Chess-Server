#pragma once

#include <iostream>
#include <tchar.h>

#include <WinSock2.h>
#include <Windows.h>
#include <WS2tcpip.h>
#include <assert.h>
#include <time.h>
#include <DbgHelp.h>
#include <MSWSock.h>

#include <vector>
#include <string>

using namespace std;

#pragma warning(disable : 4996)

enum IO_TYPE
{
	IO_ACCEPT,
	IO_READ,
	IO_WRITE
};

typedef struct _OVERLAPPED_EX
{
	OVERLAPPED	overlapped;
	IO_TYPE		ioType;
	VOID		*object;
}OVERLAPPED_EX;

#define MAX_BUFFER_LENGTH	256

#include "Log.h"