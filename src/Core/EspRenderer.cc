#include "EspRenderer.hh"

namespace esp
{
	EspRenderer* EspRenderer::s_instance = nullptr;
	std::unordered_map<TagComponent::TAG, EspRenderSystem&> EspRenderer::s_render_systems{};

	EspRenderer::EspRenderer(EspWindow& window) : m_device{window}, m_context{m_device}
	{
		m_render_scheduler = std::make_unique<EspRenderScheduler>(window, m_device);

		m_context.m_render_pass = m_render_scheduler->get_swap_chain_render_pass();

		if (EspRenderer::s_instance != nullptr)
		{
			throw std::runtime_error("The renderer instance already exists!");
		}

		EspRenderer::s_instance = this;
	}
	EspRenderer::~EspRenderer()
	{
		m_device.complete_queues();

		s_instance = nullptr;
	}

	std::unique_ptr<EspRenderer> EspRenderer::create(EspWindow& window)
	{
		/* create singleton */
		auto renderer = std::unique_ptr<EspRenderer>(new EspRenderer(window));

		ESP_CORE_INFO("Renderer created");
		return renderer;
	}

	void EspRenderer::add_render_system(const TagComponent::TAG& tag, EspRenderSystem&& system)
	{
		s_render_systems.insert({ tag, system});
	}

	void EspRenderer::begin_scene()
	{
		if (auto command_buffer = m_render_scheduler->begin_frame())
		{
			m_context.m_command_buffer = command_buffer;
		}
	}

	void EspRenderer::end_scene()
	{
		m_render_scheduler->begin_swap_chain_render_pass(m_context.m_command_buffer);

		for (auto& kv : s_render_systems)
		{
			EspRenderSystem& system = kv.second;
			system.render(m_context);
		}

		m_render_scheduler->end_swap_chain_render_pass(m_context.m_command_buffer);
		m_render_scheduler->end_frame();
	}

	void EspRenderer::on_window_resized()
	{
		m_render_scheduler->recreate_swap_chain();

		m_context.m_render_pass = m_render_scheduler->get_swap_chain_render_pass();
	}
}