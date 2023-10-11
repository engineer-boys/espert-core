#ifndef __ENGINE__APPLICATION_HH__
#define __ENGINE__APPLICATION_HH__

#include <memory>
#include "ContextApplication.hh"
#include "EspertWindow.hh"

namespace Esper
{
    class Application
    {
    private:
        std::unique_ptr<ContextApplication> _m_context;
        std::unique_ptr<EsperWindow> _m_window;

        bool _m_running;

    public:
        Application();
        virtual ~Application();

        void run();
        void set_context(std::unique_ptr<ContextApplication> context);
    };

    /* This function is defined by CLIENT */
    Application *createAppInstance();

} // namespace  Esper

#endif /* __ENGINE__APPLICATION_HH__ */
