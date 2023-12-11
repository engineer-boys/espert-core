#include "Application.hh"
#include "Layers/InputLayer.hh"
#include <ranges>

namespace esp
{
  Application::Application() : m_running(true)
  {
    m_window = EspWindow::create(EspWindow::WindowData());
    m_window->set_events_manager_fun(ESP_BIND_EVENT_FOR_FUN(Application::events_manager));

    m_timer = Timer::create();

    m_debug_messenger = EspDebugMessenger::create();
    m_render_context  = EspRenderContext::create_and_init(*m_window);
    m_debug_messenger->init();
    m_command_handler = EspCommandHandler::create_and_init();
    m_frame_manager   = EspFrameManager::create_and_init(*m_window);

    m_layer_stack = new LayerStack();
    add_application_layers();
  }

  Application::~Application()
  {
    m_frame_manager->terminate();
    m_command_handler->terminate();
    m_debug_messenger->terminate();

    delete m_layer_stack;

    m_render_context->terminate();
  }

  void Application::run()
  {
    while (m_running)
    {
      m_timer->tick();
      if (m_timer->get_dt() < ESP_MIN_FRAME_RATE) continue;
      m_timer->reset();

      m_frame_manager->begin_frame();

      for (auto layer : *m_layer_stack)
      {
        layer->update(m_timer->get_dt());
      }

      m_frame_manager->end_frame();

      m_window->update();
    }
  }

  void Application::set_context(std::unique_ptr<ApplicationContext> context) { this->m_context = std::move(context); }

  bool Application::on_window_resized(WindowResizedEvent& e)
  {
    m_frame_manager->on_window_resized(e);
    return true;
  }

  bool Application::on_window_closed(WindowClosedEvent& e)
  {
    m_running = false;
    return true;
  }

  void Application::add_application_layers() { push_layer(new InputLayer()); }

  void Application::events_manager(Event& e)
  {
    Event::try_handler<WindowResizedEvent>(e, ESP_BIND_EVENT_FOR_FUN(Application::on_window_resized));
    Event::try_handler<WindowClosedEvent>(e, ESP_BIND_EVENT_FOR_FUN(Application::on_window_closed));

    for (auto& iter : *m_layer_stack | std::views::reverse)
    {
      iter->handle_event(e, m_timer->get_dt());
      if (e.handled) { break; }
    }
  }

  void Application::push_layer(Layer* layer) { m_layer_stack->push_layer(layer); }

  void Application::push_overlayer(Layer* layer) { m_layer_stack->push_overlayer(layer); }
} // namespace esp
