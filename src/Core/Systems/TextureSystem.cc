#include "Core/Systems/TextureSystem.hh"
#include "Core/Systems/ResourceSystem.hh"

namespace esp
{
  const std::string TextureSystem::s_default_texture_name          = "default";
  const std::string TextureSystem::s_default_diffuse_texture_name  = "default_DIFF";
  const std::string TextureSystem::s_default_specular_texture_name = "default_SPEC";
  const std::string TextureSystem::s_default_normal_texture_name   = "default_NORM";
  bool TextureSystem::s_initalized                                 = false;
  TextureMap TextureSystem::m_texture_map;

  std::unique_ptr<TextureSystem> TextureSystem::init()
  {
    ESP_ASSERT(!TextureSystem::s_initalized, "Texture system has already been initialized.");

    auto texture_system = std::unique_ptr<TextureSystem>(new TextureSystem());

    texture_system->create_default_textures();

    TextureSystem::s_initalized = true;
    ESP_CORE_TRACE("Texture system initialized.");

    return texture_system;
  }

  void TextureSystem::shutdown()
  {
    ESP_ASSERT(s_initalized, "Cannot shutdown uninitialized texture system.");

    TextureSystem::s_initalized = false;
    ESP_CORE_TRACE("Resource system shutdown.");
  }

  std::shared_ptr<Texture> TextureSystem::acquire(const std::string& name)
  {
    if (m_texture_map.contains(name)) return m_texture_map.at(name);
    return load(name);
  }

  void TextureSystem::release(const std::string& name)
  {
    ESP_ASSERT(m_texture_map.contains(name), "Cannot release texture " + name + ". Not present in map.");
    if (name == s_default_texture_name || name == s_default_diffuse_texture_name ||
        name == s_default_normal_texture_name || name == s_default_specular_texture_name)
    {
      ESP_CORE_WARN("Cannot release default texture.");
      return;
    }
    m_texture_map.erase(m_texture_map.find(name));
    ESP_CORE_TRACE("Released " + name + " texture.");
  }

  std::shared_ptr<Texture> TextureSystem::load(const std::string& name)
  {
    ImageResourceParams params;
    auto resource       = ResourceSystem::load<ImageResource>(name, params);
    auto image_resource = std::unique_ptr<esp::ImageResource>(static_cast<esp::ImageResource*>(resource.release()));
    auto texture        = std::make_shared<Texture>(name, std::move(image_resource));
    m_texture_map.insert({ name, texture });
    ESP_CORE_TRACE("Loaded {} texture.", name);
    return texture;
  }

  std::shared_ptr<Texture> TextureSystem::get_default_texture() { return m_texture_map.at(s_default_texture_name); }

  std::shared_ptr<Texture> TextureSystem::get_default_diffuse_texture()
  {
    return m_texture_map.at(s_default_diffuse_texture_name);
  }

  std::shared_ptr<Texture> TextureSystem::get_default_specular_texture()
  {
    return m_texture_map.at(s_default_specular_texture_name);
  }

  std::shared_ptr<Texture> TextureSystem::get_default_normal_texture()
  {
    return m_texture_map.at(s_default_normal_texture_name);
  }

  void TextureSystem::create_default_textures()
  {
    uint8_t tex_channels    = 4;
    uint32_t tex_width      = 32;
    uint32_t tex_height     = 32;
    uint32_t tex_mip_levels = 1;
    uint64_t magenta        = 0xf800f8ff;

    uint8_t* regular_data = (uint8_t*)calloc(tex_width * tex_height * tex_channels, sizeof(uint8_t));
    for (int i = 0; i < tex_height / 2; i++)
    {
      for (int j = 0; j < tex_width / 2; j++)
      {
        memcpy(regular_data + i * tex_width * tex_channels + tex_width / 2 + j, &magenta, sizeof(uint64_t));
        memcpy(regular_data + (i + tex_height / 2) * tex_width * tex_channels + j, &magenta, sizeof(uint64_t));
      }
    }

    m_texture_map.insert({ s_default_texture_name,
                           std::make_shared<Texture>(s_default_texture_name,
                                                     regular_data,
                                                     tex_channels,
                                                     tex_width,
                                                     tex_height,
                                                     tex_mip_levels) });

    uint8_t* diffuse_data = (uint8_t*)calloc(tex_width * tex_height * tex_channels, sizeof(uint8_t));
    memset(diffuse_data, 255, tex_width * tex_height * tex_channels);

    m_texture_map.insert({ s_default_diffuse_texture_name,
                           std::make_shared<Texture>(s_default_diffuse_texture_name,
                                                     diffuse_data,
                                                     tex_channels,
                                                     tex_width,
                                                     tex_height,
                                                     tex_mip_levels) });

    uint8_t* specular_data = (uint8_t*)calloc(tex_width * tex_height * tex_channels, sizeof(uint8_t));

    m_texture_map.insert({ s_default_specular_texture_name,
                           std::make_shared<Texture>(s_default_specular_texture_name,
                                                     specular_data,
                                                     tex_channels,
                                                     tex_width,
                                                     tex_height,
                                                     tex_mip_levels) });

    uint8_t* normal_data = (uint8_t*)calloc(tex_width * tex_height * tex_channels, sizeof(uint8_t));

    for (int i = 0; i < tex_height; ++i)
    {
      for (int j = 0; j < tex_width; ++j)
      {
        int index = ((i * 16) + j) * tex_channels;

        normal_data[index + 0] = 128;
        normal_data[index + 1] = 128;
        normal_data[index + 2] = 255;
        normal_data[index + 3] = 255;
      }
    }

    m_texture_map.insert({ s_default_normal_texture_name,
                           std::make_shared<Texture>(s_default_normal_texture_name,
                                                     normal_data,
                                                     tex_channels,
                                                     tex_width,
                                                     tex_height,
                                                     tex_mip_levels) });
  }

} // namespace esp
