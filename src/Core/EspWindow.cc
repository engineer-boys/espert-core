#include "EspWindow.hh"
#include "Events/Events.hh"
#include "Platform/Vulkan/VulkanContext.hh"

static void glfw_error_callback(int error, const char* description)
{
  ESP_CORE_ERROR("GLFW error ({0}) : {1}", errno, description);
}

namespace esp
{
  EspWindow* EspWindow::s_instance = nullptr;

  std::unique_ptr<EspWindow> EspWindow::create(WindowData* data)
  {
    ESP_ASSERT(EspWindow::s_instance == nullptr, "The Espert window already exists!");
    EspWindow::s_instance = new EspWindow(data);
    EspWindow::s_instance->init();

    return std::unique_ptr<EspWindow>{ EspWindow::s_instance };
  }

  EspWindow::EspWindow(WindowData* data)
  {
    /* --- set fields --- */
    m_data = std::unique_ptr<WindowData>{ data };
  }

  EspWindow::~EspWindow() {}

  void EspWindow::init()
  {
    int success = glfwInit();
    if (success == GLFW_FALSE)
    {
      ESP_CORE_ERROR("GLFW cannot be init");
      throw std::runtime_error("GLFW cannot be init");
    }

    glfwSetErrorCallback(glfw_error_callback);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    // This is false because resizing all buffers from
    // all render passing may be inefficient.
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    m_window = glfwCreateWindow(m_data->m_width, m_data->m_height, m_data->m_title.c_str(), nullptr, nullptr);
    if (m_window == NULL)
    {
      ESP_CORE_ERROR("GLFW cannot create a window instance");
      throw std::runtime_error("GLFW cannot create a window instance");
    }
    glfwSetWindowUserPointer(m_window, m_data.get());

    set_presentation_mode();

    if (m_data->m_disable_cursor) { glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); }

    set_callbacks();
    ESP_CORE_INFO("window created: w {}, h {}, t {}", m_data->m_width, m_data->m_height, m_data->m_title);
  }

  void EspWindow::terminate()
  {
    ESP_ASSERT(EspWindow::s_instance != nullptr, "The Espert window is deleted twice!");

    glfwDestroyWindow(m_window);
    glfwTerminate();

    EspWindow::s_instance = nullptr;
  }

  void EspWindow::update()
  {
    glfwPollEvents();
  }

  void EspWindow::set_callbacks()
  {
    /* set callbacks for glfw events */
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

  void EspWindow::set_presentation_mode()
  {
    /* ---------------------------------------------------------*/
    /* ------------- PLATFORM DEPENDENT ------------------------*/
    /* ---------------------------------------------------------*/
    // #if defined(OPENGL_PLATFORM)
    //     if (m_data->m_presentation_mode == EspPresentationMode::ESP_PRESENT_MODE_IMMEDIATE_KHR)
    //     {
    //       glfwMakeContextCurrent(m_window);
    //       glfwSwapInterval(0);
    //     }
    // #elif defined(VULKAN_PLATFORM)
    // #else
    // #error Unfortunatelly, neither Vulkan nor OpenGL is supported.
    // #endif
    /* ---------------------------------------------------------*/
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
