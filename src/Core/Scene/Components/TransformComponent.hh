#ifndef SCENE_COMPONENTS_TRANSFORM_COMPONENT_HH
#define SCENE_COMPONENTS_TRANSFORM_COMPONENT_HH

#include "esppch.hh"

namespace esp
{
  struct TransformComponent
  {
   private:
    glm::vec3 m_translation;
    float m_scale;
    glm::quat m_rotation;

    glm::mat4 m_model{ 1.f };

   public:
    TransformComponent();
    TransformComponent(const glm::vec3& translation,
                       const float& s                 = 1.f,
                       const float& rotation_angle    = 0,
                       const glm::vec3& rotation_axis = { 0.f, 0.f, 0.f });

    void set_translation(const glm::vec3& translation);
    void translate(const glm::vec3& translation);
    void translate();

    void set_scale(const float& s);
    void scale(const float& s);
    void scale();

    // TODO: try to separate axis and self rotations
    void set_rotation(const float& angle, const glm::vec3& axis);
    void rotate(const float& angle, const glm::vec3& axis);
    void rotate();

    inline const glm::vec3& get_translation() { return m_translation; }
    inline void reset_model_mat() { m_model = glm::mat4{ 1.f }; }
    inline const glm::mat4& get_model_mat() { return m_model; }
  };
} // namespace esp

#endif // SCENE_COMPONENTS_TRANSFORM_COMPONENT_HH
