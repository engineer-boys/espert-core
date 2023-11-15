#ifndef ESPERT_CORE_SYSTEMS_RESOURCESYSTEM_HH
#define ESPERT_CORE_SYSTEMS_RESOURCESYSTEM_HH

#include "esppch.hh"

#include "Core/Resources/ResourceTypes.hh"

namespace esp
{
  class Loader
  {
   public:
    Loader() = default;
    virtual ~Loader() {}

    virtual std::unique_ptr<Resource> load(const fs::path& path, const ResourceParams& params) = 0;
    virtual void unload(std::unique_ptr<Resource> resource)                                    = 0;
  };

  // type map implementation from https://en.cppreference.com/w/cpp/types/type_info/hash_code
  using TypeInfoRef = std::reference_wrapper<const std::type_info>;

  struct Hasher
  {
    std::size_t operator()(TypeInfoRef code) const { return code.get().hash_code(); }
  };

  struct EqualTo
  {
    bool operator()(TypeInfoRef lhs, TypeInfoRef rhs) const { return lhs.get() == rhs.get(); }
  };

  using LoaderMap = std::unordered_map<TypeInfoRef, std::unique_ptr<Loader>, Hasher, EqualTo>;

  class ResourceSystem
  {
   private:
    static ResourceSystem* s_instance;

   private:
    fs::path m_asset_base_path;
    LoaderMap m_loader_map;

    ResourceSystem();

   public:
    ~ResourceSystem();
    PREVENT_COPY(ResourceSystem);

    static std::unique_ptr<ResourceSystem> create(const fs::path& asset_base_path);

    inline static ResourceSystem* get_instance() { return ResourceSystem::s_instance; }
    inline static const fs::path& get_asset_base_path() { return s_instance->m_asset_base_path; }

    template<class ResourceType> inline static void register_loader(std::unique_ptr<Loader> loader)
    {
      if (s_instance->m_loader_map.contains(typeid(ResourceType)))
      {
        ESP_CORE_ERROR("Loader for resource type {} has already been registered.", typeid(ResourceType).name());
        return;
      }

      ESP_CORE_TRACE("Loader for resource type {} registered.", typeid(ResourceType).name());
      s_instance->m_loader_map.insert({ typeid(ResourceType), std::move(loader) });
    }

    template<class ResourceType>
    inline static std::unique_ptr<Resource> load(const fs::path& path, const ResourceParams& params)
    {
      if (!s_instance->m_loader_map.contains(typeid(ResourceType)))
      {
        ESP_CORE_ERROR("Cannot load resource {}. Loader for type {} has not been registered.",
                       path.filename().string(),
                       typeid(ResourceType).name());
        return nullptr;
      }

      auto resource = s_instance->m_loader_map.at(typeid(ResourceType))->load(path, params);
      ESP_CORE_TRACE("Loaded {} {}.", typeid(ResourceType).name(), resource->get_filename());
      return resource;
    }
    inline static void unload(std::unique_ptr<Resource> resource)
    {
      auto& r = *resource.get();
      if (!s_instance->m_loader_map.contains(typeid(r)))
      {
        ESP_CORE_ERROR("Cannot unload resource {}. Loader of type {} has not been registered.",
                       resource->get_filename(),
                       typeid(r).name());
        return;
      }
      auto msg = "Unloaded " + std::string(typeid(r).name()) + " " + resource->get_filename();
      s_instance->m_loader_map.at(typeid(r))->unload(std::move(resource));
      ESP_CORE_TRACE(msg);
    }
  };

} // namespace esp

#endif // ESPERT_CORE_SYSTEMS_RESOURCESYSTEM_HH