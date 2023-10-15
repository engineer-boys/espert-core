#ifndef RENDERER_RENDER_CONTEXT_H_
#define RENDERER_RENDER_CONTEXT_H_

#include "EspSwapChain.hh"

namespace esp
{
	struct EspRenderContext
	{
		EspWindow& m_window;
		//EspDevice& m_device;
		//EspSwapChain& m_swap_chain;
		//VkCommandBuffer m_command_buffer; //TODO: add EspCommandBuffer class
		// EspScene& scene;
		// EspCamera& camera

	};
}

#endif //RENDERER_RENDER_CONTEXT_H_
