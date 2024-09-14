#include "FpsCamera.hh"

namespace esp
{
  const glm::vec3 FpsCamera::s_up         = glm::vec3(0.f, 1.f, 0.f);
  const glm::vec3 FpsCamera::s_front      = glm::vec3(0.f, 0.f, -1.f);
  const float FpsCamera::s_pitch_treshold = glm::radians(89.f);

  FpsCamera::FpsCamera(float pitch,
                       float jaw,
                       float move_speed,
                       glm::vec3 position,
                       glm::vec3 camera_front,
                       glm::vec3 camera_up) :
      m_pitch{ pitch }, m_jaw{ jaw }, m_move_speed{ move_speed }, m_position{ position },
      m_camera_front{ camera_front }, m_camera_up{ camera_up }
  {
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

    m_pitch = std::clamp(glm::acos(glm::dot(m_camera_front, s_up)) - ESP_PI / 2, -s_pitch_treshold, s_pitch_treshold);

    glm::vec3 front_xz = esp::normalize(glm::vec3{ m_camera_front.x, 0.f, m_camera_front.z });
    m_jaw              = m_camera_front.x < 0 ? ESP_PI / 2 - glm::acos(glm::dot(front_xz, s_front))
                                              : ESP_PI / 2 + glm::acos(glm::dot(front_xz, s_front));

    update_camera_up();
  }

  void FpsCamera::look_at(float dx, float dy, float dt)
  {
    float offset_x = dx * dt;
    float offset_y = dy * dt;

    m_jaw += glm::radians(offset_x);
    m_pitch = std::clamp(m_pitch + glm::radians(offset_y), -s_pitch_treshold, s_pitch_treshold);

    glm::vec3 direction;
    direction.x = cos(m_jaw) * cos(m_pitch);
    direction.y = sin(m_pitch);
    direction.z = sin(m_jaw) * cos(m_pitch);

    m_camera_front = esp::normalize(direction);

    update_camera_up();
  }

  void FpsCamera::update_camera_up()
  {
    glm::vec3 camera_right = esp::normalize(glm::cross(s_up, m_camera_front));
    m_camera_up            = glm::cross(m_camera_front, camera_right);
  }
} // namespace esp