/******************************************************************/
/*!
\file      ResourceManager.h
\author    Jaz Winn Ng
\par       jazwinn.ng@digipen.edu
\date      Sept 28, 2025
\brief	   Resource Manager class for managing various resource types like textures, models, audio, etc.
			- LoadResource: Loads a resource from a file path and returns a shared pointer to it.
			- GetResource: Retrieves a resource by its GUID.
			- UnloadResource: Unloads a resource by its GUID.
			- ClearAllResources: Unloads all resources managed by the ResourceManager.
			- ResourceExists: Checks if a resource with the given GUID exists in the manager.
			- GetAllResourcesOfType: Retrieves all resources of a specific type.

Copyright (C) 2025 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************/
#pragma once

#include "Config/pch.h"
#include "Resources/Resource.h"
#include "Resources/R_Model.h"
#include "Resources/R_Font.h"
#include "Resources/R_Scene.h"
#include "Resources/R_Animation.h"
#include "Resources/R_Audio.h"
#include "Resources/R_Material.h"
class ResourceManager {

public:

    ResourceManager() {
        RegisterResourceType<R_Model>(".mesh");
        RegisterResourceType<R_Font>(".fntc");
		RegisterResourceType<R_Texture>(".dds");
		RegisterResourceType<R_Scene>(".scene");
		RegisterResourceType<R_Animation>(".ani");
		RegisterResourceType<R_Audio>(".wav");
		RegisterResourceType<R_Material>(".mat");
        //Wait for texture type
    }

	~ResourceManager() = default;

    static std::shared_ptr<ResourceManager> GetInstance() {
        if (!m_instancePtr)
        {
            m_instancePtr = std::make_shared<ResourceManager>();
        }
        return m_instancePtr;
    }

	void Init(const std::string& Directory) {
		m_resourceDirectory = Directory;
	}





	template<typename T>
	std::shared_ptr<T> GetResource(const std::string& GUID) {
		//check if resrouce is already loaded
		if (GUID.empty()) return nullptr;

		if (m_resourceMap.find(GUID) != m_resourceMap.end()) {

			auto asset = m_resourceMap.at(GUID);
			if (asset) {
				return std::static_pointer_cast<T>(asset);
			}
		}

		std::string className = T::classname();

		//check if resource is registered
		if (m_resourceExtension.find(className) == m_resourceExtension.end()) {
			LOGGING_ASSERT_WITH_MSG(className + " : Not registered");
		}

		//Asset not loaded

        //create file path
        std::string path = m_resourceDirectory + "/" + className + "/" + GUID + m_resourceExtension.at(className);

		//Check if file path exists

		//load asset
		auto asset = std::make_shared<T>(GUID, path);
		asset->Load();
		m_resourceMap[GUID] = asset;
		return asset;


	}


	inline void CollectGarbage() {
		for (auto it = m_resourceMap.begin(); it != m_resourceMap.end();) {
			if (it->second.use_count() == 1) {
				LOGGING_INFO("Unloading Asset UID: " + it->first);
				it->second->Unload();
				it = m_resourceMap.erase(it);
			}
			else {
				++it;
			}
		}
	}

	std::string GetResourceDirectory() const { return m_resourceDirectory; }

private:
	template<typename T>
	void RegisterResourceType(const std::string& extension) {
		std::string className = T::classname();
		m_resourceExtension[T::classname()] = extension;
	}

private:

	static std::shared_ptr<ResourceManager> m_instancePtr;

	std::unordered_map<std::string, std::string> m_resourceExtension;


	//Key - GUID
	std::unordered_map<std::string, std::shared_ptr<Resource>> m_resourceMap;
	std::string m_resourceDirectory;
};