#ifndef ESPERT_SANDBOX_ORBITCAMERACONTROLLER_HH
#define ESPERT_SANDBOX_ORBITCAMERACONTROLLER_HH

#include "CameraController.hh"
#include "Core/Events/MouseEvent.hh"
#include "Core/Renderer/Camera/OrbitCamera.hh"

namespace esp
{
  class OrbitCameraController : public CameraController
  {
   private:
    OrbitCamera m_camera;

   public:
    OrbitCameraController(glm::vec3 target, float min_distance, float max_distance, float distance);
    OrbitCameraController();
    ~OrbitCameraController() = default;

    void update(float dt) override;
    void handle_event(Event& event, float dt) override;

    glm::mat4 get_view() override;
    inline glm::mat4 get_projection() override { return m_camera.get_projection(); }
    glm::vec3 get_position() override;
    inline Camera* get_camera() override { return &m_camera; }

    void set_perspective(float fov, float aspect_ratio, float near, float far) override;
    void set_perspective(float aspect_ratio) override;
    void set_perspective(glm::vec4 clip_plane) override;
    void set_anaglyph_perspective(float eye_distance, float plane_distance, bool left_eye) override;

   private:
    bool mouse_moved_event_handler(MouseMovedEvent& event, float dt);
  };
} // namespace esp

#endif // ESPERT_SANDBOX_ORBITCAMERACONTROLLER_HH
