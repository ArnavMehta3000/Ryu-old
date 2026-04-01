#include "EditorConfig.h"
#include "Core/Logging/Logger.h"
#include <toml++/toml.hpp>
#include <fstream>

namespace Ryu::Config
{
	EditorConfig EditorConfig::Load(const std::filesystem::path& filePath)
	{
		EditorConfig config;  // Start with defaults

		try
		{
			if (!std::filesystem::exists(filePath))
			{
				RYU_LOG_WARN("EditorConfig file not found at '{}', using defaults", filePath.string());
				return config;
			}

			auto tbl = toml::parse_file(filePath.string());

			// Load UI settings
			if (auto ui = tbl["UI"].as_table())
			{
				config.UI.ShowGrid   = (*ui)["ShowGrid"].value_or(true);
				config.UI.GridSize   = static_cast<f32>((*ui)["GridSize"].value_or(1.0));
				config.UI.ShowGizmos = (*ui)["ShowGizmos"].value_or(true);
			}

			// Load Panel states
			if (auto panels = tbl["Panels"].as_table())
			{
				config.Panels.ShowOutliner      = (*panels)["ShowOutliner"].value_or(true);
				config.Panels.ShowAssets        = (*panels)["ShowAssets"].value_or(true);
				config.Panels.ShowEntityDetails = (*panels)["ShowEntityDetails"].value_or(true);
				config.Panels.ShowConsole       = (*panels)["ShowConsole"].value_or(true);
			}

			// Load Layout
			if (auto layout = tbl["Layout"].as_table())
			{
				config.Layout.LastDockLayout = (*layout)["LastDockLayout"].value_or(std::string{});
			}

			RYU_LOG_INFO("Loaded EditorConfig from '{}'", filePath.string());
		}
		catch (const toml::parse_error& err)
		{
			RYU_LOG_ERROR("Failed to parse EditorConfig: {}", err.description());
		}

		return config;
	}

	void EditorConfig::Save(const std::filesystem::path& filePath) const
	{
		try
		{
			toml::table tbl;

			// Save UI settings
			tbl.insert("UI", toml::table{
				{"ShowGrid", UI.ShowGrid},
				{"GridSize", UI.GridSize},
				{"ShowGizmos", UI.ShowGizmos}
			});

			// Save Panel states
			tbl.insert("Panels", toml::table{
				{"ShowOutliner", Panels.ShowOutliner},
				{"ShowAssets", Panels.ShowAssets},
				{"ShowEntityDetails", Panels.ShowEntityDetails},
				{"ShowConsole", Panels.ShowConsole}
			});

			// Save Layout
			tbl.insert("Layout", toml::table{
				{"LastDockLayout", Layout.LastDockLayout}
			});

			// Write to file
			std::ofstream file(filePath);
			if (!file)
			{
				RYU_LOG_ERROR("Failed to open file for writing: '{}'", filePath.string());
				return;
			}

			file << tbl;
			RYU_LOG_INFO("Saved EditorConfig to '{}'", filePath.string());
		}
		catch (const std::exception& ex)
		{
			RYU_LOG_ERROR("Failed to save EditorConfig: {}", ex.what());
		}
	}
}
