#include "EspertWindow.hh"
#include "Events/KeyEvent.hh"
#include "Events/MouseEvent.hh"
#include "Events/WindowEvent.hh"

namespace Espert
{
static void glfw_error_callback(int error, const char* description)
{
  ESP_CORE_ERROR("GLFW error ({0}) : {1}", errno, description);
}

bool EspertWindow::_s_is_exist = false;

EspertWindow::EspertWindow(const WindowData& data)
{
  if (EspertWindow::_s_is_exist)
  {
    throw std::runtime_error("The Espert's window already exists!");
  }
  EspertWindow::_s_is_exist = true;

  EspertWindow::init(data);
  ESP_CORE_INFO("window created: w {}, h {}, t {}", _m_data.width,
                _m_data.height, _m_data.title);
}

EspertWindow::~EspertWindow()
{
  EspertWindow::_s_is_exist = false;
  destroy();
}

std::unique_ptr<EspertWindow> EspertWindow::create(const WindowData& data)
{
  std::unique_ptr<EspertWindow> window{ new EspertWindow(data) };
  return window;
}

void EspertWindow::update()
{
  glfwPollEvents();
}

void EspertWindow::init(const WindowData& data)
{
  _m_data = data;

  int success = glfwInit();
  if (success == GLFW_FALSE)
  {
    ESP_CORE_ERROR("GLFW cannot be init");
    throw std::runtime_error("GLFW cannot be init");
  }

  glfwSetErrorCallback(glfw_error_callback);
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  _m_window = glfwCreateWindow(_m_data.width, _m_data.height,
                               _m_data.title.c_str(), nullptr, nullptr);
  if (_m_window == NULL)
  {
    ESP_CORE_ERROR("GLFW cannot create a window instance");
    throw std::runtime_error("GLFW cannot create a window instance");
  }
  glfwSetWindowUserPointer(_m_window, &_m_data);

  /* set callbacks for glfw events */
  {
    glfwSetWindowSizeCallback(_m_window,
                              [](GLFWwindow* window, int width, int height)
                              {
                                WindowData* data = static_cast<WindowData*>(
                                    glfwGetWindowUserPointer(window));
                                data->width  = width;
                                data->height = height;

                                WindowResizedEvent event(width, height);
                                data->events_manager_fun(event);
                              });

    glfwSetWindowCloseCallback(_m_window,
                               [](GLFWwindow* window)
                               {
                                 WindowData* data = static_cast<WindowData*>(
                                     glfwGetWindowUserPointer(window));
                                 WindowClosedEvent event;
                                 data->events_manager_fun(event);
                               });

    glfwSetKeyCallback(
        _m_window,
        [](GLFWwindow* window, int key, int scancode, int action, int mods)
        {
          WindowData* data =
              static_cast<WindowData*>(glfwGetWindowUserPointer(window));

          switch (action)
          {
          case GLFW_PRESS:
          {
            KeyPressedEvent event(key, false);
            data->events_manager_fun(event);
            break;
          }
          case GLFW_RELEASE:
          {
            KeyReleasedEvent event(key);
            data->events_manager_fun(event);
            break;
          }
          case GLFW_REPEAT:
          {
            KeyPressedEvent event(key, true);
            data->events_manager_fun(event);
            break;
          }
          }
        });

    glfwSetMouseButtonCallback(
        _m_window,
        [](GLFWwindow* window, int button, int action, int mods)
        {
          WindowData* data =
              static_cast<WindowData*>(glfwGetWindowUserPointer(window));

          switch (action)
          {
          case GLFW_PRESS:
          {
            MouseButtonPressedEvent event(button);
            data->events_manager_fun(event);
            break;
          }
          case GLFW_RELEASE:
          {
            MouseButtonReleasedEvent event(button);
            data->events_manager_fun(event);
            break;
          }
          }
        });

    glfwSetScrollCallback(
        _m_window,
        [](GLFWwindow* window, double x_offset, double y_offset)
        {
          WindowData* data =
              static_cast<WindowData*>(glfwGetWindowUserPointer(window));

          MouseScrolledEvent event((float)x_offset, (float)y_offset);
          data->events_manager_fun(event);
        });

    glfwSetCursorPosCallback(
        _m_window,
        [](GLFWwindow* window, double x_pos, double y_pos)
        {
          WindowData* data =
              static_cast<WindowData*>(glfwGetWindowUserPointer(window));

          MouseMovedEvent event((float)x_pos, (float)y_pos);
          data->events_manager_fun(event);
        });
  }
}

void EspertWindow::destroy()
{
  glfwDestroyWindow(_m_window);
  glfwTerminate();
}

} // namespace Espert
