#ifndef ESPERT_CORE_RESOURCES_LOADERS_SHADERLOADER_HH
#define ESPERT_CORE_RESOURCES_LOADERS_SHADERLOADER_HH

#include "esppch.hh"

#include "Core/Systems/ResourceSystem.hh"

namespace esp
{
  static std::unordered_map<ShaderStage, std::string> ShaderExtensionMap = {
    { ShaderStage::VERTEX, "vert" },
    { ShaderStage::TESSELATION_CONTROL, "tesc" },
    { ShaderStage::TESSELATION_EVALUATION, "tese" },
    { ShaderStage::GEOMETRY, "geom" },
    { ShaderStage::FRAGMENT, "frag" },
    { ShaderStage::COMPUTE, "comp" }
  };

  class ShaderLoader : public Loader
  {
   public:
    virtual std::unique_ptr<Resource> load(const fs::path& path, const ResourceParams& params) override;
    virtual void unload(std::unique_ptr<Resource> resource) override;

   private:
    std::vector<uint32_t> load_shader_source(const fs::path& full_path);
  };

} // namespace esp

#endif // ESPERT_CORE_RESOURCES_LOADERS_SHADERLOADER_HH