#include "esppch.hh"

#include "Core/RenderAPI/Resources/EspShader.hh"
#include "Core/Resources/Loaders/SpirvLoader.hh"

namespace esp
{
  const SpirvExtensionMap SpirvLoader::s_spirv_extension_map = { { EspShaderStage::VERTEX, "vert" },
                                                                 { EspShaderStage::TESSELATION_CONTROL, "tesc" },
                                                                 { EspShaderStage::TESSELATION_EVALUATION, "tese" },
                                                                 { EspShaderStage::GEOMETRY, "geom" },
                                                                 { EspShaderStage::FRAGMENT, "frag" },
                                                                 { EspShaderStage::COMPUTE, "comp" } };

  std::unique_ptr<Resource> SpirvLoader::load(const fs::path& path, const ResourceParams& params)
  {
    fs::path spirv_base_path    = ResourceSystem::get_asset_base_path() / path;
    fs::path spirv_path         = spirv_base_path;
    auto spirv_params           = static_cast<const SpirvResourceParams&>(params);
    SpirvDataMap spirv_data_map = {};

    for (auto type = EspShaderStage::VERTEX; type != EspShaderStage::COMPUTE; ++type)
    {
      auto data =
          load_spirv(spirv_path.replace_extension("").replace_extension(s_spirv_extension_map.at(type) + ".spv"));

      if (data.empty() && spirv_params.shader_satge & type)
      {
        ESP_CORE_ERROR("Could not load {} shader stage {}.", path.string(), esp_shader_stage_to_string(type));
        return nullptr;
      }
      if (!data.empty()) { spirv_data_map.insert({ type, std::move(data) }); }
    }

    return std::unique_ptr<Resource>(new SpirvResource(spirv_base_path, std::move(spirv_data_map)));
  }

  void SpirvLoader::unload(std::unique_ptr<Resource> resource) { resource.reset(nullptr); }

  SpirvData SpirvLoader::load_spirv(const fs::path& full_path)
  {
    if (!fs::is_regular_file(fs::status(full_path)))
    {
      ESP_CORE_ERROR("Could not find {} or it is not a regular file.", full_path.string());
      return SpirvData();
    }

    uint64_t file_size = fs::file_size(full_path);
    if (file_size == 0) { ESP_CORE_WARN("Size of spirv file {} is 0.", full_path.string()); }

    std::ifstream file(full_path, std::ios::binary);
    // TODO: use custom allocator
    SpirvData data((file_size - 3) / 4 + 1);
    file.read(reinterpret_cast<char*>(&(data[0])), file_size);
    file.close();

    ESP_CORE_TRACE("Loaded {}.", full_path.string());

    return data;
  }

} // namespace esp