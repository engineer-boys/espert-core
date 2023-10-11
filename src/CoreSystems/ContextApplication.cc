#include "ContextApplication.hh"

namespace Esper
{
    bool ContextApplication::_s_is_exist = false;

    ContextApplication::ContextApplication()
    {
        if (ContextApplication::_s_is_exist)
        {
            throw std::runtime_error("The application contex already exists!");
        }

        ContextApplication::_s_is_exist = true;
    }

    ContextApplication::~ContextApplication()
    {
        ContextApplication::_s_is_exist = false;
    }

    std::unique_ptr<ContextApplication> ContextApplication::create()
    {
        auto context = std::unique_ptr<ContextApplication>{new ContextApplication()};

        /* create context dependencies */
        {
            context->_m_logger = Logger::create();
        }
        /* end of creating context dependencies */

        return context;
    }

} // namespace Esper
