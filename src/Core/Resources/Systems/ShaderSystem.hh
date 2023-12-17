#ifndef ESPERT_CORE_SYSTEMS_SHADERSYSTEM_HH
#define ESPERT_CORE_SYSTEMS_SHADERSYSTEM_HH

#include "esppch.hh"

#include "Core/RenderAPI/Resources/EspShader.hh"

namespace esp
{
  using ShaderMap = std::unordered_map<std::string, std::shared_ptr<EspShader>>;

  class ShaderSystem
  {
   private:
    static ShaderSystem* s_instance;

   private:
    ShaderMap m_shader_map;
    std::string m_default_shader_name = "default";

    ShaderSystem();

    static std::shared_ptr<EspShader> load(const std::string& name);

   public:
    ~ShaderSystem();
    PREVENT_COPY(ShaderSystem);

    static std::unique_ptr<ShaderSystem> create();
    void terminate();

    static std::shared_ptr<EspShader> acquire(const std::string& name);
    static void release(const std::string& name);
    static std::shared_ptr<EspShader> get_default_shader();
  };
} // namespace esp

#endif // ESPERT_CORE_SYSTEMS_SHADERSYSTEM_HH