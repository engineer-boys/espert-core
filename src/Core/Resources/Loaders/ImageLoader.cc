#if defined(__GNUC__) && !defined(NDEBUG) && defined(__OPTIMIZE__)
#warning "Undefing __OPTIMIZE__ because of fmt. This problem occurs after adding stb."
#undef __OPTIMIZE__
#endif

// stb
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "Core/Resources/Loaders/ImageLoader.hh"

namespace esp
{
  std::unique_ptr<Resource> ImageLoader::load(const fs::path& path, const ResourceParams& params)
  {
    fs::path full_path = ResourceSystem::get_asset_base_path() / path;
    if (!fs::is_regular_file(fs::status(full_path)))
    {
      ESP_CORE_ERROR("Could not find {} or it is not a regular file.", full_path.string());
      return nullptr;
    }

    int width, height, channel_count;
    auto image_params = static_cast<const ImageResourceParams&>(params);

    stbi_set_flip_vertically_on_load_thread(image_params.flip_y);

    // TODO: use custom allocator
    stbi_uc* data =
        stbi_load(full_path.string().c_str(), &width, &height, &channel_count, image_params.required_channels);

    if (data == nullptr)
    {
      ESP_CORE_ERROR("Could not load {}.", full_path.string());
      return nullptr;
    }

    return std::unique_ptr<Resource>(
        new ImageResource(full_path,
                          std::move(std::unique_ptr<uint8_t[]>(static_cast<uint8_t*>(data))),
                          channel_count,
                          width,
                          height));
  }

  void ImageLoader::unload(std::unique_ptr<Resource> resource) { resource.reset(nullptr); }
} // namespace esp