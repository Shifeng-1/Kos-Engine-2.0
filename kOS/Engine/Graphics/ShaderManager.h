/********************************************************************/
/*!
\file      ShaderManager.h
\author    Sean Tiu (2303398)
\par       Emails: s.tiu@digipen.edu
\date      Oct 03, 2025
\brief     Declares the ShaderManager class, which manages and
		   initializes all core engine shaders used throughout
		   the rendering system.

		   The ShaderManager is responsible for:
		   - Initializing and storing precompiled engine shaders.
		   - Managing a central collection of Shader objects
			 accessible via string keys.
		   - Providing shader programs for different rendering
			 stages, including:
			   * Default rendering
			   * Deferred PBR lighting
			   * G-buffer generation
			   * Depth mapping
			   * Skybox rendering
			   * Screen-space text and sprite rendering
			   * Framebuffer and composition passes

		   Shader source code is embedded directly into the binary
		   using preprocessor includes for portability and
		   efficient loading during engine startup.

Copyright (C) 2025 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/********************************************************************/
#pragma once
#include "Shader.h"

class ShaderManager
{
public:
	void Initialize()
	{
		engineShaders.insert({ "DefaultDraw",Shader(defaultDrawVS, defaultDrawFS) });
		engineShaders.insert({ "DeferredPBRShader",Shader(deferredPBRVS, deferredPBRFS) });
		engineShaders.insert({ "GBufferPBRShader",Shader(gBufferPBRVS, gBufferPBRFS) });
		engineShaders.insert({ "DepthMapShader",Shader(depthMapVS, depthMapFS) });
		engineShaders.insert({ "SkyBoxShader",Shader(skyBoxVS, skyBoxFS) });
		engineShaders.insert({ "ScreenFontShader",Shader(screenFontVS, screenFontFS) });
		engineShaders.insert({ "ScreenSpriteShader",Shader(screenSpriteVS, screenSpriteFS) });
		engineShaders.insert({ "FrameBufferShader",Shader(frameBufferVS, frameBufferFS) });
		engineShaders.insert({ "FBOCompositeShader",Shader(fboCompositeVS, fboCompositeFS) });
		engineShaders.insert({ "MaterialShader",Shader(materialVS, materialFS) });

	}

	std::unordered_map<std::string, Shader> engineShaders;
private:
	

	/*--------------------------------------------------------------------------*/
	/*---------------------------CORE ENGINE SHADERS----------------------------*/
	/*--------------------------------------------------------------------------*/
	const char* defaultDrawVS
	{
		#include "CoreEngineShaders/Shaders/DefaultDraw/DefaultDraw.vs"
	};
	const char* defaultDrawFS
	{
		#include "CoreEngineShaders/Shaders/DefaultDraw/DefaultDraw.fs"
	};
	const char* materialVS
	{
		#include "CoreEngineShaders/Shaders/MaterialShader/MaterialShader.vs"
	};
	const char* materialFS
	{
		#include "CoreEngineShaders/Shaders/MaterialShader/MaterialShader.fs"
	};
	const char* frameBufferVS
	{
		#include "CoreEngineShaders/Shaders/FrameBuffShader/FrameBuffShader.vs"
	};
	const char* frameBufferFS
	{
		#include "CoreEngineShaders/Shaders/FrameBuffShader/FrameBuffShader.fs"
	};
	const char* fboCompositeVS
	{
		#include "CoreEngineShaders/Shaders/FBOCompositeShader/FBOCompositeShader.vs"
	};
	const char* fboCompositeFS
	{
		#include "CoreEngineShaders/Shaders/FBOCompositeShader/FBOCompositeShader.fs"
	};
	const char* deferredPBRVS
	{
		#include "CoreEngineShaders/Shaders/DeferredPBR/DeferredPBR.vs"
	};
	const char* deferredPBRFS
	{
		#include "CoreEngineShaders/Shaders/DeferredPBR/DeferredPBR.fs"
	};
	const char* gBufferPBRVS
	{
		#include "CoreEngineShaders/Shaders/GBuffPBRShader/GBuffPBRShader.vs"
	};
	const char* gBufferPBRFS
	{
		#include "CoreEngineShaders/Shaders/GBuffPBRShader/GBuffPBRShader.fs"
	};
	const char* depthMapVS
	{
		#include "CoreEngineShaders/Shaders/DepthMap/DepthMap.vs"
	};
	const char* depthMapFS
	{
		#include "CoreEngineShaders/Shaders/DepthMap/DepthMap.fs"
	};
	const char* skyBoxVS
	{
		#include "CoreEngineShaders/Shaders/SkyBoxShader/SkyBoxShader.vs"
	};
	const char* skyBoxFS
	{
		#include "CoreEngineShaders/Shaders/SkyBoxShader/SkyBoxShader.fs"
	};
	const char* screenFontVS
	{
		#include "CoreEngineShaders/Shaders/ScreenFontShader/ScreenFontShader.vs"
	};
	const char* screenFontFS
	{
		#include "CoreEngineShaders/Shaders/ScreenFontShader/ScreenFontShader.fs"
	};
	const char* screenSpriteVS
	{
		#include "CoreEngineShaders/Shaders/ScreenSpriteShader/ScreenSpriteShader.vs"
	};
	const char* screenSpriteFS
	{
		#include "CoreEngineShaders/Shaders/ScreenSpriteShader/ScreenSpriteShader.fs"
	};

};