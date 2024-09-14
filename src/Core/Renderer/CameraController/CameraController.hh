#ifndef ESPERT_SANDBOX_CAMERACONTROLLER_HH
#define ESPERT_SANDBOX_CAMERACONTROLLER_HH

#include "esppch.hh"

#include "Core/Renderer/Camera/Camera.hh"

namespace esp
{
  class CameraController
  {
   protected:
    float m_fov{ ESP_PI / 4 };
    float m_near{ .1f };
    float m_far{ 100.f };
    float m_sensitivity{ 1.f };

   public:
    ~CameraController() = default;

    virtual void update(float dt)                     = 0;
    virtual void handle_event(Event& event, float dt) = 0;

    virtual glm::mat4 get_view()       = 0;
    virtual glm::mat4 get_projection() = 0;
    virtual glm::vec3 get_position()   = 0;
    virtual Camera* get_camera()       = 0;

    virtual void set_perspective(float fov, float aspect_ratio, float near, float far)             = 0;
    virtual void set_perspective(float aspect_ratio)                                               = 0;
    virtual void set_perspective(glm::vec4 clip_plane)                                             = 0;
    virtual void set_anaglyph_perspective(float eye_distance, float plane_distance, bool left_eye) = 0;
    virtual void set_sensitivity(float sensitivity) { m_sensitivity = sensitivity; }
  };
} // namespace esp

#endif // ESPERT_SANDBOX_CAMERACONTROLLER_HH
