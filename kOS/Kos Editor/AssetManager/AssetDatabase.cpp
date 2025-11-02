
#include "Config/pch.h"
#include "AssetDatabase.h"
#include "DeSerialization/json_handler.h"

utility::GUID AssetDatabase::ImportAsset(std::filesystem::path filePath, const std::string& type)
{
	if (!std::filesystem::exists(filePath)) {
		LOGGING_ERROR(filePath.string() + " , filepath does not exist");
	}

	//check if file meta exist
	std::filesystem::path metaPath = filePath.string() + ".meta";
	
	utility::GUID returnGUID;
	if (std::filesystem::exists(metaPath)) {
		AssetData data = Serialization::ReadJsonFile<AssetData>(metaPath.string());
		returnGUID = data.GUID;
	}
	else {
		//Generate Meta file
		utility::GUID newGUID = utility::GenerateGUID();

		

		AssetData data;
		data.GUID = newGUID;
		data.Type = type;

		Serialization::WriteJsonFile(metaPath.string(), &data);


		returnGUID = newGUID;
	}



	m_pathToGUID[filePath] = returnGUID;

	return returnGUID;
}


