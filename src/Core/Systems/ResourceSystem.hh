#ifndef ESPERT_CORE_SYSTEMS_RESOURCESYSTEM_HH
#define ESPERT_CORE_SYSTEMS_RESOURCESYSTEM_HH

#include "Core/Resources/ResourceTypes.hh"
#include "esppch.hh"

namespace esp
{
  class Loader
  {
   public:
    Loader()  = default;
    ~Loader() = default;

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
   public:
    PREVENT_COPY(ResourceSystem);

    static std::unique_ptr<ResourceSystem> init(const fs::path& asset_base_path);
    void shutdown();

    template<class ResourceType> inline void register_loader(std::unique_ptr<Loader> loader)
    {
      ESP_ASSERT(!m_loader_map.contains(typeid(ResourceType)),
                 "Loader for resource type " + std::string(typeid(ResourceType).name()) +
                     " has already been registered.");

      ESP_CORE_TRACE("Loader for resource type {} registered.", typeid(ResourceType).name());
      m_loader_map.insert({ typeid(ResourceType), std::move(loader) });
    }

    template<class ResourceType>
    inline std::unique_ptr<Resource> load(const fs::path& path, const ResourceParams& params)
    {
      ESP_ASSERT(m_loader_map.contains(typeid(ResourceType)),
                 "Cannot load resource " + path.filename().string() + ". Loader of type " +
                     std::string(typeid(ResourceType).name()) + " has not been registered.");
      auto resource = m_loader_map.at(typeid(ResourceType))->load(path, params);
      ESP_CORE_TRACE("Loaded {} {}.", typeid(ResourceType).name(), resource->get_filename());
      return resource;
    }
    void unload(std::unique_ptr<Resource> resource);
    inline const static fs::path& get_asset_base_path() { return s_asset_base_path; }
    inline const static bool is_initialized() { return s_initalized; }

   private:
    ResourceSystem() = default;

    static bool s_initalized;
    static fs::path s_asset_base_path;
    LoaderMap m_loader_map;
  };

} // namespace esp

#endif // ESPERT_CORE_SYSTEMS_RESOURCESYSTEM_HH