/******************************************************************/
/*!
\file      Input.cpp
\author    Elijah Teo, teo.e , 2301530
\par       teo.e@digipen.edu
\date      16 Sept, 2024
\brief     defines the class functions that contains all the callback functions that will be given to GLFW along with variables to hold the return values

The header provides declarations for the Performance class functions that
handle performance tracking and output for various engine systems such as
movement, rendering, and collision.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************/

#include "Config/pch.h"
#include "ECS/ECS.h"
#include "Input.h"
#include "../Kos Editor/Application/Window.h"

namespace Input {
	/*--------------------------------------------------------------
	  GLOBAL VARAIBLE
	--------------------------------------------------------------*/

	// The number of frames needed for a button to be pressed before the state turns from triggered to pressed.
	float secondsBeforePressed = 0.05f;

	// ECS
	auto* ecs = ecs::ECS::GetInstance();
	std::shared_ptr<InputSystem> InputSystem::m_InstancePtr = nullptr;
	// Shared pointer
	//std::shared_ptr<InputSystem> InputSystem::GetInstance(){ std::make_shared<InputSystem>(InputSystem{}) };
	
	int test2 = 10;
	int* test1 = &test2;

	void KeyCallback([[maybe_unused]] GLFWwindow* window, [[maybe_unused]] int key, [[maybe_unused]] int scancode, int action, [[maybe_unused]] int mods) {
	}
	
	
	void MouseButtonCallback([[maybe_unused]] GLFWwindow* pwin, [[maybe_unused]] int button, [[maybe_unused]] int action, [[maybe_unused]] int mod) {
	}
	
	void CursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
		int width{}, height{};
			
		glfwGetWindowSize(window, &width, &height);
		ypos = static_cast<double>(height - ypos);
	
		InputSystem::GetInstance()->mousePos.x = static_cast<float>(xpos);
		InputSystem::GetInstance()->mousePos.y = static_cast<float>(ypos);
	}		
	
	void DropCallback([[maybe_unused]] GLFWwindow* window, int count, const char** paths) {
		InputSystem::GetInstance()->droppedFiles.clear();

		for (int i = 0; i < count; ++i) {
			InputSystem::GetInstance()->droppedFiles.emplace_back(paths[i]);
		}
	}
	
	void InputSystem::SetCallBack(GLFWwindow* window) {
		glfwSetKeyCallback(window, KeyCallback);
		glfwSetDropCallback(window, DropCallback);
		glfwSetMouseButtonCallback(window, MouseButtonCallback);
		glfwSetCursorPosCallback(window, CursorPosCallback);
	}
	
	void InputSystem::HideCursor(bool check) {
		if (check) {
			glfwSetInputMode(inputWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
		else {
			glfwSetInputMode(inputWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
	
	}
	
	void InputSystem::InputInit(GLFWwindow* window) {
		InputSystem::GetInstance()->inputWindow = window;
	}

	void InputSystem::InputUpdate() {
		

		for (std::pair<const int, Key>& key : keysRegistered) {
			int state;

			if (key.first == keys::LMB || key.first == keys::RMB || key.first == keys::MMB) {
				state = glfwGetMouseButton(InputSystem::GetInstance()->inputWindow, key.first);
			}
			else {
				state = glfwGetKey(InputSystem::GetInstance()->inputWindow, key.first);
			}

			// Update all prev and curr key states first
			key.second.prevKeyState = key.second.currKeyState;
			key.second.currKeyState = KeyState::UNUSED;

			// Current checks
			if (state == GLFW_PRESS) {
				if (key.second.prevKeyState == KeyState::UNUSED) {
					if (!key.second.currPressedTimer) {
						key.second.currKeyState = KeyState::TRIGGERED;
					}
					else {
						key.second.currKeyState = KeyState::WAITING;
					}

					key.second.currPressedTimer += ecs->m_GetDeltaTime();
				}

				if(key.second.currPressedTimer >= secondsBeforePressed) {
					key.second.currKeyState = KeyState::PRESSED;
				}
			}
			else if (state == GLFW_RELEASE) {
				if (key.second.currPressedTimer) {
					key.second.currKeyState = KeyState::RELEASED;
					key.second.currPressedTimer = 0;
				}
			}
		}
	}

	bool InputSystem::IsKeyTriggered(const keyCode key) {
		if (keysRegistered[key].currKeyState == KeyState::TRIGGERED) {
			return true;
		}

		return false;
	}

	bool InputSystem::IsKeyPressed(const keyCode key) {
		if (keysRegistered[key].currKeyState == KeyState::PRESSED) {
			return true;
		}

		return false;
	}

	bool InputSystem::IsKeyReleased(const keyCode key) {
		if (keysRegistered[key].currKeyState == KeyState::RELEASED) {
			return true;
		}

		return false;
	}
	
	glm::vec2 InputSystem::GetMousePos() {
		return InputSystem::mousePos;
	}	
}