#ifndef ESPERT_CORE_APPLICATION_HH
#define ESPERT_CORE_APPLICATION_HH

#include "esppch.hh"

#include "ApplicationContext.hh"
#include "Core/Resources/Systems/ResourceSystem.hh"
#include "Core/Resources/Systems/TextureSystem.hh"
#include "EspWindow.hh"
#include "Events/WindowEvent.hh"
#include "Layers/Layer.hh"
#include "Layers/LayerStack.hh"
#include "RenderAPI/EspDebugMessenger.hh"
#include "RenderAPI/EspRenderContext.hh"
#include "RenderAPI/Work/EspJobs.hh"
#include "RenderAPI/Work/EspWorkOrchestrator.hh"
#include "Timer.hh"

namespace esp
{
  class Application
  {
    /* -------------------------- FIELDS ----------------------------------- */
   private:
    std::unique_ptr<ApplicationContext> m_context;
    std::unique_ptr<EspWindow> m_window;
    std::unique_ptr<Timer> m_timer;

    std::unique_ptr<EspDebugMessenger> m_debug_messenger;
    std::unique_ptr<ResourceSystem> m_resource_system;
    std::unique_ptr<TextureSystem> m_texture_system;

    LayerStack* m_layer_stack;

   protected:
    struct
    {
      std::unique_ptr<EspRenderContext> m_render_context;
      std::unique_ptr<EspJobs> m_jobs;
      std::unique_ptr<EspWorkOrchestrator> m_work_orchestrator;

      void terminate()
      {
        m_jobs->terminate();
        m_work_orchestrator->terminate();
        m_render_context->terminate();
      }

      inline void done_all_jobs() { m_jobs->done_all_jobs(); }
    } m_renderer;

    bool m_running;

    /* -------------------------- METHODS ---------------------------------- */
   private:
    bool on_window_resized(WindowResizedEvent& e);
    bool on_window_closed(WindowClosedEvent& e);

    void add_application_layers();

   protected:
    inline EspWindow& get_window() { return *m_window; }

   public:
    Application(const std::string title = "Espert window", unsigned int width = 1280, unsigned int height = 720);
    virtual ~Application();

    void push_layer(Layer* layer);
    void push_overlayer(Layer* layer);

    void run();
    void set_context(std::unique_ptr<ApplicationContext> context);
    void events_manager(Event& e);
  };

  /* This function is defined by CLIENT */
  Application* create_app_instance();

} // namespace esp

#endif // ESPERT_CORE_APPLICATION_HH
