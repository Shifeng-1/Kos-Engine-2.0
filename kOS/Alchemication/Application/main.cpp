/******************************************************************/
/*!
\file      main.cpp
\author    Ng Jaz winn, jazwinn.ng , 2301502
\par       jazwinn.ng@digipen.edu
\date      Oct 02, 2024
\brief     This file contains the main function of the engine



Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/********************************************************************/

#include "Application.h"


    int main()
    {
        #if defined(DEBUG) | defined(_DEBUG)
                _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
        #endif
        std::filesystem::path exePath = std::filesystem::current_path();
        std::filesystem::path root = exePath.parent_path().parent_path(); // up two levels
        std::filesystem::current_path(root);

        Application::Application app{};

        app.exePath = exePath;
       
        app.Init();
       

        app.Run();

        app.m_Cleanup();




        return 0;
    } 