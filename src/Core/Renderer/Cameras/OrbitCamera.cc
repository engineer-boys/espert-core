#include "OrbitCamera.hh"

namespace esp
{
  OrbitCamera::OrbitCamera(glm::vec3 target, float min_distance, float max_distance, float distance) :
      m_angle_x{ 0 }, m_angle_y{ 0 }, m_target{ target }, m_distance{ distance }
  {
    set_distance_range(min_distance, max_distance);
  }

  void OrbitCamera::rotate(float dx, float dy, float dt)
  {
    m_angle_x = glm::mod(m_angle_x + dx * dt * m_sensitivity, glm::two_pi<float>());
    m_angle_y = glm::mod(m_angle_y + dy * dt * m_sensitivity, glm::two_pi<float>());
  }

  void OrbitCamera::zoom(float dd, float dt)
  {
    m_distance += dd * dt * m_sensitivity;
    clamp_distance();
  }

  void OrbitCamera::set_distance_range(float min_distance, float max_distance)
  {
    if (max_distance < min_distance) { max_distance = min_distance; }
    m_min_distance = min_distance;
    m_max_distance = max_distance;

    clamp_distance();
  }

  void OrbitCamera::on_new_frame()
  {
    // update view mat
    m_view = glm::mat4{ 1.f };
    m_view = glm::translate(m_view, { 0, 0, -m_distance });
    m_view = glm::rotate(m_view, -m_angle_y, { 1, 0, 0 });
    m_view = glm::rotate(m_view, -m_angle_x, { 0, 1, 0 });
    m_view = glm::translate(m_view, -m_target);

    // update position
    if (m_distance == 0.f) { m_position = m_target; }
    {
      glm::mat4 inverse_view = glm::inverse(get_view());
      glm::vec4 pos          = inverse_view[3];

      m_position = { pos.x, pos.y, pos.z };
    }
  }

  void OrbitCamera::clamp_distance()
  {
    if (m_distance < m_min_distance) { m_distance = m_min_distance; }
    else if (m_distance > m_max_distance) { m_distance = m_max_distance; }
  }
} // namespace esp
