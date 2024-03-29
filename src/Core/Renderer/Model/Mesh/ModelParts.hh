#ifndef CORE_RENDERER_MODEL_MODEL_PARTS_HH
#define CORE_RENDERER_MODEL_MODEL_PARTS_HH

#include "esppch.hh"

#include "Core/Resources/Systems/MaterialSystem.hh"

namespace esp
{
  struct ModelNode
  {
    bool m_has_meshes;

    std::string m_name;
    glm::mat4 m_transformation;
    glm::mat4 m_precomputed_transformation;

    std::vector<uint32_t> m_meshes;
    std::vector<uint32_t> m_children;
  };

  struct Mesh
  {
    uint32_t m_first_index;
    uint32_t m_index_count;

    std::shared_ptr<Material> m_material;
  };
} // namespace esp

#endif // CORE_RENDERER_MODEL_MODEL_PARTS_HH
