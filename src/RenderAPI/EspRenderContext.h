#ifndef RENDERER_RENDER_CONTEXT_H_
#define RENDERER_RENDER_CONTEXT_H_

#include "EspSwapChain.hh"

namespace esp
{
	struct EspRenderContext
	{
		EspWindow& espWindow;
		EspDevice& espDevice;
		EspSwapChain& espSwapChain;
		VkCommandBuffer commandBuffer; //TODO: add EspCommandBuffer class
		// EspScene& scene;
		// EspCamera& camera

	};
}

#endif //RENDERER_RENDER_CONTEXT_H_
