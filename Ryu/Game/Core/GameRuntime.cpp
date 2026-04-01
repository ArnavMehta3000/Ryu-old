#include "Game/Core/GameRuntime.h"

#include "Application/App/Utils/PathManager.h"
#include "Core/Config/CmdLine.h"
#include "Engine/Engine.h"
#include "Graphics/Compiler/ShaderCompiler.h"

namespace Ryu::Game
{
	App::IApplication* Runtime::GetApplication()
	{
		return Engine::Engine::Get().GetApplication();
	}

	Logging::Logger* Runtime::GetLogger()
	{
		return &Logging::Logger::Get();
	}

	Config::CmdLine* Runtime::GetCmdLine()
	{
		return &Config::CmdLine::Get();
	}

	App::PathManager* Runtime::GetPathManager()
	{
		return &App::PathManager::Get();
	}

	Gfx::Renderer* Runtime::GetRenderer()
	{
		return Engine::Engine::Get().GetRenderer();
	}

	Gfx::ShaderCompiler* Runtime::GetShaderCompiler()
	{
		return &Gfx::ShaderCompiler::Get();
	}

	Game::InputManager* Runtime::GetInputManager()
	{
		return Engine::Engine::Get().GetInputManager();
	}
}
