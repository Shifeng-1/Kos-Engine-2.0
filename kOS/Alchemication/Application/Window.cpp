/******************************************************************/
/*!
\file      Window.cpp
\author    Sean Tiu
\par       s.tiu@digipen.edu
\date      2nd Oct, 2024
\brief

           This file contains the implementation of the `AppWindow` class
           methods, which handle the window lifecycle including initialization,
           drawing/rendering loop, and cleanup.

           The `init` method sets up GLFW and OpenGL contexts for rendering,
           while the `Draw` method manages the rendering loop including
           ImGui interface rendering. The `CleanUp` method is responsible
           for releasing resources used during the window’s operation.

           The application window makes use of GLFW for window and context
           management, and OpenGL for rendering. ImGui is integrated to
           provide a graphical interface within the application.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************/

#include "Window.h"
#include "ECS/ECS.h"
#include "Inputs/Input.h"
#include "Resources/ResourceManager.h"



namespace Application {


    void SetWindowIcon(GLFWwindow* window) {
        GLFWimage icon;

        // Load image (ensure your path is correct)
        icon.pixels = stbi_load("Alchemication/Configs/icon.png", &icon.width, &icon.height, 0, 4);
        if (!icon.pixels) {
            printf("Failed to load icon!\n");
            return;
        }

        // Set icon
        glfwSetWindowIcon(window, 1, &icon);

        // Free image data after setting the icon
        stbi_image_free(icon.pixels);
    }

    static void windowedFocusCallback([[maybe_unused]] GLFWwindow* window, int focused)
    {

        auto* app = static_cast<Application::AppWindow*>(glfwGetWindowUserPointer(window));
        if (!app) return;

        auto& ecs = app->m_ecs;
        try {
            if (!focused)
            {
                if (ecs.GetState() == ecs::RUNNING)
                    ecs.SetState(ecs::WAIT);
            }
            else
            {
                if (ecs.GetState() == ecs::WAIT)
                    ecs.SetState(ecs::RUNNING);
            }
        }
        catch (const std::exception& e) {
            LOGGING_ERROR("Exception in windowedFocusCallback: " + std::string(e.what()));
            return;
        }
    }


    static void fullScreenFocusCallback(GLFWwindow* window, int focused)
    {

        //auto& audioManager = assetmanager::AssetManager::GetInstance()->m_audioManager;
        auto* app = static_cast<Application::AppWindow*>(glfwGetWindowUserPointer(window));
        if (!app) return;

        static int oldWidth = static_cast<int>(app->windowWidth);
        static int oldHeight = static_cast<int>(app->windowHeight);

        auto& m_ecs = app->m_ecs;
        try {
            if (!focused) {
                oldWidth = static_cast<int>(app->windowWidth);
                oldHeight = static_cast<int>(app->windowHeight);

                // If the window loses focus, set it to windowed mode
                glfwSetWindowMonitor(window, nullptr, 100, 100, static_cast<int>(app->windowWidth), static_cast<int>(app->windowHeight), 0);  // Change to windowed mode with a standard resolution
                app->fullScreen = false;

                //audioManager.m_PauseAllSounds();  // Pause all sounds

                if (m_ecs.GetState() == ecs::RUNNING) {
                    //std::cout << "Window minimized!" << std::endl;
                    m_ecs.SetState(ecs::WAIT);
                    //Helper::Helpers::GetInstance()->windowMinimise = true;
                }
            }
            else if (!app->fullScreen) {
                // If the window regains focus, switch back to full screen
                glfwSetWindowMonitor(window, app->monitor, 0, 0, app->mode->width, app->mode->height, app->mode->refreshRate);
                app->fullScreen = true;

                //audioManager.m_UnpauseAllSounds();  // Unpause all sounds

                if (m_ecs.GetState() == ecs::WAIT) {
                    //std::cout << "Window restored!" << std::endl;
                    m_ecs.SetState(ecs::RUNNING);
                    //Helper::Helpers::GetInstance()->windowMinimise = false;
                }
            }
        }
        catch (const std::exception& e) {
            LOGGING_ERROR("Exception in fullScreenFocusCallback: " + std::string(e.what()));
            return;
        }

    }

    static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
    {
        auto* app = static_cast<Application::AppWindow*>(glfwGetWindowUserPointer(window));
        if (!app) return;
        app->windowHeight = height;
        app->windowWidth = width;
        glViewport(0, 0, width, height);
    }

    static void iconifyCallback(GLFWwindow* window, int iconified)
    {
        auto* app = static_cast<Application::AppWindow*>(glfwGetWindowUserPointer(window));
        if (!app) return;

        auto& m_ecs = app->m_ecs;

        if (iconified == GLFW_TRUE)
        {
            
            //audioManager.m_PauseAllSounds();  // Pause all sounds

            if (m_ecs.GetState() == ecs::RUNNING) {
                //std::cout << "Window minimized!" << std::endl;
                m_ecs.SetState(ecs::WAIT);
                //Helper::Helpers::GetInstance()->windowMinimise = true;
            }
        }
        else
        {
            
           // audioManager.m_UnpauseAllSounds();  // Unpause all sounds

            if (m_ecs.GetState() == ecs::WAIT) {
                //std::cout << "Window restored!" << std::endl;
                m_ecs.SetState(ecs::RUNNING);
                //Helper::Helpers::GetInstance()->windowMinimise = false;
            }
        }
    }

