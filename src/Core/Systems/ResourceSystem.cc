#include "Core/Systems/ResourceSystem.hh"

#include "Core/Resources/Loaders/BinaryLoader.hh"
#include "Core/Resources/Loaders/ImageLoader.hh"
#include "Core/Resources/Loaders/TextLoader.hh"

namespace esp
{
  ResourceSystem* ResourceSystem::s_instance = nullptr;

  ResourceSystem::ResourceSystem()
  {
    if (ResourceSystem::s_instance != nullptr)
    {
      throw std::runtime_error("The resource system instance already exists!");
    }

    ResourceSystem::s_instance = this;
  }

  ResourceSystem::~ResourceSystem()
  {
    if (s_instance) { terminate(); }
  }

  void ResourceSystem::terminate()
  {
    ResourceSystem::s_instance = nullptr;
    LoaderMap empty_map        = {};
    m_loader_map.swap(empty_map);
    ESP_CORE_TRACE("Resource system shutdown.");
  }

  std::unique_ptr<ResourceSystem> ResourceSystem::create(const fs::path& asset_base_path)
  {
    auto resource_system = std::unique_ptr<ResourceSystem>(new ResourceSystem());

    resource_system->m_asset_base_path = asset_base_path;

    resource_system->register_loader<BinaryResource>(std::move(std::unique_ptr<Loader>(new BinaryLoader())));
    resource_system->register_loader<TextResource>(std::move(std::unique_ptr<Loader>(new TextLoader())));
    resource_system->register_loader<ImageResource>(std::move(std::unique_ptr<Loader>(new ImageLoader())));

    ESP_CORE_TRACE("Resource system initialized with base path {}.", asset_base_path.string());

    return resource_system;
  }

  void ResourceSystem::change_asset_base_path(const fs::path& asset_base_path)
  {
    m_asset_base_path = asset_base_path;
    ESP_CORE_INFO("Changed asset base path to {}.", asset_base_path.string());
  }
} // namespace esp