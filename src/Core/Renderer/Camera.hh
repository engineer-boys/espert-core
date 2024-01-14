#ifndef RENDERER_CAMERA_HH
#define RENDERER_CAMERA_HH

#include "esppch.hh"

namespace esp
{
  class Camera
  {
   public:
    enum MoveDirection
    {
      FORWARD,
      BACKWARD,
      LEFT,
      RIGHT,
      UP,
      DOWN
    };

   private:
    static constexpr glm::vec3 S_UP{ 0.f, 1.f, 0.f };
    static constexpr glm::vec3 S_FRONT{ 0.f, 0.f, -1.f };
    static constexpr float S_PITCH_TRESHOLD{ glm::radians(89.f) };

    glm::mat4 m_view{ 1.f };
    glm::mat4 m_projection_mat{ 1.f };

    float m_fov{ ESP_PI / 4 };
    float m_near{ .1f };
    float m_far{ 100.f };
    float m_pitch{ glm::radians(0.f) };
    float m_jaw{ -ESP_PI / 2 };
    float m_move_speed{ 1.f };
    float m_sensitivity{ 1.f };

    glm::vec2 m_last_move{};
    bool m_first_move{ true };

    glm::vec3 m_position{ 0.f };
    glm::vec3 m_camera_front = S_FRONT;
    glm::vec3 m_camera_up    = S_UP;

   public:
    Camera()  = default;
    ~Camera() = default;

    Camera(const Camera&)            = delete;
    Camera& operator=(const Camera&) = delete;

    void set_perspective(float fov, float aspect_ratio, float near, float far);
    void set_perspective(float aspect_ratio);

    void move(MoveDirection direction, float dt);
    void look_at(glm::vec3 target);
    void look_at(glm::vec2 mouse_pos, float dt);

    inline void set_fov(float val) { m_fov = std::clamp(val, glm::radians(1.f), ESP_PI / 4); }
    inline void set_near(float val) { m_near = val; }
    inline void set_far(float val) { m_far = val; }
    inline void set_move_speed(float speed) { m_move_speed = speed; }
    inline void set_sensitivity(float sensitivity) { m_sensitivity = sensitivity; }
    inline void set_position(glm::vec3 position) { m_position = position; }

    inline float get_fov() const { return m_fov; }
    inline float get_near() const { return m_near; }
    inline float get_far() const { return m_far; }
    inline float get_move_speed() const { return m_move_speed; }
    inline float get_sensitivity() const { return m_sensitivity; }

    inline glm::vec3 get_positiion() const { return m_position; }

    inline const glm::mat4& get_view()
    {
      return m_view = glm::lookAt(m_position, m_position + m_camera_front, m_camera_up);
    }
    inline const glm::mat4& get_projection() { return m_projection_mat; }

   private:
    void update_camera_up();
  };
} // namespace esp

#endif // RENDERER_CAMERA_HH
