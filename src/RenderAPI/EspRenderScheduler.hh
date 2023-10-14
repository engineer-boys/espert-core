#ifndef ESP_RENDERER_H_
#define ESP_RENDERER_H_

#include "EspWindow.hh"
#include "EspDevice.hh"
#include "EspSwapChain.hh"

//std
#include <cassert>
#include <memory>
#include <vector>

namespace esp
{
	class EspRenderScheduler
	{
	 private:
		EspWindow& espWindow;
		EspDevice& espDevice;
		std::unique_ptr<EspSwapChain> espSwapChain;
		std::vector<VkCommandBuffer> commandBuffers;

		uint32_t currentImageIndex;
		int currentFrameIndex{0};
		bool isFrameStarted{false};

	 public:
		EspRenderScheduler(EspWindow& window, EspDevice& device);
		~EspRenderScheduler();

		EspRenderScheduler(const EspRenderScheduler&) = delete;
		EspRenderScheduler& operator=(const EspRenderScheduler&) = delete;

		bool isFrameInProgress() const
		{
			return isFrameStarted;
		};

		VkCommandBuffer getCurrentCommandBuffer() const
		{
			assert(isFrameStarted && "Cannot get command buffer if frame hasn't started");
			return commandBuffers[currentFrameIndex];
		}

		int getCurrentFrameIndex()
		{
			assert(isFrameStarted && "Cannot get frame index if frame hasn't started");
			return currentFrameIndex;
		}

		VkCommandBuffer beginFrame();
		void endFrame();

		VkRenderPass getSwapChainRenderPass() const
		{
			return espSwapChain->getRenderPass();
		};
		float getAspectRatio() const
		{
			return espSwapChain->extentAspectRatio();
		};
		void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
		void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

	 private:
		void createCommandBuffers();
		void freeCommandBuffers();
		void recreateSwapChain();
	};
}

#endif //ESP_RENDERER_H_
