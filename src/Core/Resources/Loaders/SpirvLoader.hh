#ifndef ESPERT_CORE_RESOURCES_LOADERS_SPIRVLOADER_HH
#define ESPERT_CORE_RESOURCES_LOADERS_SPIRVLOADER_HH

#include "esppch.hh"

#include "Core/Resources/Systems/ResourceSystem.hh"

namespace esp
{
  using SpirvExtensionMap = std::unordered_map<EspShaderStage, std::string>;

  /// @brief Loader for SpirvResource.
  class SpirvLoader : public Loader
  {
   public:
    /// @brief Maps EspShaderStage to it's standard source file extension.
    static const SpirvExtensionMap s_spirv_extension_map;

   public:
    /// @brief Loads shader source data in SPIR-V format.
    /// @param path Relative path of resource.
    /// @param params Parameters that might affect resource loading process.
    /// @return Unique pointer to base Resource class.
    virtual std::unique_ptr<Resource> load(const fs::path& path, const ResourceParams& params) override;

    /// @brief Unloads SpirvResource.
    /// @param resource Unique pointer to resource that will be unloaded.
    virtual void unload(std::unique_ptr<Resource> resource) override;

   private:
    SpirvData load_spirv(const fs::path& full_path);
  };

} // namespace esp

#endif // ESPERT_CORE_RESOURCES_LOADERS_SPIRVLOADER_HH