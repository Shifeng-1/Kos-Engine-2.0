#include "Config/pch.h"
#include "R_DepthMapCube.h"
void R_DepthMapCube::Load() {
	std::cout <<"LOADED CUBE MAP" << this->m_filePath.string() << '\n';
	dcm.LoadDepthCubeMap(this->GetFilePath().string());
}
void R_DepthMapCube::Unload() {

}