// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "../KomorisLib/ServerLibrary.h"

#include "../KomorisLib/CriticalSection.h"
#include "../KomorisLib/MultiThreadSync.h"
#include "../KomorisLib/MemoryPool.h"
#include "../KomorisLib/CircularQueue.h"
#include "../KomorisLib/NetworkWrapping.h"
#include "../KomorisLib/PacketWrapping.h"
#include "../KomorisLib/Iocp.h"
#include "../KomorisLib/Stream.h"
#include "../KomorisLib/MiniDump.h"


#define SERVER_PORT					3500
#define MAX_USER					100

#define SPACESHIP_HP				2500
#define TOWER_HP					1000
#define GAME_TIME					600


typedef enum _USER_STATUS
{
	US_NONE,
	US_LOBBY_ENTERING,
	US_LOBBY_ENTERED,
	US_GAME_STARTING,
	US_GAME_STARTED,
	US_GAME_END
} USER_STATUS;


#include "UserInfo.h"
#include "UserInfoManager.h"
#include "GameRoom.h"