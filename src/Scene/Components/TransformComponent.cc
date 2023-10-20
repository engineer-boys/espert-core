#include "TransformComponent.hh"

namespace esp
{
  glm::mat4 TransformComponent::get_transform_mat()
  {
    const float c3 = glm::cos(m_rotation.z);
    const float s3 = glm::sin(m_rotation.z);
    const float c2 = glm::cos(m_rotation.x);
    const float s2 = glm::sin(m_rotation.x);
    const float c1 = glm::cos(m_rotation.y);
    const float s1 = glm::sin(m_rotation.y);

    return glm::mat4{
      {
          m_scale.x * (c1 * c3 + s1 * s2 * s3),
          m_scale.x * (c2 * s3),
          m_scale.x * (c1 * s2 * s3 - c3 * s1),
          0.0f,
      },
      {
          m_scale.y * (c3 * s1 * s2 - c1 * s3),
          m_scale.y * (c2 * c3),
          m_scale.y * (c1 * c3 * s2 + s1 * s3),
          0.0f,
      },
      {
          m_scale.z * (c2 * s1),
          m_scale.z * (-s2),
          m_scale.z * (c1 * c2),
          0.0f,
      },
      { m_translation.x, m_translation.y, m_translation.z, 1.0f }
    };
  }

  glm::mat3 TransformComponent::get_normal_transform_mat()
  {
    const float c3               = glm::cos(m_rotation.z);
    const float s3               = glm::sin(m_rotation.z);
    const float c2               = glm::cos(m_rotation.x);
    const float s2               = glm::sin(m_rotation.x);
    const float c1               = glm::cos(m_rotation.y);
    const float s1               = glm::sin(m_rotation.y);
    const glm::vec3 inverseScale = 1.0f / m_scale;

    return glm::mat3{ {
                          inverseScale.x * (c1 * c3 + s1 * s2 * s3),
                          inverseScale.x * (c2 * s3),
                          inverseScale.x * (c1 * s2 * s3 - c3 * s1),
                      },
                      {
                          inverseScale.y * (c3 * c1 * s2 - c1 * s3),
                          inverseScale.y * (c2 * c3),
                          inverseScale.y * (c1 * c3 * s2 + s1 * s3),
                      },
                      {
                          inverseScale.z * (c2 * s1),
                          inverseScale.z * (-s2),
                          inverseScale.z * (c1 * c2),
                      } };
  }
} // namespace esp
