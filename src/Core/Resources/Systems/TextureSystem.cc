#include "TextureSystem.hh"
#include "ResourceSystem.hh"

namespace esp
{
  const std::string TextureSystem::s_default_albedo_texture_name    = "default_ALBD";
  const std::string TextureSystem::s_default_normal_texture_name    = "default_NORM";
  const std::string TextureSystem::s_default_metallic_texture_name  = "default_METAL";
  const std::string TextureSystem::s_default_roughness_texture_name = "default_ROUGH";
  const std::string TextureSystem::s_default_ao_texture_name        = "default_AO";
  TextureSystem* TextureSystem::s_instance                          = nullptr;

  TextureSystem::TextureSystem()
  {
    if (TextureSystem::s_instance != nullptr)
    {
      throw std::runtime_error("The texture system instance already exists!");
    }

    TextureSystem::s_instance = this;
  }

  TextureSystem::~TextureSystem()
  {
    if (s_instance) { terminate(); }
  }

  std::unique_ptr<TextureSystem> TextureSystem::create()
  {
    auto texture_system = std::unique_ptr<TextureSystem>(new TextureSystem());

    texture_system->create_default_textures();

    ESP_CORE_TRACE("Texture system initialized.");

    return texture_system;
  }

  void TextureSystem::terminate()
  {
    TextureSystem::s_instance = nullptr;
    TextureMap empty_map      = {};
    m_texture_map.swap(empty_map);
    ESP_CORE_TRACE("Texture system shutdown.");
  }

  std::shared_ptr<EspTexture> TextureSystem::acquire(const std::string& name)
  {
    if (s_instance->m_texture_map.contains(name)) return s_instance->m_texture_map.at(name);
    return load(name);
  }

  void TextureSystem::release(const std::string& name)
  {
    if (!s_instance->m_texture_map.contains(name))
    {
      ESP_CORE_ERROR("Cannot release texture {}. Not present in map.", name);
    }

    if (name == s_default_albedo_texture_name || name == s_default_normal_texture_name ||
        name == s_default_metallic_texture_name || name == s_default_roughness_texture_name ||
        name == s_default_ao_texture_name)
    {
      ESP_CORE_WARN("Cannot release default texture.");
      return;
    }

    s_instance->m_texture_map.erase(s_instance->m_texture_map.find(name));
    ESP_CORE_TRACE("Released texture {}.", name);
  }

  std::shared_ptr<EspTexture> TextureSystem::load(const std::string& name, bool mipmapping)
  {
    // TODO: set params.flip_y accordingly to currently used graphics API
    ImageResourceParams params;
    auto resource = ResourceSystem::load<ImageResource>(name, params);
    if (!resource)
    {
      ESP_CORE_ERROR("Could not load texture {}.", name);
      return get_default_albedo_texture();
    }
    auto image_resource = unique_cast<ImageResource>(std::move(resource));
    auto texture        = EspTexture::create(name, std::move(image_resource), mipmapping);
    s_instance->m_texture_map.insert({ name, texture });
    ESP_CORE_TRACE("Loaded texture {}.", name);
    return texture;
  }

  std::shared_ptr<EspTexture> TextureSystem::get_default_albedo_texture()
  {
    return s_instance->m_texture_map.at(s_default_albedo_texture_name);
  }

  std::shared_ptr<EspTexture> TextureSystem::get_default_normal_texture()
  {
    return s_instance->m_texture_map.at(s_default_normal_texture_name);
  }

  std::shared_ptr<EspTexture> TextureSystem::get_default_metallic_texture()
  {
    return s_instance->m_texture_map.at(s_default_metallic_texture_name);
  }

  std::shared_ptr<EspTexture> TextureSystem::get_default_roughness_texture()
  {
    return s_instance->m_texture_map.at(s_default_roughness_texture_name);
  }

  std::shared_ptr<EspTexture> TextureSystem::get_default_ao_texture()
  {
    return s_instance->m_texture_map.at(s_default_ao_texture_name);
  }

