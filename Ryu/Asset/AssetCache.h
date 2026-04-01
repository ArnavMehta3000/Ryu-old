#pragma once
#include "Asset/AssetHandle.h"
#include "Asset/AssetLoader.h"
#include "Asset/IGpuResourceFactory.h"
#include <mutex>
#include <shared_mutex>

namespace Ryu::Asset
{
	namespace fs = std::filesystem;

	// Manages CPU and GPU resources together + support for lazy loading
	template<typename TAssetData, typename TGpuResource>
	class AssetCache
	{
	public:
		struct Entry
		{
			std::unique_ptr<TAssetData>   CpuData;
			std::unique_ptr<TGpuResource> GpuResource;
			fs::path                      SourcePath;
			std::string                   Name;
			AssetState                    State = AssetState::Loading;
			bool                          IsProcedural = false;
		};

	public:
		explicit AssetCache(IGpuResourceFactory* gpuFactory) : m_gpuFactory(gpuFactory) {}

		// Load asset from file path (starts loading immediately)
		[[nodiscard]] AssetHandle<TAssetData> Load(const fs::path& path);

		// Load procedural asset with explicit ID and data
		[[nodiscard]] AssetHandle<TAssetData> Load(AssetId id, std::unique_ptr<TAssetData> data, std::string_view name = "");

		// Load procedural asset with string ID
		[[nodiscard]] AssetHandle<TAssetData> Load(std::string_view name, std::unique_ptr<TAssetData> data);

		// Get CPU/GPU data (GPU creation is lazy on first access)
		[[nodiscard]] TGpuResource* GetGpu(AssetHandle<TAssetData> handle);
		[[nodiscard]] const TAssetData* GetCpu(AssetHandle<TAssetData> handle);

		[[nodiscard]] const Entry* const GetEntry(AssetHandle<TAssetData> handle) const { return FindEntry(handle.Id); }

		// Force load without accessing
		void EnsureCpuLoaded(AssetHandle<TAssetData> handle);
		void EnsureGpuReady(AssetHandle<TAssetData> handle);

		[[nodiscard]] AssetState GetState(AssetHandle<TAssetData> handle) const;
		void Invalidate(AssetHandle<TAssetData> handle);
		void InvalidateAll();

		// Func is void(AssetHandle<TAssetData> handle, const Entry& entry)
		template<typename Func>
		inline void ForEach(Func&& func) const
		{
			std::shared_lock lock(m_mutex);
			for (const auto& [id, entry] : m_entries)
			{
				func(AssetHandle<TAssetData>{ id }, entry);
			}
		}

	private:
		[[nodiscard]] AssetId GenerateId(const fs::path& path) const;
		[[nodiscard]] AssetId GenerateId(std::string_view name) const;
		[[nodiscard]] Entry* FindEntry(AssetId id);
		[[nodiscard]] const Entry* FindEntry(AssetId id) const;

		bool LoadCpuData(Entry& entry);
		bool CreateGpuResource(Entry& entry, std::string_view name);

	private:
		mutable std::shared_mutex              m_mutex;
		std::unordered_map<AssetId, Entry>     m_entries;
		std::unordered_map<fs::path, AssetId>  m_pathToId;
		IGpuResourceFactory*                   m_gpuFactory;
	};
}

#include "Asset/AssetCache.inl"
