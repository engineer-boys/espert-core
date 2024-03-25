//
// Created by sebastian on 15.03.24.
//

#ifndef ESPERT_SANDBOX_FPSCAMERA_HH
#define ESPERT_SANDBOX_FPSCAMERA_HH

#include "Camera.hh"

namespace esp
{
  /// @brief Contains information about how the objects to render are perceived.
  class FpsCamera : public Camera
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

    float m_pitch{ glm::radians(0.f) };
    float m_jaw{ -ESP_PI / 2 };
    float m_move_speed{ 1.f };

    glm::vec3 m_position{ 0.f };
    glm::vec3 m_camera_front = S_FRONT;
    glm::vec3 m_camera_up    = S_UP;

   public:
    /// @brief Default Constructor.
    FpsCamera() = default;
    /// @brief Default destructor.
    ~FpsCamera() override = default;

    FpsCamera(const FpsCamera&)            = delete;
    FpsCamera& operator=(const FpsCamera&) = delete;

    void zoom(float dd, float dt);
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
    void look_at(float dx, float dy, float dt);
    /// @brief Sets the Camera's movement speed.
    /// @param speed Camera's movement speed.
    inline void set_move_speed(float speed) { m_move_speed = speed; }
    /// @brief Sets Camera's position.
    /// @param position Camera's position.
    inline void set_position(glm::vec3 position) { m_position = position; }

    /// @brief Returns Camera's field of view.
    /// @param return Camera's field of view.
    inline float get_fov() { return m_fov; }
    /// @brief Returns Camera's position.
    /// @return Camera's position.
    inline glm::vec3 get_position() override { return m_position; }
    /// @brief Returns Camera's view matrix.
    /// @return Camera's view matrix.
    inline glm::mat4 get_view() override
    {
      return m_view = glm::lookAt(m_position, m_position + m_camera_front, m_camera_up);
    }

   private:
    void update_camera_up();
  };
} // namespace esp
#endif // ESPERT_SANDBOX_FPSCAMERA_HH