  void TextureSystem::create_default_textures()
  {
    uint8_t tex_channels = 4;
    uint32_t tex_width   = 32;
    uint32_t tex_height  = 32;
    uint32_t magenta     = 0xfff800f8;
    uint32_t violet      = 0xffff8080;
    uint32_t grey        = 0xff757575;
    uint32_t white       = 0xffffffff;

    uint8_t* regular_data = (uint8_t*)calloc(tex_width * tex_height * tex_channels, sizeof(uint8_t));
    for (int i = 0; i < tex_height / 2; i++)
    {
      for (int j = 0; j < tex_width / 2; j++)
      {
        memcpy(regular_data + (i + j * tex_width) * tex_channels, &magenta, sizeof(magenta));
        memcpy(regular_data + ((i + tex_height / 2) + (j + tex_width / 2) * tex_width) * tex_channels,
               &magenta,
               sizeof(magenta));
      }
    }

    s_instance->m_texture_map.insert(
        { s_default_albedo_texture_name,
          EspTexture::create(s_default_albedo_texture_name,
                             std::move(std::make_unique<ImageResource>(fs::path(s_default_albedo_texture_name),
                                                                       std::unique_ptr<uint8_t[]>(regular_data),
                                                                       tex_channels,
                                                                       tex_width,
                                                                       tex_height))) });

    uint8_t* normal_data = (uint8_t*)calloc(tex_width * tex_height * tex_channels, sizeof(uint8_t));
    for (int i = 0; i < tex_width * tex_height; ++i)
    {
      memcpy(normal_data + i * tex_channels, &violet, sizeof(violet));
    }

    s_instance->m_texture_map.insert(
        { s_default_normal_texture_name,
          EspTexture::create(s_default_normal_texture_name,
                             std::move(std::make_unique<ImageResource>(fs::path(s_default_normal_texture_name),
                                                                       std::unique_ptr<uint8_t[]>(normal_data),
                                                                       tex_channels,
                                                                       tex_width,
                                                                       tex_height))) });

    uint8_t* metallic_data = (uint8_t*)calloc(tex_width * tex_height * tex_channels, sizeof(uint8_t));
    for (int i = 0; i < tex_width * tex_height; ++i)
    {
      memcpy(metallic_data + i * tex_channels, &grey, sizeof(grey));
    }

    s_instance->m_texture_map.insert(
        { s_default_metallic_texture_name,
          EspTexture::create(s_default_metallic_texture_name,
                             std::move(std::make_unique<ImageResource>(fs::path(s_default_metallic_texture_name),
                                                                       std::unique_ptr<uint8_t[]>(metallic_data),
                                                                       tex_channels,
                                                                       tex_width,
                                                                       tex_height))) });

    uint8_t* roughness_data = (uint8_t*)calloc(tex_width * tex_height * tex_channels, sizeof(uint8_t));
    for (int i = 0; i < tex_width * tex_height; ++i)
    {
      memcpy(roughness_data + i * tex_channels, &grey, sizeof(grey));
    }

    s_instance->m_texture_map.insert(
        { s_default_roughness_texture_name,
          EspTexture::create(s_default_roughness_texture_name,
                             std::move(std::make_unique<ImageResource>(fs::path(s_default_roughness_texture_name),
                                                                       std::unique_ptr<uint8_t[]>(roughness_data),
                                                                       tex_channels,
                                                                       tex_width,
                                                                       tex_height))) });

    uint8_t* ao_data = (uint8_t*)calloc(tex_width * tex_height * tex_channels, sizeof(uint8_t));
    for (int i = 0; i < tex_width * tex_height; ++i)
    {
      memcpy(ao_data + i * tex_channels, &white, sizeof(white));
    }

    s_instance->m_texture_map.insert(
        { s_default_ao_texture_name,
          EspTexture::create(s_default_ao_texture_name,
                             std::move(std::make_unique<ImageResource>(fs::path(s_default_ao_texture_name),
                                                                       std::unique_ptr<uint8_t[]>(ao_data),
                                                                       tex_channels,
                                                                       tex_width,
                                                                       tex_height))) });
  }

} // namespace esp
