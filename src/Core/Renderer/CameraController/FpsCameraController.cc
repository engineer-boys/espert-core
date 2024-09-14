#include "FpsCameraController.hh"
#include "Core/Input/EspInput.hh"
#include "Core/Input/EspKeyCodes.hh"
#include "Core/Input/EspMouseButtonCode.hh"
#include "Core/RenderAPI/Work/EspWorkOrchestrator.hh"

namespace esp
{
  FpsCameraController::FpsCameraController(float pitch,
                                           float jaw,
                                           float move_speed,
                                           glm::vec3 position,
                                           glm::vec3 camera_front,
                                           glm::vec3 camera_up) :
      m_camera(pitch, jaw, move_speed, position, camera_front, camera_up)
  {
  }

  FpsCameraController::FpsCameraController() : m_camera() {}

  void FpsCameraController::update(float dt) { handle_keyboard_input(dt); }

  void FpsCameraController::handle_event(Event& event, float dt)
  {
    Event::try_handler<MouseMovedEvent>(event,
                                        ESP_BIND_EVENT_FOR_FUN(FpsCameraController::mouse_moved_event_handler, dt));
  }

  glm::mat4 FpsCameraController::get_view() { return m_camera.get_view(); }

  glm::vec3 FpsCameraController::get_position() { return m_camera.get_position(); }

  void FpsCameraController::set_perspective(float fov, float aspect_ratio, float near, float far)
  {
    m_camera.set_perspective(fov, aspect_ratio, near, far);
  }

  void FpsCameraController::set_move_speed(float val) { m_camera.set_move_speed(val); }

  void FpsCameraController::set_position(glm::vec3 val) { m_camera.set_position(val); }

  void FpsCameraController::set_perspective(float aspect_ratio)
  {
    m_camera.set_perspective(m_fov, aspect_ratio, m_near, m_far);
  }

  void FpsCameraController::set_perspective(glm::vec4 clip_plane)
  {
    m_camera.set_perspective(clip_plane, m_near, m_far);
  }

  void FpsCameraController::set_anaglyph_perspective(float eye_distance, float plane_distance, bool left_eye)

  {
    m_camera.set_anaglyph_perspective(eye_distance, plane_distance, left_eye, m_fov, m_near, m_far);
  }

  bool FpsCameraController::mouse_moved_event_handler(MouseMovedEvent& event, float dt)
  {
    if (EspInput::is_mouse_button_pressed(ESP_MOUSE_BUTTON_LEFT))
    {
      m_camera.look_at(event.get_dx(), -event.get_dy(), m_sensitivity * dt);
    }
    if (EspInput::is_mouse_button_pressed(ESP_MOUSE_BUTTON_RIGHT)) { zoom(event.get_dy(), dt); }

    return false;
  }

  void FpsCameraController::handle_keyboard_input(float dt)
  {
    if (EspInput::is_key_pressed(ESP_KEY_W)) { m_camera.move(FORWARD, dt); }
    if (EspInput::is_key_pressed(ESP_KEY_S)) { m_camera.move(BACKWARD, dt); }
    if (EspInput::is_key_pressed(ESP_KEY_A)) { m_camera.move(LEFT, dt); }
    if (EspInput::is_key_pressed(ESP_KEY_D)) { m_camera.move(RIGHT, dt); }
    if (EspInput::is_key_pressed(ESP_KEY_SPACE)) { m_camera.move(UP, dt); }
    if (EspInput::is_key_pressed(ESP_KEY_LEFT_SHIFT)) { m_camera.move(DOWN, dt); }
  }

  void FpsCameraController::zoom(float dd, float dt)
  {
    m_fov = std::clamp(m_fov + dd * dt / (2.f * m_sensitivity), glm::radians(1.f), ESP_PI / 4);
    set_perspective(EspWorkOrchestrator::get_swap_chain_extent_aspect_ratio());
  }
} // namespace esp