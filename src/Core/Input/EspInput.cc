#include "EspInput.hh"

#include "EspWindow.hh"

namespace esp
{
  bool EspInput::is_key_pressed(int keycode)
  {
    auto window = EspWindow::get_instance()->get_window();
    auto state  = glfwGetKey(window, keycode);

    return state == GLFW_PRESS || state == GLFW_REPEAT;
  }

  bool EspInput::is_mouse_button_pressed(int button)
  {
    auto window = EspWindow::get_instance()->get_window();
    auto state  = glfwGetMouseButton(window, button);

    return state == GLFW_PRESS;
  }

  float EspInput::get_mouse_x()
  {
    auto window = EspWindow::get_instance()->get_window();
    double x_pos, y_pos;

    glfwGetCursorPos(window, &x_pos, &y_pos);

    return static_cast<float>(x_pos);
  }

  float EspInput::get_mouse_y()
  {
    auto window = EspWindow::get_instance()->get_window();
    double x_pos, y_pos;

    glfwGetCursorPos(window, &x_pos, &y_pos);

    return static_cast<float>(y_pos);
  }
} // namespace esp
