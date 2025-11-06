/******************************************************************/
/*!
\file      json_handler.cpp
\author    Jaz Winn Ng, jazwinn.ng, 2301502 
\par       jazwinn.ngc@digipen.edu
\date      Aug 29, 2025
\brief     This file handles JSON-related operations for component serialization and deserialization in the ECS framework.
			- m_LoadConfig: Loads configuration settings such as window dimensions and FPS cap from a config file.
			- m_JsonFileValidation: Validates if the JSON file exists and creates a new one if it doesn't.
			- m_LoadComponentsJson: Loads component data from a JSON file into the ECS.
			- m_SaveComponentsJson: Saves component data from the ECS to a JSON file.
			- m_SaveEntity: Serializes individual entity data to a JSON structure.
			- m_LoadEntity: Deserializes individual entity data from a JSON structure.
			- m_LoadEntity: Deserializes individual entity data from a JSON structure.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/********************************************************************/
/********************************************************************/

//REFLECTION

//SAVING

//template <typename T>//T refers to component
//void m_saveComponentreflect(T* component, rapidjson::Value& entityData, rapidjson::Document::AllocatorType& allocator);

//LOADING

//template <typename T>//T refers to component
//void m_LoadComponentreflect(T* component, rapidjson::Value& entityData);


/********************************************************************/
#include "Config/pch.h"
#include "json_handler.h"

#include <RAPIDJSON/filewritestream.h>
#include <RAPIDJSON/istreamwrapper.h>
#include <RAPIDJSON/prettywriter.h>
#include <RAPIDJSON/filereadstream.h>


#include "ECS/ECS.h"
#include "Debugging/Logging.h"
#include "ECS/ecs.h"


#include "Physics/PhysicsLayer.h"



namespace serialization{

	void  Serialization::LoadScene(const std::filesystem::path& jsonFilePath, const std::string sceneName)
	{
		// Open the JSON file for reading
		std::ifstream inputFile(jsonFilePath.string());

		if (!inputFile) {
			LOGGING_ERROR("Failed to open JSON file for reading: {}", jsonFilePath.string().c_str());
			return;
		}

		std::string fileContent((std::istreambuf_iterator<char>(inputFile)), std::istreambuf_iterator<char>());
		inputFile.close();

		// Parse the JSON content
		rapidjson::Document doc;
		doc.Parse(fileContent.c_str());

		std::string scenename = sceneName.empty() ? jsonFilePath.filename().string() : sceneName;

		/*******************INSERT INTO FUNCTION*****************************/

		// Iterate through each component entry in the JSON array
		for (rapidjson::SizeType i = 0; i < doc.Size(); i++) {
			const rapidjson::Value& entityData = doc[i];

			if (entityData.HasMember(SceneData::classname())) {
				//load scene data
				SceneData sceneData;

				LoadComponentreflect(&sceneData, entityData);

				
				m_ecs.AddScene(scenename, sceneData);

			}
			else {
				LoadEntity(entityData, std::nullopt, scenename);
			}
		}

		LOGGING_INFO("Load Json Successful");
	}

	void Serialization::SaveScene(const std::filesystem::path& scene, const std::filesystem::path& targetFilePath)
	{
		std::string jsonFilePath = targetFilePath.empty() ? scene.string() : targetFilePath.string();
		JsonFileValidation(jsonFilePath);

		// Create JSON object to hold the updated values
		rapidjson::Document doc;

		if (!doc.IsArray()) {
			doc.SetArray();  // Initialize as an empty array
		}

		rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();

		//save scene data
		auto sceneName = scene.filename().string();
		{

			SceneData data;
			if (m_ecs.sceneMap.find(sceneName) != m_ecs.sceneMap.end())
			{
				data = m_ecs.sceneMap.at(sceneName);
			}

			rapidjson::Value sceneData(rapidjson::kObjectType);
			saveComponentreflect(&data, sceneData, allocator);
			doc.PushBack(sceneData, allocator);
		}



		{
			if (m_ecs.sceneMap.find(sceneName) != m_ecs.sceneMap.end())
			{
				std::unordered_set<ecs::EntityID> savedEntities;  //track saved entities
				//Start saving the entities
				std::vector<ecs::EntityID> entities = m_ecs.sceneMap.find(sceneName)->second.sceneIDs;
				for (const auto& entityId : entities) {
					if (!m_ecs.GetParent(entityId).has_value()) {
						SaveEntity(entityId, doc, allocator, savedEntities);
					}
				}
			}
		}



		// Write the JSON back to file
		rapidjson::StringBuffer writeBuffer;
		rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(writeBuffer);
		doc.Accept(writer);

		std::ofstream outputFile(jsonFilePath);
		//if file exist
		if (outputFile) {
			outputFile << writeBuffer.GetString();
			outputFile.close();
		}
		else { //craete new file
			std::ofstream createFile(jsonFilePath);
			createFile << writeBuffer.GetString();
			createFile.close();
		}

		LOGGING_INFO("Save Json Successful");
	}

