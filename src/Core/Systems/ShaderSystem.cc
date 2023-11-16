#include "Core/Systems/ShaderSystem.hh"

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
    ShaderSystem::s_instance = nullptr;
    ESP_CORE_TRACE("Shader system shutdown.");
  }

  std::unique_ptr<ShaderSystem> ShaderSystem::create(const fs::path& shader_base_path)
  {
    auto shader_system = std::unique_ptr<ShaderSystem>(new ShaderSystem());

    shader_system->m_shader_base_path = shader_base_path;

    ESP_CORE_TRACE("Shader system initialized with base path {}.", shader_base_path.string());

    return shader_system;
  }
} // namespace esp