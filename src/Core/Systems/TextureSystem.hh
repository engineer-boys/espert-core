#ifndef ESPERT_CORE_SYSTEMS_TEXTURESYSTEM_HH
#define ESPERT_CORE_SYSTEMS_TEXTURESYSTEM_HH

#include "esppch.hh"

#include "Core/Resources/ResourceTypes.hh"
#include "Core/Resources/ResourceUtils.hh"

namespace esp
{
  class Texture
  {
   public:
    Texture(const std::string& name,
            uint8_t* data,
            uint8_t channel_count,
            uint32_t width,
            uint32_t height,
            uint32_t mip_levels) :
        m_name(name),
        m_pixels(data), m_channel_count(channel_count), m_width(width), m_height(height), m_mip_levels(mip_levels),
        m_has_transparency(false)
    {
      if (m_channel_count != 4) { return; }
      for (int i = 0; i < get_size(); i += m_channel_count)
      {
        if (m_pixels.get()[i + 3] < 255)
        {
          m_has_transparency = true;
          break;
        }
      }
    }
    Texture(const std::string& name, std::unique_ptr<ImageResource> image_resource) :
        m_name(name), m_pixels(static_cast<uint8_t*>(image_resource->release())),
        m_channel_count(image_resource->get_channel_count()), m_width(image_resource->get_width()),
        m_height(image_resource->get_height()), m_mip_levels(image_resource->get_mip_levels()),
        m_has_transparency(false)
    {
      if (m_channel_count != 4) { return; }
      for (int i = 0; i < get_size(); i += m_channel_count)
      {
        if (m_pixels.get()[i + 3] < 255)
        {
          m_has_transparency = true;
          break;
        }
      }
    }

    inline const std::string get_name() const { return m_name; }
    inline const uint64_t get_size() const { return m_channel_count * m_width * m_height; }
    inline const uint8_t get_channel_count() const { return m_channel_count; }
    inline const uint32_t get_width() const { return m_width; }
    inline const uint32_t get_height() const { return m_height; }
    inline const uint32_t get_mip_levels() const { return m_mip_levels; }
    inline const bool has_transparency() const { return m_has_transparency; }

   private:
    std::string m_name;
    std::unique_ptr<uint8_t[]> m_pixels;
    uint8_t m_channel_count;
    uint32_t m_width;
    uint32_t m_height;
    uint32_t m_mip_levels;
    bool m_has_transparency;
  };

  using TextureMap = std::unordered_map<std::string, std::shared_ptr<Texture>>;

  class TextureSystem
  {
   public:
    PREVENT_COPY(TextureSystem);

    static std::unique_ptr<TextureSystem> init();
    void shutdown();
    static std::shared_ptr<Texture> acquire(const std::string& name);
    void release(const std::string& name);
    inline const static bool is_initialized() { return s_initalized; }
    std::shared_ptr<Texture> get_default_texture();
    std::shared_ptr<Texture> get_default_diffuse_texture();
    std::shared_ptr<Texture> get_default_specular_texture();
    std::shared_ptr<Texture> get_default_normal_texture();

   private:
    TextureSystem() = default;

    static std::shared_ptr<Texture> load(const std::string& name);
    void create_default_textures();

    static const std::string s_default_texture_name;
    static const std::string s_default_diffuse_texture_name;
    static const std::string s_default_specular_texture_name;
    static const std::string s_default_normal_texture_name;
    static bool s_initalized;
    static TextureMap m_texture_map;
  };
} // namespace esp

#endif // ESPERT_CORE_SYSTEMS_TEXTURESYSTEM_HH