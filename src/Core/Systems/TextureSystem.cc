#include "Core/Systems/TextureSystem.hh"
#include "Core/Systems/ResourceSystem.hh"

namespace esp
{
  const std::string s_default_texture_name          = "default";
  const std::string s_default_diffuse_texture_name  = "default_DIFF";
  const std::string s_default_specular_texture_name = "default_SPEC";
  const std::string s_default_normal_texture_name   = "default_NORM";
  bool TextureSystem::s_initalized                  = false;

  std::unique_ptr<TextureSystem> TextureSystem::init()
  {
    ESP_ASSERT(!TextureSystem::s_initalized, "Texture system has already been initialized.");

    auto texture_system = std::unique_ptr<TextureSystem>(new TextureSystem());

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

} // namespace esp
