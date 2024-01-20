#ifndef RENDERER_NMODEL_NMODEL_PARAMS_HH
#define RENDERER_NMODEL_NMODEL_PARAMS_HH

#include "esppch.hh"

#include "Core/RenderAPI/Pipeline/EspPipelineBuilder.hh"

#include "Core/Renderer/Model3/Mesh/NVertex.hh"

namespace esp
{
  struct NModelMaterialParams
  {
    bool m_albedo    = false;
    bool m_normal    = false;
    bool m_metallic  = false;
    bool m_roughness = false;
    bool m_ao        = false;
  };

  struct NModelParams
  {
    /// @brief vec3
    bool m_position = false;

    /// @brief vec3
    bool m_color = false;

    /// @brief vec3
    bool m_normal = false;

    /// @brief vec2
    bool m_tex_coord = false;

    /// @brief ivec4
    bool m_bone_ids = false;

    /// @brief vec4
    bool m_weights = false;

    /// @brief vec3
    bool m_tangent = false;

    NModelMaterialParams m_material_params = {};

    EspVertexLayout get_vertex_layouts(bool instancing = false) const;

    void parse_to_vertex_byte_buffer(std::vector<NVertex>& vertex_buffer,
                                     std::vector<uint8_t>& vertex_byte_buffer) const;
  };
} // namespace esp

#endif // RENDERER_NMODEL_NMODEL_PARAMS_HH
