#include "OrbitCameraController.hh"
#include "Core/Input/EspInput.hh"
#include "Core/Input/EspMouseButtonCode.hh"

namespace esp
{
  OrbitCameraController::OrbitCameraController(glm::vec3 target,
                                               float min_distance,
                                               float max_distance,
                                               float distance) : m_camera(target, min_distance, max_distance, distance)
  {
  }

  OrbitCameraController::OrbitCameraController() : m_camera() {}

  void OrbitCameraController::update(float dt) {}

  void OrbitCameraController::handle_event(Event& event, float dt)
  {
    Event::try_handler<MouseMovedEvent>(event,
                                        ESP_BIND_EVENT_FOR_FUN(OrbitCameraController::mouse_moved_event_handler, dt));
  }

  glm::mat4 OrbitCameraController::get_view() { return m_camera.get_view(); }

  glm::vec3 OrbitCameraController::get_position() { return m_camera.get_position(); }

  void OrbitCameraController::set_perspective(float fov, float aspect_ratio, float near, float far)
  {
    m_camera.set_perspective(fov, aspect_ratio, near, far);
  }

  void OrbitCameraController::set_perspective(float aspect_ratio)
  {
    m_camera.set_perspective(m_fov, aspect_ratio, m_near, m_far);
  }

  void OrbitCameraController::set_perspective(glm::vec4 clip_plane)
  {
    m_camera.set_perspective(clip_plane, m_near, m_far);
  }

  void OrbitCameraController::set_anaglyph_perspective(float eye_distance, float plane_distance, bool left_eye)
  {
    m_camera.set_anaglyph_perspective(eye_distance, plane_distance, left_eye, m_fov, m_near, m_far);
  }

  bool OrbitCameraController::mouse_moved_event_handler(MouseMovedEvent& event, float dt)
  {
    if (EspInput::is_mouse_button_pressed(ESP_MOUSE_BUTTON_LEFT))
    {
      m_camera.rotate(event.get_dx(), event.get_dy(), m_sensitivity * dt);
    }
    if (EspInput::is_mouse_button_pressed(ESP_MOUSE_BUTTON_RIGHT))
    {
      m_camera.zoom(event.get_dy(), m_sensitivity * dt);
    }

    return false;
  }
} // namespace esp