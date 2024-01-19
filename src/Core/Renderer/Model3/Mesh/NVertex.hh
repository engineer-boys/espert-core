#ifndef CORE_RENDERER_MESH_NVERTEX_HH
#define CORE_RENDERER_MESH_NVERTEX_HH

#include "esppch.hh"

#include "Core/RenderAPI/Pipeline/EspPipelineBuilder.hh"

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

  struct NModelParams
  {
    /// @brief vec3
    bool m_position;

    /// @brief vec3
    bool m_color;

    /// @brief vec3
    bool m_normal;

    /// @brief vec2
    bool m_tex_coord;

    /// @brief ivec4
    bool m_bone_ids;

    /// @brief vec4
    bool m_weights;

    /// @brief vec3
    bool m_tangent;

    EspVertexLayout get_vertex_layouts(bool instancing = false) const;

    void parse_to_vertex_byte_buffer(std::vector<NVertex>& vertex_buffer,
                                     std::vector<uint8_t>& vertex_byte_buffer) const;
  };

} // namespace esp

#endif // CORE_RENDERER_MESH_NVERTEX_HH