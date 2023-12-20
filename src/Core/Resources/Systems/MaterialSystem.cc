#include "Core/Resources/Systems/MaterialSystem.hh"

namespace esp
{
  Material::Material(MaterialTexutresMap textures, std::shared_ptr<EspShader> shader) :
      m_name(""), m_textures_map(std::move(textures)), m_shader(shader)
  {
    m_uniform_manager = m_shader->create_uniform_manager();
    m_uniform_manager->load_texture(0, 1, m_textures_map.at(EspTextureType::ALBEDO));
    m_uniform_manager->load_texture(0, 2, m_textures_map.at(EspTextureType::NORMAL));
    m_uniform_manager->load_texture(0, 3, m_textures_map.at(EspTextureType::METALLIC));
    m_uniform_manager->load_texture(0, 4, m_textures_map.at(EspTextureType::ROUGHNESS));
    m_uniform_manager->load_texture(0, 5, m_textures_map.at(EspTextureType::AO));
    m_uniform_manager->build();
  }

  Material::Material(const std::string& name, MaterialTexutresMap textures, std::shared_ptr<EspShader> shader) :
      Material(textures, shader)
  {
    m_name = name;
  }

  Material& Material::update_buffer_uniform(uint32_t set, uint32_t binding, uint64_t offset, uint32_t size, void* data)
  {
    m_uniform_manager->update_buffer_uniform(set, binding, offset, size, data);
    return *this;
  }

  Material& Material::attach()
  {
    m_uniform_manager->attach();
    return *this;
  }

  MaterialSystem* MaterialSystem::s_instance = nullptr;

  MaterialSystem::MaterialSystem() : m_material_by_texture_map({})
  {
    if (MaterialSystem::s_instance != nullptr)
    {
      throw std::runtime_error("The material system instance already exists!");
    }

    MaterialSystem::s_instance = this;
  }

  void MaterialSystem::fill_default_textures(MaterialTexutresMap& textures)
  {
    for (auto type = EspTextureType::ALBEDO; type < EspTextureType::ENUM_END; ++type)
    {
      if (!textures.contains(type)) { textures.insert({ type, TextureSystem::get_default_texture(type) }); }
    }
  }

  MaterialSystem::~MaterialSystem()
  {
    if (s_instance) { terminate(); }
  }

  std::unique_ptr<MaterialSystem> MaterialSystem::create()
  {
    auto material_system = std::unique_ptr<MaterialSystem>(new MaterialSystem());

    ESP_CORE_TRACE("Material system initialized.");

    return material_system;
  }

  void MaterialSystem::terminate()
  {
    MaterialSystem::s_instance       = nullptr;
    MaterialByNameMap empty_name_map = {};
    m_material_by_name_map.swap(empty_name_map);
    MaterialByTextureMap empty_texture_map = {};
    m_material_by_texture_map.swap(empty_texture_map);
    ESP_CORE_TRACE("Material system shutdown.");
  }

  std::shared_ptr<Material> MaterialSystem::acquire(const std::string& name)
  {
    if (s_instance->m_material_by_name_map.contains(name)) return s_instance->m_material_by_name_map.at(name);
    ESP_CORE_ERROR("No material found by name {}.", name);
    return nullptr;
  }

  std::shared_ptr<Material> MaterialSystem::acquire(std::vector<std::shared_ptr<EspTexture>> textures,
                                                    std::shared_ptr<EspShader> shader)
  {
    if (s_instance->m_material_by_texture_map.contains(textures))
      return s_instance->m_material_by_texture_map.at(textures);
    return create_material(std::move(textures), shader);
  }

  std::shared_ptr<Material> MaterialSystem::acquire(const std::string& name,
                                                    std::vector<std::shared_ptr<EspTexture>> textures,
                                                    std::shared_ptr<EspShader> shader)
  {
    if (s_instance->m_material_by_name_map.contains(name)) return s_instance->m_material_by_name_map.at(name);
    if (s_instance->m_material_by_texture_map.contains(textures))
      return s_instance->m_material_by_texture_map.at(textures);
    return create_material(name, std::move(textures), shader);
  }

  std::shared_ptr<Material> MaterialSystem::create_material(const std::string& name,
                                                            std::vector<std::shared_ptr<EspTexture>> textures,
                                                            std::shared_ptr<EspShader> shader)
  {
    MaterialTexutresMap textures_map = {};
    for (auto& texture : textures)
    {
      if (textures_map.contains(texture->get_type()))
      {
        ESP_CORE_ERROR("Cannot create material with more than one texture of same type.");
        return nullptr;
      }
      textures_map.insert({ texture->get_type(), texture });
    }
    fill_default_textures(textures_map);

    auto material = std::shared_ptr<Material>(new Material(name, std::move(textures_map), std::move(shader)));
    s_instance->m_material_by_name_map.insert({ name, material });
    s_instance->m_material_by_texture_map.insert({ textures, material });

    return material;
  }

  std::shared_ptr<Material> MaterialSystem::create_material(std::vector<std::shared_ptr<EspTexture>> textures,
                                                            std::shared_ptr<EspShader> shader)
  {
    MaterialTexutresMap textures_map = {};
    for (auto& texture : textures)
    {
      if (textures_map.contains(texture->get_type()))
      {
        ESP_CORE_ERROR("Cannot create material with more than one texture of same type.");
        return nullptr;
      }
      textures_map.insert({ texture->get_type(), texture });
    }
    fill_default_textures(textures_map);

    auto material = std::shared_ptr<Material>(new Material(std::move(textures_map), std::move(shader)));
    s_instance->m_material_by_texture_map.insert({ textures, material });

    return material;
  }

  void MaterialSystem::release(const std::string& name)
  {
    if (!s_instance->m_material_by_name_map.contains(name))
    {
      ESP_CORE_ERROR("Cannot release material {}. Not present in map.", name);
    }

    s_instance->m_material_by_name_map.erase(s_instance->m_material_by_name_map.find(name));
    ESP_CORE_TRACE("Released material {}.", name);
  }

  void MaterialSystem::release(std::vector<std::shared_ptr<EspTexture>> textures)
  {
    if (!s_instance->m_material_by_texture_map.contains(textures))
    {
      ESP_CORE_ERROR("Cannot release material unnamed. Not present in map.");
    }

    s_instance->m_material_by_texture_map.erase(s_instance->m_material_by_texture_map.find(textures));
    ESP_CORE_TRACE("Released unnamed material.");
  }

  void MaterialSystem::release(const std::string& name, std::vector<std::shared_ptr<EspTexture>> textures)
  {
    release(name);
    release(textures);
  }
} // namespace esp
