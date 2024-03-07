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

    // ---------------------------------------- MG1 -------------------------------------------------------
    float m_angle_x{ 0.f };
    float m_angle_y{ 0.f };
    float m_angle_z{ 0.f };
    // ----------------------------------------------------------------------------------------------------

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

    // ---------------------------------------- MG1 -------------------------------------------------------
    inline void rotate_x(float angle) { m_angle_x += angle; }
    inline void rotate_y(float angle) { m_angle_y += angle; }
    inline void rotate_z(float angle) { m_angle_z += angle; }
    // ----------------------------------------------------------------------------------------------------

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
    //    inline glm::mat4 get_model_mat() const
    //    {
    //      glm::mat4 translation_mat = glm::translate(glm::mat4(1.0f), m_translation);
    //      glm::mat4 rotation_mat    = glm::mat4_cast(m_rotation);
    //      glm::mat4 scale_mat       = glm::scale(glm::mat4(1.0f), { m_scale, m_scale, m_scale });
    //
    //      return translation_mat * rotation_mat * scale_mat;
    //    }

    // ---------------------------------------- MG1 -------------------------------------------------------
    inline glm::mat4 get_model_mat() const
    {
      auto translation_mat = glm::mat4(1.f);
      translation_mat[3]   = { m_translation, 1 };

      auto rotation_mat = get_rotate_x_max() * get_rotate_y_mat() * get_rotate_z_mat();

      auto scale_mat  = glm::mat4(1.f);
      scale_mat[0][0] = scale_mat[1][1] = scale_mat[2][2] = m_scale;

      return translation_mat * rotation_mat * scale_mat;
    }
    // ----------------------------------------------------------------------------------------------------

    /// @brief Returns component's translation
    /// @return Component's translation
    inline glm::vec3 get_translation() const { return m_translation; }
    /// @brief Returns component's scale
    /// @return Component's scale
    inline float get_scale() const { return m_scale; }
    /// @brief Returns component's rotation
    /// @return Component's rotation
    inline glm::quat get_rotation() const { return m_rotation; }

   private:
    inline glm::mat4 get_rotate_x_max() const
    {
      float cos_alpha = cos(m_angle_x);
      float sin_alpha = sin(m_angle_x);

      auto rot_x  = glm::mat4(1.f);
      rot_x[1][1] = cos_alpha;
      rot_x[2][1] = -sin_alpha;
      rot_x[1][2] = sin_alpha;
      rot_x[2][2] = cos_alpha;

      return rot_x;
    }

    inline glm::mat4 get_rotate_y_mat() const
    {
      float cos_alpha = cos(m_angle_y);
      float sin_alpha = sin(m_angle_y);

      glm::mat4 rot_y{ 1.f };
      rot_y[0][0] = cos_alpha;
      rot_y[2][0] = sin_alpha;
      rot_y[0][2] = -sin_alpha;
      rot_y[2][2] = cos_alpha;

      return rot_y;
    }

    inline glm::mat4 get_rotate_z_mat() const
    {
      float cos_alpha = cos(m_angle_z);
      float sin_alpha = sin(m_angle_z);

      glm::mat4 rot_z{ 1.f };
      rot_z[0][0] = cos_alpha;
      rot_z[1][0] = -sin_alpha;
      rot_z[0][1] = sin_alpha;
      rot_z[1][1] = cos_alpha;

      return rot_z;
    }
  };
} // namespace esp

#endif // SCENE_COMPONENTS_TRANSFORM_COMPONENT_HH
