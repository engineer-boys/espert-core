#include "Core/Resources/Systems/ShaderSystem.hh"
#include "Core/Resources/ResourceTypes.hh"
#include "Core/Resources/Systems/ResourceSystem.hh"

namespace esp
{
  ShaderSystem* ShaderSystem::s_instance = nullptr;

  ShaderSystem::ShaderSystem()
  {
    if (ShaderSystem::s_instance != nullptr) { throw std::runtime_error("The shader system instance already exists!"); }

    ShaderSystem::s_instance = this;
  }

  ShaderSystem::~ShaderSystem()
  {
    if (s_instance) { terminate(); }
  }

  std::unique_ptr<ShaderSystem> ShaderSystem::create()
  {
    auto shader_system = std::unique_ptr<ShaderSystem>(new ShaderSystem());

    load(s_instance->m_default_shader_name, {});

    ESP_CORE_TRACE("Shader system initialized.");

    return shader_system;
  }

  void ShaderSystem::terminate()
  {
    ShaderSystem::s_instance = nullptr;
    ShaderMap empty_map      = {};
    m_shader_map.swap(empty_map);
    ESP_CORE_TRACE("Shader system shutdown.");
  }

  std::shared_ptr<EspShader> ShaderSystem::acquire(const std::string& name, const EspShaderConfig& config)
  {
    auto key = std::make_pair(name, config);
    if (s_instance->m_shader_map.contains(key)) { return s_instance->m_shader_map.at(key); }
    return load(name, config);
  }

  void ShaderSystem::release(const std::string& name, const EspShaderConfig& config)
  {
    auto key = std::make_pair(name, config);
    if (!s_instance->m_shader_map.contains(key))
    {
      ESP_CORE_ERROR("Cannot release shader {}. Not present in map.", name);
    }

    if (name == s_instance->m_default_shader_name)
    {
      ESP_CORE_WARN("Cannot release a default shader.");
      return;
    }

    s_instance->m_shader_map.erase(s_instance->m_shader_map.find(key));
    ESP_CORE_TRACE("Released shader {}.", name);
  }

  std::shared_ptr<EspShader> ShaderSystem::load(const std::string& name, const EspShaderConfig& config)
  {
    SpirvResourceParams params;
    auto resource = ResourceSystem::load<SpirvResource>(name, params);
    if (!resource)
    {
      ESP_CORE_ERROR("Could not load shader {}.", name);
      return get_default_shader();
    }
    auto spirv_resource = unique_cast<SpirvResource>(std::move(resource));
    auto shader         = EspShader::create(name, std::move(spirv_resource), config);
    s_instance->m_shader_map.insert({ std::make_pair(name, config), shader });
    ESP_CORE_TRACE("Loaded shader {}.", name);
    return shader;
  }

  std::shared_ptr<EspShader> ShaderSystem::get_default_shader()
  {
    return s_instance->m_shader_map.at(std::make_pair(s_instance->m_default_shader_name, EspShaderConfig()));
  }
} // namespace esp
