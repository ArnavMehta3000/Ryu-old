#pragma once
#include "Core/Common/API.h"
#include "Core/Common/StandardTypes.h"
#include <filesystem>
#include <string>

namespace Ryu::Config
{
	struct RYU_API GameConfig
	{
		// Graphics Settings
		struct
		{
			bool EnableVsync          = true;
			bool EnableDebugLayer     = false;
			bool AllowTearing         = true;
			bool EnableGPUValidation  = false;
			bool UseWARP              = false;
		} Graphics;

		// Logging Settings
		struct
		{
			i32         LogLevel           = 1;  // Info level
			bool        LogToFile          = false;
			bool        LogToConsole       = true;
			bool        ForceLogToOutput   = false;
			std::string LogFilePath        = "Game.log";
		} Logging;

		static GameConfig Load(const std::filesystem::path& filePath);
		void Save(const std::filesystem::path& filePath) const;
	};
}
