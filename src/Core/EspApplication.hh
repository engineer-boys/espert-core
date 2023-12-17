#ifndef ESPERT_CORE_APPLICATION_HH
#define ESPERT_CORE_APPLICATION_HH

#include "esppch.hh"

#include "Core/Resources/Systems/MaterialSystem.hh"
#include "Core/Resources/Systems/ResourceSystem.hh"
#include "Core/Resources/Systems/ShaderSystem.hh"
#include "Core/Resources/Systems/TextureSystem.hh"
#include "EspApplicationContext.hh"
#include "EspWindow.hh"
#include "Events/WindowEvent.hh"
#include "Layers/Layer.hh"
#include "Layers/LayerStack.hh"
#include "RenderAPI/EspDebugMessenger.hh"
#include "RenderAPI/EspRenderContext.hh"
#include "RenderAPI/Work/EspJob.hh"
#include "RenderAPI/Work/EspWorkOrchestrator.hh"
#include "Utils/Timer.hh"

namespace esp
{
  class EspApplication
  {
    /* -------------------------- FIELDS ----------------------------------- */
   protected:
    std::unique_ptr<EspApplicationContext> m_context;
    std::unique_ptr<EspWindow> m_window;
    std::unique_ptr<Timer> m_timer;

    std::unique_ptr<EspDebugMessenger> m_debug_messenger;
    std::unique_ptr<ResourceSystem> m_resource_system;
    std::unique_ptr<TextureSystem> m_texture_system;
    std::unique_ptr<ShaderSystem> m_shader_system;
    std::unique_ptr<MaterialSystem> m_material_system;
<<<<<<< HEAD:src/Core/EspApplication.hh
=======

    static fs::path s_asset_base_path;
>>>>>>> e6fdd16 (minor fixes):src/Core/Application.hh

    LayerStack* m_layer_stack;

   protected:
    struct
    {
      std::unique_ptr<EspRenderContext> m_render_context;
      std::unique_ptr<EspJob> m_jobs;
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
    static fs::path s_asset_base_path;

    /* -------------------------- METHODS ---------------------------------- */
   private:
    bool on_window_closed(WindowClosedEvent& e);

    void add_application_layers();

   protected:
    inline EspWindow& get_window() { return *m_window; }

   public:
    EspApplication(const std::string title = "Espert window", unsigned int width = 1280, unsigned int height = 720);
    virtual ~EspApplication();

    void push_layer(Layer* layer);
    void push_overlayer(Layer* layer);

    void run();
    void set_context(std::unique_ptr<EspApplicationContext> context);
    void events_manager(Event& e);
  };

  /* This function is defined by CLIENT */
  EspApplication* create_app_instance();

} // namespace esp

#endif // ESPERT_CORE_APPLICATION_HH
