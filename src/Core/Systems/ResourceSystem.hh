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

    virtual std::unique_ptr<Resource>
    load(const std::string& name, ResourceType resource_type, const ResourceParams& params) = 0;
    virtual void unload(std::unique_ptr<Resource> resource)                                 = 0;
    virtual const ResourceType& get_resource_type()                                         = 0;
  };

  using LoaderMap = std::unordered_map<ResourceType, std::unique_ptr<Loader>>;

  class ResourceSystem
  {
   public:
    PREVENT_COPY(ResourceSystem);

    static std::unique_ptr<ResourceSystem> init(const fs::path& asset_base_path);
    void shutdown();
    void register_loader(std::unique_ptr<Loader> loader);
    std::unique_ptr<Resource> load(const std::string& name, ResourceType resource_type, const ResourceParams& params);
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