#ifndef ESPERT_CORE_SYSTEMS_TEXTURESYSTEM_HH
#define ESPERT_CORE_SYSTEMS_TEXTURESYSTEM_HH

#include "esppch.hh"

#include "Core/RenderAPI/Resources/EspCubemap.hh"
#include "Core/RenderAPI/Resources/EspTexture.hh"
#include "Core/Resources/ResourceTypes.hh"
#include "Core/Resources/ResourceUtils.hh"

namespace esp
{
  using TextureMap = std::unordered_map<std::string, std::shared_ptr<EspTexture>>;
  using CubemapMap = std::unordered_map<std::string, std::shared_ptr<EspCubemap>>;

  struct TextureParams
  {
    bool mipmapping     = false;
    EspTextureType type = EspTextureType::ALBEDO;
  };

  class TextureSystem
  {
   private:
    static TextureSystem* s_instance;

   private:
    const std::unordered_map<EspTextureType, std::string> m_default_texture_name_map = {
      { EspTextureType::ALBEDO, "default_ALBD" },
      { EspTextureType::NORMAL, "default_NORM" },
      { EspTextureType::METALLIC, "default_METAL" },
      { EspTextureType::ROUGHNESS, "default_ROUGH" },
      { EspTextureType::AO, "default_AO" }
    };

    TextureMap m_texture_map;
    CubemapMap m_cubemap_map;

    TextureSystem();

    static std::shared_ptr<EspTexture> load(const std::string& name, bool mipmapping = false);
    static std::shared_ptr<EspCubemap> load_cube(const std::string& name);
    static void create_default_textures();
    static bool texture_matching_params(std::shared_ptr<EspTexture> texture, const TextureParams& params);
    static bool is_default_texture_name(const std::string& name);

   public:
    ~TextureSystem();
    PREVENT_COPY(TextureSystem);

    static std::unique_ptr<TextureSystem> create();
    void terminate();

    static std::shared_ptr<EspTexture> acquire(const std::string& name);
    static std::shared_ptr<EspCubemap> acquire_cube(const std::string& name);
    static void release(const std::string& name);
    static std::shared_ptr<EspTexture> get_default_texture(EspTextureType type = EspTextureType::ALBEDO);
  };
} // namespace esp

#endif // ESPERT_CORE_SYSTEMS_TEXTURESYSTEM_HH