#ifndef ESPERT_SANDBOX_FPSCAMERACONTROLLER_HH
#define ESPERT_SANDBOX_FPSCAMERACONTROLLER_HH

#include "CameraController.hh"
#include "Core/Events/MouseEvent.hh"
#include "Core/Renderer/Camera/FpsCamera.hh"

namespace esp
{
  class FpsCameraController : public CameraController
  {
   private:
    FpsCamera m_camera;

   public:
    FpsCameraController(float pitch,
                        float jaw,
                        float move_speed,
                        glm::vec3 position,
                        glm::vec3 camera_front,
                        glm::vec3 camera_up);
    FpsCameraController();
    ~FpsCameraController() = default;

    void update(float dt) override;
    void handle_event(Event& event, float dt) override;

    glm::mat4 get_view() override;
    inline glm::mat4 get_projection() override { return m_camera.get_projection(); }
    glm::vec3 get_position() override;
    inline Camera* get_camera() override { return &m_camera; }

    void set_move_speed(float val);
    void set_position(glm::vec3 val);
    void set_perspective(float fov, float aspect_ratio, float near, float far) override;
    void set_perspective(float aspect_ratio) override;
    void set_perspective(glm::vec4 clip_plane) override;
    void set_anaglyph_perspective(float eye_distance, float plane_distance, bool left_eye) override;

   private:
    bool mouse_moved_event_handler(MouseMovedEvent& event, float dt);
    void handle_keyboard_input(float dt);
    void zoom(float dd, float dt);
  };
} // namespace esp

#endif // ESPERT_SANDBOX_FPSCAMERACONTROLLER_HH
