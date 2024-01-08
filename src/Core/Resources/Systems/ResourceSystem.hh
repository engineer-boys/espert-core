#ifndef ESPERT_CORE_SYSTEMS_RESOURCESYSTEM_HH
#define ESPERT_CORE_SYSTEMS_RESOURCESYSTEM_HH

#include "esppch.hh"

#include "Core/Resources/ResourceTypes.hh"

namespace esp
{
  /// @brief Abstract loader class. Defines interface for resource loaders.
  class Loader
  {
   public:
    /// @brief Default constructor.
    Loader() = default;
    /// @brief Virtual destructor.
    virtual ~Loader() {}

    /// @brief Abstract method for loading resources.
    /// @param path Resource path relative to the root of resource system.
    /// @param params Parameters that might affect resource loading process.
    /// @return Unique pointer to base Resource.
    virtual std::unique_ptr<Resource> load(const fs::path& path, const ResourceParams& params) = 0;

    /// @brief Abstract method for destruction of a resource.
    /// @param resource Unique pointer to base Resource class.
    virtual void unload(std::unique_ptr<Resource> resource) = 0;
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

  /// @brief System responsible for loading and unloading resources from drive.
  class ResourceSystem
  {
   private:
    static ResourceSystem* s_instance;

   private:
    fs::path m_asset_base_path;
    LoaderMap m_loader_map;

    ResourceSystem();

   public:
    /// @brief Destructor terminates instance of ResourceSystem.
    ~ResourceSystem();

    PREVENT_COPY(ResourceSystem);

    /// @brief Creates ResourceSystem singleton instance and sets root directory.
    /// @param asset_base_path Absolute path which resource relative paths relate to.
    /// @return Unique pointer to ResourceSystem.
    static std::unique_ptr<ResourceSystem> create(const fs::path& asset_base_path);

    /// @brief Destorys ResourceSystem instance and registered loaders.
    void terminate();

    /// @brief Returns raw pointer to ResourceSystem instance.
    /// @return Pointer to ResourceSystem.
    inline static ResourceSystem* get_instance() { return ResourceSystem::s_instance; }

    /// @brief Returns ResourceSystem's root path.
    /// @return Const reference to ResourceSystem's root path..
    inline static const fs::path& get_asset_base_path() { return s_instance->m_asset_base_path; }

    /// @brief Registers loader for specific resource type.
    /// @tparam ResourceType Type of resource for which loader will be registered.
    /// @param loader Unique pointer to instance of loader.
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

    /// @brief Runs load method of specific loader registered for ResourceType and returns it's return value.
    /// @tparam ResourceType Type of resource that will be loaded.
    /// @param path Relative path to the resource.
    /// @param params Additional params that might affect loading process.
    /// @return Unique pointer to base Resource class returned by responsive load method.
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
      if (resource) { ESP_CORE_TRACE("Loaded {} {}.", typeid(ResourceType).name(), resource->get_filename()); }
      return resource;
    }

    /// @brief Runs unload method of specific loader registered for ResourceType.
    /// @param resource Unique pointer to resource that will be unloaded.
    inline static void unload(std::unique_ptr<Resource> resource)
    {
      auto& r = *resource;
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