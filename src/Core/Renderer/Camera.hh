#ifndef RENDERER_CAMERA_HH
#define RENDERER_CAMERA_HH

#include "esppch.hh"

namespace esp
{
  /// @brief Contains information about how the objects to render are perceived.
  class Camera
  {
   public:
    /// @brief Direction in which the Camera is moving.
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
    /// @brief Default Constructor.
    Camera() = default;
    /// @brief Default destructor.
    ~Camera() = default;

    Camera(const Camera&)            = delete;
    Camera& operator=(const Camera&) = delete;

    /// @brief Sets Camera perspective.
    /// @param fov Camera's field of view.
    /// @param aspect_ratio Aspect ratio of the screen.
    /// @param near_plane The nearest plane that the Camera can see.
    /// @param far_plane The furthest plane that the Camera can see.
    void set_perspective(float fov, float aspect_ratio, float near_plane, float far_plane);
    /// @brief Sets Camera perspective.
    /// @param aspect_ratio Aspect ratio of the screen.
    void set_perspective(float aspect_ratio);

    /// @brief Moves camera in certain direction based on the delta time since last frame.
    /// @param direction Direction in which the Camera moves.
    /// @param dt Delta time since last frame.
    void move(MoveDirection direction, float dt);
    /// @brief Sets the point at which the Camera is pointing.
    /// @param target The point at which the Camera is pointing.
    void look_at(glm::vec3 target);
    /// @brief Changes the point at which the Camera is pointing based on mouse movement.
    /// @param mouse_pos New mouse position.
    /// @param dt Delta time since last frame.
    void look_at(glm::vec2 mouse_pos, float dt);

    /// @brief Sets Cmaera's field of view.
    /// @param val Field of view.
    inline void set_fov(float val) { m_fov = std::clamp(val, glm::radians(1.f), ESP_PI / 4); }
    /// @brief Sets Camera's nearest plane.
    /// @param val Nearest plane.
    inline void set_near(float val) { m_near = val; }
    /// @brief Sets Camera's furthest plane.
    /// @param val Furthest plane.
    inline void set_far(float val) { m_far = val; }
    /// @brief Sets the Camera's movement speed.
    /// @param speed Camera's movement speed.
    inline void set_move_speed(float speed) { m_move_speed = speed; }
    /// @brief Sets the Camera's sensitivity.
    /// @param sensitivity Camera's sensitivity.
    inline void set_sensitivity(float sensitivity) { m_sensitivity = sensitivity; }
    /// @brief Sets Camera's position.
    /// @param position Camera's position.
    inline void set_position(glm::vec3 position) { m_position = position; }

    /// @brief Returns Camera's field of view.
    /// @return Camera's field of view.
    inline float get_fov() const { return m_fov; }
    /// @brief Returns Camera's nearest plane.
    /// @return Camera's nearest plane.
    inline float get_near() const { return m_near; }
    /// @brief Returns Camera's furthest plane.
    /// @return Camera's furthest plane.
    inline float get_far() const { return m_far; }
    /// @brief Returns Camera's movement speed.
    /// @return Camera's movement speed.
    inline float get_move_speed() const { return m_move_speed; }
    /// @brief Returns Camera's sensitivity.
    /// @return Camera's sensitivity.
    inline float get_sensitivity() const { return m_sensitivity; }

    /// @brief Returns Camera's position.
    /// @return Camera's position.
    inline glm::vec3 get_position() const { return m_position; }

    /// @brief Returns Camera's view matrix.
    /// @return Camera's view matrix.
    inline const glm::mat4& get_view()
    {
      return m_view = glm::lookAt(m_position, m_position + m_camera_front, m_camera_up);
    }
    /// @brief Returns Camera's projection matrix.
    /// @return
    inline const glm::mat4& get_projection() { return m_projection_mat; }

   private:
    void update_camera_up();
  };
} // namespace esp

#endif // RENDERER_CAMERA_HH
