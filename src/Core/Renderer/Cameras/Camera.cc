#include <cmath>

#include "Camera.hh"
#include "Core/RenderAPI/Work/EspWorkOrchestrator.hh"

namespace esp
{
  void Camera::set_perspective(float fov, float aspect_ratio, float near_plane, float far_plane)
  {
    ESP_ASSERT(glm::abs(aspect_ratio - std::numeric_limits<float>::epsilon()) > 0.0f, "")

    // OpenGL definition (perspectiveRH_N0)
    m_projection_mat       = glm::mat4(0);
    auto tan_half_fov      = std::tan(fov / 2.f);
    m_projection_mat[0][0] = 1.f / (aspect_ratio * tan_half_fov);
    m_projection_mat[1][1] = 1.f / tan_half_fov;
    m_projection_mat[2][2] = -(far_plane + near_plane) / (far_plane - near_plane);
    m_projection_mat[3][2] = -(2 * far_plane * near_plane) / (far_plane - near_plane);
    m_projection_mat[2][3] = -1;
  }

  void Camera::set_perspective(float aspect_ratio) { set_perspective(m_fov, aspect_ratio, m_near, m_far); }

  void Camera::set_perspective(glm::vec4 clip_plane)
  {
    // OpenGL definition (frustumRH_NO)
    m_projection_mat       = glm::mat4(0);
    m_projection_mat[0][0] = (2 * m_near) / (clip_plane.y - clip_plane.x);
    m_projection_mat[1][1] = (2 * m_near) / (clip_plane.w - clip_plane.z);
    m_projection_mat[2][0] = (clip_plane.y + clip_plane.x) / (clip_plane.y - clip_plane.x);
    m_projection_mat[2][1] = (clip_plane.w + clip_plane.z) / (clip_plane.w - clip_plane.z);
    m_projection_mat[2][2] = -(m_far + m_near) / (m_far - m_near);
    m_projection_mat[2][3] = -1;
    m_projection_mat[3][2] = -(2 * m_far * m_near) / (m_far - m_near);
  }

  void Camera::set_anaglyph_perspective(float eye_distance, float plane_distance, bool left_eye)
  {
    auto frustum_shift  = (eye_distance / 2) * m_near / plane_distance;
    auto left_right_dir = left_eye ? -1.f : 1.f;
    auto aspect_ratio   = EspWorkOrchestrator::get_swap_chain_extent_aspect_ratio();
    auto top            = std::tan(m_fov / 2) * m_near;
    auto bottom         = -top;
    auto left           = -aspect_ratio * top + frustum_shift * left_right_dir;
    auto right          = aspect_ratio * top + frustum_shift * left_right_dir;

    set_perspective({ left, right, bottom, top });
  }
} // namespace esp