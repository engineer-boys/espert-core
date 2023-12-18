#ifndef ESPERT_CORE_SYSTEMS_TEXTURESYSTEM_HH
#define ESPERT_CORE_SYSTEMS_TEXTURESYSTEM_HH

#include "esppch.hh"

#include "Core/RenderAPI/Resources/EspTexture.hh"
#include "Core/Resources/ResourceTypes.hh"
#include "Core/Resources/ResourceUtils.hh"

namespace esp
{
  using TextureMap = std::unordered_map<std::string, std::shared_ptr<EspTexture>>;

  class TextureSystem
  {
   private:
    static TextureSystem* s_instance;

   private:
    static const std::string s_default_albedo_texture_name;
    static const std::string s_default_normal_texture_name;
    static const std::string s_default_metallic_texture_name;
    static const std::string s_default_roughness_texture_name;
    static const std::string s_default_ao_texture_name;

    TextureMap m_texture_map;

    TextureSystem();

    static std::shared_ptr<EspTexture> load(const std::string& name, bool mipmapping = false);
    static void create_default_textures();

   public:
    ~TextureSystem();
    PREVENT_COPY(TextureSystem);

    static std::unique_ptr<TextureSystem> create();
    void terminate();

    static std::shared_ptr<EspTexture> acquire(const std::string& name);
    static void release(const std::string& name);
    static std::shared_ptr<EspTexture> get_default_albedo_texture();
    static std::shared_ptr<EspTexture> get_default_normal_texture();
    static std::shared_ptr<EspTexture> get_default_metallic_texture();
    static std::shared_ptr<EspTexture> get_default_roughness_texture();
    static std::shared_ptr<EspTexture> get_default_ao_texture();
  };
} // namespace esp

#endif // ESPERT_CORE_SYSTEMS_TEXTURESYSTEM_HH