#include "Config/pch.h"
#include "R_Material.h"
#include "..\DeSerialization\json_handler.h"
void R_Material::Load()
{
   this->md= Serialization::ReadJsonFile<MaterialData>(this->GetFilePath().string());
   std::cout << "MATERIAL PATH" << this->GetFilePath().string() << '\n';
   std::cout << "MATERIAL LOADED" << this->md.diffuseMaterialGUID << '\n';
}
void R_Material::Unload(){}