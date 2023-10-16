#ifndef RENDERER_RENDER_CONTEXT_H_
#define RENDERER_RENDER_CONTEXT_H_

#include "EspSwapChain.hh"

namespace esp
{
	struct EspRenderContext
	{
		//EspWindow& m_window;
		EspDevice& m_device;

		//TODO: try to encapsulate those values
		VkRenderPass m_render_pass = nullptr;
		VkCommandBuffer m_command_buffer = nullptr;
		//

		// EspScene& scene;
		// EspCamera& camera
	};
}

#endif //RENDERER_RENDER_CONTEXT_H_
