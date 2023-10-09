#ifndef __ENGINE__CONTEXT_APPLICATION_HH__
#define __ENGINE__CONTEXT_APPLICATION_HH__

#include <memory>
#include "Logger.hh"

namespace Esper
{
    class ContextApplication
    {
    private:
        static bool _s_is_exist;

    private:
        std::unique_ptr<Logger> _m_logger;

        ContextApplication();

    public:
        ~ContextApplication();

        static std::unique_ptr<ContextApplication> create();
    };

} // namespace Esper

#endif /* __ENGINE__CONTEXT_APPLICATION_HH__ */
