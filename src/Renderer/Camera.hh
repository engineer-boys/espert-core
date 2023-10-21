#ifndef RENDERER_CAMERA_H
#define RENDERER_CAMERA_H

#include "esppch.hh"

namespace esp::renderer
{
  class Camera
  {
   private:
    glm::mat4 m_projection_mat{ 1.f };

    float m_fov{ glm::radians(50.f) };
    float m_near{ .1f };
    float m_far{ 100.f };

   public:
    Camera()  = default;
    ~Camera() = default;

    Camera(const Camera&)            = delete;
    Camera& operator=(const Camera&) = delete;

    void set_orthographic(float left, float right, float top, float bottom, float near, float far);
    void set_perspective(float fov, float aspect_ratio, float near, float far);
    void set_perspective(float aspect_ratio);

    inline void set_fov(float val) { m_fov = val; }
    inline void set_near(float val) { m_near = val; }
    inline void set_far(float val) { m_far = val; }

    inline float get_fov() { return m_fov; }
    inline float get_near() { return m_near; }
    inline float get_far() { return m_far; }
    inline const glm::mat4& get_projection() { return m_projection_mat; }
  };
} // namespace esp::renderer

#endif // RENDERER_CAMERA_H
