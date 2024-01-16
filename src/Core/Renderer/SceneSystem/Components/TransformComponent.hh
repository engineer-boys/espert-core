#ifndef SCENE_COMPONENTS_TRANSFORM_COMPONENT_HH
#define SCENE_COMPONENTS_TRANSFORM_COMPONENT_HH

#include "esppch.hh"

namespace esp
{
  /// @brief ECS component that allows translation of component in space.
  struct TransformComponent
  {
   private:
    glm::vec3 m_translation;
    float m_scale;
    glm::quat m_rotation;

   public:
    /// @brief Default constructor.
    TransformComponent() : m_translation{ 0.f, 0.f, 0.f }, m_scale{ 1.f }, m_rotation{ 1.f, 0.f, 0.f, 0.f } {}

    /// @brief Translates by given argument
    /// @param vec Vector to translate by
    inline void translate(glm::vec3 vec) { m_translation += vec; }
    /// @brief Rotates by given arguments
    /// @param angle Angle to rotate by in radians
    /// @param axis Axis to rotate around
    inline void rotate(float angle, glm::vec3 axis) { m_rotation *= glm::angleAxis(angle, esp::normalize(axis)); }
    /// @brief Scales by given argument
    /// @param val Value to scale by
    inline void scale(float val) { m_scale *= val; }

    /// @brief Sets translation to given argument
    /// @param vec Translation vector
    inline void set_translation(glm::vec3 vec) { m_translation = vec; }
    /// @brief Sets rotation to given arguments
    /// @param angle Rotation angle in radians
    /// @param axis Rotation axis
    inline void set_rotation(float angle, glm::vec3 axis) { m_rotation = glm::angleAxis(angle, esp::normalize(axis)); }
    /// @brief Sets scale to given argument
    /// @param val Scale value
    inline void set_scale(float val) { m_scale = val; }

    /// @brief Calculates model matrix based on arguments.
    /// @param translation Translation vector
    /// @param rotation Rotation quaternion
    /// @param scale Float scale
    /// @return model matrix.
    inline static glm::mat4 calculate_model_mat(glm::vec3 translation, glm::quat rotation, float scale)
    {
      glm::mat4 translation_mat = glm::translate(glm::mat4(1.0f), translation);
      glm::mat4 rotation_mat    = glm::mat4_cast(rotation);
      glm::mat4 scale_mat       = glm::scale(glm::mat4(1.0f), { scale, scale, scale });

      return translation_mat * rotation_mat * scale_mat;
    }

    /// @brief Returns component's model matrix.
    /// @return Component's model matrix.
    inline glm::mat4 get_model_mat() const
    {
      glm::mat4 translation_mat = glm::translate(glm::mat4(1.0f), m_translation);
      glm::mat4 rotation_mat    = glm::mat4_cast(m_rotation);
      glm::mat4 scale_mat       = glm::scale(glm::mat4(1.0f), { m_scale, m_scale, m_scale });

      return translation_mat * rotation_mat * scale_mat;
    }
    /// @brief Returns component's translation
    /// @return Component's translation
    inline glm::vec3 get_translation() const { return m_translation; }
    /// @brief Returns component's scale
    /// @return Component's scale
    inline float get_scale() const { return m_scale; }
    /// @brief Returns component's rotation
    /// @return Component's rotation
    inline glm::quat get_rotation() const { return m_rotation; }
  };
} // namespace esp

#endif // SCENE_COMPONENTS_TRANSFORM_COMPONENT_HH
