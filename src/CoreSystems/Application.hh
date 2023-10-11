#ifndef __ENGINE__APPLICATION_HH__
#define __ENGINE__APPLICATION_HH__

#include "esppch.hh"
#include "ContextApplication.hh"
#include "Events/WindowEvent.h"
#include "EspertWindow.hh"

namespace Esper
{
    class Application
    {
    private:
        std::unique_ptr<ContextApplication> _m_context;
        std::unique_ptr<EsperWindow> _m_window;

        bool _m_running;

    private:
        bool on_window_closed(WindowClosedEvent &e);

    public:
        Application();
        virtual ~Application();

        void run();
        void set_context(std::unique_ptr<ContextApplication> context);

        void events_manager(Event &e);
    };

    /* This function is defined by CLIENT */
    Application *createAppInstance();

} // namespace  Esper

#endif /* __ENGINE__APPLICATION_HH__ */
