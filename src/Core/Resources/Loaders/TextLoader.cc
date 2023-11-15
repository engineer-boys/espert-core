#include "esppch.hh"

#include "Core/Resources/Loaders/TextLoader.hh"

namespace esp
{
  std::unique_ptr<Resource> TextLoader::load(const fs::path& path, const ResourceParams& params)
  {
    fs::path full_path = ResourceSystem::get_asset_base_path() / path;
    ESP_ASSERT(fs::is_regular_file(fs::status(full_path)),
               "Could not find " + full_path.string() + " or it is not a regular file.");

    uint64_t file_size = fs::file_size(full_path);
    if (file_size == 0) { ESP_CORE_WARN("Size of text file {} is 0.", full_path.string()); }

    // TODO: use custom allocator
    void* data = (void*)calloc(sizeof(char), file_size);
    ESP_ASSERT(data != nullptr, "Could not allocate memory for " + full_path.string() + ".");

    std::ifstream file(full_path);
    file.read(static_cast<char*>(data), file_size);

    file.clear();
    file.seekg(0);
    file.unsetf(std::ios_base::skipws);
    uint64_t line_count = std::count(std::istream_iterator<char>(file), std::istream_iterator<char>(), '\n');
    if (file_size > 0) line_count++;

    file.close();

    return std::unique_ptr<Resource>(
        new TextResource(full_path, file_size, resource_data_t(data, VOID_DELETER), line_count));
  }

  void TextLoader::unload(std::unique_ptr<Resource> resource) { resource.reset(nullptr); }

} // namespace esp
