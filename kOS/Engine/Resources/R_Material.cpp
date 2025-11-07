#include "Config/pch.h"
#include "R_Material.h"
#include "..\DeSerialization\json_handler.h"
void R_Material::Load()
{
   this->md= serialization::ReadJsonFile<MaterialData>(this->GetFilePath().string());
   std::cout << "MATERIAL PATH" << this->GetFilePath().string() << '\n';
   std::cout << "MATERIAL LOADED" << this->md.diffuseMaterialGUID.GetToString() << '\n';
   //Get resources from texture guid and load them
}
void R_Material::Unload(){}