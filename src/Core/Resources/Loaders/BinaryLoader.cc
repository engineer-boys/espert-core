#include "esppch.hh"

#include "Core/Resources/Loaders/BinaryLoader.hh"

namespace esp
{
  std::unique_ptr<Resource> BinaryLoader::load(const fs::path& path, const ResourceParams& params)
  {
    fs::path full_path = ResourceSystem::get_asset_base_path() / path;
    if (!fs::is_regular_file(fs::status(full_path)))
    {
      ESP_CORE_ERROR("Could not find {} or it is not a regular file.", full_path.string());
      return nullptr;
    }

    uint64_t file_size = fs::file_size(full_path);
    if (file_size == 0) { ESP_CORE_WARN("Size of binary file {} is 0.", full_path.string()); }

    // TODO: use custom allocator
    void* data = (void*)calloc(sizeof(char), file_size);
    ESP_ASSERT(data != nullptr, "Could not allocate memory for " + full_path.string() + ".");

    std::ifstream file(full_path, std::ios::binary);
    file.read(static_cast<char*>(data), file_size);
    file.close();

    return std::unique_ptr<Resource>(new BinaryResource(full_path, file_size, resource_data_t(data, VOID_DELETER)));
  }

  void BinaryLoader::unload(std::unique_ptr<Resource> resource) { resource.reset(nullptr); }

} // namespace esp
