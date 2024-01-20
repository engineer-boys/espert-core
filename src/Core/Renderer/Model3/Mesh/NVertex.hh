#ifndef CORE_RENDERER_MESH_NVERTEX_HH
#define CORE_RENDERER_MESH_NVERTEX_HH

#include "esppch.hh"

namespace esp
{
  struct NVertex
  {
    glm::vec3 m_position{};
    glm::vec3 m_color{};
    glm::vec3 m_normal{};
    glm::vec2 m_tex_coord{};

    glm::ivec4 m_bone_ids{};
    glm::vec4 m_weights{};

    glm::vec3 m_tangent{};
  };
} // namespace esp

#endif // CORE_RENDERER_MESH_NVERTEX_HH
