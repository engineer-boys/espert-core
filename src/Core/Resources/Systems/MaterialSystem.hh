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

  /// @brief Equals operator needed to use MaterialTextureLayout as map key.
  /// @param a First object to sompare.
  /// @param b Second object to sompare.
  /// @return True if objects are equal. False otherwise.
  inline bool operator==(const MaterialTextureLayout& a, const MaterialTextureLayout& b)
  {
    return a.set == b.set && a.binding == b.binding && a.type == b.type;
  }

  using MaterialTexutresMap = std::unordered_map<EspTextureType, std::shared_ptr<EspTexture>>;

  /// @brief Collection of one EspShader and many EspTextures. (at most one EspTexture of each EspTextureType)
  class Material
  {
   private:
    MaterialTexutresMap m_textures_map;
    std::vector<MaterialTextureLayout> m_material_texture_layouts;
    std::string m_name;

   public:
    /// @brief Constructor without material name (cannot be acquired by name). Sets up underlying EspUniformManager.
    /// @param textures Map of textures to use in material.
    /// @param layouts Information on textures' sets and bindings inside shader.
    Material(MaterialTexutresMap textures, std::vector<MaterialTextureLayout> layouts);

    /// @brief Constructor with material name (can be acquired by name). Sets up underlying EspUniformManager.
    /// @param textures Map of textures to use in material.
    /// @param layouts Information on textures' sets and bindings inside shader.
    Material(const std::string& name, MaterialTexutresMap textures, std::vector<MaterialTextureLayout> layouts);

    /// @brief Default destructor.
    ~Material() = default;

    PREVENT_COPY(Material);

    /// @brief Create EspUniformManager based on the internal texture configuration and EspShader.
    /// @param shader Shader for which the EspUniformManager will be created.
    /// @return Unique pointer to created EspUniformManager.
    std::unique_ptr<EspUniformManager> create_uniform_manager(std::shared_ptr<EspShader> shader) const;

    /// @brief Equals operator needed to use Material as map key.
    /// @param other The other Material to compare to.
    /// @return True if materials are the same.
    inline bool operator==(const Material& other)
    {
      return m_name == other.m_name && m_material_texture_layouts == other.m_material_texture_layouts &&
          m_textures_map == other.m_textures_map;
    }
    /// @brief Returns MaterialTexutresMap.
    /// @return MaterialTexutresMap.
    inline const MaterialTexutresMap& get_material_textures_map() const { return m_textures_map; }
    /// @brief Returns vector of MaterialTextureLayout.
    /// @return Vector of MaterialTextureLayout.
    inline const std::vector<MaterialTextureLayout>& get_material_texture_layouts() const
    {
      return m_material_texture_layouts;
    }
    /// @brief Returns material's name.
    /// @return Material's name.
    inline const std::string get_name() const { return m_name; }
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
                                                     std::vector<MaterialTextureLayout> layouts);
    static std::shared_ptr<Material> create_material(std::vector<std::shared_ptr<EspTexture>> textures,
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
    /// @param layouts Layout of textures in shader.
    /// @return Shared pointer to Material.
    static std::shared_ptr<Material> acquire(std::vector<std::shared_ptr<EspTexture>> textures,
                                             std::vector<MaterialTextureLayout> layouts = {
                                                 { 1, 0, EspTextureType::ALBEDO },
                                                 { 1, 1, EspTextureType::NORMAL },
                                                 { 1, 2, EspTextureType::METALLIC },
                                                 { 1, 3, EspTextureType::ROUGHNESS },
                                                 { 1, 4, EspTextureType::AO } });

    /// @brief Returns Material with matching name and configuration. If no such material is found then creates it.
    /// @param name Name of Material.
    /// @param textures Vector for textures to use.
    /// @param layouts Layout of textures in shader.
    /// @return Shared pointer to Material.
    static std::shared_ptr<Material> acquire(const std::string& name,
                                             std::vector<std::shared_ptr<EspTexture>> textures,
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

template<> struct std::hash<esp::MaterialTextureLayout>
{
  std::size_t operator()(const esp::MaterialTextureLayout& k) const
  {
    std::size_t seed = 0;
    esp::hash_combine(seed, k.binding);
    esp::hash_combine(seed, k.set);
    esp::hash_combine(seed, k.type);
    return seed;
  }
};

template<> struct std::hash<std::vector<esp::MaterialTextureLayout>>
{
  std::size_t operator()(const std::vector<esp::MaterialTextureLayout>& k) const
  {
    std::size_t seed = 0;
    for (const auto& e : k)
    {
      esp::hash_combine(seed, e);
    }
    return seed;
  }
};

template<> struct std::hash<esp::MaterialTexutresMap>
{
  std::size_t operator()(const esp::MaterialTexutresMap& k) const
  {
    std::size_t seed = 0;
    for (const auto& e : k)
    {
      esp::hash_combine(seed, e.first);
      esp::hash_combine(seed, e.second);
    }
    return seed;
  }
};

template<> struct std::hash<esp::Material>
{
  std::size_t operator()(const esp::Material& k) const
  {
    std::size_t seed = 0;
    esp::hash_combine(seed, k.get_name());
    esp::hash_combine(seed, k.get_material_textures_map());
    esp::hash_combine(seed, k.get_material_texture_layouts());
    return seed;
  }
};

template<> struct std::hash<std::shared_ptr<esp::Material>>
{
  std::size_t operator()(const std::shared_ptr<esp::Material>& k) const
  {
    std::size_t seed = 0;
    esp::hash_combine(seed, *k);
    return seed;
  }
};

#endif // ESPERT_CORE_SYSTEMS_MATERIALSYSTEM_HH
