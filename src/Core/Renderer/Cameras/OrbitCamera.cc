#include "OrbitCamera.hh"

namespace esp
{
  OrbitCamera::OrbitCamera(glm::vec3 target, float min_distance, float max_distance, float distance) :
      m_angle_x{ 0 }, m_angle_y{ 0 }, m_target{ target }, m_distance{ distance }
  {
    set_distance_range(min_distance, max_distance);
  }

  glm::mat4 OrbitCamera::get_view()
  {
    glm::mat4 view{ 1.f };
    view = glm::translate(view, { 0, 0, -m_distance });
    view = glm::rotate(view, -m_angle_y, { 1, 0, 0 });
    view = glm::rotate(view, -m_angle_x, { 0, 1, 0 });
    view = glm::translate(view, -m_target);

    return view;
  }

  glm::vec3 OrbitCamera::get_position()
  {
    if (m_distance == 0.f) { return m_target; }
    {
      glm::mat4 inverse_view = glm::inverse(get_view());
      glm::vec4 pos          = inverse_view[3];

      return { pos.x, pos.y, pos.z };
    }
  }

  void OrbitCamera::rotate(float dx, float dy)
  {
    m_angle_x = glm::mod(m_angle_x + dx, glm::two_pi<float>());
    m_angle_y = glm::mod(m_angle_y + dy, glm::two_pi<float>());
  }

  void OrbitCamera::zoom(float dd)
  {
    m_distance += dd;
    clamp_distance();
  }

  void OrbitCamera::set_distance_range(float min_distance, float max_distance)
  {
    if (max_distance < min_distance) { max_distance = min_distance; }
    m_min_distance = min_distance;
    m_max_distance = max_distance;

    clamp_distance();
  }

  void OrbitCamera::clamp_distance()
  {
    if (m_distance < m_min_distance) { m_distance = m_min_distance; }
    else if (m_distance > m_max_distance) { m_distance = m_max_distance; }
  }
} // namespace esp
