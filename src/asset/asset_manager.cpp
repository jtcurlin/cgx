// Copyright © 2024 Jacob Curlin

#include "asset/asset_manager.h"
#include "core/event_handler.h"
#include "core/events/asset_events.h"
#include "asset/import/asset_importer.h"

namespace cgx::asset
{
AssetManager::AssetManager()  = default;
AssetManager::~AssetManager() = default;

void AssetManager::register_importer(const std::shared_ptr<AssetImporter>& importer)
{
    const auto& extensions = importer->get_supported_file_extensions();

    for (const auto& ext : extensions) {
        // check importer not already registered for current extension
        if (auto it = m_extension_to_importer_map.find(ext) ; it != m_extension_to_importer_map.end()) {
            CGX_ERROR(
                "AssetManager::RegisterImporter: attempted to register >1 importer for an extension type"
                " ({}) which is currently unsupported.",
                ext);
            std::exit(1);
        }
        // map extension with non-owning weak ptr to importer
        m_extension_to_importer_map[ext] = importer;
    }

    // store owning shared ptr to importer
    importer->initialize(shared_from_this());
    // need to pass shared pointer to this assetmanager instance)
    m_importers.push_back(importer);

    auto&      event_handler = core::EventHandler::get_instance();
    core::event::Event event(core::event::importer::REGISTERED);
    event.set_param(core::event::importer::LABEL, importer->get_label());
    event.set_param(core::event::importer::SUPPORTED_EXTENSIONS, extensions);
    event_handler.send_event(event);
}

AssetID AssetManager::import_asset(const std::string& path)
{
    const std::filesystem::path fs_path = clean_path(path);

    // if path already loaded, return id
    if (const auto path_it = m_source_path_to_id.find(fs_path.string()) ; path_it != m_source_path_to_id.end()) {
        return path_it->second; // return asset ID
    }

    // if importer registered for file extension and importer valid, import path
    if (const auto ext_it = m_extension_to_importer_map.find(fs_path.extension().string()) ;
        ext_it != m_extension_to_importer_map.end()) {
        if (const auto importer = ext_it->second.lock()) {
            return importer->import(fs_path.string());
        }
        CGX_ERROR("AssetManager: Failed to acquire importer for specified path extension [{}]", fs_path.string());
    }
    else {
        CGX_ERROR(
            "AssetManager: Failed to import resource, no importer " "registered for specified path extension [{}]",
            fs_path.string());
    }
    return k_invalid_id;
}

AssetID AssetManager::add_asset(const std::shared_ptr<Asset>& asset, bool return_original_on_duplicate)
{
    if (asset == nullptr) {
        CGX_ERROR("AssetManager: failed to add asset. (null asset)");
        return k_invalid_id;
    }

    const auto path_fs = clean_path(asset->get_external_path());

    if (path_fs.string().empty()) {
        CGX_ERROR("AssetManager:: failed to add asset. (no path assigned)");
        return k_invalid_id;
    }

    if (const auto path_it = m_source_path_to_id.find(path_fs.string()) ; path_it != m_source_path_to_id.end()) {
        if (return_original_on_duplicate) {
            return m_source_path_to_id[path_fs.string()];
        }
        CGX_ERROR(
            "AssetManager: failed to add asset. (path [{}] already registered, duplicates not permitted)",
            path_fs.string());
        return k_invalid_id;
    }

    if (const auto asset_it = m_assets.find(asset->get_id()) ; asset_it != m_assets.end()) {
        CGX_ERROR("AssetManager:: failed to add asset. duplicate UID. [{}]", path_fs.string())
    }

    asset->set_internal_path(asset->get_path_prefix() + asset->get_tag());

    m_type_to_id[asset->get_asset_type()].push_back(asset->get_id());
    m_source_path_to_id[path_fs.string()] = asset->get_id();
    m_name_to_id[asset->get_tag()].push_back(asset->get_id());
    m_assets[asset->get_id()] = asset;

    auto&      event_handler = core::EventHandler::get_instance();
    core::event::Event event(core::event::asset::ADDED);
    event.set_param(core::event::asset::ID, asset->get_id());
    event.set_param(core::event::asset::TYPE, asset->get_asset_type());
    event.set_param(core::event::asset::TAG, asset->get_tag());
    event.set_param(core::event::asset::INTERNAL_PATH, asset->get_internal_path());
    event.set_param(core::event::asset::EXTERNAL_PATH, asset->get_external_path());
    event_handler.send_event(event);

    return asset->get_id();
}

bool AssetManager::remove_asset(AssetID asset_id)
{
    if (const auto asset_it = m_assets.find(asset_id) ; asset_it != m_assets.end()) {
        const auto& asset = asset_it->second;

        const std::string asset_typename = asset->get_asset_typename();
        const std::string asset_tag = asset->get_tag();
        const std::string asset_internal_path = asset->get_internal_path();
        const std::string asset_external_path = asset->get_external_path();

        // remove asset's entry from type->id map if present
        if (auto& asset_ids = m_type_to_id[asset->get_asset_type()] ; !asset_ids.empty()) {
            const auto new_end = std::remove(asset_ids.begin(), asset_ids.end(), asset->get_id());
            asset_ids.erase(new_end, asset_ids.end());
        }

        // remove asset's entry from name->id map if present
        if (!asset->get_tag().empty()) {
            if (auto& asset_ids = m_name_to_id[asset->get_tag()] ; !asset_ids.empty()) {
                const auto new_end = std::remove(asset_ids.begin(), asset_ids.end(), asset->get_id());
                asset_ids.erase(new_end, asset_ids.end());
            }
        }

        // remove asset's entry from path->id map if present
        m_source_path_to_id.erase(asset->get_internal_path());

        // remove resource (remove id->resource mapping)
        m_assets.erase(asset_id);

        // dispatch resource-removed event
        auto&      event_handler = core::EventHandler::get_instance();
        core::event::Event event(core::event::asset::REMOVED);
        event.set_param(core::event::asset::ID, asset_id);
        event.set_param(core::event::asset::TYPE, asset_typename);
        event.set_param(core::event::asset::TAG, asset_tag);
        event.set_param(core::event::asset::INTERNAL_PATH, asset_internal_path);
        event.set_param(core::event::asset::EXTERNAL_PATH, asset_external_path);
        event_handler.send_event(event);
        return true;
    }
    CGX_WARN("AssetManager: failed to remove asset. (asset ID [{}] not registered)", asset_id);
    return false;
}

std::shared_ptr<Asset> AssetManager::get_asset(AssetID asset_id)
{
    if (const auto asset_it = m_assets.find(asset_id) ; asset_it != m_assets.end()) {
        return {asset_it->second};
    }

    CGX_ERROR("AssetManager: failed to get asset. (asset ID [{}] not registered)", asset_id);
    return {};
}

AssetID AssetManager::get_id_by_path(const std::string& path)
{
    auto fs_path = clean_path(path).string();
    if (const auto asset_it = m_source_path_to_id.find(fs_path) ; asset_it != m_source_path_to_id.end()) {
        return asset_it->second; // return id
    }
    return k_invalid_id;
}

std::vector<AssetID> AssetManager::getAllIDs()
{
    std::vector<AssetID> asset_id_list;
    for (auto& [asset_id, asset] : m_assets) {
        asset_id_list.push_back(asset_id);
    }
    return asset_id_list;
}

const std::vector<AssetID>& AssetManager::getAllIDs(const AssetType::Type type_filter)
{
    return m_type_to_id[type_filter];
}

const std::unordered_map<AssetID, std::shared_ptr<Asset>>& AssetManager::get_assets() const
{
    return m_assets;
}

const std::vector<std::shared_ptr<AssetImporter>>& AssetManager::get_importers() const
{
    return m_importers;
}

bool AssetManager::is_path_supported(const std::string& path) const
{
    const std::filesystem::path fs_path   = clean_path(path);
    std::string                 extension = fs_path.extension().string();

    std::transform(
        extension.begin(),
        extension.end(),
        extension.begin(),
        [](unsigned char c) {
            return std::tolower(c);
        });

    // Check if the extension is supported
    if (m_extension_to_importer_map.find(extension) != m_extension_to_importer_map.end()) {
        return true;
    }
    return false;
}

std::filesystem::path AssetManager::clean_path(std::string path)
{
    // Transform to lowercase and convert backslashes to forward slashes
    std::transform(
        path.begin(),
        path.end(),
        path.begin(),
        [](char c) -> char {
            if (c == '\\') return '/';                          // Convert backslashes to forward slashes
            return std::tolower(static_cast<unsigned char>(c)); // Convert to lowercase
        });

    // Assign the modified string back to the path, normalize lexically and make preferred
    return std::filesystem::path(path).lexically_normal().make_preferred();
}
}
