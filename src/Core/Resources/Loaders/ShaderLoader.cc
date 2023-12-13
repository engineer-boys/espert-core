#include "esppch.hh"

#include "Core/Resources/Loaders/ShaderLoader.hh"
#include "Core/Systems/ResourceSystem.hh"

namespace esp
{
  std::unique_ptr<Resource> ShaderLoader::load(const fs::path& path, const ResourceParams& params)
  {
    fs::path shader_path = ResourceSystem::get_asset_base_path() / path;
    auto shader_resource = std::unique_ptr<ShaderResource>(new ShaderResource(path));
    auto shader_params   = static_cast<const ShaderResourceParams&>(params);
    uint8_t iter         = 1;

    while (iter <= static_cast<uint8_t>(ShaderStage::COMPUTE))
    {
      auto data = load_shader_source(shader_path.replace_extension("").replace_extension(
          ShaderExtensionMap.at(static_cast<ShaderStage>(iter)) + ".spv"));

      if (data.empty() && shader_params.stages && shader_params.stages ^ iter)
      {
        ESP_CORE_ERROR("Could not load {} shader stage {}.", path, iter);
        return nullptr;
      }
      if (!data.empty()) { shader_resource->add_shader_source(static_cast<ShaderStage>(iter), std::move(data)); }

      iter <<= 1;
    }

    return std::move(shader_resource);
  }

  void ShaderLoader::unload(std::unique_ptr<Resource> resource) { resource.reset(nullptr); }

  std::vector<uint32_t> ShaderLoader::load_shader_source(const fs::path& full_path)
  {
    if (!fs::is_regular_file(fs::status(full_path))) { return std::vector<uint32_t>(); }

    uint64_t file_size = fs::file_size(full_path);
    if (file_size == 0) { ESP_CORE_WARN("Size of shader source file {} is 0.", full_path.string()); }

    std::ifstream file(full_path, std::ios::binary);
    // TODO: use custom allocator
    std::vector<uint32_t> data((file_size - 3) / 4 + 1);
    file.read(reinterpret_cast<char*>(&(data[0])), file_size);
    file.close();

    ESP_CORE_TRACE("Loaded {}.", full_path.string());

    return data;
  }

} // namespace esp
