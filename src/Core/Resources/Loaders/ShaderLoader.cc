#include "esppch.hh"

#include "Core/Resources/Loaders/ShaderLoader.hh"

namespace esp
{
  std::unique_ptr<Resource> ShaderLoader::load(const fs::path& path, const ResourceParams& params)
  {
    fs::path full_path = ResourceSystem::get_asset_base_path() / path;
    ESP_ASSERT(fs::is_regular_file(fs::status(full_path)),
               "Could not find {} or it is not a regular file.",
               full_path.string());

    uint64_t file_size = fs::file_size(full_path);
    if (file_size == 0) { ESP_CORE_WARN("Size of shader file {} is 0.", full_path.string()); }

    void* data = nullptr;

    return std::unique_ptr<Resource>(new ShaderResource(full_path, file_size, resource_data_t(data, VOID_DELETER)));
  }

  void ShaderLoader::unload(std::unique_ptr<Resource> resource) { resource.reset(nullptr); }

} // namespace esp
