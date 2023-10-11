
#include <stdexcept>

#include "EspertWindow.hh"
#include "Logger.hh"

namespace Esper
{
    bool EsperWindow::_s_is_exist = false;

    EsperWindow::EsperWindow(const WindowData &data)
    {
        if (EsperWindow::_s_is_exist)
        {
            throw std::runtime_error("The Esper's window already exists!");
        }
        EsperWindow::_s_is_exist = true;

        EsperWindow::init(data);
        EP_CORE_INFO("window created: w {}, h {}, t {}", _m_data.width, _m_data.height, _m_data.title);
    }

    EsperWindow::~EsperWindow()
    {
        EsperWindow::_s_is_exist = false;
        destroy();
    }

    std::unique_ptr<EsperWindow> EsperWindow::create(const WindowData &data)
    {
        std::unique_ptr<EsperWindow> window{new EsperWindow(data)};
        return window;
    }

    void EsperWindow::on_update()
    {
        glfwPollEvents();
    }

    void EsperWindow::init(const WindowData &data)
    {
        _m_data = data;

        int success = glfwInit();
        if (success == GLFW_FALSE)
        {
            EP_CORE_ERROR("GLFW cannot be init");
            throw std::runtime_error("GLFW cannot be init");
        }

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        _m_window = glfwCreateWindow(_m_data.width, _m_data.height, _m_data.title.c_str(), nullptr, nullptr);
        if (_m_window == NULL)
        {
            EP_CORE_ERROR("GLFW cannot create a window instance");
            throw std::runtime_error("GLFW cannot create a window instance");
        }
    }

    void EsperWindow::destroy()
    {
        glfwDestroyWindow(_m_window);
    }

} // namespace Esper
