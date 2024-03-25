#include "FpsCamera.hh"
#include "Core/RenderAPI/Work/EspWorkOrchestrator.hh"

namespace esp
{
  void FpsCamera::zoom(float dd, float dt)
  {
    m_fov = std::clamp(m_fov + dd * dt / (2.f * m_sensitivity), glm::radians(1.f), ESP_PI / 4);
    set_perspective(EspWorkOrchestrator::get_swap_chain_extent_aspect_ratio());
  }

  void FpsCamera::move(MoveDirection direction, float dt)
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

  void FpsCamera::look_at(glm::vec3 target)
  {
    m_camera_front = esp::normalize(m_position - target);

    m_pitch = std::clamp(glm::acos(glm::dot(m_camera_front, S_UP)) - ESP_PI / 2, -S_PITCH_TRESHOLD, S_PITCH_TRESHOLD);

    glm::vec3 front_xz = esp::normalize(glm::vec3{ m_camera_front.x, 0.f, m_camera_front.z });
    m_jaw              = m_camera_front.x < 0 ? ESP_PI / 2 - glm::acos(glm::dot(front_xz, S_FRONT))
                                              : ESP_PI / 2 + glm::acos(glm::dot(front_xz, S_FRONT));

    update_camera_up();
  }

  void FpsCamera::look_at(float dx, float dy, float dt)
  {
    float offset_x = dx * m_sensitivity * dt;
    float offset_y = dy * m_sensitivity * dt;

    m_jaw += glm::radians(offset_x);
    m_pitch = std::clamp(m_pitch + glm::radians(offset_y), -S_PITCH_TRESHOLD, S_PITCH_TRESHOLD);

    glm::vec3 direction;
    direction.x = cos(m_jaw) * cos(m_pitch);
    direction.y = sin(m_pitch);
    direction.z = sin(m_jaw) * cos(m_pitch);

    m_camera_front = esp::normalize(direction);

    update_camera_up();
  }

  void FpsCamera::update_camera_up()
  {
    glm::vec3 camera_right = esp::normalize(glm::cross(S_UP, m_camera_front));
    m_camera_up            = glm::cross(m_camera_front, camera_right);
  }
} // namespace esp