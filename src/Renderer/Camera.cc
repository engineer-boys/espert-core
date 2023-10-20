#include "Camera.hh"

namespace esp
{
  void renderer::Camera::set_orthographic(float left,
                                          float right,
                                          float top,
                                          float bottom,
                                          float near,
                                          float far)
  {
    m_projection_mat       = glm::mat4{ 1.0f };
    m_projection_mat[0][0] = 2.f / (right - left);
    m_projection_mat[1][1] = 2.f / (bottom - top);
    m_projection_mat[2][2] = 1.f / (far - near);
    m_projection_mat[3][0] = -(right + left) / (right - left);
    m_projection_mat[3][1] = -(bottom + top) / (bottom - top);
    m_projection_mat[3][2] = -near / (far - near);
  }

  void renderer::Camera::set_perspective(float fov,
                                         float aspect_ratio,
                                         float near,
                                         float far)
  {
    assert(glm::abs(aspect_ratio - std::numeric_limits<float>::epsilon()) >
           0.0f);
    const float tan_half_fov_y = tan(fov / 2.f);
    m_projection_mat           = glm::mat4{ 0.0f };
    m_projection_mat[0][0]     = 1.f / (aspect_ratio * tan_half_fov_y);
    m_projection_mat[1][1]     = 1.f / (tan_half_fov_y);
    m_projection_mat[2][2]     = far / (far - near);
    m_projection_mat[2][3]     = 1.f;
    m_projection_mat[3][2]     = -(far * near) / (far - near);
  }

  void renderer::Camera::set_perspective(float aspect_ratio)
  {
    set_perspective(m_fov, aspect_ratio, m_near, m_far);
  }
} // namespace esp