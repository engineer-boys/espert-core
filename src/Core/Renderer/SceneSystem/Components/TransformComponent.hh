#ifndef SCENE_COMPONENTS_TRANSFORM_COMPONENT_HH
#define SCENE_COMPONENTS_TRANSFORM_COMPONENT_HH

#include "esppch.hh"

namespace esp
{
  namespace action
  {
    struct TransformAction;
  }

  /// @brief ECS component that allows translation of component in space.
  struct TransformComponent
  {
   private:
    glm::vec3 m_translation;
    float m_scale;
    glm::quat m_rotation;

    glm::mat4 m_model{ 1.f };

   public:
    /// @brief Default constructor.
    TransformComponent() : m_translation{ 0.f, 0.f, 0.f }, m_scale{ 1.f }, m_rotation{ 1.f, 0.f, 0.f, 0.f } {}

    /// @brief Resets component's model matrix.
    inline void reset() { m_model = glm::mat4{ 1.f }; }
    /// @brief Returns component's model matrix.
    /// @return Component's model matrix.
    inline const glm::mat4& get_model_mat() { return m_model; }

    inline const glm::vec3& get_translation() const { return m_translation; }
    inline const float& get_scale() const { return m_scale; }
    inline const glm::quat& get_rotation() const { return m_rotation; }

   private:
    inline void translate() { m_model = glm::translate(m_model, m_translation); }
    inline void rotate() { m_model *= glm::mat4_cast(m_rotation); }
    inline void scale() { m_model = glm::scale(m_model, { m_scale, m_scale, m_scale }); }

    friend struct action::TransformAction;
  };
} // namespace esp

#endif // SCENE_COMPONENTS_TRANSFORM_COMPONENT_HH
