#pragma once
#include "Config/pch.h"
#include "Reflection/Reflection.h"

enum class Arguments {
	None,
	Animation,
	Mesh
};
struct AssetData
{
	utility::GUID GUID;
	std::string Type;
	Arguments Arg1 = Arguments::None;

	REFLECTABLE(AssetData, GUID, Type, Arg1);
};



class AssetDatabase {
	//return GUID
public:
	utility::GUID ImportAsset(std::filesystem::path filePath, const std::string& type);

	utility::GUID GetGUID(std::filesystem::path filePath) {
		if (m_pathToGUID.find(filePath) == m_pathToGUID.end()) {
			throw std::runtime_error("Asset not imported to Database");
		}

		return m_pathToGUID.at(filePath);
	}

private:
	std::unordered_map<std::filesystem::path, utility::GUID> m_pathToGUID;

};


