#ifndef SCENE_COMPONENTS_TRANSFORM_COMPONENT_HH
#define SCENE_COMPONENTS_TRANSFORM_COMPONENT_HH

#include "esppch.hh"

namespace esp
{
  struct TransformComponent
  {
    glm::vec3 m_translation{}; // (position offset)
    glm::vec3 m_scale{ 1.0f };
    glm::vec3 m_rotation{};

    TransformComponent()                          = default;
    TransformComponent(const TransformComponent&) = default;
    TransformComponent(const glm::vec3& translation, const glm::vec3& scale, const glm::vec3& rotation) :
        m_translation{ translation }, m_scale{ scale }, m_rotation{ rotation }
    {
    }

    inline glm::mat4 get_model_mat()
    {
      const float c3 = glm::cos(m_rotation.z);
      const float s3 = glm::sin(m_rotation.z);
      const float c2 = glm::cos(m_rotation.x);
      const float s2 = glm::sin(m_rotation.x);
      const float c1 = glm::cos(m_rotation.y);
      const float s1 = glm::sin(m_rotation.y);

      return glm::mat4{ {
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
                        { m_translation.x, m_translation.y, m_translation.z, 1.0f } };
    }
  };
} // namespace esp

#endif // SCENE_COMPONENTS_TRANSFORM_COMPONENT_HH
