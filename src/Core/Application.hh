#ifndef ESPERT_CORE_APPLICATION_HH_
#define ESPERT_CORE_APPLICATION_HH_

#include "ApplicationContext.hh"
#include "EspWindow.hh"
#include "Events/WindowEvent.hh"
#include "Layers/Layer.hh"
#include "Layers/LayerStack.hh"
#include "MiniRAPI/MiniRContext.hh"
#include "esppch.hh"

namespace esp
{
  class Application
  {
   private:
    std::unique_ptr<ApplicationContext> _m_context;
    std::unique_ptr<EspWindow> _m_window;
    std::unique_ptr<MiniRContext> m_minir_context;

    bool _m_running;

    LayerStack* _m_layer_stack;

   private:
    bool on_window_resized(WindowResizedEvent& e);
    bool on_window_closed(WindowClosedEvent& e);

   protected:
    inline EspWindow& get_window() { return *_m_window; }

   public:
    Application();
    virtual ~Application();

    void run();
    void set_context(std::unique_ptr<ApplicationContext> context);
    void events_manager(Event& e);

    void push_layer(Layer* layer);
    void push_overlayer(Layer* layer);
  };

  /* This function is defined by CLIENT */
  Application* createAppInstance();

} // namespace esp

#endif // ESPERT_CORE_APPLICATION_HH_
