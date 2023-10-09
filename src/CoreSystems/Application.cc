#include "Application.hh"

namespace Esper
{
    Application::Application()
    {
    }

    Application::~Application()
    {
    }

    void Application::run()
    {
    }

    void Application::set_context(std::unique_ptr<ContextApplication> context)
    {
        this->_m_context = std::move(context);
    }
} // namespace Esper
