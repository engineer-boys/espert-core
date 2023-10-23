#include "Application.hh"
#include <ranges>

namespace esp
{
  Application::Application() : _m_running(true)
  {
    MiniR_context_glfw_hints();
    _m_window = EspWindow::create(EspWindow::WindowData());
    _m_window->set_events_manager_fun(
        ESP_BIND_EVENT_FOR_FUN(Application::events_manager));

    _m_layer_stack = new LayerStack();

    m_minir_context = MiniRContext::create_and_init(*(_m_window.get()));
  }

  Application::~Application()
  {
    delete _m_layer_stack;
    /*
      They will be deleted by unique_ptr automatically:
      - window
      - mnir context
    */
  }

  void Application::run()
  {
    while (_m_running)
    {
      for (auto layer : *_m_layer_stack)
      {
        layer->update();
      }

      _m_window->update();
    }
  }

  void Application::set_context(std::unique_ptr<ApplicationContext> context)
  {
    this->_m_context = std::move(context);
  }

  bool Application::on_window_resized(WindowResizedEvent& e) { return true; }

  bool Application::on_window_closed(WindowClosedEvent& e)
  {
    _m_running = false;
    return true;
  }

  void Application::events_manager(Event& e)
  {
    Event::try_hanlder<WindowResizedEvent>(
        e,
        ESP_BIND_EVENT_FOR_FUN(Application::on_window_resized));
    Event::try_hanlder<WindowClosedEvent>(
        e,
        ESP_BIND_EVENT_FOR_FUN(Application::on_window_closed));

    for (auto& iter : *_m_layer_stack | std::views::reverse)
    {
      iter->handle_event(e);
      if (e.handled) { break; }
    }
  }

  void Application::push_layer(Layer* layer)
  {
    _m_layer_stack->push_layer(layer);
  }

  void Application::push_overlayer(Layer* layer)
  {
    _m_layer_stack->push_overlayer(layer);
  }
} // namespace esp
