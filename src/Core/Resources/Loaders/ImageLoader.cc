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
    ESP_ASSERT(fs::is_regular_file(fs::status(full_path)),
               "Could not find " + full_path.string() + " or it is not a regular file.");

    int width, height, channel_count;
    auto image_params = static_cast<const ImageResourceParams&>(params);

    stbi_set_flip_vertically_on_load_thread(image_params.flip_y);

    // TODO: use custom allocator
    void* data = (void*)stbi_load(full_path.c_str(), &width, &height, &channel_count, image_params.required_channels);
    ESP_ASSERT(data != nullptr, "Could not load " + full_path.string() + ".");

    int mip_levels = std::floor(std::log2(std::max(width, height))) + 1;

    return std::unique_ptr<Resource>(new ImageResource(full_path,
                                                       std::unique_ptr<void, VOID_DELETER_TYPE>(data, VOID_DELETER),
                                                       channel_count,
                                                       width,
                                                       height,
                                                       mip_levels));
  }

  void ImageLoader::unload(std::unique_ptr<Resource> resource) { resource.reset(nullptr); }
} // namespace esp
