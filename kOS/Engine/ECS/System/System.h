/******************************************************************/
/*!
\file      System.h
\author    Ng Jaz winn, jazwinn.ng , 2301502
\par       jazwinn.ng@digipen.edu
\date      Oct 02, 2025
\brief     This file contains the system class that is the parent
		   class to all systems in the entity component system

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/********************************************************************/
#ifndef SYSTEM_H
#define SYSTEM_H
#pragma once

#include "ECS/Component/Component.h"
#include "ECS/SparseSet.h"
#include "Events/Delegate.h"


#include "ECS/ECS.h"
#include "ECS/Component/ComponentHeader.h"
#include "Graphics/GraphicsManager.h"
#include "Resources/ResourceManager.h"
#include "Inputs/Input.h"
#include "Physics/PhysicsManager.h"
#include "Scripting/ScriptManager.h"
#include "Debugging/Performance.h"
#include "Audio/AudioManager.h"

namespace ecs { class ECS; }
class ScriptManager;  
class Peformance; 

namespace ecs {



	class ISystem {
	protected:
		ECS& m_ecs;
		GraphicsManager& m_graphicsManager;
		ResourceManager& m_resourceManager;
		Input::InputSystem& m_inputSystem;
		physics::PhysicsManager& m_physicsManager;
		ScriptManager& m_scriptManager;
		Peformance& m_performance;
		audio::AudioManager& m_audioManager;

	public:
		ISystem(ECS& ecs, GraphicsManager& graphics, ResourceManager& rm, Input::InputSystem& is, physics::PhysicsManager& pm, ScriptManager& sm, Peformance& peformance, audio::AudioManager& audiom)
			: m_ecs(ecs)
			, m_graphicsManager(graphics)
			, m_resourceManager(rm)
			, m_inputSystem(is)
			, m_physicsManager(pm) 
			, m_scriptManager(sm)
			, m_performance(peformance)
			, m_audioManager(audiom)
		{
		}


		inline void AssignSignature(ComponentSignature signature) {
			m_systemSignature = signature;
		}

		inline void RegisterSystem(EntityID ID) {

			if(!m_entities.ContainsEntity(ID)){
				m_entities.Set(ID, ID);
				onRegister.Invoke(ID);
			}
		}

		inline void DeregisterSystem(EntityID ID) {
			m_entities.Delete(ID);
			onDeregister.Invoke(ID);
		}

		inline void SetState(const std::bitset<GAMESTATE_COUNT>& state) {
			m_systemGameState = state;
		}

		inline bool TestState(GAMESTATE state) {
			return m_systemGameState.test(state);
		}

		ComponentSignature GetSignature() {
			return m_systemSignature;
		}

		virtual void Init() = 0;
		virtual void Update() = 0;

		Delegate<EntityID> onRegister;
		Delegate<EntityID> onDeregister;
	
	protected:

		SparseSet<EntityID> m_entities;
		ComponentSignature m_systemSignature; // set signature based on what component the systems need
		std::bitset<GAMESTATE_COUNT> m_systemGameState;
	};

}









#endif SYSTEM_H

