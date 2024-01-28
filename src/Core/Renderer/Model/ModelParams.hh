#ifndef RENDERER_MODEL_MODEL_PARAMS_HH
#define RENDERER_MODEL_MODEL_PARAMS_HH

#include "esppch.hh"

#include "Core/RenderAPI/Worker/EspWorkerBuilder.hh"

#include "Core/Renderer/Model/Mesh/Vertex.hh"

#include "Core/Resources/Systems/MaterialSystem.hh"

namespace esp
{
  enum EspPostProcessSteps
  {
    EspProcessDefault   = 0x0,
    EspProcessFlipUVs   = 0x800000,
    EspCaclTangentSpace = 0x1,
  };

  struct ModelParams
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

    std::vector<MaterialTextureLayout> m_material_texture_layout = {};

    uint32_t m_load_process_flags = EspPostProcessSteps::EspProcessDefault;

    EspVertexLayout get_vertex_layouts(bool instancing = false) const;

    void parse_to_vertex_byte_buffer(std::vector<Vertex>& vertex_buffer,
                                     std::vector<uint8_t>& vertex_byte_buffer) const;
  };
} // namespace esp

#endif // RENDERER_MODEL_MODEL_PARAMS_HH
