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

    float AppWindow::windowHeight;

    float AppWindow::windowWidth;

    bool AppWindow::fullScreen{ true };

    const GLFWvidmode* AppWindow::mode;

    GLFWmonitor* AppWindow::monitor;

    void SetWindowIcon(GLFWwindow* window) {
        GLFWimage icon;

        // Load image (ensure your path is correct)
        icon.pixels = stbi_load("../Configs/icon.png", &icon.width, &icon.height, 0, 4);
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

        ecs::ECS* ecs = ecs::ECS::GetInstance();
        if (!focused) {

           

            if (ecs->GetState() == ecs::RUNNING) {
                //std::cout << "Window minimized!" << std::endl;
                ecs::ECS::GetInstance()->SetState(ecs::WAIT);
                //Helper::Helpers::GetInstance()->windowMinimise = true;
            }
        }
        else {

          

            if (ecs->GetState() == ecs::WAIT) {
                //std::cout << "Window restored!" << std::endl;
                ecs::ECS::GetInstance()->SetState(ecs::RUNNING);
                //Helper::Helpers::GetInstance()->windowMinimise = false;
            }
        }

    }


    static void fullScreenFocusCallback(GLFWwindow* window, int focused)
    {
        static int oldWidth = static_cast<int>(AppWindow::windowWidth);
        static int oldHeight = static_cast<int>(AppWindow::windowHeight);
        //auto& audioManager = assetmanager::AssetManager::GetInstance()->m_audioManager;
        ecs::ECS* ecs = ecs::ECS::GetInstance();
        if (!focused) {
            oldWidth = static_cast<int>(AppWindow::windowWidth);
            oldHeight = static_cast<int>(AppWindow::windowHeight);

            // If the window loses focus, set it to windowed mode
            glfwSetWindowMonitor(window, nullptr, 100, 100, static_cast<int>(AppWindow::windowWidth), static_cast<int>(AppWindow::windowHeight), 0);  // Change to windowed mode with a standard resolution
            AppWindow::fullScreen = false;

            //audioManager.m_PauseAllSounds();  // Pause all sounds

            if (ecs->GetState() == ecs::RUNNING) {
                //std::cout << "Window minimized!" << std::endl;
                ecs::ECS::GetInstance()->SetState(ecs::WAIT);
                //Helper::Helpers::GetInstance()->windowMinimise = true;
            }
        }
        else if (!AppWindow::fullScreen) {
            // If the window regains focus, switch back to full screen
            glfwSetWindowMonitor(window, AppWindow::monitor, 0, 0, AppWindow::mode->width, AppWindow::mode->height, AppWindow::mode->refreshRate);
            AppWindow::fullScreen = true;

            //audioManager.m_UnpauseAllSounds();  // Unpause all sounds

            if (ecs->GetState() == ecs::WAIT) {
                //std::cout << "Window restored!" << std::endl;
                ecs::ECS::GetInstance()->SetState(ecs::RUNNING);
                //Helper::Helpers::GetInstance()->windowMinimise = false;
            }
        }

    }

    static void iconifyCallback([[maybe_unused]]GLFWwindow* window, int iconified)
    {
       // auto& audioManager = assetmanager::AssetManager::GetInstance()->m_audioManager;
        ecs::ECS* ecs = ecs::ECS::GetInstance();
        if (iconified == GLFW_TRUE)
        {
            
            //audioManager.m_PauseAllSounds();  // Pause all sounds

            if (ecs->GetState() == ecs::RUNNING) {
                //std::cout << "Window minimized!" << std::endl;
                ecs::ECS::GetInstance()->SetState(ecs::WAIT);
                //Helper::Helpers::GetInstance()->windowMinimise = true;
            }
        }
        else
        {
            
           // audioManager.m_UnpauseAllSounds();  // Unpause all sounds

            if (ecs->GetState() == ecs::WAIT) {
                //std::cout << "Window restored!" << std::endl;
                ecs::ECS::GetInstance()->SetState(ecs::RUNNING);
                //Helper::Helpers::GetInstance()->windowMinimise = false;
            }
        }
    }

	int AppWindow::init(int windowWidth, int windowHeight){
        /* Initialize the library */
        if (!glfwInit())
            return -1;
        
        //Set Context Version
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);





        /* Create a windowed mode window and its OpenGL context */

        monitor = glfwGetPrimaryMonitor();
        mode = glfwGetVideoMode(monitor);
        window = glfwCreateWindow(windowWidth, windowHeight, "Kos 2.0", enabledFullScreen ? monitor : NULL, NULL);

        Input::InputSystem::GetInstance()->InputInit(window);
        if (!window)
        {
            glfwTerminate();
            return -1;
        }
        //set call back
        if(enabledFullScreen) glfwSetWindowFocusCallback(window, fullScreenFocusCallback);
        glfwSetWindowIconifyCallback(window, iconifyCallback);
        glfwMaximizeWindow(window); // Maximize the window

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
        this->windowWidth = static_cast<float>(windowWidth);
        this->windowHeight = static_cast<float>(windowHeight);

        
        return 0;
	}




	int AppWindow::Draw() {



        if ((Input::InputSystem::GetInstance()->IsKeyPressed(keys::LeftAlt) || Input::InputSystem::GetInstance()->IsKeyPressed(keys::RightAlt)) && Input::InputSystem::GetInstance()->IsKeyTriggered(keys::ENTER)) {
            if (enabledFullScreen) {
                glfwSetWindowFocusCallback(window, windowedFocusCallback);
                glfwSetWindowMonitor(window, nullptr, 100, 100, static_cast<int>(AppWindow::windowWidth), static_cast<int>(AppWindow::windowHeight), 0);
                enabledFullScreen = false;
            }
            else {
                glfwSetWindowFocusCallback(window, fullScreenFocusCallback);
                glfwSetWindowMonitor(window, AppWindow::monitor, 0, 0, AppWindow::mode->width, AppWindow::mode->height, AppWindow::mode->refreshRate);

                enabledFullScreen = true;
            }
        }

        return 0;
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
            glfwSetCursor(AppWindow::window, defaultCursor);
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
            glfwSetCursor(AppWindow::window, defaultCursor);
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
            glfwSetCursor(AppWindow::window, defaultCursor);
            return;
        }

        glfwSetCursor(AppWindow::window, customCursor);
        stbi_image_free(image.pixels);

        stbi_set_flip_vertically_on_load(true);
    }

  

}