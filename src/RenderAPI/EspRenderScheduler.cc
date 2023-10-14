#include "EspRenderScheduler.hh"

// std
#include <stdexcept>
#include <array>

namespace esp
{
	EspRenderScheduler::EspRenderScheduler(EspWindow& window, EspDevice& device) : espWindow{ window }, espDevice{ device }
	{
		recreateSwapChain();
		createCommandBuffers();
	}

	EspRenderScheduler::~EspRenderScheduler()
	{
		freeCommandBuffers();
	}

	VkCommandBuffer EspRenderScheduler::beginFrame()
	{
		assert(!isFrameStarted && "Cannot call beginFrame while frame already in progress");

		auto result = espSwapChain->acquireNextImage(&currentImageIndex);

		//window has changed in such a way that is no longer compatible with the swap chain (example: resizing)
		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			recreateSwapChain();
			return nullptr;
		}

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			throw std::runtime_error("failed to acquire swap chain image");
		}

		isFrameStarted = true;

		auto commandBuffer = getCurrentCommandBuffer();
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to begin recording command buffer");
		}

		return commandBuffer;
	}

	void EspRenderScheduler::endFrame()
	{
		assert(isFrameStarted && "Cannot call endFrame while frame is not in progress");

		auto commandBuffer = getCurrentCommandBuffer();
		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to record command buffer");
		}

		auto result = espSwapChain->submitCommandBuffers(&commandBuffer, &currentImageIndex);
		//TODO: make this an on_window_resized event callback
		/*if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || espWindow.wasFramebufferResized())
		{
			espWindow.resetFramebufferResized();
			recreateSwapChain();
		}
		else if (result != VK_SUCCESS)
		{
			throw std::runtime_error("failed to present swap chain image");
		}*/

		isFrameStarted = false;
		currentFrameIndex = (currentFrameIndex + 1) % EspSwapChain::MAX_FRAMES_IN_FLIGHT;
	}

	void EspRenderScheduler::beginSwapChainRenderPass(VkCommandBuffer commandBuffer)
	{
		assert(isFrameStarted && "Cannot call beginSwapChainRenderPass while frame is not in progress");
		assert(commandBuffer == getCurrentCommandBuffer()
			&& "Cannot begin render pass on command buffer from a different frame");

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = espSwapChain->getRenderPass();
		renderPassInfo.framebuffer = espSwapChain->getFrameBuffer(currentImageIndex);
		//renderArea - area where shader loads and stores will take place
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = espSwapChain->getSwapChainExtent();

		//clearValues - values the frame buffer will use to clear itself
		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.1f, 0.1f, 0.1f, 1.0f }; //color attachment
		clearValues[1].depthStencil = { 1.0f, 0 };                                  //depth attachment

		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(espSwapChain->getSwapChainExtent().width);
		viewport.height = static_cast<float>(espSwapChain->getSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor{{ 0, 0 }, espSwapChain->getSwapChainExtent() };
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
	}

	void EspRenderScheduler::endSwapChainRenderPass(VkCommandBuffer commandBuffer)
	{
		assert(isFrameStarted && "Cannot call endSwapChainRenderPass while frame is in progress");
		assert(commandBuffer == getCurrentCommandBuffer()
			&& "Cannot end render pass on command buffer from a different frame");

		vkCmdEndRenderPass(commandBuffer);
	}

	void EspRenderScheduler::createCommandBuffers()
	{
		commandBuffers.resize(EspSwapChain::MAX_FRAMES_IN_FLIGHT);

		VkCommandBufferAllocateInfo allocateInfo{};
		allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocateInfo.level =
			VK_COMMAND_BUFFER_LEVEL_PRIMARY; //primary - can be submitted to device queue but can't be called by other command buffers
		allocateInfo.commandPool = espDevice.getCommandPool();
		allocateInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

		if (vkAllocateCommandBuffers(espDevice.device(), &allocateInfo, commandBuffers.data()) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to allocate command buffers");
		}
	}

	void EspRenderScheduler::freeCommandBuffers()
	{
		vkFreeCommandBuffers(
			espDevice.device(),
			espDevice.getCommandPool(),
			static_cast<uint32_t>(commandBuffers.size()),
			commandBuffers.data());
		commandBuffers.clear();
	}

	void EspRenderScheduler::recreateSwapChain()
	{
		//as long as 1 dimension is 0 (example: minimizing window) we wait
		auto extent = espWindow.get_extent();
		while (extent.width == 0 || extent.height == 0)
		{
			extent = espWindow.get_extent();
			glfwWaitEvents();
		}

		vkDeviceWaitIdle(espDevice.device()); //wait until the current swap chain is not being used

		if (espSwapChain == nullptr)
		{
			espSwapChain = std::make_unique<EspSwapChain>(espDevice, extent);
		}
		else
		{
			std::shared_ptr<EspSwapChain> oldSwapChain = std::move(espSwapChain);
			espSwapChain = std::make_unique<EspSwapChain>(espDevice, extent, oldSwapChain);

			if (!oldSwapChain->compareSwapFormats(*espSwapChain))
			{
				throw std::runtime_error("Swap chain image/depth format has changed");
			}
		}
	}
}