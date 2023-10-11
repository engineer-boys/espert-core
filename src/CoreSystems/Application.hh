#ifndef ESPERT_CORESYSTEMS_APPLICATION_HH_
#define ESPERT_CORESYSTEMS_APPLICATION_HH_

#include "esppch.hh"
#include "ApplicationContext.hh"
#include "Events/WindowEvent.h"
#include "EspertWindow.hh"

namespace Espert
{
    class Application
    {
    private:
        std::unique_ptr<ApplicationContext> _m_context;
        std::unique_ptr<EspertWindow> _m_window;

        bool _m_running;

    private:
        bool on_window_closed(WindowClosedEvent &e);

    public:
        Application();
        virtual ~Application();

        void run();
        void set_context(std::unique_ptr<ApplicationContext> context);

        void events_manager(Event &e);
    };

    /* This function is defined by CLIENT */
    Application *createAppInstance();

} // namespace Espert

#endif  // ESPERT_CORESYSTEMS_APPLICATION_HH_