	void Serialization::SaveEntity(ecs::EntityID entityId, rapidjson::Value& parentArray, rapidjson::Document::AllocatorType& allocator, std::unordered_set<ecs::EntityID>& savedEntities) {
		auto signature = m_ecs.GetEntitySignature(entityId);

		if (savedEntities.find(entityId) != savedEntities.end()) {
			return;
		}

		rapidjson::Value entityData(rapidjson::kObjectType);


		{//Update entity without GUID
			ecs::NameComponent* nameComp = m_ecs.GetComponent<ecs::NameComponent>(entityId);
			if(nameComp) {
				if (nameComp->entityGUID.Empty()) {
					nameComp->entityGUID = utility::GenerateGUID();
					m_ecs.InsertGUID(nameComp->entityGUID, entityId);
				}
				std::string GUIDString = nameComp->entityGUID.GetToString();
				rapidjson::Value key("entityGUID", allocator); // key name
				rapidjson::Value value;
				value.SetString(GUIDString.c_str(),
					static_cast<rapidjson::SizeType>(GUIDString.size()),
					allocator);

				entityData.AddMember(key, value, allocator);
			}

		}




		const auto& componentKey = m_ecs.GetComponentKeyData();
		for (const auto& [ComponentName, key] : componentKey) {
			if (signature.test(m_ecs.GetComponentKey(ComponentName))) {
				auto* component = m_ecs.GetIComponent<ecs::Component*>(ComponentName, entityId);
				if (component) {
					auto& actionInvoker = m_ecs.componentAction[ComponentName];
					actionInvoker->Save(component, entityData, allocator);

				}
			}
		}


		// Add children
		std::optional<std::vector<ecs::EntityID>> childrenOptional = m_ecs.GetChild(entityId);
		if (childrenOptional.has_value()) {
			std::vector<ecs::EntityID> children = childrenOptional.value();
			if (!children.empty()) {
				rapidjson::Value childrenArray(rapidjson::kArrayType);
				for (const auto& childID : children) {
					SaveEntity(childID, childrenArray, allocator, savedEntities);
				}
				entityData.AddMember("children", childrenArray, allocator);
			}
		}

		parentArray.PushBack(entityData, allocator);
		savedEntities.insert(entityId);
	}

	void Serialization::LoadEntity(const rapidjson::Value& entityData, std::optional<ecs::EntityID> parentID, const std::string& sceneName)
	{
		ecs::EntityID newEntityId = m_ecs.CreateEntity(sceneName);

		{
			//Set GUID
			if (entityData.HasMember("entityGUID") && entityData["entityGUID"].IsString()) {
				std::string guidStr = entityData["entityGUID"].GetString();
				ecs::NameComponent* nameComp = m_ecs.GetComponent<ecs::NameComponent>(newEntityId);
				if (nameComp) {
					nameComp->entityGUID.SetFromString(guidStr);
					
					m_ecs.InsertGUID(nameComp->entityGUID, newEntityId);
				}
			}
		}

		

		const auto& componentKey = m_ecs.GetComponentKeyData();
		for (const auto& [ComponentName, key] : componentKey) {
			if (entityData.HasMember(ComponentName.c_str()) && entityData[ComponentName.c_str()].IsObject()) {
				auto& actionInvoker = m_ecs.componentAction[ComponentName];
				actionInvoker->Load(newEntityId, entityData);
			}

		}


		//Attach entity to parent
		if (parentID.has_value()) {
			m_ecs.SetParent(parentID.value(), newEntityId);
		}

		// Load children 
		if (entityData.HasMember("children") && entityData["children"].IsArray()) {
			const rapidjson::Value& childrenArray = entityData["children"];
			for (rapidjson::SizeType i = 0; i < childrenArray.Size(); i++) {
				LoadEntity(childrenArray[i], newEntityId, sceneName);
			}
		}
	}

}





