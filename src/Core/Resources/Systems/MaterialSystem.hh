#ifndef ESPERT_CORE_SYSTEMS_MATERIALSYSTEM_HH
#define ESPERT_CORE_SYSTEMS_MATERIALSYSTEM_HH

#include "esppch.hh"

#include "Core/RenderAPI/Resources/EspShader.hh"
#include "Core/RenderAPI/Resources/EspTexture.hh"
#include "Core/RenderAPI/Uniforms/EspUniformManager.hh"
#include "Core/Resources/Systems/ShaderSystem.hh"

namespace esp
{
  /// @brief Describes texture entry in shader.
  struct MaterialTextureLayout
  {
    /// @brief Number of descriptor set.
    uint32_t set;
    /// @brief Number of binding.
    uint32_t binding;
    /// @brief Type of texture.
    EspTextureType type;
  };

  using MaterialTexutresMap = std::unordered_map<EspTextureType, std::shared_ptr<EspTexture>>;

  /// @brief Collection of one EspShader and many EspTextures. (at most one EspTexture of each EspTextureType)
  class Material
  {
   private:
    MaterialTexutresMap m_textures_map;
    std::shared_ptr<EspShader> m_shader;
    std::unique_ptr<EspUniformManager> m_uniform_manager;
    std::string m_name;

   public:
    /// @brief Constructor without material name (cannot be acquired by name). Sets up underlying EspUniformManager.
    /// @param textures Map of textures to use in material.
    /// @param shader Shader used to draw this material.
    /// @param layouts Information on textures' sets and bindings inside shader.
    Material(MaterialTexutresMap textures,
             std::shared_ptr<EspShader> shader,
             std::vector<MaterialTextureLayout> layouts);

    /// @brief Constructor with material name (can be acquired by name). Sets up underlying EspUniformManager.
    /// @param textures Map of textures to use in material.
    /// @param shader Shader used to draw this material.
    /// @param layouts Information on textures' sets and bindings inside shader.
    Material(const std::string& name,
             MaterialTexutresMap textures,
             std::shared_ptr<EspShader> shader,
             std::vector<MaterialTextureLayout> layouts);

    /// @brief Default destructor.
    ~Material() = default;

    PREVENT_COPY(Material);

    /// @brief Update uniform buffer of underlying EspUniformManager.
    /// @param set Number of descriptor set.
    /// @param binding Number of binding.
    /// @param offset Offset in uniform buffer.
    /// @param size Size of data to update.
    /// @param data Pointer to data.
    /// @return Reference to material.
    Material& update_buffer_uniform(uint32_t set, uint32_t binding, uint64_t offset, uint32_t size, void* data);

    /// @brief Attach material to be used in rendering process.
    /// @return Reference to material.
    Material& attach();
  };

  using MaterialByTextureMap = std::unordered_map<std::vector<std::shared_ptr<EspTexture>>, std::shared_ptr<Material>>;
  using MaterialByNameMap    = std::unordered_map<std::string, std::shared_ptr<Material>>;

  /// @brief System responsible for creation and caching materials.
  class MaterialSystem
  {
   private:
    static MaterialSystem* s_instance;

   private:
    MaterialByTextureMap m_material_by_texture_map;
    MaterialByNameMap m_material_by_name_map;

    MaterialSystem();
    static void fill_default_textures(MaterialTexutresMap& textures);
    static std::shared_ptr<Material> create_material(const std::string& name,
                                                     std::vector<std::shared_ptr<EspTexture>> textures,
                                                     std::shared_ptr<EspShader> shader,
                                                     std::vector<MaterialTextureLayout> layouts);
    static std::shared_ptr<Material> create_material(std::vector<std::shared_ptr<EspTexture>> textures,
                                                     std::shared_ptr<EspShader> shader,
                                                     std::vector<MaterialTextureLayout> layouts);

   public:
    /// @brief Terminates MaterialSystem.
    ~MaterialSystem();

    PREVENT_COPY(MaterialSystem);

    /// @brief Creates MaterialSystem singleton instance.
    /// @return Unique pointer to MaterialSystem instance.
    static std::unique_ptr<MaterialSystem> create();

    /// @brief Destorys MaterialSystem instance and removes references to all loaded materials.
    void terminate();

    /// @brief Returns Material with matching name.
    /// @param name Name of material to get.
    /// @return Shared pointer to Material with matching name or nullptr if no such Material exists.
    static std::shared_ptr<Material> acquire(const std::string& name);

    /// @brief Returns Material with matching configuration. If no such material is found then creates it. It cannot be
    /// referenced by name.
    /// @param textures Vector for textures to use.
    /// @param shader Shader that Material will be using.
    /// @param layouts Layout of textures in shader.
    /// @return Shared pointer to Material.
    static std::shared_ptr<Material> acquire(std::vector<std::shared_ptr<EspTexture>> textures,
                                             std::shared_ptr<EspShader> shader = ShaderSystem::get_default_shader(),
                                             std::vector<MaterialTextureLayout> layouts = {
                                                 { 1, 0, EspTextureType::ALBEDO },
                                                 { 1, 1, EspTextureType::NORMAL },
                                                 { 1, 2, EspTextureType::METALLIC },
                                                 { 1, 3, EspTextureType::ROUGHNESS },
                                                 { 1, 4, EspTextureType::AO } });

    /// @brief Returns Material with matching name and configuration. If no such material is found then creates it.
    /// @param name Name of Material.
    /// @param textures Vector for textures to use.
    /// @param shader Shader that Material will be using.
    /// @param layouts Layout of textures in shader.
    /// @return Shared pointer to Material.
    static std::shared_ptr<Material> acquire(const std::string& name,
                                             std::vector<std::shared_ptr<EspTexture>> textures,
                                             std::shared_ptr<EspShader> shader = ShaderSystem::get_default_shader(),
                                             std::vector<MaterialTextureLayout> layouts = {
                                                 { 1, 0, EspTextureType::ALBEDO },
                                                 { 1, 1, EspTextureType::NORMAL },
                                                 { 1, 2, EspTextureType::METALLIC },
                                                 { 1, 3, EspTextureType::ROUGHNESS },
                                                 { 1, 4, EspTextureType::AO } });

    /// @brief Remove Material reference from map by name.
    /// @param name Name of Material.
    static void release(const std::string& name);

    /// @brief Remove Material reference from map by vector of textures.
    /// @param textures Vector of textures.
    static void release(std::vector<std::shared_ptr<EspTexture>> textures);

    /// @brief Remove Material reference from map by name and vector of textures.
    /// @param name Name of Material.
    /// @param textures Vector of textures.
    static void release(const std::string& name, std::vector<std::shared_ptr<EspTexture>> textures);
  };
} // namespace esp

#endif // ESPERT_CORE_SYSTEMS_MATERIALSYSTEM_HH
