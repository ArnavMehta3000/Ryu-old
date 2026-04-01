#include "Core/Logging/Logger.h"
#include "Core/Profiling/Profiling.h"
#include "AssetCache.h"

namespace Ryu::Asset
{
	template<typename TAssetData, typename TGpuResource>
	inline AssetHandle<TAssetData> AssetCache<TAssetData, TGpuResource>::Load(const fs::path& path)
	{
		RYU_PROFILE_SCOPE();
		std::unique_lock lock(m_mutex);

		// Check if already loaded
		if (auto it = m_pathToId.find(path); it != m_pathToId.end())
		{
			return AssetHandle<TAssetData>{ it->second };
		}

		const AssetId id = GenerateId(path);

		Entry entry
		{
			.SourcePath   = path,
			.State        = AssetState::Loading,
			.IsProcedural = false,
		};

		// Start loading CPU data immediately
		entry.CpuData = LoadAsset<TAssetData>(path);
		if (entry.CpuData)
		{
			entry.State = AssetState::Loaded;
		}
		else
		{
			RYU_LOG_ERROR("Failed to load asset from {}", path.string());
			entry.State = AssetState::Failed;
		}

		m_entries[id] = std::move(entry);
		m_pathToId[path] = id;

		return AssetHandle<TAssetData>{ id };
	}

	template<typename TAssetData, typename TGpuResource>
	inline AssetHandle<TAssetData> AssetCache<TAssetData, TGpuResource>::Load(AssetId id, std::unique_ptr<TAssetData> data, std::string_view name)
	{
		RYU_PROFILE_SCOPE();
		std::unique_lock lock(m_mutex);

		// Check if already loaded
		if (m_entries.contains(id))
		{
			return AssetHandle<TAssetData>{ id };
		}

		Entry entry
		{
		   .CpuData      = std::move(data),
		   .Name         = name.data(),
		   .State        = AssetState::Loaded,  // CPU data already provided
		   .IsProcedural = true,
		};

		m_entries[id] = std::move(entry);

		return AssetHandle<TAssetData>{ id };
	}

	template<typename TAssetData, typename TGpuResource>
	inline AssetHandle<TAssetData> AssetCache<TAssetData, TGpuResource>::Load(std::string_view name, std::unique_ptr<TAssetData> data)
	{
		RYU_PROFILE_SCOPE();
		const AssetId id = std::hash<std::string_view>{}(name);
		return Load(id, std::move(data), name);
	}

	template<typename TAssetData, typename TGpuResource>
	inline TGpuResource* AssetCache<TAssetData, TGpuResource>::GetGpu(AssetHandle<TAssetData> handle)
	{
		RYU_PROFILE_SCOPE();
		if (!handle.IsValid())
		{
			return nullptr;
		}

		std::unique_lock lock(m_mutex);

		Entry* entry = FindEntry(handle.Id);
		if (!entry)
		{
			return nullptr;
		}

		const std::string name = entry->SourcePath.filename().string();

		// Return if ready
		if (entry->State == AssetState::Ready && entry->GpuResource)
		{
			return entry->GpuResource.get();
		}

		// Create GPU resource if needed (lazy GPU creation)
		if (entry->State == AssetState::Loaded && entry->CpuData)
		{
			RYU_LOG_DEBUG("Creating GPU resource for asset {}", name);
			if (!CreateGpuResource(*entry, name))
			{
				RYU_LOG_ERROR("Failed to create GPU resource for asset {}", name);
				entry->State = AssetState::Failed;
				return nullptr;
			}
		}

		return entry->GpuResource.get();
	}

	template<typename TAssetData, typename TGpuResource>
	inline const TAssetData* AssetCache<TAssetData, TGpuResource>::GetCpu(AssetHandle<TAssetData> handle)
	{
		RYU_PROFILE_SCOPE();

		if (!handle.IsValid())
		{
			return nullptr;
		}

		std::unique_lock lock(m_mutex);

		Entry* entry = FindEntry(handle.Id);
		if (!entry)
		{
			return nullptr;
		}

		return entry->CpuData.get();
	}

	template<typename TAssetData, typename TGpuResource>
	inline void AssetCache<TAssetData, TGpuResource>::EnsureCpuLoaded(AssetHandle<TAssetData> handle)
	{
		std::unique_lock lock(m_mutex);

		Entry* entry = FindEntry(handle.Id);
		if (!entry)
			return;

		// CPU data is already loaded in Load() method, nothing to do
	}

	template<typename TAssetData, typename TGpuResource>
	inline void AssetCache<TAssetData, TGpuResource>::EnsureGpuReady(AssetHandle<TAssetData> handle)
	{
		std::unique_lock lock(m_mutex);

		Entry* entry = FindEntry(handle.Id);
		if (!entry)
			return;

		// Create GPU resource if needed
		if (entry->State == AssetState::Loaded && entry->CpuData)
		{
			CreateGpuResource(*entry, entry->Name);
		}
	}

