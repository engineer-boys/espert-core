#ifndef SCENE_COMPONENTS_TRANSFORM_COMPONENT_HH
#define SCENE_COMPONENTS_TRANSFORM_COMPONENT_HH

#include "esppch.hh"

namespace esp
{
  struct TransformComponent
  {
   private:
    glm::vec3 m_translation{ 0.f, 0.f, 0.f };
    float m_scale{ 1.f };
    glm::quat m_rotation{ glm::angleAxis(0.f, glm::vec3{ 0.f, 0.f, 0.f }) };

    glm::mat4 m_model{ 1.f };

   public:
    TransformComponent()                          = default;
    TransformComponent(const TransformComponent&) = default;
    TransformComponent(const glm::vec3& translation,
                       const float& s                 = 1.f,
                       const float& rotation_angle    = 0,
                       const glm::vec3& rotation_axis = {
                           0.f,
                           0.f,
                           0.f,
                       })
    {
      translate(translation);
      rotate(rotation_angle, rotation_axis);
      scale(s);
    }

    inline void translate(const glm::vec3& translation)
    {
      m_translation += translation;
      translate();
    }
    inline void translate() { m_model = glm::translate(m_model, m_translation); }

    inline void scale(const float& s)
    {
      m_scale *= s;
      scale();
    }
    inline void scale() { m_model = glm::scale(m_model, { m_scale, m_scale, m_scale }); }

    // TODO: try to separate axis and self rotations
    inline void rotate(const float& angle, const glm::vec3& axis)
    {
      m_rotation *= glm::angleAxis(angle, esp::normalize(axis));
      rotate();
    }
    inline void rotate() { m_model *= glm::mat4_cast(m_rotation); }

    inline void reset_model_mat() { m_model = glm::mat4{ 1.f }; }
    inline glm::mat4& get_model_mat() { return m_model; }
  };
} // namespace esp

#endif // SCENE_COMPONENTS_TRANSFORM_COMPONENT_HH
