#ifndef RENDERER_CAMERA_HH
#define RENDERER_CAMERA_HH

#include "esppch.hh"

namespace esp
{
  class Camera
  {
   protected:
    float m_fov{ ESP_PI / 4 };
    float m_near{ .1f };
    float m_far{ 100.f };
    float m_sensitivity{ 1.f };

    glm::mat4 m_projection_mat{ 1.f };

   public:
    virtual ~Camera() {}

    void set_perspective(float fov, float aspect_ratio, float near_plane, float far_plane);
    void set_perspective(float aspect_ratio);

    inline void set_sensitivity(float sensitivity) { m_sensitivity = sensitivity; }

    inline glm::mat4 get_projection() { return m_projection_mat; }

    virtual glm::mat4 get_view()     = 0;
    virtual glm::vec3 get_position() = 0;
  };
} // namespace esp

#endif // RENDERER_CAMERA_HH
