#include "Application.hh"

#define BIND_EVENT_FOR_FUN(fun) std::bind(&Application::fun, this, std::placeholders::_1)

namespace Espert
{
    Application::Application() : _m_running(true)
    {
        _m_window = EspertWindow::create(EspertWindow::WindowData());
        _m_window->set_events_manager_fun(BIND_EVENT_FOR_FUN(events_manager));
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

    void Application::set_context(std::unique_ptr<ApplicationContext> context)
    {
        this->_m_context = std::move(context);
    }

    bool Application::on_window_closed(WindowClosedEvent &e)
    {
        _m_running = false;
        return true;
    }

    void Application::events_manager(Event &e)
    {
        Event::try_hanlder<WindowClosedEvent>(e, BIND_EVENT_FOR_FUN(on_window_closed));
    }
} // namespace Espert
