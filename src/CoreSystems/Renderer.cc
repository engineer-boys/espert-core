#include "Renderer.hh"

namespace esp
{
	Renderer* Renderer::s_instance = nullptr;

	Renderer::Renderer(EspWindow& window)
		: m_device{window}, m_render_scheduler{window, m_device}
	{
		if (Renderer::s_instance != nullptr)
		{
			throw std::runtime_error("The renderer instance already exists!");
		}

		Renderer::s_instance = this;
	}
	Renderer::~Renderer()
	{
		s_instance = nullptr;
	}

	std::unique_ptr<Renderer> Renderer::create(EspWindow& window)
	{
		/* create singleton */
		auto renderer = std::unique_ptr<Renderer>(new Renderer(window));

		ESP_CORE_INFO("Renderer is created");
		return renderer;
	}
}