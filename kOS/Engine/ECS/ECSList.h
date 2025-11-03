/******************************************************************/
/*!
\file      ECSList.h
\author    Ng Jaz winn, jazwinn.ng , 2301502
\par       jazwinn.ng@digipen.edu
\date      Oct 02, 2025
\brief     This file contains all the data that is frequently used in
		   other non-ECS systems.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/********************************************************************/
#ifndef ECSLIST_H
#define ECSLIST_H
#include "Config/pch.h"

namespace ecs {

	constexpr size_t MAXCOMPONENT = 64;
	constexpr size_t MAXSYSTEM = 64;
	using EntityID = int;
	using ComponentSignature = std::bitset<MAXCOMPONENT>;

	// Max number of entities allowed to be created
	static EntityID MaxEntity = 2000;

	enum GAMESTATE {
		START,
		RUNNING,
		STOP,
		WAIT,
		TERMINATE,
		GAMESTATE_COUNT
	};

}



#endif ECSLIST_H