#include "EspRenderer.hh"

namespace esp
{
	EspRenderer* EspRenderer::s_instance = nullptr;

	EspRenderer::EspRenderer(EspWindow& window) : m_device{ window}
	{
		m_render_scheduler = std::make_unique<EspRenderScheduler>(window, m_device);

		if (EspRenderer::s_instance != nullptr)
		{
			throw std::runtime_error("The renderer instance already exists!");
		}

		EspRenderer::s_instance = this;
	}
	EspRenderer::~EspRenderer()
	{
		s_instance = nullptr;
	}

	std::unique_ptr<EspRenderer> EspRenderer::create(EspWindow& window)
	{
		/* create singleton */
		auto renderer = std::unique_ptr<EspRenderer>(new EspRenderer(window));

		ESP_CORE_INFO("Renderer created");
		return renderer;
	}

	void EspRenderer::on_window_resized()
	{
		m_render_scheduler->recreate_swap_chain();
	}
}