#ifndef ESPERT_CORE_APPLICATION_HH
#define ESPERT_CORE_APPLICATION_HH

#include "ApplicationContext.hh"
#include "EspWindow.hh"
#include "Events/WindowEvent.hh"
#include "Layers/Layer.hh"
#include "Layers/LayerStack.hh"
#include "RenderAPI/EspCommandHandler.hh"
#include "RenderAPI/EspFrameManager.hh"
#include "RenderAPI/EspRenderContext.hh"
#include "esppch.hh"

namespace esp
{
  class Application
  {
   private:
    std::unique_ptr<ApplicationContext> m_context;
    std::unique_ptr<EspWindow> m_window;

    std::unique_ptr<EspRenderContext> m_render_context;
    std::unique_ptr<EspCommandHandler> m_command_handler;
    std::unique_ptr<EspFrameManager> m_frame_manager;

    bool m_running;

    LayerStack* m_layer_stack;

   private:
    bool on_window_resized(WindowResizedEvent& e);
    bool on_window_closed(WindowClosedEvent& e);

   protected:
    inline EspWindow& get_window() { return *m_window; }

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
  Application* create_app_instance();

} // namespace esp

#endif // ESPERT_CORE_APPLICATION_HH
