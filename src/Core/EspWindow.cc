#include "EspWindow.hh"
#include "Events/Events.hh"
#include "Platform/Vulkan/VulkanContext.hh"

namespace esp
{
  static void glfw_error_callback(int error, const char* description)
  {
    ESP_CORE_ERROR("GLFW error ({0}) : {1}", errno, description);
  }

  bool EspWindow::s_is_exist = false;

  EspWindow::EspWindow(const WindowData& data)
  {
    if (EspWindow::s_is_exist) { throw std::runtime_error("The Espert window already exists!"); }
    EspWindow::s_is_exist = true;

    EspWindow::init(data);
    ESP_CORE_INFO("window created: w {}, h {}, t {}", m_data.m_width, m_data.m_height, m_data.m_title);
  }

  EspWindow::~EspWindow()
  {
    EspWindow::s_is_exist = false;
    destroy();
  }

  std::unique_ptr<EspWindow> EspWindow::create(const WindowData& data)
  {
    std::unique_ptr<EspWindow> window{ new EspWindow(data) };
    return window;
  }

  void EspWindow::update()
  {
    glfwPollEvents();
    handle_keys_presses();
  }

  void EspWindow::init(const WindowData& data)
  {
    m_data = data;

    int success = glfwInit();
    if (success == GLFW_FALSE)
    {
      ESP_CORE_ERROR("GLFW cannot be init");
      throw std::runtime_error("GLFW cannot be init");
    }

    glfwSetErrorCallback(glfw_error_callback);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    m_window = glfwCreateWindow(m_data.m_width, m_data.m_height, m_data.m_title.c_str(), nullptr, nullptr);
    if (m_window == NULL)
    {
      ESP_CORE_ERROR("GLFW cannot create a window instance");
      throw std::runtime_error("GLFW cannot create a window instance");
    }
    glfwSetWindowUserPointer(m_window, &m_data);

    // TODO: set flag for cursor disabled
    // glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    //

    set_callbacks();
  }

  void EspWindow::destroy()
  {
    glfwDestroyWindow(m_window);
    glfwTerminate();
  }

  void EspWindow::set_callbacks()
  {
    /* set callbacks for glfw events */
    glfwSetWindowSizeCallback(m_window,
                              [](GLFWwindow* window, int width, int height)
                              {
                                WindowData* data = static_cast<WindowData*>(glfwGetWindowUserPointer(window));
                                data->m_width    = width;
                                data->m_height   = height;

                                WindowResizedEvent event(width, height);
                                data->m_events_manager_fun(event);
                              });

    glfwSetWindowCloseCallback(m_window,
                               [](GLFWwindow* window)
                               {
                                 WindowData* data = static_cast<WindowData*>(glfwGetWindowUserPointer(window));
                                 WindowClosedEvent event;
                                 data->m_events_manager_fun(event);
                               });

    glfwSetKeyCallback(m_window,
                       [](GLFWwindow* window, int key, int scancode, int action, int mods)
                       {
                         WindowData* data = static_cast<WindowData*>(glfwGetWindowUserPointer(window));

                         switch (action)
                         {
                         case GLFW_PRESS:
                         {
                           KeyPressedEvent event(key, false);
                           data->m_events_manager_fun(event);
                           break;
                         }
                         case GLFW_RELEASE:
                         {
                           KeyReleasedEvent event(key);
                           data->m_events_manager_fun(event);
                           break;
                         }
                         }
                       });

    glfwSetMouseButtonCallback(m_window,
                               [](GLFWwindow* window, int button, int action, int mods)
                               {
                                 WindowData* data = static_cast<WindowData*>(glfwGetWindowUserPointer(window));

                                 switch (action)
                                 {
                                 case GLFW_PRESS:
                                 {
                                   MouseButtonPressedEvent event(button);
                                   data->m_events_manager_fun(event);
                                   break;
                                 }
                                 case GLFW_RELEASE:
                                 {
                                   MouseButtonReleasedEvent event(button);
                                   data->m_events_manager_fun(event);
                                   break;
                                 }
                                 }
                               });

    glfwSetScrollCallback(m_window,
                          [](GLFWwindow* window, double x_offset, double y_offset)
                          {
                            WindowData* data = static_cast<WindowData*>(glfwGetWindowUserPointer(window));

                            MouseScrolledEvent event((float)x_offset, (float)y_offset);
                            data->m_events_manager_fun(event);
                          });

    glfwSetCursorPosCallback(m_window,
                             [](GLFWwindow* window, double x_pos, double y_pos)
                             {
                               WindowData* data = static_cast<WindowData*>(glfwGetWindowUserPointer(window));

                               MouseMovedEvent event((float)x_pos, (float)y_pos);
                               data->m_events_manager_fun(event);
                             });
  }

  void EspWindow::handle_keys_presses()
  {
    if (glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS)
    {
      KeyPressedEvent event(GLFW_KEY_W, true);
      m_data.m_events_manager_fun(event);
    }
    if (glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS)
    {
      KeyPressedEvent event(GLFW_KEY_S, true);
      m_data.m_events_manager_fun(event);
    }
    if (glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS)
    {
      KeyPressedEvent event(GLFW_KEY_A, true);
      m_data.m_events_manager_fun(event);
    }
    if (glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS)
    {
      KeyPressedEvent event(GLFW_KEY_D, true);
      m_data.m_events_manager_fun(event);
    }
    if (glfwGetKey(m_window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
      KeyPressedEvent event(GLFW_KEY_SPACE, true);
      m_data.m_events_manager_fun(event);
    }
    if (glfwGetKey(m_window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    {
      KeyPressedEvent event(GLFW_KEY_LEFT_SHIFT, true);
      m_data.m_events_manager_fun(event);
    }
  }

  void EspWindow::create_window_surface()
  {
    /* ---------------------------------------------------------*/
    /* ------------- PLATFORM DEPENDENT ------------------------*/
    /* ---------------------------------------------------------*/
    // #if defined(OPENGL_PLATFORM)
    //     ...
    // #elif defined(VULKAN_PLATFORM)
    auto& context_data = VulkanContext::get_context_data();
    if (glfwCreateWindowSurface(context_data.m_instance,
                                m_window,
                                nullptr,
                                const_cast<VkSurfaceKHR*>(&context_data.m_surface)) != VK_SUCCESS)
    {
      ESP_CORE_ERROR("Failed to create window surface");
      throw std::runtime_error("Failed to create window surface");
    }
    // #else
    // #error Unfortunatelly, neither Vulkan nor OpenGL is supported.
    // #endif
    /* ---------------------------------------------------------*/
  }
} // namespace esp
