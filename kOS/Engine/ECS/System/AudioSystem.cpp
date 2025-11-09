/******************************************************************/
/*!
\file      AudioSystem.cpp
\author    Chiu Jun Jie
\par       junjie.c@digipen.edu
\date      Oct 03, 2025
\brief     This file implements the definition of the AudioSystem
		   class. It handles the logic behind the Audio played in
		   the engine and works with the Audio Manager interface.


Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/********************************************************************/
#include "Config/pch.h"
#include "AudioSystem.h"
#include "Resources/ResourceManager.h"
#include "Audio/AudioManager.h"

namespace ecs {

	void AudioSystem::Init() {
	}

	inline FMOD_VECTOR ToF(const glm::vec3& v) { return FMOD_VECTOR{ v.x, v.y, v.z }; }

	void AudioSystem::Update() {

		if (auto* core = m_audioManager.GetCore()) {
			FMOD_VECTOR p = ToF(m_listenerPos);
			FMOD_VECTOR v{ 0,0,0 };
			FMOD_VECTOR f = ToF(m_listenerFwd);
			FMOD_VECTOR u = ToF(m_listenerUp);
			core->set3DListenerAttributes(0, &p, &v, &f, &u);
		}

		const auto& entities = m_entities.Data();
		for (const EntityID id : entities) {
			auto* transform = m_ecs.GetComponent<TransformComponent>(id);
			auto* nameComp = m_ecs.GetComponent<NameComponent>(id);
			auto* audioComp = m_ecs.GetComponent<AudioComponent>(id);

			if (!nameComp || !audioComp) continue;

			if (nameComp->hide) continue;

			//Loop through all audio files
			for (auto& af : audioComp->audioFiles) {

				if (af.use3D && transform && af.channel) {
					FMOD::Channel* ch = static_cast<FMOD::Channel*>(af.channel);
					glm::vec3 pos = transform->WorldTransformation.position;
					FMOD_VECTOR fpos{ pos.x, pos.y, pos.z };
					FMOD_VECTOR fvel{ 0,0,0 };
					ch->set3DAttributes(&fpos, &fvel);
				}

				if (!af.requestPlay) continue;
				if (af.audioGUID.Empty()) continue;

				//Get GUID
				auto res = m_resourceManager.GetResource<R_Audio>(af.audioGUID);
				if (!res) continue;

				//LOad sound
				if (!res->GetSound()) {
					res->SetSystem(m_audioManager.GetCore());

					res->Load();
				}

				FMOD::Sound* sound = res->GetSound();
				FMOD::System* system = res->GetSystem();

				if (!sound || !system) continue;

				//Player sound
				if (af.use3D) {
					sound->setMode(FMOD_3D | FMOD_3D_LINEARROLLOFF);

					const float minD = std::max(0.01f, af.minDistance);
					const float maxD = std::max(minD + 0.01f, af.maxDistance);
					sound->set3DMinMaxDistance(minD, maxD);
				}
				else {
					sound->setMode(FMOD_2D | FMOD_LOOP_OFF);
				}

				FMOD::Channel* ch = nullptr;
				if (system->playSound(sound, nullptr, true, &ch) == FMOD_OK && ch) {

					ch->setVolume(std::clamp(af.volume, 0.0f, 1.0f));

					if (af.use3D) {
						const glm::vec3 pos = transform ? transform->WorldTransformation.position : glm::vec3(0);
						const FMOD_VECTOR fpos{ pos.x, pos.y, pos.z };
						const FMOD_VECTOR fvel{ 0,0,0 };
						ch->set3DAttributes(&fpos, &fvel);
						ch->set3DLevel(1.0f);

						if (af.loop) {
							ch->setMode(FMOD_LOOP_NORMAL);
							ch->setLoopCount(-1);
						}
						else {
							ch->setMode(FMOD_LOOP_OFF);
							ch->setLoopCount(0);
						}
					}
					else {
						ch->setPan(std::clamp(af.pan, -1.0f, 1.0f));
						if (af.loop) {
							ch->setMode(FMOD_LOOP_NORMAL);
							ch->setLoopCount(-1);
						}
						else {
							ch->setMode(FMOD_LOOP_OFF);
							ch->setLoopCount(0);
						}
					}

					ch->setPaused(false);
					af.channel = ch;
				}

				//Ensure only play once per frame
				af.requestPlay = false;
			}
		}

		//Update sound every frame
		if (auto* core = m_audioManager.GetCore()) {
			core->update();
		}
	}

	void AudioSystem::SetListener(const glm::vec3& pos,
		const glm::vec3& fwd,
		const glm::vec3& up) {
		m_listenerPos = pos;
		m_listenerFwd = fwd;
		m_listenerUp = up;
	}

}
