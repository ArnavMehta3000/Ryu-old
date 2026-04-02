#pragma once
#include "Core/Common/API.h"
#include "Core/Common/StandardTypes.h"
#include <filesystem>

namespace Ryu::Config
{
	struct RYU_API GameConfig
	{
		struct
		{
			bool EnableVsync         = false;
			bool AllowTearing        = true;
			bool EnableDebugLayer    = false;
			bool EnableGPUValidation = false;
			bool UseWARP             = false;
		} Graphics;

		// Loads from configDir/GraphicsSettings.toml
		static GameConfig Load(const std::filesystem::path& configDir);
	};
}
