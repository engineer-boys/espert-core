#ifndef SYSTEMS_RENDER_ESP_RENDER_SYSTEM_H_
#define SYSTEMS_RENDER_ESP_RENDER_SYSTEM_H_

#include "RenderAPI/EspPipeline.hh"
#include "RenderAPI/EspRenderContext.hh"

namespace esp
{
	class EspRenderSystem
	{
	 protected:
		EspDevice& m_device;
		std::unique_ptr<EspPipeline> m_pipeline;
		std::unique_ptr<EspPipelineLayout> m_pipeline_layout;

	 public:
		EspRenderSystem(EspDevice& device) : m_device{device} {}
		inline virtual ~EspRenderSystem()
		{
			vkDestroyPipelineLayout(m_device.get_device(), m_pipeline_layout->get_layout(), nullptr);
		}

		EspRenderSystem(const EspRenderSystem&) = delete;
		EspRenderSystem& operator=(const EspRenderSystem&) = delete;

		virtual void render(EspRenderContext& render_context) = 0;
	};
}

#endif //SYSTEMS_RENDER_ESP_RENDER_SYSTEM_H_
