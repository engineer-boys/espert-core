#ifndef CORE_RENDERER_MESH_NMATERIAL_HH
#define CORE_RENDERER_MESH_NMATERIAL_HH

#include "esppch.hh"

#include <optional>

namespace esp
{
  struct NMaterial
  {
    std::optional<int32_t> m_albedo;
    std::optional<int32_t> m_normal;
    std::optional<int32_t> m_metallic;
    std::optional<int32_t> m_roughness;
    std::optional<int32_t> m_ao;

    inline bool is_empty() const
    {
      return !m_albedo.has_value() && !m_normal.has_value() && !m_metallic.has_value() && !m_roughness.has_value() &&
          !m_ao.has_value();
    }
  };
} // namespace esp

#endif // CORE_RENDERER_MESH_NMATERIAL_HH
