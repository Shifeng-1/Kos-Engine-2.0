/******************************************************************/
/*!
\file      R_Audio.h
\author    Chiu Jun Jie
\par	   junjie.c@digipen.edu
\date      Oct 03, 2025
\brief     This file contains the definition of the Audio Resource
		   class. It handles the loading and unloading of the Audio
		   through the resource manager.


Copyright (C) 2025 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/********************************************************************/
#include "Config/pch.h"
#include "R_Audio.h"

void R_Audio::Load()
{
	FMOD::System* sys = m_system;
	if (!sys) {
		return;
	}

	if (m_filePath.empty()) {
		return;
	}

	//Check if file path exist
	if (!std::filesystem::exists(m_filePath)) {
		std::cout << "[Audio] file not found: " << m_filePath.string() << "\n";
		return;
	}

	//Reset sound if have sound
	if (m_sound) { 
		m_sound->release(); 
		m_sound = nullptr; }

	unsigned int flags = FMOD_DEFAULT;
	if (m_createFlags != 0) {
		flags = m_createFlags;
	}
	FMOD_RESULT r = sys->createSound(m_filePath.string().c_str(), flags, nullptr, &m_sound);

	if (r != FMOD_OK || !m_sound) {
		r = sys->createSound(m_filePath.string().c_str(), flags | FMOD_CREATESTREAM, nullptr, &m_sound);
		if (r != FMOD_OK || !m_sound) {
			m_sound = nullptr;
			return;
		}
	}
}

void R_Audio::Unload()
{
	//Release all sounds
	if (m_sound) {
		m_sound->release();
		m_sound = nullptr;
	}
}
