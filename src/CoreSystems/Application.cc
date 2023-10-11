#include "Application.hh"

namespace Esper
{
    Application::Application() : _m_running(true)
    {
        _m_window = EsperWindow::create(EsperWindow::WindowData());
    }

    Application::~Application()
    {
    }

    void Application::run()
    {
        while (_m_running)
        {
            _m_window->on_update();
        }
        }

    void Application::set_context(std::unique_ptr<ContextApplication> context)
    {
        this->_m_context = std::move(context);
    }
} // namespace Esper
