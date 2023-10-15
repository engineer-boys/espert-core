#ifndef ESPERT_CORE_APPLICATIONCONTEXT_HH_
#define ESPERT_CORE_APPLICATIONCONTEXT_HH_

#include "esppch.hh"

namespace esp
{
    class ApplicationContext
    {
    private:
        static bool _s_is_exist;

    private:
        std::unique_ptr<Logger> _m_logger;

        ApplicationContext();

    public:
        ~ApplicationContext();

        static std::unique_ptr<ApplicationContext> create();
    };

} // namespace Espert

#endif  // ESPERT_CORE_APPLICATIONCONTEXT_HH_
