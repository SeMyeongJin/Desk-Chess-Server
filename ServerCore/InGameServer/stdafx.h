// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "../KomorisLib/ServerLibrary.h"

#define SERVER_PORT					3500
#define MAX_USER					100

typedef enum _USER_STATUS
{
	US_NONE,
	US_LOBBY_ENTERING,
	US_LOBBY_ENTERED,
	US_GAME_STARTING,
	US_GAME_STARTED,
	US_GAME_END
} USER_STATUS;