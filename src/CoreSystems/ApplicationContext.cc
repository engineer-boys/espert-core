#include "ApplicationContext.hh"

namespace Espert
{
    bool ApplicationContext::_s_is_exist = false;

    ApplicationContext::ApplicationContext()
    {
        if (ApplicationContext::_s_is_exist)
        {
            throw std::runtime_error("The application contex already exists!");
        }

        ApplicationContext::_s_is_exist = true;
    }

    ApplicationContext::~ApplicationContext()
    {
        ApplicationContext::_s_is_exist = false;
    }

    std::unique_ptr<ApplicationContext> ApplicationContext::create()
    {
        auto context = std::unique_ptr<ApplicationContext>{new ApplicationContext()};

        /* create context dependencies */
        {
            context->_m_logger = Logger::create();
        }
        /* end of creating context dependencies */

        return context;
    }

}  // namespace Espert
