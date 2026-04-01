#pragma once
#include "Core/API.h"
#include "Core/Defines.h"
#include <filesystem>
#include <string>

namespace Ryu::Config
{
	struct RAPI EditorConfig
	{
		// UI Settings
		struct
		{
			bool ShowGrid   = true;
			f32  GridSize   = 1.0f;
			bool ShowGizmos = true;
		} UI;

		// Panel States
		struct
		{
			bool ShowOutliner      = true;
			bool ShowAssets        = true;
			bool ShowEntityDetails = true;
			bool ShowConsole       = true;
		} Panels;

		// Layout
		struct
		{
			std::string LastDockLayout;
		} Layout;

		static EditorConfig Load(const std::filesystem::path& filePath);
		void Save(const std::filesystem::path& filePath) const;
	};
}
