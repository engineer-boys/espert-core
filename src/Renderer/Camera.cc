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
    m_projection_mat = glm::orthoLH(left, right, bottom, top, near, far);
  }

  void renderer::Camera::set_perspective(float fov,
                                         float aspect_ratio,
                                         float near,
                                         float far)
  {
    ESP_ASSERT(glm::abs(aspect_ratio - std::numeric_limits<float>::epsilon()) >
                   0.0f,
               "")

    m_projection_mat = glm::perspective(fov, aspect_ratio, near, far);
    m_projection_mat[2][2] *= -1;
    m_projection_mat[2][3] *= -1;
  }

  void renderer::Camera::set_perspective(float aspect_ratio)
  {
    set_perspective(m_fov, aspect_ratio, m_near, m_far);
  }
} // namespace esp