/******************************************************************/
/*!
\file      Input.h
\author    Elijah Teo, teo.e , 2301530
\par       teo.e@digipen.edu
\date      16 Sept, 2024
\brief     Declares a class that contains all the callback functions that will be given to GLFW along with variables to hold the return values

The header provides declarations for the Performance class functions that
handle performance tracking and output for various engine systems such as
movement, rendering, and collision.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************/
#ifndef INPUT_H
#define INPUT_H

#include <GLFW/glfw3.h>
#include "Keycodes.h"

namespace Input {
	enum KeyState {
		UNUSED = 0,
		TRIGGERED,
		WAITING,
		PRESSED,
		RELEASED
	};

	struct Key {
		KeyState prevKeyState;
		KeyState currKeyState;

		float currPressedTimer;
	};

	class InputSystem {

	public:
		void OnCursorPos(double xpos, double ypos);
		void OnDrop(int count, const char** paths);

		glm::vec2 currentMousePos;
		glm::vec2 prevMousePos;
		std::vector<std::string> droppedFiles;
		GLFWwindow* inputWindow;

		void HideCursor(bool check);
		bool IsKeyTriggered(const keyCode key);
		bool IsKeyPressed(const keyCode key);
		bool IsKeyReleased(const keyCode key);
		glm::vec2 GetMousePos();
		float GetAxisRaw(std::string axisType);

		void InputInit(GLFWwindow* window);
		void InputUpdate(float deltaTime);
		void InputExitFrame(float deltaTime);
	private:
		std::unordered_map<int, Key> keysRegistered;
	};

}




#endif INPUT_H
