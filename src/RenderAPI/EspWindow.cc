#include "EspWindow.hh"
#include "Events/Events.hh"

namespace esp
{
    static void glfw_error_callback(int error, const char *description)
    {
        ESP_CORE_ERROR("GLFW error ({0}) : {1}", errno, description);
    }

    bool EspWindow::_s_is_exist = false;

    EspWindow::EspWindow(const WindowData &data)
    {
        if (EspWindow::_s_is_exist)
        {
            throw std::runtime_error("The Espert's window already exists!");
        }
		EspWindow::_s_is_exist = true;

        EspWindow::init(data);
        ESP_CORE_INFO("window created: w {}, h {}, t {}", _m_data.m_width, _m_data.m_height, _m_data.m_title);
    }

    EspWindow::~EspWindow()
    {
		EspWindow::_s_is_exist = false;
        destroy();
    }

    std::unique_ptr<EspWindow> EspWindow::create(const WindowData &data)
    {
        std::unique_ptr<EspWindow> window{ new EspWindow(data)};
        return window;
    }

    void EspWindow::update()
    {
        glfwPollEvents();
    }

    void EspWindow::init(const WindowData &data)
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
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

        _m_window = glfwCreateWindow(_m_data.m_width, _m_data.m_height, _m_data.m_title.c_str(), nullptr, nullptr);
        if (_m_window == NULL)
        {
            ESP_CORE_ERROR("GLFW cannot create a window instance");
            throw std::runtime_error("GLFW cannot create a window instance");
        }
        glfwSetWindowUserPointer(_m_window, &_m_data);

        set_callbacks();
    }

    void EspWindow::destroy()
    {
        glfwDestroyWindow(_m_window);
        glfwTerminate();
    }

	void EspWindow::set_callbacks()
	{
		/* set callbacks for glfw events */
		glfwSetWindowSizeCallback(
			_m_window, [](GLFWwindow *window, int width, int height)
			{
			  WindowData *data = static_cast<WindowData *>(glfwGetWindowUserPointer(window));
			  data->m_width = width;
			  data->m_height = height;

			  WindowResizedEvent event(width, height);
			  data->m_events_manager_fun(event);
			});

		glfwSetWindowCloseCallback(
			_m_window, [](GLFWwindow *window)
			{
			  WindowData *data = static_cast<WindowData *>(glfwGetWindowUserPointer(window));
			  WindowClosedEvent event;
			  data->m_events_manager_fun(event);
			});

		glfwSetKeyCallback(
			_m_window, [](GLFWwindow *window, int key, int scancode, int action, int mods)
			{
			  WindowData *data = static_cast<WindowData *>(glfwGetWindowUserPointer(window));

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
				  case GLFW_REPEAT:
				  {
					  KeyPressedEvent event(key, true);
					  data->m_events_manager_fun(event);
					  break;
				  }
			  }
			});

		glfwSetMouseButtonCallback(
			_m_window, [](GLFWwindow *window, int button, int action, int mods)
			{
			  WindowData *data = static_cast<WindowData *>(glfwGetWindowUserPointer(window));

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

		glfwSetScrollCallback(
			_m_window, [](GLFWwindow *window, double x_offset, double y_offset)
			{
			  WindowData *data = static_cast<WindowData *>(glfwGetWindowUserPointer(window));

			  MouseScrolledEvent event((float)x_offset, (float)y_offset);
			  data->m_events_manager_fun(event);
			});

		glfwSetCursorPosCallback(
			_m_window, [](GLFWwindow *window, double x_pos, double y_pos)
			{
			  WindowData *data = static_cast<WindowData *>(glfwGetWindowUserPointer(window));

			  MouseMovedEvent event((float)x_pos, (float)y_pos);
			  data->m_events_manager_fun(event);
			});
	}

	void EspWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface)
	{
		if (glfwCreateWindowSurface(instance, _m_window, nullptr, surface) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create window surface");
		}
	}
}  // namespace Espert
