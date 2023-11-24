#include "TransformComponent.hh"

namespace esp
{
  TransformComponent::TransformComponent() :
      m_translation{ 0.f, 0.f, 0.f }, m_scale{ 1.f }, m_rotation{ 1.f, 0.f, 0.f, 0.f }
  {
  }
  TransformComponent::TransformComponent(const glm::vec3& translation,
                                         const float& s,
                                         const float& rotation_angle,
                                         const glm::vec3& rotation_axis)
  {
    set_translation(translation);
    set_rotation(rotation_angle, rotation_axis);
    set_scale(s);
  }

  void TransformComponent::set_translation(const glm::vec3& translation)
  {
    m_translation = translation;
    translate();
  }

  void TransformComponent::translate(const glm::vec3& translation)
  {
    m_translation += translation;
    translate();
  }

  void TransformComponent::translate() { m_model = glm::translate(m_model, m_translation); }

  void TransformComponent::set_scale(const float& s)
  {
    m_scale = s;
    scale();
  }

  void TransformComponent::scale(const float& s)
  {
    m_scale *= s;
    scale();
  }

  void TransformComponent::scale() { m_model = glm::scale(m_model, { m_scale, m_scale, m_scale }); }

  void TransformComponent::set_rotation(const float& angle, const glm::vec3& axis)
  {
    m_rotation = glm::angleAxis(angle, esp::normalize(axis));
    rotate();
  }

  void TransformComponent::rotate(const float& angle, const glm::vec3& axis)
  {
    m_rotation *= glm::angleAxis(angle, esp::normalize(axis));
    rotate();
  }

  void TransformComponent::rotate() { m_model *= glm::mat4_cast(m_rotation); }
} // namespace esp