	template<typename TAssetData, typename TGpuResource>
	inline AssetState AssetCache<TAssetData, TGpuResource>::GetState(AssetHandle<TAssetData> handle) const
	{
		RYU_PROFILE_SCOPE();
		std::shared_lock lock(m_mutex);

		if (const Entry* entry = FindEntry(handle.Id))
		{
			return entry->State;
		}

		return AssetState::Failed;
	}

	template<typename TAssetData, typename TGpuResource>
	inline void AssetCache<TAssetData, TGpuResource>::Invalidate(AssetHandle<TAssetData> handle)
	{
		RYU_PROFILE_SCOPE();
		std::unique_lock lock(m_mutex);

		Entry* entry = FindEntry(handle.Id);
		if (!entry || entry->IsProcedural)
		{
			return;
		}

		// Clear resources and reload
		entry->GpuResource.reset();
		entry->CpuData.reset();
		entry->State = AssetState::Loading;

		// Reload CPU data
		entry->CpuData = LoadAsset<TAssetData>(entry->SourcePath);
		if (entry->CpuData)
		{
			entry->State = AssetState::Loaded;
		}
		else
		{
			RYU_LOG_ERROR("Failed to reload asset from {}", entry->SourcePath.string());
			entry->State = AssetState::Failed;
		}
	}

	template<typename TAssetData, typename TGpuResource>
	inline void AssetCache<TAssetData, TGpuResource>::InvalidateAll()
	{
		RYU_PROFILE_SCOPE();
		std::unique_lock lock(m_mutex);

		for (auto& [id, entry] : m_entries)
		{
			if (!entry.IsProcedural)
			{
				entry.GpuResource.reset();
				entry.CpuData.reset();
				entry.State = AssetState::Loading;

				// Reload CPU data
				entry.CpuData = LoadAsset<TAssetData>(entry.SourcePath);
				if (entry.CpuData)
				{
					entry.State = AssetState::Loaded;
				}
				else
				{
					entry.State = AssetState::Failed;
				}
			}
		}
	}

	template<typename TAssetData, typename TGpuResource>
	inline AssetId AssetCache<TAssetData, TGpuResource>::GenerateId(const fs::path& path) const
	{
		RYU_PROFILE_SCOPE();
		return std::hash<std::string>{}(fs::weakly_canonical(path).string());
	}

	template<typename TAssetData, typename TGpuResource>
	inline AssetId AssetCache<TAssetData, TGpuResource>::GenerateId(std::string_view name) const
	{
		return std::hash<std::string_view>{}(name);
	}

	template<typename TAssetData, typename TGpuResource>
	inline typename AssetCache<TAssetData, TGpuResource>::Entry* AssetCache<TAssetData, TGpuResource>::FindEntry(AssetId id)
	{
		RYU_PROFILE_SCOPE();
		auto it = m_entries.find(id);
		return it != m_entries.end() ? &it->second : nullptr;
	}

	template<typename TAssetData, typename TGpuResource>
	inline const AssetCache<TAssetData, TGpuResource>::Entry* AssetCache<TAssetData, TGpuResource>::FindEntry(AssetId id) const
	{
		RYU_PROFILE_SCOPE();
		auto it = m_entries.find(id);
		return it != m_entries.end() ? &it->second : nullptr;
	}

	template<typename TAssetData, typename TGpuResource>
	inline bool AssetCache<TAssetData, TGpuResource>::LoadCpuData(Entry& entry)
	{
		RYU_PROFILE_SCOPE();
		if (entry.IsProcedural)
		{
			return entry.CpuData != nullptr;
		}

		entry.State = AssetState::Loading;
		entry.CpuData = LoadAsset<TAssetData>(entry.SourcePath);

		if (entry.CpuData)
		{
			entry.State = AssetState::Loaded;
			return true;
		}

		entry.State = AssetState::Failed;
		return false;
	}

	template<typename TAssetData, typename TGpuResource>
	bool AssetCache<TAssetData, TGpuResource>::CreateGpuResource(Entry& entry, std::string_view name)
	{
		RYU_PROFILE_SCOPE();
		if (!m_gpuFactory || !entry.CpuData)
		{
			return false;
		}

		// This requires specialization per asset type
		// See GpuResourceFactory implementation
		if constexpr (IsSame<TAssetData, MeshData>)
		{
			entry.GpuResource = m_gpuFactory->CreateMesh(*entry.CpuData, name);
		}
		else if constexpr (IsSame<TAssetData, TextureData>)
		{
			entry.GpuResource = m_gpuFactory->CreateTexture(*entry.CpuData, name);
		}

		if (entry.GpuResource)
		{
			entry.State = AssetState::Ready;
			return true;
		}

		return false;
	}
}
