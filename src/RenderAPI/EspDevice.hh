#ifndef RENDERER_ESP_DEVICE_H_
#define RENDERER_ESP_DEVICE_H_

#include "EspWindow.hh"

// std
#include <string>
#include <vector>

namespace esp
{
	struct SwapChainSupportDetails
	{
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	struct QueueFamilyIndices
	{
		uint32_t graphicsFamily;
		uint32_t presentFamily;
		bool graphicsFamilyHasValue = false;
		bool presentFamilyHasValue = false;
		bool isComplete() const
		{
			return graphicsFamilyHasValue && presentFamilyHasValue;
		}
	};

	class EspDevice
	{
	 private:
		VkInstance instance;
		VkDebugUtilsMessengerEXT debugMessenger;
		VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
		EspWindow& window;
		VkCommandPool commandPool;

		VkDevice device_;
		VkSurfaceKHR surface_;
		VkQueue graphicsQueue_;
		VkQueue presentQueue_;

		const std::vector<const char*> validationLayers =
			{
				"VK_LAYER_KHRONOS_validation"
			};
		const std::vector<const char*> instanceExtensions =
			{
				VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME,
				"VK_KHR_get_physical_device_properties2"
			};
		const std::vector<const char*> deviceExtensions =
			{
				VK_KHR_SWAPCHAIN_EXTENSION_NAME,
				"VK_KHR_portability_subset"
			};

	 public:
#ifdef NDEBUG
		const bool enableValidationLayers = false;
#else
		const bool enableValidationLayers = true;
#endif

		EspDevice(EspWindow& window);
		~EspDevice();

		// Not copyable or movable
		EspDevice(const EspDevice&) = delete;
		EspDevice& operator=(const EspDevice&) = delete;
		EspDevice(EspDevice&&) = delete;
		EspDevice& operator=(EspDevice&&) = delete;

		inline VkCommandPool getCommandPool() { return commandPool; }
		inline VkDevice device() { return device_; }
		inline VkSurfaceKHR surface() { return surface_; }
		inline VkQueue graphicsQueue() { return graphicsQueue_; }
		inline VkQueue presentQueue() { return presentQueue_; }
		inline SwapChainSupportDetails getSwapChainSupport() { return querySwapChainSupport(physicalDevice); }

		uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
		QueueFamilyIndices findPhysicalQueueFamilies()
		{
			return findQueueFamilies(physicalDevice);
		}
		VkFormat findSupportedFormat(
			const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

		void completeQueues()
		{
			vkDeviceWaitIdle(device_);
		}

		// Buffer Helper Functions
		void createBuffer(
			VkDeviceSize size,
			VkBufferUsageFlags usage,
			VkMemoryPropertyFlags properties,
			VkBuffer& buffer,
			VkDeviceMemory& bufferMemory);
		VkCommandBuffer beginSingleTimeCommands();
		void endSingleTimeCommands(VkCommandBuffer commandBuffer);
		void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
		void copyBufferToImage(
			VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layerCount);

		void createImageWithInfo(
			const VkImageCreateInfo& imageInfo,
			VkMemoryPropertyFlags properties,
			VkImage& image,
			VkDeviceMemory& imageMemory);

		VkPhysicalDeviceProperties properties;

	 private:
		void createInstance();
		void setupDebugMessenger();
		void createSurface();
		void pickPhysicalDevice();
		void createLogicalDevice();
		void createCommandPool();

		// helper functions
		bool isDeviceSuitable(VkPhysicalDevice device);
		std::vector<const char*> getRequiredExtensions();
		bool checkValidationLayerSupport();
		QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
		void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
		void hasGflwRequiredInstanceExtensions();
		bool checkDeviceExtensionSupport(VkPhysicalDevice device);
		SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
	};

}

#endif //RENDERER_ESP_DEVICE_H_
