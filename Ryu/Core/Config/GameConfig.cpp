#include "GameConfig.h"
#include "Core/Logging/Logger.h"
#include <toml++/toml.hpp>
#include <fstream>

namespace Ryu::Config
{
	GameConfig GameConfig::Load(const std::filesystem::path& filePath)
	{
		GameConfig config;  // Start with defaults

		try
		{
			if (!std::filesystem::exists(filePath))
			{
				RYU_LOG_WARN("GameConfig file not found at '{}', using defaults", filePath.string());
				return config;
			}

			auto tbl = toml::parse_file(filePath.string());

			// Load Graphics settings
			if (auto graphics = tbl["Graphics"].as_table())
			{
				config.Graphics.EnableVsync         = (*graphics)["EnableVsync"].value_or(true);
				config.Graphics.EnableDebugLayer    = (*graphics)["EnableDebugLayer"].value_or(false);
				config.Graphics.AllowTearing        = (*graphics)["AllowTearing"].value_or(true);
				config.Graphics.EnableGPUValidation = (*graphics)["EnableGPUValidation"].value_or(false);
				config.Graphics.UseWARP             = (*graphics)["UseWARP"].value_or(false);
			}

			// Load Logging settings
			if (auto logging = tbl["Logging"].as_table())
			{
				config.Logging.LogLevel         = (*logging)["LogLevel"].value_or(1);
				config.Logging.LogToFile        = (*logging)["LogToFile"].value_or(false);
				config.Logging.LogToConsole     = (*logging)["LogToConsole"].value_or(true);
				config.Logging.ForceLogToOutput = (*logging)["ForceLogToOutput"].value_or(false);
				config.Logging.LogFilePath      = (*logging)["LogFilePath"].value_or(std::string{"Game.log"});
			}

			RYU_LOG_INFO("Loaded GameConfig from '{}'", filePath.string());
		}
		catch (const toml::parse_error& err)
		{
			RYU_LOG_ERROR("Failed to parse GameConfig: {}", err.description());
		}

		return config;
	}

	void GameConfig::Save(const std::filesystem::path& filePath) const
	{
		try
		{
			toml::table tbl;

			// Save Graphics settings
			tbl.insert("Graphics", toml::table{
				{"EnableVsync", Graphics.EnableVsync},
				{"EnableDebugLayer", Graphics.EnableDebugLayer},
				{"AllowTearing", Graphics.AllowTearing},
				{"EnableGPUValidation", Graphics.EnableGPUValidation},
				{"UseWARP", Graphics.UseWARP}
			});

			// Save Logging settings
			tbl.insert("Logging", toml::table{
				{"LogLevel", Logging.LogLevel},
				{"LogToFile", Logging.LogToFile},
				{"LogToConsole", Logging.LogToConsole},
				{"ForceLogToOutput", Logging.ForceLogToOutput},
				{"LogFilePath", Logging.LogFilePath}
			});

			// Write to file
			std::ofstream file(filePath);
			if (!file)
			{
				RYU_LOG_ERROR("Failed to open file for writing: '{}'", filePath.string());
				return;
			}

			file << tbl;
			RYU_LOG_INFO("Saved GameConfig to '{}'", filePath.string());
		}
		catch (const std::exception& ex)
		{
			RYU_LOG_ERROR("Failed to save GameConfig: {}", ex.what());
		}
	}
}
