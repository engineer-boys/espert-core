#include "Core/Systems/ResourceSystem.hh"

#include "Core/Resources/Loaders/BinaryLoader.hh"
#include "Core/Resources/Loaders/ImageLoader.hh"
#include "Core/Resources/Loaders/TextLoader.hh"

namespace esp
{
  bool ResourceSystem::s_initalized = false;
  fs::path ResourceSystem::s_asset_base_path;
  LoaderMap ResourceSystem::m_loader_map;

  std::unique_ptr<ResourceSystem> ResourceSystem::create(const fs::path& asset_base_path)
  {
    ESP_ASSERT(!ResourceSystem::s_initalized, "Resource system has already been initialized.");
    ResourceSystem::s_asset_base_path = asset_base_path;

    auto resource_system = std::unique_ptr<ResourceSystem>(new ResourceSystem());

    resource_system->register_loader<BinaryResource>(std::move(std::unique_ptr<Loader>(new BinaryLoader())));
    resource_system->register_loader<TextResource>(std::move(std::unique_ptr<Loader>(new TextLoader())));
    resource_system->register_loader<ImageResource>(std::move(std::unique_ptr<Loader>(new ImageLoader())));

    ResourceSystem::s_initalized = true;
    ESP_CORE_TRACE("Resource system initialized with base path {}.", s_asset_base_path.string());

    return resource_system;
  }

  void ResourceSystem::terminate()
  {
    ESP_ASSERT(s_initalized, "Cannot shutdown uninitialized resource system.");

    LoaderMap empty_map;
    m_loader_map.swap(empty_map);

    ResourceSystem::s_initalized = false;
    ESP_CORE_TRACE("Resource system shutdown.");
  }

  void ResourceSystem::unload(std::unique_ptr<Resource> resource)
  {
    ESP_ASSERT(m_loader_map.contains(typeid(*resource)),
               "Cannot unload resource " + resource->get_filename() + ". Loader of type " +
                   std::string(typeid(*resource).name()) + " has not been registered.");
    auto msg = "Unloaded " + std::string(typeid(*resource).name()) + " " + resource->get_filename();
    m_loader_map.at(typeid(*resource))->unload(std::move(resource));
    ESP_CORE_TRACE(msg);
  }

} // namespace esp