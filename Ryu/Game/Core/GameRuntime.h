#pragma once

namespace Ryu::App { class IApplication; class PathManager; }
namespace Ryu::Logging { class Logger; }
namespace Ryu::Config { class CmdLine; }
namespace Ryu::Gfx { class Renderer; class ShaderCompiler; }
namespace Ryu::Game { class InputManager; }

namespace Ryu::Game
{
	class Runtime
	{
	public:
		[[nodiscard]] static App::IApplication* GetApplication();
		[[nodiscard]] static Logging::Logger* GetLogger();
		[[nodiscard]] static Config::CmdLine* GetCmdLine();
		[[nodiscard]] static App::PathManager* GetPathManager();
		[[nodiscard]] static Gfx::Renderer* GetRenderer();
		[[nodiscard]] static Gfx::ShaderCompiler* GetShaderCompiler();
		[[nodiscard]] static Game::InputManager* GetInputManager();
	};
}

#define RYU_RUNTIME() ::Ryu::Game::Runtime
