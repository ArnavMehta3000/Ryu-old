#include "GameConfig.h"
#include "Core/Logging/Logger.h"
#include <toml++/toml.hpp>

namespace Ryu::Config
{
	GameConfig GameConfig::Load(const fs::path& configDir)
	{
		GameConfig config;

		const auto graphicsFile = configDir / "GraphicsSettings.toml";

		try
		{
			if (!fs::exists(graphicsFile))
			{
				RYU_LOG_WARN("GraphicsSettings.toml not found at '{}', using defaults", graphicsFile.string());
				return config;
			}

			auto tbl = toml::parse_file(graphicsFile.string());

			if (auto debug = tbl["Debug"].as_table())
			{
				config.Graphics.EnableDebugLayer    = (*debug)["EnableDebugLayer"].value_or(false);
				config.Graphics.EnableGPUValidation = (*debug)["EnableGPUValidation"].value_or(false);
			}

			if (auto rendering = tbl["Rendering"].as_table())
			{
				config.Graphics.AllowTearing = (*rendering)["AllowTearing"].value_or(true);
				config.Graphics.EnableVsync  = (*rendering)["EnableVsync"].value_or(false);
				config.Graphics.UseWARP      = (*rendering)["UseWARP"].value_or(false);
			}

			RYU_LOG_INFO("Loaded graphics config from '{}'", graphicsFile.string());
		}
		catch (const toml::parse_error& err)
		{
			RYU_LOG_ERROR("Failed to parse GraphicsSettings.toml: {}", err.description());
		}

		return config;
	}
}
