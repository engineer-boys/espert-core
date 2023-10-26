#include "Application.hh"
#include <ranges>

namespace esp
{
  Application::Application() : m_running(true)
  {
    m_window = EspWindow::create(EspWindow::WindowData());
    m_window->set_events_manager_fun(ESP_BIND_EVENT_FOR_FUN(Application::events_manager));

    m_render_context  = EspRenderContext::create_and_init(*m_window);
    m_frame_scheduler = EspFrameManager::create_and_init(*m_window, glm::vec4{ .5f, .1f, .1f, 1.f });

    m_layer_stack = new LayerStack();
  }

  Application::~Application() {}

  void Application::run()
  {
    while (m_running)
    {
      for (auto layer : *m_layer_stack)
      {
        layer->update();
      }

      m_frame_scheduler->begin_frame();
      /*...*/
      m_frame_scheduler->end_frame();

      m_window->update();
    }

    delete m_layer_stack;
  }

  void Application::set_context(std::unique_ptr<ApplicationContext> context) { this->m_context = std::move(context); }

  bool Application::on_window_resized(WindowResizedEvent& e)
  {
    m_frame_scheduler->on_window_resized(e);
    return true;
  }

  bool Application::on_window_closed(WindowClosedEvent& e)
  {
    m_running = false;

    m_frame_scheduler->terminate();
    m_render_context->terminate();

    return true;
  }

  void Application::events_manager(Event& e)
  {
    Event::try_hanlder<WindowResizedEvent>(e, ESP_BIND_EVENT_FOR_FUN(Application::on_window_resized));
    Event::try_hanlder<WindowClosedEvent>(e, ESP_BIND_EVENT_FOR_FUN(Application::on_window_closed));

    for (auto& iter : *m_layer_stack | std::views::reverse)
    {
      iter->handle_event(e);
      if (e.handled) { break; }
    }
  }

  void Application::push_layer(Layer* layer) { m_layer_stack->push_layer(layer); }

  void Application::push_overlayer(Layer* layer) { m_layer_stack->push_overlayer(layer); }
} // namespace esp
