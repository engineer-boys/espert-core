#include "Application.hh"
#include "VulkanRenderAPI/EspRenderContext.hh"
#include <ranges>

namespace esp
{
    Application::Application() : _m_running(true)
    {
		_m_window = EspWindow::create(EspWindow::WindowData());
        _m_window->set_events_manager_fun(ESP_BIND_EVENT_FOR_FUN(Application::events_manager));

		_m_layer_stack = new LayerStack();
    }

    Application::~Application()
    {
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

		delete _m_layer_stack;
    }

    void Application::set_context(std::unique_ptr<ApplicationContext> context)
    {
        this->_m_context = std::move(context);
    }

	bool Application::on_window_resized(WindowResizedEvent& e)
	{
		return true;
	}

    bool Application::on_window_closed(WindowClosedEvent &e)
    {
        _m_running = false;
		//TODO: abstract this call
		EspRenderContext::get_device().complete_queues();
		//
        return true;
    }

    void Application::events_manager(Event &e)
    {
		Event::try_hanlder<WindowResizedEvent>(e, ESP_BIND_EVENT_FOR_FUN(Application::on_window_resized));
        Event::try_hanlder<WindowClosedEvent>(e, ESP_BIND_EVENT_FOR_FUN(Application::on_window_closed));
        
        for (auto& iter : *_m_layer_stack | std::views::reverse)
        {
            iter->handle_event(e);
            if (e.handled)
            {
                break;
            }
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
} // namespace Espert