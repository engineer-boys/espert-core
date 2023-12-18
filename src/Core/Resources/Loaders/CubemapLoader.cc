#include "esppch.hh"

#include "Core/RenderAPI/Resources/EspCubemapFace.hh"
#include "Core/Resources/Loaders/CubemapLoader.hh"

namespace esp
{
  std::unique_ptr<Resource> CubemapLoader::load(const fs::path& path, const ResourceParams& params)
  {
    fs::path base_path = ResourceSystem::get_asset_base_path() / path;

    FaceResourceMap face_resource_map = {};
    for (auto face = EspCubemapFace::TOP; face < EspCubemapFace::ENUM_END; ++face)
    {
      fs::path face_path = fs::path(base_path.stem().string() + "_" + esp_cubemap_face_to_str(face))
                               .replace_extension(base_path.extension());
      auto resource = ResourceSystem::load<ImageResource>(face_path, params);
      if (resource == nullptr)
      {
        ESP_CORE_ERROR("Could not load cubemap {path}.");
        return nullptr;
      }
      auto image_resource = unique_cast<ImageResource>(std::move(resource));
      face_resource_map.insert({ face, std::move(image_resource) });
    }

    return std::unique_ptr<Resource>(new CubemapResource(base_path, std::move(face_resource_map)));
  }

  void CubemapLoader::unload(std::unique_ptr<Resource> resource) { resource.reset(nullptr); }

} // namespace esp