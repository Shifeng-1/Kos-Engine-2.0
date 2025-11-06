/******************************************************************/
/*!
\file      Application.cpp
\author    Ng Jaz winn, jazwinn.ng , 2301502
\par       jazwinn.ng@digipen.edu
\date      Oct 02, 2024
\brief     This file contains the main update loop of the engine.
           It initializes the various major systems and call upon
           them in the update loop. When the window is closed,
           the applciations cleanup function is called to 
           call the major systems to clean their program.
           


Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/********************************************************************/





#include "Application.h"
#include "ApplicationData.h"
#include "Scene/SceneManager.h"
#include "Inputs/Input.h"
#include "Graphics/GraphicsManager.h"
#include "Editor/EditorCamera.h"
#include "AssetManager/AssetManager.h"
#include "Configs/ConfigPath.h"
#include "Debugging/Performance.h"
#include "Scripting/ScriptManager.h"
#include "Physics/PhysicsManager.h"

namespace Application {

    int Application::Init() {
        
		WindowSettings windowData = serialization::ReadJsonFile<WindowSettings>(configpath::configFilePath);

        /*--------------------------------------------------------------
        INITIALIZE LOGGING SYSTEM
        --------------------------------------------------------------*/
        LOGGING_INIT_LOGS(configpath::logFilePath);
        LOGGING_INFO("Application Start");
        LOGGING_INFO("Load Log Successful");

       /*--------------------------------------------------------------
          INITIALIZE OPENGL WINDOW
       --------------------------------------------------------------*/
        lvWindow.init(windowData.windowWidth, windowData.windowHeight);
        LOGGING_INFO("Load Window Successful");

        /*--------------------------------------------------------------
           INITIALIZE ECS
        --------------------------------------------------------------*/
        ecs.Load();
        ecs.Init();
        LOGGING_INFO("Load ECS Successful");


        /*--------------------------------------------------------------
           INITIALIZE Asset Manager - Editor
        --------------------------------------------------------------*/
		assetManager.Init(configpath::assetFilePath, configpath::resourceFilePath);
        LOGGING_INFO("Load Resource Successful");
        
        /*--------------------------------------------------------------
          INITIALIZE GRAPHICS PIPE
        --------------------------------------------------------------*/
        graphicsManager.gm_Initialize(static_cast<float>(windowData.gameResWidth), static_cast<float>(windowData.gameResHeight));
        LOGGING_INFO("Load Graphic Pipeline Successful");

        /*--------------------------------------------------------------
           INITIALIZE Resource Manager
        --------------------------------------------------------------*/
        resourceManager.Init(configpath::resourceFilePath);

        /*--------------------------------------------------------------
        INITIALIZE SCIRPT
        --------------------------------------------------------------*/
        scriptManager.Init(exePath.string());

        /*--------------------------------------------------------------
           INITIALIZE Start Scene
        --------------------------------------------------------------*/
        //for game only
        //resourceManager->GetResource<R_Scene>(windowData.startScene);
        LOGGING_INFO("Load Asset Successful");

        
        /*--------------------------------------------------------------
            INITIALIZE EDITOR // LAST INIT
         --------------------------------------------------------------*/

        const char* glsl_version = "#version 130";
        Editor.Initialize(glsl_version, configpath::editorTagPath, configpath::imguiINIPath);
        LOGGING_INFO("Load ImGui Successful");


        

        LOGGING_INFO("Application Init Successful");

        EditorCamera::editorCamera.position.z = -50.f;

        //Sean use this to test animationn serialization
        //ResourceManager::GetInstance()->GetResource<R_Animation>("bf8a061d-e1b2-8f34-ec30-a655db0af661");
        return 0;
	}



    int Application::Run() {


        //float FPSCapTime = 1.f / help->m_fpsCap;
        double lastFrameTime = glfwGetTime();
        const double fixedDeltaTime = 1.0 / 60.0;
        float accumulatedTime = 0.0;

        /*--------------------------------------------------------------
            GAME LOOP
        --------------------------------------------------------------*/
        while (!glfwWindowShouldClose(lvWindow.window))
        {
            try {
                /* Poll for and process events */
                glfwPollEvents();

                ///*--------------------------------------------------------------
                //    Calculate time
                // --------------------------------------------------------------*/
                double currentFrameTime = glfwGetTime();
                float deltaTime = static_cast<float>(currentFrameTime - lastFrameTime);
                lastFrameTime = currentFrameTime;
                accumulatedTime += (deltaTime);

                peformance.SetDeltaTime(deltaTime);

                int currentNumberOfSteps = 0;
                while( accumulatedTime >= fixedDeltaTime) {
                    accumulatedTime -= static_cast<float>(fixedDeltaTime);
                    ++currentNumberOfSteps;
                }
                
                /*--------------------------------------------------------------
                    UPDATE INPUT
                --------------------------------------------------------------*/
                input.InputUpdate(deltaTime);
                Editor.InputUpdate();

                /*--------------------------------------------------------------
                 Update Window
                --------------------------------------------------------------*/
                lvWindow.Update();

                /*--------------------------------------------------------------
                    UPDATE ECS
                --------------------------------------------------------------*/
                ecs.Update(static_cast<float>(fixedDeltaTime));

                /*--------------------------------------------------------------
                    Update IMGUI FRAME
                --------------------------------------------------------------*/
                Editor.Update();

                /*--------------------------------------------------------------
                    UPDATE INPUT FRAME EXIT
                --------------------------------------------------------------*/

                input.InputExitFrame(deltaTime);
                
                /*--------------------------------------------------------------
                    UPDATE Render Pipeline
                --------------------------------------------------------------*/
                graphicsManager.gm_Update();

                /*--------------------------------------------------------------
                    Execute Render Pipeline
                --------------------------------------------------------------*/
                graphicsManager.gm_Render();
                

                /*--------------------------------------------------------------
                    Draw IMGUI FRAME
                --------------------------------------------------------------*/
                Editor.Render();

                /*--------------------------------------------------------------
                   Reset Framebuffer
                --------------------------------------------------------------*/
                graphicsManager.gm_ResetFrameBuffer();



                /*--------------------------------------------------------------
                    SceneManager EndFrame
                --------------------------------------------------------------*/
                sceneManager.EndFrame();
                /*--------------------------------------------------------------
                    ecs Endframe
                --------------------------------------------------------------*/
                ecs.EndFrame();

                graphicsManager.gm_ClearGBuffer();

                glfwSwapBuffers(lvWindow.window);
            }
            catch (const std::exception& e) {
                LOGGING_ERROR("Exception in game loop: {}", e.what());
            }
        }
        return 0;
    }

	int Application::m_Cleanup() {

        ecs.Unload();
        physicsManager.Shutdown();
        Editor.Shutdown();
        lvWindow.CleanUp();
        glfwTerminate();

        LOGGING_INFO("Application Closed");

        return 0;
	}

}