#include "Camera.hh"

namespace esp
{
  void Camera::set_perspective(float fov, float aspect_ratio, float near_plane, float far_plane)
  {
    ESP_ASSERT(glm::abs(aspect_ratio - std::numeric_limits<float>::epsilon()) > 0.0f, "")

    // m_projection_mat = glm::perspective(fov, aspect_ratio, near_plane, far_plane);

    // OpenGL definition (RH_N0)
    m_projection_mat       = glm::mat4(1.f);
    float tan_half_fov     = tan(fov / 2.f);
    m_projection_mat[0][0] = 1.f / (aspect_ratio * tan_half_fov);
    m_projection_mat[1][1] = 1.f / tan_half_fov;
    m_projection_mat[2][2] = -(far_plane + near_plane) / (far_plane - near_plane);
    m_projection_mat[3][2] = -(2 * far_plane * near_plane) / (far_plane - near_plane);
    m_projection_mat[2][3] = -1;
  }

  void Camera::set_perspective(float aspect_ratio) { set_perspective(m_fov, aspect_ratio, m_near, m_far); }
} // namespace esp