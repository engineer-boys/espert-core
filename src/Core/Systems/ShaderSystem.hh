#ifndef ESPERT_CORE_SYSTEMS_SHADERSYSTEM_HH
#define ESPERT_CORE_SYSTEMS_SHADERSYSTEM_HH

#include "esppch.hh"

#include "Core/RenderAPI/Pipeline/EspPipeline.hh"
#include "Core/Resources/ResourceTypes.hh"

namespace esp
{
  class Shader
  {
   private:
    std::shared_ptr<EspPipeline> m_pipeline;

   public:
    Shader(std::unique_ptr<ShaderResource> shader_resource);

    PREVENT_COPY(Shader);

    void attach();
    std::unique_ptr<EspUniformManager> create_uniform_manager();
    inline std::shared_ptr<EspPipeline> get_pipeline() { return m_pipeline; }
  };

  class ShaderSystem
  {
   private:
    static ShaderSystem* s_instance;

   private:
    std::unordered_map<std::string, std::shared_ptr<Shader>> m_shader_map;

    ShaderSystem();

    static std::shared_ptr<Shader> load(const std::string& name);

   public:
    ~ShaderSystem();
    PREVENT_COPY(ShaderSystem);

    static std::unique_ptr<ShaderSystem> create();
    static std::shared_ptr<Shader> acquire(const std::string& name);

    inline static ShaderSystem* get_instance() { return ShaderSystem::s_instance; }
  };

} // namespace esp

#endif // ESPERT_CORE_SYSTEMS_SHADERSYSTEM_HH