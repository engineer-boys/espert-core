#ifndef ESPERT_SANDBOX_FPSCAMERA_HH
#define ESPERT_SANDBOX_FPSCAMERA_HH

#include "Camera.hh"

namespace esp
{
  enum MoveDirection
  {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
  };

  class FpsCamera : public Camera
  {
   private:
    static const glm::vec3 s_up;
    static const glm::vec3 s_front;
    static const float s_pitch_treshold;

    float m_pitch;
    float m_jaw;
    float m_move_speed;

    glm::vec3 m_position;
    glm::vec3 m_camera_front;
    glm::vec3 m_camera_up;

   public:
    FpsCamera(float pitch            = glm::radians(0.f),
              float jaw              = -ESP_PI / 2,
              float move_speed       = 1.f,
              glm::vec3 position     = glm::vec3(0.f),
              glm::vec3 camera_front = s_front,
              glm::vec3 camera_up    = s_up);
    ~FpsCamera() = default;

    FpsCamera(const FpsCamera&)            = delete;
    FpsCamera& operator=(const FpsCamera&) = delete;

    inline glm::vec3 get_position() { return m_position; }
    inline glm::mat4 get_view() { return glm::lookAt(m_position, m_position + m_camera_front, m_camera_up); }

    void move(MoveDirection direction, float dt);
    void look_at(glm::vec3 target);
    void look_at(float dx, float dy, float dt);

    inline void set_move_speed(float speed) { m_move_speed = speed; }
    inline void set_position(glm::vec3 position) { m_position = position; }

   private:
    void update_camera_up();
  };
} // namespace esp
#endif // ESPERT_SANDBOX_FPSCAMERA_HH
