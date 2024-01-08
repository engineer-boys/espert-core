#ifndef ESPERT_CORE_RESOURCES_LOADERS_CUBEMAPLOADER_HH
#define ESPERT_CORE_RESOURCES_LOADERS_CUBEMAPLOADER_HH

#include "esppch.hh"

#include "Core/Resources/Systems/ResourceSystem.hh"

namespace esp
{
  /// @brief Loader for CubemapResource.
  class CubemapLoader : public Loader
  {
   public:
    /// @brief Loads all faces of cubemap as ImageResources.
    /// @param path Relative path of resource.
    /// @param params Parameters that might affect resource loading process.
    /// @return Unique pointer to base Resource class.
    virtual std::unique_ptr<Resource> load(const fs::path& path, const ResourceParams& params) override;

    /// @brief Unloads CubemapResource.
    /// @param resource Unique pointer to resource that will be unloaded.
    virtual void unload(std::unique_ptr<Resource> resource) override;
  };

} // namespace esp

#endif // ESPERT_CORE_RESOURCES_LOADERS_CUBEMAPLOADER_HH