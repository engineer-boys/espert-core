#ifndef CORE_RENDERER_MODEL_MESH_VERTEX_HH
#define CORE_RENDERER_MODEL_MESH_VERTEX_HH

#include "esppch.hh"

namespace esp
{
  struct Vertex
  {
    glm::vec3 m_position{};
    glm::vec3 m_color{};
    glm::vec3 m_normal{};
    glm::vec2 m_tex_coord{};

    glm::ivec4 m_bone_ids{};
    glm::vec4 m_weights{};

    glm::vec3 m_tangent{};

    inline static uint32_t size_of_position() { return static_cast<uint32_t>(sizeof(glm::vec3)); }
    inline static uint32_t size_of_color() { return static_cast<uint32_t>(sizeof(glm::vec3)); }
    inline static uint32_t size_of_normal() { return static_cast<uint32_t>(sizeof(glm::vec3)); }
    inline static uint32_t size_of_tex_coord() { return static_cast<uint32_t>(sizeof(glm::vec2)); }
    inline static uint32_t size_of_bone_ids() { return static_cast<uint32_t>(sizeof(glm::ivec4)); }
    inline static uint32_t size_of_weights() { return static_cast<uint32_t>(sizeof(glm::vec4)); }
    inline static uint32_t size_of_tangent() { return static_cast<uint32_t>(sizeof(glm::vec3)); }
  };
} // namespace esp

#endif // CORE_RENDERER_MODEL_MESH_VERTEX_HH
