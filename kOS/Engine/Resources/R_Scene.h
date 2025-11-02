/******************************************************************/
/*!
\file      R_Scene.h
\author    Jaz Winn Ng
\par       jazwinn.ng@digipen.edu
\date      Sept 28, 2025
\brief	   Scene resource class for loading and unloading scene data.

Copyright (C) 2025 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************/
#pragma once

#include "Resource.h"

class R_Scene :public Resource
{

public:
	using Resource::Resource;

	void Load() override;

	void Unload() override;

	int DuplicatePrefabIntoScene(const std::string& scene);

	REFLECTABLE(R_Scene);


};

