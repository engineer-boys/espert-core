#include "Camera.hh"

namespace esp
{
  Camera* Camera::s_current_camera = nullptr;

  //  void Camera::look_at(const glm::vec3& eye, const glm::vec3& center, const glm::vec3& up)
  //  {
  //    m_view_mat = glm::lookAt(eye, center, up);
  //  }

  void Camera::set_orthographic(float left, float right, float top, float bottom, float near, float far)
  {
    m_projection_mat = glm::orthoLH(left, right, bottom, top, near, far);
  }

  void Camera::set_perspective(float fov, float aspect_ratio, float near, float far)
  {
    ESP_ASSERT(glm::abs(aspect_ratio - std::numeric_limits<float>::epsilon()) > 0.0f, "")

    m_projection_mat = glm::perspective(fov, aspect_ratio, near, far);
  }

  void Camera::set_perspective(float aspect_ratio) { set_perspective(m_fov, aspect_ratio, m_near, m_far); }

  void Camera::look_at(glm::vec3 target)
  {
    m_front = glm::normalize(target - m_position);
    update_camera_up();
  }

  void Camera::update_camera_up()
  {
    glm::vec3 up           = { 0.f, -1.f, 0.f };
    glm::vec3 camera_right = glm::normalize(glm::cross(up, m_front));
    m_camera_up            = glm::cross(m_front, camera_right);
  }
} // namespace esp