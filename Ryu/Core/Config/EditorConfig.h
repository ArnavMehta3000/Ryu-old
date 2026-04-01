#pragma once
#include "Core/Common/API.h"
#include "Core/Common/StandardTypes.h"
#include <filesystem>
#include <string>

namespace Ryu::Config
{
	struct RYU_API EditorConfig
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
