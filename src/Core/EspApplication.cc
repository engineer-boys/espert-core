#include "EspApplication.hh"
#include <ranges>

namespace esp
{
  EspApplication::EspApplication(const std::string title, unsigned int width, unsigned int height) : m_running(true)
  {
    // create (alloc and init) window
    m_window = EspWindow::create(new EspWindow::WindowData(title, width, height));
    m_window->set_events_manager_fun(ESP_BIND_EVENT_FOR_FUN(EspApplication::events_manager));

    // create (alloc and init) timer
    m_timer = Timer::create();

    // set temporary m_renderer struct
    m_debug_messenger           = EspDebugMessenger::create();
    m_renderer.m_render_context = EspRenderContext::build(*m_window);
    m_debug_messenger->init();

    m_renderer.m_work_orchestrator = EspWorkOrchestrator::build();
    m_renderer.m_jobs              = EspJob::build();

    // create basic systems
    m_resource_system = ResourceSystem::create(fs::current_path().parent_path() / "resources");
    m_texture_system  = TextureSystem::create();

    // create (alloc and init) layer stacks
    m_layer_stack = new LayerStack();
  }

  EspApplication::~EspApplication()
  {
    // all jobs, which are processing by renderer, must be completed
    // before making free other resources.
    m_renderer.done_all_jobs();

    // [1] resources from layers should be deleted because they may depend on
    // other resources allocated by Application.
    delete m_layer_stack;

    // [2] terminate all systems
    m_texture_system->terminate();
    m_resource_system->terminate();

    // [3] m_renderer has to be killed and then debug messenger
    m_debug_messenger->terminate();
    m_renderer.terminate();

    // [4] Window instance has to be killed
    m_window->terminate();

    // the last one is Application context to be killed (implicitely)
  }

  void EspApplication::run()
  {
    while (m_running)
    {
      m_timer->tick();
      if (m_timer->get_dt() < ESP_MIN_FRAME_RATE) continue;
      m_timer->reset();

      m_renderer.m_work_orchestrator->begin_frame();

      for (auto layer : *m_layer_stack)
      {
        layer->update(m_timer->get_dt());
      }

      m_renderer.m_work_orchestrator->end_frame();

      m_window->update();
    }
  }

  void EspApplication::set_context(std::unique_ptr<EspApplicationContext> context)
  {
    this->m_context = std::move(context);
  }

  bool EspApplication::on_window_closed(WindowClosedEvent& e)
  {
    m_running = false;
    return true;
  }

  void EspApplication::events_manager(Event& e)
  {
    Event::try_handler<WindowClosedEvent>(e, ESP_BIND_EVENT_FOR_FUN(EspApplication::on_window_closed));

    for (auto& iter : *m_layer_stack | std::views::reverse)
    {
      iter->handle_event(e, m_timer->get_dt());
      if (e.handled) { break; }
    }
  }

  void EspApplication::push_layer(Layer* layer) { m_layer_stack->push_layer(layer); }

  void EspApplication::push_overlayer(Layer* layer) { m_layer_stack->push_overlayer(layer); }
} // namespace esp