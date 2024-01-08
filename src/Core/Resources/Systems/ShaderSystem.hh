#ifndef ESPERT_CORE_SYSTEMS_SHADERSYSTEM_HH
#define ESPERT_CORE_SYSTEMS_SHADERSYSTEM_HH

#include "esppch.hh"

#include "Core/RenderAPI/Resources/EspShader.hh"

namespace esp
{
  using ShaderMap = std::unordered_map<std::string, std::shared_ptr<EspShader>>;

  /// @brief System responsible for loading, configuration and caching shaders.
  class ShaderSystem
  {
   private:
    static ShaderSystem* s_instance;

   private:
    ShaderMap m_shader_map;
    std::string m_default_shader_name = "Shaders/default";

    ShaderSystem();

    static std::shared_ptr<EspShader> load(const std::string& name);

   public:
    /// @brief Terminates ShaderSystem.
    ~ShaderSystem();

    PREVENT_COPY(ShaderSystem);

    /// @brief Creates ShaderSystem singleton instance.
    /// @return Unique pointer to ShaderSystem instance.
    static std::unique_ptr<ShaderSystem> create();

    /// @brief Destorys ShaderSystem instance and removes references to all loaded shaders.
    void terminate();

    /// @brief Returns EspShader by name/relative path. If no such shader is found then loads and configures it.
    /// @param name Name/relative path to shader sources (without any extensions).
    /// @return Shared pointer to EspShader.
    static std::shared_ptr<EspShader> acquire(const std::string& name);

    /// @brief Removes reference to shader from internal map.
    /// @param name Name/relative path to shader.
    static void release(const std::string& name);

    /// @brief Returns default shader.
    /// @return Shared pointer to default shader.
    static std::shared_ptr<EspShader> get_default_shader();
  };
} // namespace esp

#endif // ESPERT_CORE_SYSTEMS_SHADERSYSTEM_HH