#ifndef ESPERT_CORE_SYSTEMS_MATERIALSYSTEM_HH
#define ESPERT_CORE_SYSTEMS_MATERIALSYSTEM_HH

#include "esppch.hh"

#include "Core/RenderAPI/Resources/EspShader.hh"
#include "Core/RenderAPI/Resources/EspTexture.hh"
#include "Core/RenderAPI/Uniforms/EspUniformManager.hh"
#include "Core/Resources/Systems/ShaderSystem.hh"

namespace esp
{
  using MaterialTexutresMap = std::unordered_map<EspTextureType, std::shared_ptr<EspTexture>>;
  class Material
  {
   private:
    MaterialTexutresMap m_textures_map;
    std::shared_ptr<EspShader> m_shader;
    std::unique_ptr<EspUniformManager> m_uniform_manager;
    std::string m_name;

   public:
    Material(MaterialTexutresMap textures, std::shared_ptr<EspShader> shader);
    Material(const std::string& name, MaterialTexutresMap textures, std::shared_ptr<EspShader> shader);
    ~Material() = default;
    PREVENT_COPY(Material);
    Material& update_buffer_uniform(uint32_t set, uint32_t binding, uint64_t offset, uint32_t size, void* data);
    Material& attach();
  };

  using MaterialByTextureMap = std::unordered_map<std::vector<std::shared_ptr<EspTexture>>, std::shared_ptr<Material>>;
  using MaterialByNameMap    = std::unordered_map<std::string, std::shared_ptr<Material>>;

  class MaterialSystem
  {
   private:
    static MaterialSystem* s_instance;

   private:
    MaterialByTextureMap m_material_by_texture_map;
    MaterialByNameMap m_material_by_name_map;

    MaterialSystem();
    static void fill_default_textures(MaterialTexutresMap& textures);
    static std::shared_ptr<Material> create_material(
        const std::string& name,
        std::vector<std::shared_ptr<EspTexture>> textures,
        std::shared_ptr<EspShader> shader = ShaderSystem::get_default_shader());
    static std::shared_ptr<Material> create_material(
        std::vector<std::shared_ptr<EspTexture>> textures,
        std::shared_ptr<EspShader> shader = ShaderSystem::get_default_shader());

   public:
    ~MaterialSystem();
    PREVENT_COPY(MaterialSystem);

    static std::unique_ptr<MaterialSystem> create();
    void terminate();

    static std::shared_ptr<Material> acquire(const std::string& name);
    static std::shared_ptr<Material> acquire(std::vector<std::shared_ptr<EspTexture>> textures,
                                             std::shared_ptr<EspShader> shader = ShaderSystem::get_default_shader());
    static std::shared_ptr<Material> acquire(const std::string& name,
                                             std::vector<std::shared_ptr<EspTexture>> textures,
                                             std::shared_ptr<EspShader> shader = ShaderSystem::get_default_shader());
    static void release(const std::string& name);
    static void release(std::vector<std::shared_ptr<EspTexture>> textures);
    static void release(const std::string& name, std::vector<std::shared_ptr<EspTexture>> textures);
  };
} // namespace esp

#endif // ESPERT_CORE_SYSTEMS_MATERIALSYSTEM_HH
