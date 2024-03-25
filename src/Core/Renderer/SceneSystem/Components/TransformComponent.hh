#ifndef SCENE_COMPONENTS_TRANSFORM_COMPONENT_HH
#define SCENE_COMPONENTS_TRANSFORM_COMPONENT_HH

#include "esppch.hh"

namespace esp
{
  static glm::mat4 calculate_model_mat(glm::vec3 translation, glm::quat rotation, float scale);
  static glm::mat4 calculate_translation_mat(glm::mat4 mat, glm::vec3 v);
  static glm::mat4 calculate_rotation_mat(glm::quat q);
  static glm::mat4 calculate_scale_mat(glm::mat4 mat, glm::vec3 v);

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
    inline void rotate(glm::quat quat) { m_rotation = quat * m_rotation; }
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
    inline void set_rotation(glm::quat quat) { m_rotation = quat; }
    /// @brief Sets scale to given argument
    /// @param val Scale value
    inline void set_scale(float val) { m_scale = val; }
    /// @brief Returns component's model matrix.
    /// @return Component's model matrix.
    inline glm::mat4 get_model_mat() const { return calculate_model_mat(m_translation, m_rotation, m_scale); }
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

  static glm::mat4 calculate_model_mat(glm::vec3 translation, glm::quat rotation, float scale)
  {
    glm::mat4 translation_mat = calculate_translation_mat(glm::mat4(1.0f), translation);
    glm::mat4 rotation_mat    = calculate_rotation_mat(rotation);
    glm::mat4 scale_mat       = calculate_scale_mat(glm::mat4(1.0f), { scale, scale, scale });

    return translation_mat * rotation_mat * scale_mat;
  }

  static glm::mat4 calculate_translation_mat(glm::mat4 mat, glm::vec3 v)
  {
    glm::mat4 result(mat);
    result[3] = mat[0] * v[0] + mat[1] * v[1] + mat[2] * v[2] + mat[3];
    return result;
  }

  static glm::mat4 calculate_rotation_mat(glm::quat q)
  {
    glm::mat4 result(1.f);
    float qxx(q.x * q.x);
    float qyy(q.y * q.y);
    float qzz(q.z * q.z);
    float qxz(q.x * q.z);
    float qxy(q.x * q.y);
    float qyz(q.y * q.z);
    float qwx(q.w * q.x);
    float qwy(q.w * q.y);
    float qwz(q.w * q.z);

    result[0][0] = 1.f - 2.f * (qyy + qzz);
    result[0][1] = 2.f * (qxy + qwz);
    result[0][2] = 2.f * (qxz - qwy);

    result[1][0] = 2.f * (qxy - qwz);
    result[1][1] = 1.f - 2.f * (qxx + qzz);
    result[1][2] = 2.f * (qyz + qwx);

    result[2][0] = 2.f * (qxz + qwy);
    result[2][1] = 2.f * (qyz - qwx);
    result[2][2] = 1.f - 2.f * (qxx + qyy);
    return result;
  }

  static glm::mat4 calculate_scale_mat(glm::mat4 mat, glm::vec3 v)
  {
    glm::mat4 result(mat);
    result[0] = mat[0] * v[0];
    result[1] = mat[1] * v[1];
    result[2] = mat[2] * v[2];
    result[3] = mat[3];
    return result;
  }
} // namespace esp

#endif // SCENE_COMPONENTS_TRANSFORM_COMPONENT_HH
