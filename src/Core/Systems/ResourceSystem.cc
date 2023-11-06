#include "Core/Systems/ResourceSystem.hh"

#include "Core/Resources/Loaders/BinaryLoader.hh"
#include "Core/Resources/Loaders/TextLoader.hh"

namespace esp
{
  bool ResourceSystem::s_initalized = false;
  fs::path ResourceSystem::s_asset_base_path;

  std::unique_ptr<ResourceSystem> ResourceSystem::init(const fs::path& asset_base_path)
  {
    ESP_ASSERT(!ResourceSystem::s_initalized, "Resource system has already been initialized.");
    ResourceSystem::s_asset_base_path = asset_base_path;

    auto resource_system = std::unique_ptr<ResourceSystem>(new ResourceSystem());

    resource_system->register_loader(std::move(std::unique_ptr<Loader>(new BinaryLoader())));
    resource_system->register_loader(std::move(std::unique_ptr<Loader>(new TextLoader())));

    ResourceSystem::s_initalized = true;
    ESP_CORE_TRACE("Resource system initialized with base path {}.", s_asset_base_path.string());

    return resource_system;
  }

  void ResourceSystem::shutdown()
  {
    ESP_ASSERT(s_initalized, "Cannot shutdown uninitialized system.");

    LoaderMap empty_map;
    m_loader_map.swap(empty_map);

    ResourceSystem::s_initalized = false;
    ESP_CORE_TRACE("Resource system shutdown.");
  }

  void ResourceSystem::register_loader(std::unique_ptr<Loader> loader)
  {
    ESP_ASSERT(!m_loader_map.contains(loader->get_resource_type()),
               "Loader for resource type {} has already been registered.",
               loader->get_resource_type().name());

    ESP_CORE_TRACE("Loadef for resource type {} registered.", loader->get_resource_type().name());
    m_loader_map.insert({ loader->get_resource_type(), std::move(loader) });
  }

  std::unique_ptr<Resource>
  ResourceSystem::load(const std::string& name, ResourceType resource_type, const ResourceParams& params)
  {
    ESP_ASSERT(m_loader_map.contains(resource_type),
               "Cannot load resource {}. Loader of type {} has not been registered.",
               name,
               resource_type.name());
    auto resource = m_loader_map.at(resource_type)->load(name, resource_type, params);
    ESP_CORE_TRACE("Loaded {} {}.", resource->get_resource_type().name(), resource->get_filename());
    return resource;
  }

  void ResourceSystem::unload(std::unique_ptr<Resource> resource)
  {
    ESP_ASSERT(m_loader_map.contains(resource->get_resource_type()),
               "Cannot unload resource {}. Loader of type {} has not been registered.",
               resource->get_filename(),
               resource->get_resource_type().name());
    ESP_CORE_TRACE("Unloaded {} {}.", resource->get_resource_type().name(), resource->get_filename());
    m_loader_map.at(resource->get_resource_type())->unload(std::move(resource));
  }

} // namespace esp