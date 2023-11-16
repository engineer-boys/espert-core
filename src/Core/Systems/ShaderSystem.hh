#ifndef ESPERT_CORE_SYSTEMS_SHADERSYSTEM_HH
#define ESPERT_CORE_SYSTEMS_SHADERSYSTEM_HH

#include "esppch.hh"

#include "Core/Resources/ResourceTypes.hh"

class Shader
{
};

namespace esp
{
  class ShaderSystem
  {
   private:
    static ShaderSystem* s_instance;

   private:
    fs::path m_shader_base_path;
    std::unordered_map<std::string, Shader> m_shader_map;

    ShaderSystem();

   public:
    ~ShaderSystem();
    PREVENT_COPY(ShaderSystem);

    static std::unique_ptr<ShaderSystem> create(const fs::path& shader_base_path);

    inline static ShaderSystem* get_instance() { return ShaderSystem::s_instance; }
    inline static const fs::path& get_shader_base_path() { return s_instance->m_shader_base_path; }
  };

} // namespace esp

#endif // ESPERT_CORE_SYSTEMS_SHADERSYSTEM_HH