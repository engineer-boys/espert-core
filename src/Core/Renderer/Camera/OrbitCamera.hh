
#ifndef ESPERT_SANDBOX_ORBITCAMERA_HH
#define ESPERT_SANDBOX_ORBITCAMERA_HH

#include "Camera.hh"
#include "esppch.hh"

namespace esp
{
  class OrbitCamera : public Camera
  {
   private:
    float m_angle_x;
    float m_angle_y;

    float m_distance;
    float m_min_distance;
    float m_max_distance;

    glm::vec3 m_target;

   public:
    OrbitCamera(glm::vec3 target   = { 0, 0, 0 },
                float min_distance = 0.f,
                float max_distance = FLT_MAX,
                float distance     = 0.f);

    glm::mat4 get_view();
    glm::vec3 get_position();

    void rotate(float dx, float dy, float dt);
    void zoom(float dd, float dt);

   private:
    void set_distance_range(float min_distance, float max_distance);
    void clamp_distance();
  };
} // namespace esp

#endif // ESPERT_SANDBOX_ORBITCAMERA_HH
