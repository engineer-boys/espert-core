#include "MaterialSystem.hh"

namespace esp
{

  std::shared_ptr<Material> Material::create(EspPipeline& pipeline, std::vector<std::shared_ptr<EspTexture>>& textures)
  {
    return std::shared_ptr<Material>(new Material(pipeline, textures));
  }

  void Material::attach() { m_material_manager->attach(); }

  Material::Material(EspPipeline& pipeline, std::vector<std::shared_ptr<EspTexture>>& textures)
  {
    m_material_manager = pipeline.create_uniform_manager(1, 1);

    std::shared_ptr<EspTexture> albedo_texture, normal_texture, metallic_texture, roughness_texture, ao_texture;

    for (auto& texture : textures)
    {
      if (texture->get_name().find("albedo") != std::string::npos) { albedo_texture = texture; }
      if (texture->get_name().find("normal") != std::string::npos) { normal_texture = texture; }
      if (texture->get_name().find("metallic") != std::string::npos) { metallic_texture = texture; }
      if (texture->get_name().find("roughness") != std::string::npos) { roughness_texture = texture; }
      if (texture->get_name().find("ao") != std::string::npos) { ao_texture = texture; }
    }

    if (albedo_texture == nullptr) { albedo_texture = TextureSystem::get_default_albedo_texture(); }
    if (normal_texture == nullptr) { normal_texture = TextureSystem::get_default_normal_texture(); }
    if (metallic_texture == nullptr) { metallic_texture = TextureSystem::get_default_metallic_texture(); }
    if (roughness_texture == nullptr) { roughness_texture = TextureSystem::get_default_roughness_texture(); }
    if (ao_texture == nullptr) { ao_texture = TextureSystem::get_default_ao_texture(); }

    // TODO: it's kind of a constraint to hard code set's and binding's value
    // but imo it's fine to say to user that 'set 1 and bindings 0-4 are meant for textures'.
    // We can work around it obviously so I leave it here to decide:)
    m_material_manager->load_texture(1, 0, albedo_texture);
    m_material_manager->load_texture(1, 1, normal_texture);
    m_material_manager->load_texture(1, 2, metallic_texture);
    m_material_manager->load_texture(1, 3, roughness_texture);
    m_material_manager->load_texture(1, 4, ao_texture);

    m_material_manager->build();
  }
} // namespace esp