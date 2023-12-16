#include "InputLayer.hh"

#include "Core/EspWindow.hh"
#include "Core/Renderer/Scene/Scene.hh"

namespace esp
{
  void InputLayer::handle_event(Event& event, float dt)
  {
    Event::try_handler<KeyPressedEvent>(event, ESP_BIND_EVENT_FOR_FUN(InputLayer::key_pressed_event_handler, dt));
    Event::try_handler<MouseMovedEvent>(event, ESP_BIND_EVENT_FOR_FUN(InputLayer::mouse_move_event_handler, dt));
    Event::try_handler<MouseScrolledEvent>(event, ESP_BIND_EVENT_FOR_FUN(InputLayer::mouse_scroll_event_handler));
  }

  bool InputLayer::key_pressed_event_handler(KeyPressedEvent& event, float dt)
  {
    auto camera = Scene::get_current_camera();

    if (camera == nullptr || !event.is_repeated()) { return false; }

    switch (event.get_code())
    {
    case GLFW_KEY_W:
      camera->move(Camera::FORWARD, dt);
      break;
    case GLFW_KEY_S:
      camera->move(Camera::BACKWARD, dt);
      break;
    case GLFW_KEY_A:
      camera->move(Camera::LEFT, dt);
      break;
    case GLFW_KEY_D:
      camera->move(Camera::RIGHT, dt);
      break;
    case GLFW_KEY_SPACE:
      camera->move(Camera::UP, dt);
      break;
    case GLFW_KEY_LEFT_SHIFT:
      camera->move(Camera::DOWN, dt);
      break;
    default:
      break;
    }

    return true;
  }

  bool InputLayer::mouse_move_event_handler(MouseMovedEvent& event, float dt)
  {
    auto camera = Scene::get_current_camera();

    if (camera == nullptr) { return false; }

    float x_pos = event.get_x();
    float y_pos = event.get_y();

    camera->look_at({ x_pos, y_pos }, dt);

    return true;
  }

  bool InputLayer::mouse_scroll_event_handler(esp::MouseScrolledEvent& event)
  {
    auto camera = Scene::get_current_camera();

    if (camera == nullptr) { return false; }

    camera->set_fov(camera->get_fov() - glm::radians(event.get_offset_y()));

    return true;
  }
} // namespace esp