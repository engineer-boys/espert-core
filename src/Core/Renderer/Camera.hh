#ifndef RENDERER_CAMERA_HH
#define RENDERER_CAMERA_HH

#include "esppch.hh"

namespace esp
{
  class Camera
  {
   private:
    glm::mat4 m_view{ 1.f };
    glm::mat4 m_projection_mat{ 1.f };

    float m_fov{ glm::radians(50.f) };
    float m_near{ .1f };
    float m_far{ 100.f };
    float m_move_speed{ 1.f };

    // TODO: this will probably end up in scene class
    static Camera* s_current_camera;
    //

   public:
    glm::vec3 m_position{ 0.f };

   private:
    glm::vec3 m_front{ 0.f, 0.f, 1.f };
    glm::vec3 m_camera_up{ 0.f, -1.f, 0.f };

   public:
    Camera()  = default;
    ~Camera() = default;

    Camera(const Camera&)            = delete;
    Camera& operator=(const Camera&) = delete;

    void set_orthographic(float left, float right, float top, float bottom, float near, float far);
    void set_perspective(float fov, float aspect_ratio, float near, float far);
    void set_perspective(float aspect_ratio);

    void look_at(glm::vec3 target);

    inline const glm::vec3& get_front() { return m_front; }
    inline const glm::vec3& get_camera_up() { return m_camera_up; };

    inline void set_fov(float val) { m_fov = val; }
    inline void set_near(float val) { m_near = val; }
    inline void set_far(float val) { m_far = val; }
    inline void set_move_speed(float speed) { m_move_speed = speed; }

    inline float get_fov() { return m_fov; }
    inline float get_near() { return m_near; }
    inline float get_far() { return m_far; }
    inline float get_move_speed() { return m_move_speed; }

    inline const glm::mat4& get_view()
    {
      m_view = glm::lookAt(m_position, m_position + m_front, m_camera_up);
      return m_view;
    }
    inline const glm::mat4& get_projection() { return m_projection_mat; }

    // TODO: this will probably end up in scene class
    inline static void set_current_camera(Camera* camera) { s_current_camera = camera; }
    inline static Camera* get_current_camera() { return s_current_camera; }
    //

   private:
    void update_camera_up();
  };
} // namespace esp

#endif // RENDERER_CAMERA_HH
