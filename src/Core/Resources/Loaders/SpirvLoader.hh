#ifndef ESPERT_CORE_RESOURCES_LOADERS_SPIRVLOADER_HH
#define ESPERT_CORE_RESOURCES_LOADERS_SPIRVLOADER_HH

#include "esppch.hh"

#include "Core/Systems/ResourceSystem.hh"

namespace esp
{
  using SpirvExtensionMap = std::unordered_map<EspShaderStage, std::string>;

  class SpirvLoader : public Loader
  {
   public:
    static const SpirvExtensionMap s_spirv_extension_map;

   public:
    virtual std::unique_ptr<Resource> load(const fs::path& path, const ResourceParams& params) override;
    virtual void unload(std::unique_ptr<Resource> resource) override;

   private:
    SpirvData load_spirv(const fs::path& full_path);
  };

} // namespace esp

#endif // ESPERT_CORE_RESOURCES_LOADERS_SPIRVLOADER_HH