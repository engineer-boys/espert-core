#ifndef ESPERT_CORE_SYSTEMS_TEXTURESYSTEM_HH
#define ESPERT_CORE_SYSTEMS_TEXTURESYSTEM_HH

#include "esppch.hh"

#include "Core/RenderAPI/Resources/EspTexture.hh"
#include "Core/Resources/ResourceTypes.hh"
#include "Core/Resources/ResourceUtils.hh"

namespace esp
{
  using TextureMap = std::unordered_map<std::string, std::shared_ptr<EspTexture>>;
  using CubemapMap = std::unordered_map<std::string, std::shared_ptr<EspTexture>>;

  /// @brief Parameters that might affect texture loading process.
  struct TextureParams
  {
    /// @brief True if texture is SRGB.
    bool is_srgb = true;
    /// @brief True if texture is HDR.
    bool is_hdr = false;
    /// @brief Information wheter texture will contain mipmaps.
    bool mipmapping = false;
    /// @brief Type of texture. In case of loading failure the default texture of this type will be returned.
    EspTextureType type = EspTextureType::ALBEDO;
  };

  /// @brief System responsible for loading, configuring and caching textures.
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

    static std::shared_ptr<EspTexture> load(const std::string& name, const TextureParams& params);
    static std::shared_ptr<EspTexture> load_cubemap(const std::string& name, const TextureParams& params);
    static void create_default_textures();
    static bool texture_matching_params(std::shared_ptr<EspTexture> texture, const TextureParams& params);
    static bool is_default_texture_name(const std::string& name);

   public:
    /// @brief Terminates TextureSystem.
    ~TextureSystem();

    PREVENT_COPY(TextureSystem);

    /// @brief Creates TextureSystem singleton instance and loads default textures.
    /// @return Unique pointer to TextureSystem instance.
    static std::unique_ptr<TextureSystem> create();

    /// @brief Destorys TextureSystem instance and removes references to all loaded textures.
    void terminate();

    /// @brief Returns texture with supplied name. If there is no such texture in map then loads it from the drive.
    /// @param name Texture name/relative path. Also used as identifier in texture map.
    /// @param params Parameters that might affect loading process.
    /// @return Shared pointer to EspTexture. It's also stored in systems internal map.
    static std::shared_ptr<EspTexture> acquire(const std::string& name, const TextureParams& params = {});

    /// @brief Returns cubemap with supplied name. If there is no such cubemap in map then loads it from the drive.
    /// @param name Cubemap name/relative path. Also used as identifier in cubemap map.
    /// @param params Parameters that might affect loading process.
    /// @return Shared pointer to EspTexture. It's also stored in systems internal map.
    static std::shared_ptr<EspTexture> acquire_cubemap(const std::string& name, const TextureParams& params = {});

    /// @brief Removes texture's/cubemap's reference from internal map.
    /// @param name Name/relative path to texture/cubemap soon-to-be-released.
    static void release(const std::string& name);

    /// @brief Returns shared pointer to default texture of said type.
    /// @param type Type of default texture to be returned.
    /// @return Shared pointer to one of default textures.
    static std::shared_ptr<EspTexture> get_default_texture(EspTextureType type = EspTextureType::ALBEDO);
  };
} // namespace esp

#endif // ESPERT_CORE_SYSTEMS_TEXTURESYSTEM_HH