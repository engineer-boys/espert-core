#include "InputLayer.hh"

#include "Core/EspWindow.hh"
#include "Core/Renderer/Camera.hh"

namespace esp
{
  void InputLayer::handle_event(Event& event, float dt)
  {
    Event::try_handler<KeyPressedEvent>(event, ESP_BIND_EVENT_FOR_FUN(InputLayer::key_pressed_event_handler, dt));
  }

  bool InputLayer::key_pressed_event_handler(KeyPressedEvent& event, float dt)
  {
    auto camera = Camera::get_current_camera();

    if (camera == nullptr) { return false; }

    const float camera_speed = camera->get_move_speed() * dt;
    if (event.get_code() == GLFW_KEY_W) { camera->m_position += camera->get_front() * camera_speed; }
    if (event.get_code() == GLFW_KEY_S) { camera->m_position -= camera->get_front() * camera_speed; }
    if (event.get_code() == GLFW_KEY_A)
    {
      camera->m_position -= glm::normalize(glm::cross(camera->get_front(), camera->get_camera_up())) * camera_speed;
    }
    if (event.get_code() == GLFW_KEY_D)
    {
      camera->m_position += glm::normalize(glm::cross(camera->get_front(), camera->get_camera_up())) * camera_speed;
    }

    return true;
  }
} // namespace esp