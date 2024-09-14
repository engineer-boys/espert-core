#ifndef RENDERER_CAMERA_HH
#define RENDERER_CAMERA_HH

#include "esppch.hh"

namespace esp
{
  class Camera
  {
   protected:
    glm::mat4 m_projection_mat{ 1.f };

   public:
    ~Camera() = default;

    inline glm::mat4 get_projection() { return m_projection_mat; }

    void set_perspective(float fov, float aspect_ratio, float near, float far);
    void set_perspective(glm::vec4 clip_plane, float near, float far); // {l, r, b, t}
    void set_anaglyph_perspective(float eye_distance,
                                  float plane_distance,
                                  bool left_eye,
                                  float fov,
                                  float near,
                                  float far);
  };
} // namespace esp

#endif // RENDERER_CAMERA_HH
