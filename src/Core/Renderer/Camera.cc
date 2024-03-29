#include "Camera.hh"

namespace esp
{
  void Camera::set_perspective(float fov, float aspect_ratio, float near_plane, float far_plane)
  {
    ESP_ASSERT(glm::abs(aspect_ratio - std::numeric_limits<float>::epsilon()) > 0.0f, "")

    m_projection_mat = glm::perspective(fov, aspect_ratio, near_plane, far_plane);
  }

  void Camera::set_perspective(float aspect_ratio) { set_perspective(m_fov, aspect_ratio, m_near, m_far); }

  void Camera::move(MoveDirection direction, float dt)
  {
    switch (direction)
    {
    case FORWARD:
      m_position += m_camera_front * m_move_speed * dt;
      break;
    case BACKWARD:
      m_position -= m_camera_front * m_move_speed * dt;
      break;
    case LEFT:
      m_position -= esp::normalize(glm::cross(m_camera_front, m_camera_up)) * m_move_speed * dt;
      break;
    case RIGHT:
      m_position += esp::normalize(glm::cross(m_camera_front, m_camera_up)) * m_move_speed * dt;
      break;
    case UP:
      m_position += m_camera_up * m_move_speed * dt;
      break;
    case DOWN:
      m_position -= m_camera_up * m_move_speed * dt;
      break;
    }
  }

  void Camera::look_at(glm::vec3 target)
  {
    m_camera_front = esp::normalize(m_position - target);

    m_pitch = std::clamp(glm::acos(glm::dot(m_camera_front, S_UP)) - ESP_PI / 2, -S_PITCH_TRESHOLD, S_PITCH_TRESHOLD);

    glm::vec3 front_xz = esp::normalize(glm::vec3{ m_camera_front.x, 0.f, m_camera_front.z });
    m_jaw              = m_camera_front.x < 0 ? ESP_PI / 2 - glm::acos(glm::dot(front_xz, S_FRONT))
                                              : ESP_PI / 2 + glm::acos(glm::dot(front_xz, S_FRONT));

    update_camera_up();
  }

  void Camera::look_at(glm::vec2 mouse_pos, float dt)
  {
    if (m_first_move)
    {
      m_last_move  = mouse_pos;
      m_first_move = false;
      return;
    }

    float offset_x = (mouse_pos.x - m_last_move.x) * m_sensitivity * dt;
    float offset_y = (m_last_move.y - mouse_pos.y) * m_sensitivity * dt;
    m_last_move    = mouse_pos;

    m_jaw += glm::radians(offset_x);
    m_pitch = std::clamp(m_pitch + glm::radians(offset_y), -S_PITCH_TRESHOLD, S_PITCH_TRESHOLD);

    glm::vec3 direction;
    direction.x = cos(m_jaw) * cos(m_pitch);
    direction.y = sin(m_pitch);
    direction.z = sin(m_jaw) * cos(m_pitch);

    m_camera_front = esp::normalize(direction);

    update_camera_up();
  }

  void Camera::update_camera_up()
  {
    glm::vec3 camera_right = esp::normalize(glm::cross(S_UP, m_camera_front));
    m_camera_up            = glm::cross(m_camera_front, camera_right);
  }
} // namespace esp