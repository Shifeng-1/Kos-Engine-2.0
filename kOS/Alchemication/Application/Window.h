/******************************************************************/
/*!
\file      Window.h
\author    Sean Tiu
\par       s.tiu@digipen.edu
\date      2nd Oct, 2024
\brief

           This file contains the declaration of the `AppWindow` class,
           which is responsible for managing the main application window,
           including window creation, rendering, and cleanup.

           The `AppWindow` class utilizes GLFW for creating the window
           and OpenGL for rendering, along with ImGui for rendering the
           graphical user interface. The class provides methods for
           initializing the window, handling the drawing loop, and cleaning up
           resources when the window is closed.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************/


#ifndef APPWINDOW_H
#define APPWINDOW_H

#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Inputs/Input.h"
#include "ECS/ECS.h"

namespace Application {

    /**
     * @brief Represents the application window and its associated properties.
     *
     * The `AppWindow` class is responsible for managing the main application window,
     * including initialization, drawing, and cleanup.
     */
    class AppWindow {

    public:
        ecs::ECS& m_ecs;
        Input::InputSystem& m_inputSystem;

        AppWindow(ecs::ECS& ecs, Input::InputSystem& inputSystem) :
            m_ecs(ecs),
            m_inputSystem(inputSystem)
        {
        }

        int init(int windowWidth, int windowHeight);
        int Draw();
        int CleanUp();

		static float windowHeight;
		static float windowWidth;


        GLFWwindow* window;

        static GLFWmonitor* monitor;
        static const GLFWvidmode* mode;

        static bool fullScreen;
        bool enabledFullScreen{ false };//use this to set launch application fullscreen or not

		
		void setCursorImage(const std::string& image, bool centered);
        static GLFWcursor* currCursor;
    };
}

#endif // APPWINDOW_H