    static void CursorPosCallback(GLFWwindow* window,double xpos,double ypos) {
        auto* app = static_cast<Application::AppWindow*>(glfwGetWindowUserPointer(window));
        if (!app) return;
        app->m_inputSystem.OnCursorPos(xpos, ypos);
    
    }

    static void DropCallback(GLFWwindow* window, int count, const char** paths) {
        auto* app = static_cast<Application::AppWindow*>(glfwGetWindowUserPointer(window));
        if (!app) return;
        app->m_inputSystem.OnDrop(count, paths);
    }

	int AppWindow::init(int _windowWidth, int _windowHeight){
        
        /* Initialize the library */
        if (!glfwInit())
            return -1;

        //Set Context Version
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);



        /* Create a windowed mode window and its OpenGL context */

        monitor = glfwGetPrimaryMonitor();
        mode = glfwGetVideoMode(monitor);
        window = glfwCreateWindow(_windowWidth, _windowHeight, "Alchemication", enabledFullScreen ? monitor : NULL, NULL);
        glfwSetWindowUserPointer(window, this);
        m_inputSystem.InputInit(window);

        
        if (!window)
        {
            glfwTerminate();
            return -1;
        }
        //set call back
        CheckFullscreen();
        glfwSetWindowIconifyCallback(window, iconifyCallback);
        glfwMaximizeWindow(window); // Maximize the window

		//set input callback
        glfwSetCursorPosCallback(window, CursorPosCallback);
        glfwSetDropCallback(window,DropCallback);

        /* Make the window's context current */
        glfwMakeContextCurrent(window);

        /* Only initialize GLAD after defining OpenGL context*/
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "Failed to initialize GLAD" << std::endl;
            return -1;
        }

        //create icon
        SetWindowIcon(window);
        windowWidth = static_cast<float>(_windowWidth);
        windowHeight = static_cast<float>(_windowHeight);
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


        return 0;
	}




	int AppWindow::Update() {



        if ((m_inputSystem.IsKeyPressed(keys::LeftAlt) || m_inputSystem.IsKeyPressed(keys::RightAlt)) && m_inputSystem.IsKeyTriggered(keys::ENTER)) {
            CheckFullscreen();
        }

        if (m_ecs.GetState()==ecs::RUNNING) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
        else {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);//disabled for game mode
        }

        return 0;
	}

    void AppWindow::CheckFullscreen(){
         if (enabledFullScreen) {
             glfwSetWindowFocusCallback(window, fullScreenFocusCallback);
             glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
             enabledFullScreen = false;
        }
        else {
            glfwSetWindowFocusCallback(window, windowedFocusCallback);
            glfwSetWindowMonitor(window, nullptr, 100, 100, static_cast<int>(windowWidth), static_cast<int>(windowHeight), 0);
            enabledFullScreen = true;
        }
    }

	int AppWindow::CleanUp() {

        glfwDestroyWindow(window);
        return 0;
	}

    void AppWindow::setCursorImage(const std::string& imageFile, bool centered)
    {
        static const char* cursorOptions[] =
        {
            "default",
            "../Assets/Texture/UI/img_startScreenCursor.png",
            "../Assets/Texture/UI/img_inGameCursor.png"
        };


        //check if image file belong to any of the above, ELSE ERROR OCCUR
        std::string test{};
        for (auto cursor : cursorOptions) {
            if (cursor == imageFile) {
                test = imageFile;
                break;
            }

        }


        if (imageFile.empty() || imageFile == "default" || test.empty()) {
            // Set default cursor
            GLFWcursor* defaultCursor = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
            if (!defaultCursor) {
                LOGGING_ERROR("Failed to create default GLFW cursor.");
                return;
            }
            glfwSetCursor(window, defaultCursor);
            return;
        }

        stbi_set_flip_vertically_on_load(false);

        // Load custom cursor image
        GLFWimage image;
        image.pixels = stbi_load(imageFile.c_str(), &image.width, &image.height, 0, 4);

        if (!image.pixels) {
            LOGGING_ERROR("Cursor Image Loading Failed: Recheck file path (" + imageFile + ")");

            // Fall back to default cursor
            GLFWcursor* defaultCursor = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
            if (!defaultCursor) {
                LOGGING_ERROR("Failed to create default GLFW cursor.");
                return;
            }
            glfwSetCursor(window, defaultCursor);
            return;
        }

        // Create custom cursor
        GLFWcursor* customCursor = glfwCreateCursor(&image, centered ? image.width / 2 : 0, centered ? image.height / 2 : 0);

        if (!customCursor) {
            LOGGING_ERROR("Cursor Creation Failed: Using default cursor instead.");
            stbi_image_free(image.pixels);

            // Fall back to default cursor
            GLFWcursor* defaultCursor = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
            if (!defaultCursor) {
                LOGGING_ERROR("Failed to create default GLFW cursor.");
                return;
            }
            glfwSetCursor(window, defaultCursor);
            return;
        }

        glfwSetCursor(window, customCursor);
        stbi_image_free(image.pixels);

        stbi_set_flip_vertically_on_load(true);
    }

  

}