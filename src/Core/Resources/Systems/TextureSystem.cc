#include "TextureSystem.hh"
#include "ResourceSystem.hh"

namespace esp
{
  TextureSystem* TextureSystem::s_instance = nullptr;

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
    TextureSystem::s_instance    = nullptr;
    TextureMap empty_texture_map = {};
    m_texture_map.swap(empty_texture_map);
    CubemapMap empty_cubemap_map = {};
    m_cubemap_map.swap(empty_cubemap_map);
    ESP_CORE_TRACE("Texture system shutdown.");
  }

  std::shared_ptr<EspTexture> TextureSystem::acquire(const std::string& name, const TextureParams& params)
  {
    if (s_instance->m_texture_map.contains(name))
    {
      auto texture = s_instance->m_texture_map.at(name);
      if (!texture_matching_params(texture, params)) { ESP_CORE_WARN("Returning texture with mismatched params."); }

      return texture;
    }
    return load(name, params);
  }

  std::shared_ptr<EspTexture> TextureSystem::acquire_cubemap(const std::string& name, const TextureParams& params)
  {
    if (s_instance->m_cubemap_map.contains(name)) return s_instance->m_cubemap_map.at(name);
    return load_cubemap(name, params);
  }

  void TextureSystem::release(const std::string& name)
  {
    if (!s_instance->m_texture_map.contains(name))
    {
      ESP_CORE_ERROR("Cannot release texture {}. Not present in map.", name);
    }

    if (is_default_texture_name(name))
    {
      ESP_CORE_WARN("Cannot release a default texture.");
      return;
    }

    s_instance->m_texture_map.erase(s_instance->m_texture_map.find(name));
    ESP_CORE_TRACE("Released texture {}.", name);
  }

  std::shared_ptr<EspTexture> TextureSystem::load(const std::string& name, const TextureParams& params)
  {
    // TODO: set params.flip_y accordingly to currently used graphics API
    ImageResourceParams img_params;
    auto resource = ResourceSystem::load<ImageResource>(name, img_params);
    if (!resource)
    {
      ESP_CORE_ERROR("Could not load texture {}.", name);
      return get_default_texture(params.type);
    }
    auto image_resource = unique_cast<ImageResource>(std::move(resource));

    /*
      TODO: The code below shouldn't be commented out. However, current render API
      implementation doesn't support images with fewer than 4 channels.
    */
    EspTextureFormat format;
    // switch (image_resource->get_channel_count())
    // {
    // case 1:
    //   format = params.is_srgb ? EspTextureFormat::ESP_FORMAT_R8_SRGB : EspTextureFormat::ESP_FORMAT_R8_UNORM;
    //   break;

    // case 3:
    //   format = params.is_srgb ? EspTextureFormat::ESP_FORMAT_R8G8B8_SRGB : EspTextureFormat::ESP_FORMAT_R8G8B8_UNORM;
    //   break;

    // case 4:
    format = params.is_srgb ? EspTextureFormat::ESP_FORMAT_R8G8B8A8_SRGB : EspTextureFormat::ESP_FORMAT_R8G8B8A8_UNORM;
    //   break;

    // default:
    //   ESP_ERROR("You load texture {} with unsupported channel count", name);
    //   throw std::runtime_error("Unsupported channel count");
    // }

    auto texture = EspTexture::create(name, std::move(image_resource), params.type, params.mipmapping, format);
    s_instance->m_texture_map.insert({ name, texture });
    ESP_CORE_TRACE("Loaded texture {}.", name);
    return texture;
  }

  std::shared_ptr<EspTexture> TextureSystem::load_cubemap(const std::string& name, const TextureParams& params)
  {
    // TODO: set params.flip_y accordingly to currently used graphics API
    CubemapResourceParams cube_params;
    auto resource = ResourceSystem::load<CubemapResource>(name, cube_params);
    if (!resource)
    {
      ESP_CORE_ERROR("Could not load cubemap {}.", name);
      return nullptr;
    }
    auto cubemap_resource = unique_cast<CubemapResource>(std::move(resource));

    /*
      TODO: The code below shouldn't be commented out. However, current render API
      implementation doesn't support images with fewer than 4 channels.
    */
    EspTextureFormat format;
    // switch (cubemap_resource->get_channel_count())
    // {
    // case 1:
    //   format = params.is_srgb ? EspTextureFormat::ESP_FORMAT_R8_SRGB : EspTextureFormat::ESP_FORMAT_R8_UNORM;
    //   break;

    // case 3:
    //   format = params.is_srgb ? EspTextureFormat::ESP_FORMAT_R8G8B8_SRGB : EspTextureFormat::ESP_FORMAT_R8G8B8_UNORM;
    //   break;

    // case 4:
    format = params.is_srgb ? EspTextureFormat::ESP_FORMAT_R8G8B8A8_SRGB : EspTextureFormat::ESP_FORMAT_R8G8B8A8_UNORM;
    //   break;

    // default:
    //   ESP_ERROR("You load texture {} with unsupported channel count", name);
    //   throw std::runtime_error("Unsupported channel count");
    // }

    auto cubemap = EspTexture::create_cubemap(name, std::move(cubemap_resource), format);
    s_instance->m_cubemap_map.insert({ name, cubemap });
    ESP_CORE_TRACE("Loaded cubemap {}.", name);
    return cubemap;
  }

  std::shared_ptr<EspTexture> TextureSystem::get_default_texture(EspTextureType type)
  {
    return s_instance->m_texture_map.at(s_instance->m_default_texture_name_map.at(type));
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

    auto name = s_instance->m_default_texture_name_map.at(EspTextureType::ALBEDO);
    s_instance->m_texture_map.insert(
        { name,
          EspTexture::create(name,
                             std::move(std::make_unique<ImageResource>(fs::path(name),
                                                                       std::unique_ptr<uint8_t[]>(regular_data),
                                                                       tex_channels,
                                                                       tex_width,
                                                                       tex_height))) });

    uint8_t* normal_data = (uint8_t*)calloc(tex_width * tex_height * tex_channels, sizeof(uint8_t));
    for (int i = 0; i < tex_width * tex_height; ++i)
    {
      memcpy(normal_data + i * tex_channels, &violet, sizeof(violet));
    }

    name = s_instance->m_default_texture_name_map.at(EspTextureType::NORMAL);
    s_instance->m_texture_map.insert(
        { name,
          EspTexture::create(name,
                             std::move(std::make_unique<ImageResource>(fs::path(name),
                                                                       std::unique_ptr<uint8_t[]>(normal_data),
                                                                       tex_channels,
                                                                       tex_width,
                                                                       tex_height))) });

    uint8_t* metallic_data = (uint8_t*)calloc(tex_width * tex_height * tex_channels, sizeof(uint8_t));
    for (int i = 0; i < tex_width * tex_height; ++i)
    {
      memcpy(metallic_data + i * tex_channels, &grey, sizeof(grey));
    }

    name = s_instance->m_default_texture_name_map.at(EspTextureType::METALLIC);
    s_instance->m_texture_map.insert(
        { name,
          EspTexture::create(name,
                             std::move(std::make_unique<ImageResource>(fs::path(name),
                                                                       std::unique_ptr<uint8_t[]>(metallic_data),
                                                                       tex_channels,
                                                                       tex_width,
                                                                       tex_height))) });

    uint8_t* roughness_data = (uint8_t*)calloc(tex_width * tex_height * tex_channels, sizeof(uint8_t));
    for (int i = 0; i < tex_width * tex_height; ++i)
    {
      memcpy(roughness_data + i * tex_channels, &grey, sizeof(grey));
    }

    name = s_instance->m_default_texture_name_map.at(EspTextureType::ROUGHNESS);
    s_instance->m_texture_map.insert(
        { name,
          EspTexture::create(name,
                             std::move(std::make_unique<ImageResource>(fs::path(name),
                                                                       std::unique_ptr<uint8_t[]>(roughness_data),
                                                                       tex_channels,
                                                                       tex_width,
                                                                       tex_height))) });

    uint8_t* ao_data = (uint8_t*)calloc(tex_width * tex_height * tex_channels, sizeof(uint8_t));
    for (int i = 0; i < tex_width * tex_height; ++i)
    {
      memcpy(ao_data + i * tex_channels, &white, sizeof(white));
    }

    name = s_instance->m_default_texture_name_map.at(EspTextureType::AO);
    s_instance->m_texture_map.insert(
        { name,
          EspTexture::create(name,
                             std::move(std::make_unique<ImageResource>(fs::path(name),
                                                                       std::unique_ptr<uint8_t[]>(ao_data),
                                                                       tex_channels,
                                                                       tex_width,
                                                                       tex_height))) });
  }

  bool TextureSystem::texture_matching_params(std::shared_ptr<EspTexture> texture, const TextureParams& params)
  {
    return texture->get_type() == params.type;
  }

  bool TextureSystem::is_default_texture_name(const std::string& name)
  {
    return name == s_instance->m_default_texture_name_map.at(EspTextureType::ALBEDO) ||
        name == s_instance->m_default_texture_name_map.at(EspTextureType::NORMAL) ||
        name == s_instance->m_default_texture_name_map.at(EspTextureType::METALLIC) ||
        name == s_instance->m_default_texture_name_map.at(EspTextureType::ROUGHNESS) ||
        name == s_instance->m_default_texture_name_map.at(EspTextureType::AO);
  }

} // namespace esp
